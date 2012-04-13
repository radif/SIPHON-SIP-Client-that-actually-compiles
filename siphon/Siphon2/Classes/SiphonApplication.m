/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008-2010 Samuel <samuelv0304@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#import "SiphonApplication.h"
#include "version.h"

#import "ContactViewController.h"
#import "RecentsViewController.h"
//#import "FavoritesListController.h"
#import "VoicemailController.h"

#import "Reachability.h"

#import "RecentCall.h"

#include <unistd.h>
#if defined(CYDIA) && (CYDIA == 1)
#import <CFNetwork/CFNetwork.h>
#include <sys/stat.h>
#endif

#define THIS_FILE "SiphonApplication.m"
#define kDelayToCall 10.0
static NSString *kVoipOverEdge = @"siphonOverEDGE";

typedef enum ConnectionState {
  DISCONNECTED,
  IN_PROGRESS,
  CONNECTED,
  ERROR
} ConnectionState;

@interface UIApplication ()

- (BOOL)launchApplicationWithIdentifier:(NSString *)identifier suspended:(BOOL)suspended;
- (void)addStatusBarImageNamed:(NSString *)imageName removeOnExit:(BOOL)remove;
- (void)addStatusBarImageNamed:(NSString *)imageName;
- (void)removeStatusBarImageNamed:(NSString *)imageName;

@end


@interface SiphonApplication (private)
- (BOOL) sipStartup;
- (void) sipCleanup;
- (BOOL) sipConnect;
- (BOOL) sipDisconnect;

@end


@implementation SiphonApplication

@synthesize window;
//@synthesize navController;
@synthesize tabBarController;
@synthesize recentsViewController;

@synthesize launchDefault;
@synthesize isConnected;
@synthesize isIpod;

/***** MESSAGE *****/
-(void)displayParameterError:(NSString *)msg
{
  NSString *message = NSLocalizedString(msg, msg);
  NSString *error = [message stringByAppendingString:NSLocalizedString(
      @"\nTo correct this parameter, select \"Settings\" from your Home screen, "
       "and then tap the \"Siphon\" entry.", @"SiphonApp")];
  
  
  UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:nil 
                                                   message:error
#if defined(CYDIA) && (CYDIA == 1)
                                                  delegate:self
#else
                                                  delegate:nil
#endif
                                         cancelButtonTitle:NSLocalizedString(@"Cancel", @"SiphonApp") 
                                         otherButtonTitles:NSLocalizedString(@"Settings", @"SiphonApp"), nil ] autorelease];
  [alert show];
  //[alert release];
}

#if defined(CYDIA) && (CYDIA == 1)
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
  if (buttonIndex == 1)
    [[UIApplication sharedApplication] launchApplicationWithIdentifier:@"com.apple.Preferences"
                                                             suspended:NO];
}

#endif

-(void)displayError:(NSString *)error withTitle:(NSString *)title
{
  UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:title 
                                                   message:error 
                                                  delegate:nil 
                                         cancelButtonTitle:NSLocalizedString(@"OK", @"SiphonApp") 
                                         otherButtonTitles:nil] autorelease];
   [alert show];
   //[alert release];
}

-(void)displayStatus:(pj_status_t)status withTitle:(NSString *)title
{
  char msg[80];
  pj_str_t pj_msg = pj_strerror(status, msg, 80);
  PJ_UNUSED_ARG(pj_msg);
  
  NSString *message = [NSString stringWithUTF8String:msg];
  
  [self displayError:message withTitle:nil];
  //[message release];
}

#if defined(CYDIA) && (CYDIA == 1)
#pragma mark -
#pragma mark Power Management
// Technical Q&A QA1340 : Registering and unregistering for sleep and wake notifications
// http://developer.apple.com/mac/library/qa/qa2004/qa1340.html
void powerCallback( void * refCon, io_service_t service, natural_t messageType, 
                   void * messageArgument )
{
  [(SiphonApplication *)refCon powerMessageReceived: messageType 
                                       withArgument: messageArgument];
}

- (void)powerMessageReceived:(natural_t)messageType withArgument:(void *) messageArgument
{
  /*printf( "messageType %08lx, arg %08lx\n",
         (long unsigned int)messageType,
         (long unsigned int)messageArgument );*/
  
  switch ( messageType )
  {
      
    case kIOMessageCanSystemSleep:
      /* Idle sleep is about to kick in. This message will not be sent for forced sleep.
       Applications have a chance to prevent sleep by calling IOCancelPowerChange.
       Most applications should not prevent idle sleep.
       
       Power Management waits up to 30 seconds for you to either allow or deny idle sleep.
       If you don't acknowledge this power change by calling either IOAllowPowerChange
       or IOCancelPowerChange, the system will wait 30 seconds then go to sleep.
       */
      
      //Uncomment to cancel idle sleep
      IOCancelPowerChange( root_port, (long)messageArgument );
      // we will allow idle sleep
      //IOAllowPowerChange( root_port, (long)messageArgument );
      break;
      
    case kIOMessageSystemWillSleep:
      /* The system WILL go to sleep. If you do not call IOAllowPowerChange or
       IOCancelPowerChange to acknowledge this message, sleep will be
       delayed by 30 seconds.
       
       NOTE: If you call IOCancelPowerChange to deny sleep it returns kIOReturnSuccess,
       however the system WILL still go to sleep. 
       */
      
      IOAllowPowerChange( root_port, (long)messageArgument );
      break;
      
    case kIOMessageSystemWillPowerOn:
      //System has started the wake up process...
      break;
      
    case kIOMessageSystemHasPoweredOn:
      //System has finished waking up...
      break;
      
    default:
      break;      
  }
}

- (void)keepAwakeEnabled
{
  NSLog(@"keepAwakeEnabled");
  root_port = IORegisterForSystemPower(self, &notifyPortRef, powerCallback, 
                                       &notifierObject);
  if ( root_port == 0 )
  {
    NSLog(@"IORegisterForSystemPower failed\n");
    return;
  }
  
  // add the notification port to the application runloop
  CFRunLoopAddSource(CFRunLoopGetCurrent(),
                     IONotificationPortGetRunLoopSource(notifyPortRef),
                     kCFRunLoopCommonModes );

  [super addStatusBarImageNamed:@"Siphon" removeOnExit: YES];
}

- (void)keepAwakeDisabled
{
  NSLog(@"keepAwakeDisabled");
  if (root_port == 0)
    return;
  
  [self removeStatusBarImageNamed:@"Siphon"];
  
  // remove the sleep notification port from the application runloop
  CFRunLoopRemoveSource( CFRunLoopGetCurrent(),
                        IONotificationPortGetRunLoopSource(notifyPortRef),
                        kCFRunLoopCommonModes );
  
  // deregister for system sleep notifications
  IODeregisterForSystemPower( &notifierObject );
  
  // IORegisterForSystemPower implicitly opens the Root Power Domain IOService
  // so we close it here
  IOServiceClose( root_port );
  
  // destroy the notification port allocated by IORegisterForSystemPower
  IONotificationPortDestroy( notifyPortRef );  
}
#endif

#if 1
- (void) activateWWAN
{
   self.networkActivityIndicatorVisible = YES;
  //NSURL * url = [[NSURL alloc] initWithString:[NSString stringWithCString:"http://www.anyurl.com"]];
  NSURL * url = [[NSURL alloc] initWithString:[NSString stringWithCString:"http://www.google.com"]];
  NSData * data = [NSData dataWithContentsOfURL:url];
  [url release];
   self.networkActivityIndicatorVisible = NO;
}

- (BOOL)wakeUpNetwork
{
  BOOL overEDGE = FALSE;
  if (isIpod == FALSE)
  {
    overEDGE = [[NSUserDefaults standardUserDefaults] boolForKey:kVoipOverEdge];
}

  NetworkStatus netStatus = [_hostReach currentReachabilityStatus];
  BOOL connectionRequired= [_hostReach connectionRequired];
  if ((overEDGE && netStatus == NotReachable) ||
      (!overEDGE && netStatus != ReachableViaWiFi))
    return NO;
  //if (overEDGE && netStatus == ReachableViaWWAN)
  if (connectionRequired)
  {
    [self activateWWAN];
  }

  return YES;
}
#endif

/***** SIP ********/
/* */
- (BOOL)sipStartup
{
  if (_app_config.pool)
    return YES;
  
  self.networkActivityIndicatorVisible = YES;
  
  if (sip_startup(&_app_config) != PJ_SUCCESS)
  {
    self.networkActivityIndicatorVisible = NO;
    return NO;
  }
  self.networkActivityIndicatorVisible = NO;
  
  /** Call management **/
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(processCallState:)
                                               name: kSIPCallState object:nil];
  
  /** Registration management */
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(processRegState:)
                                               name: kSIPRegState object:nil];
  
  return YES;
}

/* */
- (void)sipCleanup
{
  //[[NSNotificationCenter defaultCenter] removeObserver:self];
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name: kSIPRegState
                                                object:nil];
  [[NSNotificationCenter defaultCenter] removeObserver:self 
                                                  name:kSIPCallState 
                                                object:nil];
  [self sipDisconnect];
  
  if (_app_config.pool != NULL)
  {
    sip_cleanup(&_app_config);
  }
}

/* */
- (BOOL)sipConnect
{
  pj_status_t status;
  
  if (![self sipStartup])
  	return FALSE;

  if ([self wakeUpNetwork] == NO)
    return NO;
  
  if (_sip_acc_id == PJSUA_INVALID_ID)
  {
    self.networkActivityIndicatorVisible = YES;
    if ((status = sip_connect(_app_config.pool, &_sip_acc_id)) != PJ_SUCCESS)
    {
      self.networkActivityIndicatorVisible = NO;
      return FALSE;
    }
  }
  
  return TRUE;
}

/* */
- (BOOL)sipDisconnect
{
  if ((_sip_acc_id != PJSUA_INVALID_ID) &&
      (sip_disconnect(&_sip_acc_id) != PJ_SUCCESS))
  {
    return FALSE;
  }
  
  _sip_acc_id = PJSUA_INVALID_ID;

  isConnected = FALSE;

  return TRUE;
}

- (void)initUserDefaults:(NSMutableDictionary *)dict fromSettings:(NSString *)settings
{
  NSDictionary *prefItem;
  
  NSString *pathStr = [[NSBundle mainBundle] bundlePath];
  NSString *settingsBundlePath = [pathStr stringByAppendingPathComponent:@"Settings.bundle"];
  NSString *finalPath = [settingsBundlePath stringByAppendingPathComponent:settings];
  NSDictionary *settingsDict = [NSDictionary dictionaryWithContentsOfFile:finalPath];
  NSArray *prefSpecifierArray = [settingsDict objectForKey:@"PreferenceSpecifiers"];
  
  for (prefItem in prefSpecifierArray)
  {
    NSString *keyValueStr = [prefItem objectForKey:@"Key"];
    if (keyValueStr)
    {
      id defaultValue = [prefItem objectForKey:@"DefaultValue"];
      if (defaultValue)
      {
        [dict setObject:defaultValue forKey: keyValueStr];
      }
    }
  }
}

- (void)initUserDefaults
{
#if defined(CYDIA) && (CYDIA == 1)
  // TODO Franchement pas beau ;-)
  NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
  NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                        [NSNumber numberWithInt: 1800], @"regTimeout",
                        [NSNumber numberWithBool:NO], @"enableNat",
                        [NSNumber numberWithBool:NO], @"enableMJ",
                        [NSNumber numberWithInt: 5060], @"localPort",
                        [NSNumber numberWithInt: 4000], @"rtpPort",
                        [NSNumber numberWithInt: 15], @"kaInterval",
                        [NSNumber numberWithBool:NO], @"enableEC",
                        [NSNumber numberWithBool:YES], @"disableVad",
                        [NSNumber numberWithInt: 0], @"codec",
                        [NSNumber numberWithBool:NO], @"dtmfWithInfo",
                        [NSNumber numberWithBool:NO], @"enableICE",
                        [NSNumber numberWithInt: 0], @"logLevel",
                        [NSNumber numberWithBool:YES],  @"enableG711u",
                        [NSNumber numberWithBool:YES],  @"enableG711a",
                        [NSNumber numberWithBool:NO],   @"enableG722",
                        [NSNumber numberWithBool:NO],   @"enableG7221",
                        [NSNumber numberWithBool:NO],   @"enableG729",
                        [NSNumber numberWithBool:YES],  @"enableGSM",
                        [NSNumber numberWithBool:NO], @"keepAwake",
                        nil];
  
  [userDef registerDefaults:dict];
  [userDef synchronize];
#else
  NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
  
  NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithCapacity: 10];
  [self initUserDefaults:dict fromSettings:@"Advanced.plist"];
  [self initUserDefaults:dict fromSettings:@"Network.plist"];
  [self initUserDefaults:dict fromSettings:@"Phone.plist"];
  [self initUserDefaults:dict fromSettings:@"Codec.plist"];
  
  [userDef registerDefaults:dict];
  [userDef synchronize];
  //[dict release];
#endif // CYDIA
}

- (void)initModel
{
  NSString *model = [[UIDevice currentDevice] model];
  isIpod = [model hasPrefix:@"iPod"];
  //NSLog(@"%@", model);
}

- (UIView *)applicationStartWithoutSettings
  {    
    // TODO: go to settings immediately
    UIView *mainView = [[UIView alloc] initWithFrame:[[UIScreen mainScreen]
                                                      applicationFrame]];
    mainView.backgroundColor = [UIColor whiteColor];
    
    UINavigationBar *navBar = [[UINavigationBar alloc] init];
    [navBar setFrame:CGRectMake(0, 0, 320,45)];
    navBar.barStyle = UIBarStyleBlackOpaque;
    [navBar pushNavigationItem: [[UINavigationItem alloc] initWithTitle:VERSION_STRING]
                                 animated: NO];
    [mainView addSubview:navBar];

    UIImageView *background = [[UIImageView alloc]
      initWithFrame:CGRectMake(0.0f, 45.0f, 320.0f, 185.0f)];
    [background setImage:[UIImage imageNamed:@"settings.png"]];
    [mainView addSubview:background];

    UILabel *text = [[UILabel alloc]
      initWithFrame: CGRectMake(0, 220, 320, 200.0f)];
    text.backgroundColor = [UIColor clearColor];
    text.textAlignment = UITextAlignmentCenter;
    text.numberOfLines = 0;
    text.lineBreakMode = UILineBreakModeWordWrap;
    text.font = [UIFont systemFontOfSize: 18];
    text.text = NSLocalizedString(@"Siphon requires a valid\nSIP account.\n\nTo enter this information, select \"Settings\" from your Home screen, and then tap the \"Siphon\" entry.", @"SiphonApp");
    [mainView addSubview:text];

    text = [[UILabel alloc] initWithFrame: CGRectMake(0, 420, 320, 40.0f)];
    text.backgroundColor = [UIColor clearColor];
    text.textAlignment = UITextAlignmentCenter;
    text.font = [UIFont systemFontOfSize: 16];
    text.text = NSLocalizedString(@"Press the Home button", @"SiphonApp");
    [mainView addSubview:text];
    
  return mainView;
}

- (UIView *)applicationStartWithSettings
    {
    /* Favorites List*/
        /*
    FavoritesListController *favoritesListCtrl = [[FavoritesListController alloc]
                                                  initWithStyle:UITableViewStylePlain];
                                                  //autorelease];
    favoritesListCtrl.phoneCallDelegate = self;

    UINavigationController *favoritesViewCtrl = [[[UINavigationController alloc]
                                                   initWithRootViewController:
                                                   favoritesListCtrl]
                                                  autorelease];
    favoritesViewCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
    [favoritesListCtrl release];
*/
    /* Recents list */
    recentsViewController = [[RecentsViewController alloc]
                              initWithStyle:UITableViewStylePlain];
                                             //autorelease];
    recentsViewController.phoneCallDelegate = self;
    UINavigationController *recentsViewCtrl = [[[UINavigationController alloc]
                                                   initWithRootViewController:
                                                   recentsViewController]
                                                  autorelease];
    recentsViewCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
    [recentsViewController release];

    /* Dialpad */
    phoneViewController = [[[PhoneViewController alloc]
                            initWithNibName:nil bundle:nil] autorelease];
    phoneViewController.phoneCallDelegate = self;

    /* Contacts */
    ContactViewController *contactsViewCtrl = [[[ContactViewController alloc]
                                                init] autorelease];
    contactsViewCtrl.phoneCallDelegate = self;

    /* Voicemail */
    VoicemailController *voicemailController = [[VoicemailController alloc]
                                                initWithStyle:UITableViewStyleGrouped];
    voicemailController.phoneCallDelegate = self;
    UINavigationController *voicemailNavCtrl = [[[UINavigationController alloc]
                                                initWithRootViewController:
                                                voicemailController]
                                               autorelease];
    voicemailNavCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
    [voicemailController release];

    tabBarController = [[UITabBarController alloc] init];
    tabBarController.viewControllers = [NSArray arrayWithObjects:
                                        /*favoritesViewCtrl,*/ recentsViewCtrl,
                                        phoneViewController, contactsViewCtrl, 
                                        voicemailNavCtrl, nil];
    tabBarController.selectedIndex = 2;

  return tabBarController.view;
}

/***** APPLICATION *****/
#if 0 //def __IPHONE_3_0
- (BOOL)application:(UIApplication *)application 
      didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  if (launchOptions == nil)
  {
    // Démarrage normal
  }
  NSURL *url = [launchOptions objectForKey:UIApplicationLaunchOptionsURLKey];
  // UIApplicationLaunchOptionsSourceApplicationKey : Pour retrouver l'appli qui lance
  if (url != nil)
  {
    // Démarrage avec URL
  }
  NSDictionary *userInfo = [launchOptions objectForKey: UIApplicationLaunchOptionsRemoteNotificationKey];
  if (userInfo != nil)
  {
    // Réception d'une notification
  }
}
/* Sent to the delegate when a running application receives a remote notification.
 * RQ: Ne devrais pas être appelée, le serveur ne devrait pas communiquer avec une 
 *     application lancée.
 * RQ: Une appli peut être lancée sans être connectée au serveur (absence de WiFi)
 */
- (void)application:(UIApplication *)application 
      didReceiveRemoteNotification:(NSDictionary *)userInfo
{
  
}
#else
- (void)applicationDidFinishLaunching:(UIApplication *)application
{
#if defined(CYDIA) && (CYDIA == 1)
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
  NSString *libraryDirectory = [NSString stringWithFormat:@"%@/Siphon", [paths objectAtIndex:0]];
  mkdir([libraryDirectory UTF8String], 0755);
#endif
  
  _sip_acc_id = PJSUA_INVALID_ID;

  isConnected = FALSE;
  
  [self initModel];

  self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];

  NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
  [self initUserDefaults];
  
	if (![[userDef objectForKey: @"username"] length] ||
		![[userDef objectForKey: @"server"] length])
  {
    [window addSubview: [self applicationStartWithoutSettings]];
    //[window makeKeyAndVisible];
    
    launchDefault = NO;
  }
  else
  {
    NSString *server = [userDef stringForKey: @"proxyServer"];
    NSArray *array = [server componentsSeparatedByString:@","];
    NSEnumerator *enumerator = [array objectEnumerator];
    while (server = [enumerator nextObject]) 
      if ([server length])break;// {[server retain]; break;}
    //[enumerator release];
   // [array release];
    if (!server || [server length] < 1)
      server = [userDef stringForKey: @"server"];

    NSRange range = [server rangeOfString:@":" 
                                  options:NSCaseInsensitiveSearch|NSBackwardsSearch];
    if (range.length > 0)
    {
      server = [server substringToIndex:range.location];
    }
    
    // Build GUI
    callViewController = [[CallViewController alloc] initWithNibName:nil bundle:nil];
    
    [window addSubview: [self applicationStartWithSettings]];
    //[window makeKeyAndVisible];

   [[NSNotificationCenter defaultCenter] addObserver:self 
                                            selector:@selector(reachabilityChanged:) 
                                                 name:kReachabilityChangedNotification 
                                              object:nil];
    _hostReach = [[Reachability reachabilityWithHostName: server] retain];
    [_hostReach startNotifer];
    
    launchDefault = YES;
    [self performSelector:@selector(sipConnect) withObject:nil afterDelay:0.2];
    
    if ([userDef boolForKey:@"keepAwake"])
      [self keepAwakeEnabled];
  }

  [window makeKeyAndVisible];
}
#endif

- (void)applicationWillTerminate:(UIApplication *)application
{
  if ([[NSUserDefaults standardUserDefaults] boolForKey:@"keepAwake"])
    [self keepAwakeDisabled];
  
  // TODO enregistrer le numéro en cours pour le rappeler au retour ?
  [_hostReach stopNotifer];
  [[NSNotificationCenter defaultCenter] removeObserver:self 
                                                  name:kReachabilityChangedNotification 
                                                object:nil];
  [self sipCleanup]; // FIXME non logique avec l'appel au démarrage : sipConnect
  //[[NSNotificationCenter defaultCenter] removeObserver:self]; // FIXME il y a des observers qui trainent
  //[tabBarController release] should be ok !!!
  [callViewController release];
  //[tabBarController release];
  
  //int count  = [recentsViewController retainCount];
  //(void)count;
  // FIXME: logically previous [tabBarController release] should be ok !!!
  [recentsViewController finalizeDatabase];
}

#if 1 //ndef __IPHONE_3_0
- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
  pjsua_call_id call_id;
  pj_status_t status;
  
  if (launchDefault == NO)
    return NO;
  launchDefault = NO;

  if (!url)
  {
    // The URL is nil. There's nothing more to do.
    return NO;
  }

  NSString *URLString = [url absoluteString];
  if (!URLString)
  {
    // The URL's absoluteString is nil. There's nothing more to do.
    return NO;
  }
  NSString *URLSip = [URLString stringByReplacingOccurrencesOfString:@"://"
                                                          withString:@":" 
                                                             options:0 
                                                               range:NSMakeRange(3,4)];
  if (_app_config.pool == NULL || pjsua_verify_sip_url([URLSip UTF8String]) != PJ_SUCCESS)
  {
    return NO;
  }
  
  // FIXME: use private variable
  [[NSUserDefaults standardUserDefaults] setObject:URLSip forKey:@"callURL"];
  [[NSUserDefaults standardUserDefaults] setObject:[NSDate date] forKey:@"dateOfCall"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  if ([self isConnected])
  {
    status = sip_dial_with_uri(_sip_acc_id, [URLSip UTF8String], &call_id);
    launchDefault = YES;
    if (status != PJ_SUCCESS)
    {
      // FIXME
      const pj_str_t *str = pjsip_get_status_text(status);
      NSString *msg = [[NSString alloc]
                       initWithBytes:str->ptr 
                       length:str->slen 
                       encoding:[NSString defaultCStringEncoding]];
      [self displayError:msg withTitle:@"registration error"];
      return NO;
    }
  }
  else
  {
    [self performSelector:@selector(outOfTimeToCall) withObject:nil afterDelay:kDelayToCall];
  }

  return YES;
}
#endif

- (void)outOfTimeToCall
{
  launchDefault = YES;
  [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"dateOfCall"];
  [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"callURL"];
}

- (void)dealloc
{
  [_hostReach release];

  [phoneViewController release];
  [recentsViewController release];
  
	//[navController release];
  [callViewController release];
  [tabBarController release];  
	[window release];
	[super dealloc];
}

/************ **************/
//- (void)prefsHaveChanged:(NSNotification *)notification
//{
//  NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
//  [self displayError:[userDef objectForKey: @"sip_user"] withTitle:@"username"];
//}

- (NSString *)normalizePhoneNumber:(NSString *)number
{
  const char *phoneDigits = "22233344455566677778889999",
             *nb = [[number uppercaseString] UTF8String];
  int i, len = [number length];
  char *u, *c, *utf8String = (char *)calloc(sizeof(char), len+1);
  c = (char *)nb; u = utf8String;
  for (i = 0; i < len; ++c, ++i)
  {
    if (*c == ' ' || *c == '(' || *c == ')' || *c == '/' || *c == '-' || *c == '.')
      continue;
/*    if (*c >= '0' && *c <= '9')
    {
      *u = *c;
      u++;
    }
    else*/ if (*c >= 'A' && *c <= 'Z')
    {
      *u = phoneDigits[*c - 'A'];
    }
    else
      *u = *c;
    u++;
  }
  NSString * norm = [[NSString alloc] initWithUTF8String:utf8String];
  free(utf8String);
  return norm;
}


/** FIXME plutôt à mettre dans l'objet qui gère les appels **/
-(void) dialup:(NSString *)phoneNumber number:(BOOL)isNumber
{
  pjsua_call_id call_id;
  pj_status_t status;
  NSString *number;
  
  UInt32 hasMicro, size;

  // Verify if microphone is available (perhaps we should verify in another place ?)
  size = sizeof(hasMicro);
  AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable,
                          &size, &hasMicro);
  if (!hasMicro)
  {
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"No Microphone Available", @"SiphonApp")
                                                    message:NSLocalizedString(@"Connect a microphone to phone", @"SiphonApp")
                                                   delegate:nil 
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"SiphonApp")
                                          otherButtonTitles:nil];
    [alert show];
    [alert release];
    return;
  }
  
  if (isNumber)
    number = [self normalizePhoneNumber:phoneNumber];
  else
    number = phoneNumber;

  if ([[NSUserDefaults standardUserDefaults] boolForKey:@"removeIntlPrefix"])
  {
    number = [number stringByReplacingOccurrencesOfString:@"+"
                                               withString:@"" 
                                                     options:0 
                                                       range:NSMakeRange(0,1)];
  }
  else
  {
  NSString *prefix = [[NSUserDefaults standardUserDefaults] stringForKey: 
                      @"intlPrefix"];
  if ([prefix length] > 0)
  {
    number = [number stringByReplacingOccurrencesOfString:@"+"
                                                   withString:prefix 
                                                      options:0 
                                                        range:NSMakeRange(0,1)];
  }
  }
  
  // Manage pause symbol
  NSArray * array = [number componentsSeparatedByString:@","];
  [callViewController setDtmfCmd:@""];
  if ([array count] > 1)
  {
    number = [array objectAtIndex:0];
    [callViewController setDtmfCmd:[array objectAtIndex:1]];
  }

  if (!isConnected && [self wakeUpNetwork] == NO)
  {
    _phoneNumber = [[NSString stringWithString: number] retain];
    if (isIpod)
    {
      UIAlertView *alertView = [[[UIAlertView alloc] initWithTitle:nil 
                                                           message:NSLocalizedString(@"You must enable Wi-Fi or SIP account to place a call.",@"SiphonApp") 
                                                          delegate:nil 
                                                 cancelButtonTitle:NSLocalizedString(@"OK",@"SiphonApp")
                                                 otherButtonTitles:nil] autorelease];
      [alertView show];
    }
    else
    {
      UIActionSheet *actionSheet = [[[UIActionSheet alloc] initWithTitle:NSLocalizedString(@"The SIP server is unreachable!",@"SiphonApp") 
                                                               delegate:self 
                                                      cancelButtonTitle:NSLocalizedString(@"Cancel",@"SiphonApp") 
                                                 destructiveButtonTitle:nil 
                                                      otherButtonTitles:NSLocalizedString(@"Cellular call",@"SiphonApp"),
                                     nil] autorelease];
      actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
      [actionSheet showInView: self.window];
    }
    return;
  }

  if ([self sipConnect])
  {
    NSRange range = [number rangeOfString:@"@"];
    if (range.location != NSNotFound)
    {
      status = sip_dial_with_uri(_sip_acc_id, [[NSString stringWithFormat:@"sip:%@", number] UTF8String], &call_id);
    }
    else
    status = sip_dial(_sip_acc_id, [number UTF8String], &call_id);
    if (status != PJ_SUCCESS)
    {
      // FIXME
      //[self displayStatus:status withTitle:nil];
      const pj_str_t *str = pjsip_get_status_text(status);
      NSString *msg = [[NSString alloc]
                       initWithBytes:str->ptr 
                       length:str->slen 
                       encoding:[NSString defaultCStringEncoding]];
      [self displayError:msg withTitle:@"registration error"];
    }
  }
}
/** Fin du FIXME */


- (void)processCallState:(NSNotification *)notification
{
#if 0
  NSNumber *value = [[ notification userInfo ] objectForKey: @"CallID"];
  pjsua_call_id callId = [value intValue];
#endif
  int state = [[[ notification userInfo ] objectForKey: @"State"] intValue];

  switch(state)
  {
    case PJSIP_INV_STATE_NULL: // Before INVITE is sent or received.
      return;
    case PJSIP_INV_STATE_CALLING: // After INVITE is sent.
#ifdef __IPHONE_3_0
      [UIDevice currentDevice].proximityMonitoringEnabled = YES;
#else
      self.proximitySensingEnabled = YES;
#endif
    case PJSIP_INV_STATE_INCOMING: // After INVITE is received.
      self.idleTimerDisabled = YES;
      self.statusBarStyle = UIStatusBarStyleBlackTranslucent;
      if (pjsua_call_get_count() == 1)
      {
          [self.window addSubview:callViewController.view];
          [callViewController retain];
//        [tabBarController presentModalViewController:callViewController animated:YES];
      }
    case PJSIP_INV_STATE_EARLY: // After response with To tag.
    case PJSIP_INV_STATE_CONNECTING: // After 2xx is sent/received.
      break;
    case PJSIP_INV_STATE_CONFIRMED: // After ACK is sent/received.
#ifdef __IPHONE_3_0
      [UIDevice currentDevice].proximityMonitoringEnabled = YES;
#else
      self.proximitySensingEnabled = YES;
#endif
      break;
    case PJSIP_INV_STATE_DISCONNECTED:
#if 0
      self.idleTimerDisabled = NO;
#ifdef __IPHONE_3_0
      [UIDevice currentDevice].proximityMonitoringEnabled = NO;
#else
      self.proximitySensingEnabled = NO;
#endif
      if (pjsua_call_get_count() <= 1)
        [self performSelector:@selector(disconnected:) 
                   withObject:nil afterDelay:1.0];
#endif
      break;
  }
  [callViewController processCall: [ notification userInfo ]];
}

- (void)callDisconnecting
{
  self.idleTimerDisabled = NO;
#ifdef __IPHONE_3_0
  [UIDevice currentDevice].proximityMonitoringEnabled = NO;
#else
  self.proximitySensingEnabled = NO;
#endif
  if (pjsua_call_get_count() <= 1)
    [self performSelector:@selector(disconnected:) 
               withObject:nil afterDelay:1.0];
}

- (void)processRegState:(NSNotification *)notification
{
//  const pj_str_t *str;
  //NSNumber *value = [[ notification userInfo ] objectForKey: @"AccountID"];
  //pjsua_acc_id accId = [value intValue];
  self.networkActivityIndicatorVisible = NO;
  int status = [[[ notification userInfo ] objectForKey: @"Status"] intValue];
  
  switch(status)
  {
    case 200: // OK
      isConnected = TRUE;
      if (launchDefault == NO)
      {
        pjsua_call_id call_id;
        NSDate *date = [[NSUserDefaults standardUserDefaults] objectForKey:@"dateOfCall"];
        NSString *url = [[NSUserDefaults standardUserDefaults] stringForKey:@"callURL"];
        if (date && [date timeIntervalSinceNow] < kDelayToCall)
        {          
          sip_dial_with_uri(_sip_acc_id, [url UTF8String], &call_id);
        }
        [self outOfTimeToCall];
      }
      break;
    case 403: // registration failed
    case 404: // not found
      //sprintf(TheGlobalConfig.accountError, "SIP-AUTH-FAILED");
      //break;
    case 503:
    case PJSIP_ENOCREDENTIAL: 
      // This error is caused by the realm specified in the credential doesn't match the realm challenged by the server
      //sprintf(TheGlobalConfig.accountError, "SIP-REGISTER-FAILED");
      //break;
    default:
      isConnected = FALSE;
//      [self sipDisconnect];
  }
} 

- (void) disconnected:(id)fp8
{
  self.statusBarStyle = UIStatusBarStyleDefault;
  //[tabBarController dismissModalViewControllerAnimated: YES];
    [[callViewController view] removeFromSuperview];
    [callViewController release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet 
clickedButtonAtIndex:(NSInteger)buttonIndex
{
  NSURL *url;
  NSString *urlStr;
  switch (buttonIndex) 
  {
    case 0: // Call with GSM
      urlStr = [NSString stringWithFormat:@"tel://%@",_phoneNumber,nil];
      url = [NSURL URLWithString:urlStr];
      [self openURL: url];
      break;
    default:
      break;
  }
  [_phoneNumber release];
}

//-(RecentsViewController *)recentsViewController
//{
//  return recentsViewController;
//}

- (app_config_t *)pjsipConfig
{
  return &_app_config;
}

- (void)reachabilityChanged:(NSNotification *)notification
{
  // FIXME on doit pouvoir faire plus intelligent !!
  //NSLog(@"reachabilityChanged");
 // SCNetworkReachabilityFlags flags = [[[ notification userInfo ] 
  //                                     objectForKey: @"Flags"] intValue];
  Reachability* curReach = [notification object];
  if ([curReach currentReachabilityStatus] == NotReachable)
  {
  [phoneViewController reachabilityChanged:notification];
  [self sipDisconnect];
  }
  else
  {
  [self sipConnect];
}
}

@end
