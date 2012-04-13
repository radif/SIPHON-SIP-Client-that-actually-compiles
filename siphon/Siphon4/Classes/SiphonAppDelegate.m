/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008-2011 Samuel <samuelv0304@gmail.com>
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

#import "SiphonAppDelegate.h"
#import "FavoritesListController.h"
#import "PhoneViewController.h"
#import <AddressBookUI/AddressBookUI.h>
#import "InAppSettings.h"

#import "SIPController.h"
#import "SIPAccountController.h"

#import "AKSIPCall.h"

#define KEEP_ALIVE_INTERVAL 600

@implementation SiphonAppDelegate

@synthesize window = window_;
@synthesize tabBarController = tabBarController_;


- (void)setupMainUserInterface
{
	UINavigationController *localNav;
	
	// Set up the window and content view
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

	// Create a tabBar controller and an array to contain the view controllers
	self.tabBarController = [[UITabBarController alloc] init];
	NSMutableArray *localViewControllersArray = [[NSMutableArray alloc] initWithCapacity:5];
	
	// Setup the view controllers
	/* Favorites List*/
  FavoritesListController *favorites = [[[FavoritesListController alloc]
																				 initWithStyle:UITableViewStylePlain] autorelease];  
  localNav = [[UINavigationController alloc] initWithRootViewController: favorites];
  localNav.navigationBar.barStyle = UIBarStyleBlackOpaque;
	[localViewControllersArray addObject:localNav];
	[localNav release];
	
	/* Dial pad */
	PhoneViewController *phone = [[PhoneViewController alloc] init];
	[localViewControllersArray addObject:phone];
	[phone release];
	
	/* Contacts */
	ABPeoplePickerNavigationController *picker = [[ABPeoplePickerNavigationController alloc] init];
	picker.tabBarItem = [[UITabBarItem alloc] initWithTabBarSystemItem:UITabBarSystemItemContacts tag:'cont'];
	[localViewControllersArray addObject:picker];
	[picker release];
	
	/* Settings */
	InAppSettingsViewController *settings = [[[InAppSettingsViewController alloc] init] autorelease];
	localNav = [[UINavigationController alloc] initWithRootViewController:settings];
	localNav.navigationBar.barStyle = UIBarStyleBlackOpaque;
	localNav.tabBarItem.image = [UIImage imageNamed:@"settings"];
	localNav.tabBarItem.title = NSLocalizedString(@"Settings", @"Modify the settings");
	[localViewControllersArray addObject:localNav];
	[localNav release];

	// Set the tab bar controller view.
  self.tabBarController.viewControllers = localViewControllersArray;
	
	// The localViewControllersArray is now retained by the tabBarController
	// so we can release the local version
	[localViewControllersArray release];
  
  self.tabBarController.selectedIndex = 1;
  
	// Set the window subview as the tab bar controller
	[self.window addSubview: self.tabBarController.view];
}

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	[self setupMainUserInterface];
	[self.window makeKeyAndVisible];
	
  [[SIPController sharedInstance] controllerWillStart];
}

- (BOOL)application:(UIApplication *)application 
			handleOpenURL:(NSURL *)url
{
	return YES;
}

- (void)application:(UIApplication *)application 
		didReceiveRemoteNotification:(NSDictionary *)userInfo
{
	
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification
{
	NSDictionary *userInfo = notification.userInfo;
	NSString *type = (NSString *)[userInfo objectForKey:@"NotificationType"];
	
	if ([type isEqualToString:@"SIP"])
	{
		NSNumber *callIdentifier = (NSNumber *)[userInfo objectForKey:AKSIPCallIdentifier];
		NSNumber *accountIdentifier = (NSNumber *)[userInfo objectForKey:AKSIPAccountIdentifier];
		
		if (callIdentifier && accountIdentifier)
		{
			AccountController *accountController = [[SIPController sharedInstance] accountContollerByIdentifier:[accountIdentifier intValue]];
			AKSIPCall *theCall = nil;
			for (AKSIPCall *aCall in [accountController.account calls])
				if (aCall.identifier == [callIdentifier intValue])
				{
					theCall = aCall;
					break;
				}
			
			if (!theCall || theCall.state == PJSIP_INV_STATE_DISCONNECTED)
				return ;
			
			[accountController acceptIncomingCallInBackground:theCall];
		}
	}
}

#pragma mark -
#pragma mark Application lifecycle
- (BOOL)application:(UIApplication *)application 
didFinishLaunchingWithOptions:(NSDictionary *)launchOptions 
{    
    
	// Override point for customization after application launch.
    
	//[self.window makeKeyAndVisible];
	
	[self applicationDidFinishLaunching:application];
	
  // Override point for customization after application launch
	if (!launchOptions)
    return YES;
  
  if ([launchOptions objectForKey:UIApplicationLaunchOptionsSourceApplicationKey])
  {
    NSURL *url = [launchOptions objectForKey:UIApplicationLaunchOptionsURLKey];
    return [self application:application handleOpenURL: url];
  }
  
  NSDictionary *userInfo = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
  if (userInfo)
  {
    [self application:application didReceiveRemoteNotification:userInfo];
    return YES;
  }
  
  UILocalNotification *notification = [launchOptions objectForKey:UIApplicationLaunchOptionsLocalNotificationKey];
  if (notification)
  {
    //[self application:application didReceiveLocalNotification:notification];
    return YES;
  }
  
	return NO;
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

- (void)keepAlive 
{
/*	if (!pj_thread_is_registered())
	{
		pj_thread_register("ipjsua", a_thread_desc, &a_thread);
	}    

	for (int i=0; i<(int)pjsua_acc_get_count(); ++i) 
	{
		if (!pjsua_acc_is_valid(i))
			continue;
		
		if (app_config.acc_cfg[i].reg_timeout < KEEP_ALIVE_INTERVAL)
			app_config.acc_cfg[i].reg_timeout = KEEP_ALIVE_INTERVAL;
		pjsua_acc_set_registration(i, PJ_TRUE);
	}*/
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
	if (![[UIDevice currentDevice] isMultitaskingSupported]) // FIXME: Does user want this feature
  {	
    [self applicationWillTerminate:application];
    return;
  }
	
	[self performSelectorOnMainThread:@selector(keepAlive) withObject:nil waitUntilDone:YES];
	[application setKeepAliveTimeout:KEEP_ALIVE_INTERVAL handler: ^{
		[self performSelectorOnMainThread:@selector(keepAlive) 
													 withObject:nil waitUntilDone:YES];
		}];
}


- (void)applicationWillEnterForeground:(UIApplication *)application 
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
  [[UIApplication sharedApplication] clearKeepAliveTimeout];
}


- (void)applicationDidBecomeActive:(UIApplication *)application 
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
	[[SIPController sharedInstance] controllerWillTerminate];
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application 
{
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc 
{
	[tabBarController_ release];
	[window_ release];
	[super dealloc];
}


@end
