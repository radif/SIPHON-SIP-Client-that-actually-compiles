/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2010-2011 Samuel <samuelv0304@gmail.com>
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

#import "ActiveCallViewController.h"
#import "SIPController.h"
#import "SIPCallController.h"
#ifdef SIP_CALL_TRANSFER
#import "SIPCallTransferController.h"
#endif /* SIP_CALL_TRANSFER */

#import "AKSIPCall.h"

#import "BottomButtonBar.h"
#import "BottomDualButtonBar.h"

#if MULTI_CALL
#import "ContactViewController.h"
#import "FavoritesListController.h"
#import "RecentsViewController.h"
#import "PhoneViewController.h"
#endif /* MULTI_CALL */

#define kTransitionDuration	0.5

@interface ActiveCallViewController (private)

- (void)showKeypad:(BOOL)display animated:(BOOL)animated;

#if MULTI_CALL
- (void) presentViewController:(UIViewController *)viewController animated:(BOOL)animated;
- (void) dismissViewControllerAnimated:(BOOL)animated;

- (UIViewController *)newAddCallViewController;
#endif /* MULTI_CALL */

@end


@implementation ActiveCallViewController

@synthesize callController = callController_;
@synthesize callTimer = callTimer_;
@synthesize enteredDTMF = enteredDTMF_;
@dynamic hangUpButton;
@dynamic menuButton;

#if MULTI_CALL
@synthesize addCallViewController = addCallViewController_;
#endif /* MULTI_CALL */

- (UIButton *)hangUpButton
{
  return [singleButtonBar_ button];
  //return [dualButtonBar_ button];
}

- (UIButton *)menuButton
{
  return [dualButtonBar_ button2];
}

- (void) setOnHoldState:(BOOL)state
{
		[[menuView_ buttonAtPosition:4] setSelected:state];
}

#pragma mark -

- (id)initWithNibName:(NSString *)nibNameOrNil
       callController:(CallController *)callController 
{  
  self = [super initWithNibName:nibNameOrNil
                         bundle:nil];
  if (self != nil) 
  {
    enteredDTMF_ = [[NSMutableString alloc] init];
    [self setCallController:callController];
		[self setWantsFullScreenLayout:YES];
  }
  return self;
}

- (id)init 
{
  [self dealloc];
  NSString *reason = @"Initialize ActiveCallViewController with initWithCallController:";
  @throw [NSException exceptionWithName:@"AKBadInitCall"
                                 reason:reason
                               userInfo:nil];
  return nil;
}

/*
 // Implement loadView to create a view hierarchy programmatically, without using a nib.
 */
- (void)loadView 
{
	CGRect frame = [[UIScreen mainScreen ] applicationFrame];
  UIView *view = [[UIView alloc] initWithFrame:frame];
  [view setAutoresizingMask:UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth];
  
	[view setBackgroundColor:[UIColor clearColor]];
  
  // Button
  singleButtonBar_ = [[BottomButtonBar alloc] initForEndCall];
  [[singleButtonBar_ button] addTarget:self action:@selector(hangUpCall:)
                      forControlEvents:UIControlEventTouchUpInside];
  
  dualButtonBar_ = [[BottomDualButtonBar alloc] initForEndCall];
  [[dualButtonBar_ button] addTarget:self action:@selector(hangUpCall:)
                    forControlEvents:UIControlEventTouchUpInside];
  
  UIButton *menuButton = [BottomButtonBar createButtonWithTitle:NSLocalizedString(@"Hide Keypad", @"Call View")
                                                          image:nil 
                                                          frame:CGRectZero 
                                                     background:[UIImage imageNamed:@"bottombarblue"]
                                              backgroundPressed:[UIImage imageNamed:@"bottombarblue_pressed"]];
  [menuButton addTarget:self action:@selector(hideKeypad:) 
       forControlEvents:UIControlEventTouchUpInside];
  [dualButtonBar_ setButton2:menuButton];
  
  // Menu / Dialpad
  // create the container view which we will use for transition animation (centered horizontally)
	frame = CGRectMake(0.0f, 70.0f, 320.0f, 320.0f);
	containerView_ = [[UIView alloc] initWithFrame:frame];
  [view addSubview:containerView_];
  
  /** Phone Pad **/
  //_phonePad = [[PhonePad alloc] initWithFrame: CGRectMake(0.0f, 70.0f, 320.0f, 320.0f)];
  phonePad_ = [[PhonePad alloc] initWithFrame: CGRectMake(0.0f, 0.0f, 320.0f, 320.0f)];
  [phonePad_ setPlaysSounds: YES];
  [phonePad_ setDelegate: self];
  
  /** Menu **/
  menuView_ = [[MenuCallView alloc] initWithFrame: CGRectMake(18.0f, 52.0f, 285.0f, 216.0f)];
  [menuView_ setDelegate:self];
  [menuView_ setTitle:NSLocalizedString(@"mute", @"Call View")
                image:[UIImage imageNamed:@"mute"] forPosition:0];
  [menuView_ setTitle:NSLocalizedString(@"keypad", @"Call View")
                image:[UIImage imageNamed:@"dialer"] forPosition:1];
	[menuView_ setTitle:NSLocalizedString(@"audio source", @"Call View")
                image:[UIImage imageNamed:@"route"] forPosition:2];
#if MULTI_CALL
  [menuView_ setTitle:NSLocalizedString(@"add call", @"Call View")
                image:[UIImage imageNamed:@"addcall"] forPosition:3];
#else
  [[menuView_ buttonAtPosition:3] setEnabled:NO];
#endif
  [menuView_ setTitle:NSLocalizedString(@"hold", @"Call View")
                image:[UIImage imageNamed:@"hold"] forPosition:4];
#if MULTI_CALL
	[menuView_ setTitle:NSLocalizedString(@"contacts", @"go to address book")
                image:[UIImage imageNamed:@"contacts"] forPosition:5];
#else
  [[menuView_ buttonAtPosition:5] setEnabled:NO];
#endif
  
  self.view = view;
  [view release];
}

- (void)dealloc 
{
  [enteredDTMF_ release];

  [singleButtonBar_ release];
  [dualButtonBar_ release];
  
  [containerView_ release];
  [phonePad_ release];
  [menuView_ release];
	
#if MULTI_CALL
	[addCallViewController_ release];
	addCallViewController_ = nil;
#endif /* MULTI_CALL */
  
  [super dealloc];
}

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
	[[menuView_ buttonAtPosition:4] setSelected:[[self callController] isCallOnHold]];
	
  [self showKeypad:NO animated:NO];
  [super viewWillAppear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
  [singleButtonBar_ removeFromSuperview];
  [dualButtonBar_ removeFromSuperview];
  
  [menuView_ removeFromSuperview];
  [phonePad_ removeFromSuperview];
	
#if MULTI_CALL
	if ([self addCallViewController])
	{
		/*NSUInteger index = [[[self callController] viewControllers] indexOfObject:[self addCallViewController]];
		if (index != NSNotFound)
		{
			[[self callController] removeObjectFromViewControllersAtIndex:index];
		}*/
		[self dismissViewControllerAnimated:NO];
	}
#endif
  
  [super viewDidDisappear:animated]; 
}

- (void)showKeypad:(BOOL)display animated:(BOOL)animated
{
  if (display)
  {
    [singleButtonBar_ removeFromSuperview];
		[[dualButtonBar_ button] setAlpha:0.];
		[[dualButtonBar_ button2] setAlpha:0.];
		[self.view addSubview:dualButtonBar_];
		
		[UIView transitionWithView:containerView_
											duration:(animated ? kTransitionDuration : 0.)
											 options:UIViewAnimationOptionTransitionFlipFromLeft
										animations:^{
											[menuView_ removeFromSuperview];
											[containerView_ addSubview:phonePad_];
											[[dualButtonBar_ button] setAlpha:1.];
											[[dualButtonBar_ button2] setAlpha:1.];
										} 
										completion:NULL/*^(BOOL finished){
										}*/];
	}
  else
  {
    [dualButtonBar_ removeFromSuperview];
		[[singleButtonBar_ button] setAlpha:0.];
		[self.view addSubview:singleButtonBar_];
		
		[UIView transitionWithView:containerView_
											duration:(animated ? kTransitionDuration : 0.)
											 options:UIViewAnimationOptionTransitionFlipFromRight
										animations:^{
											[phonePad_ removeFromSuperview];
											[containerView_ addSubview:menuView_];
											[[singleButtonBar_ button] setAlpha:1.];
										}
										completion:NULL/*^(BOOL finished){
										}*/];
  }
}

- (void)hideKeypad:(id)sender
{
  [self showKeypad:NO animated:YES];
}

- (void)hangUpCall:(id)sender 
{
  [[self callController] hangUpCall];
}

- (void)toggleCallHold:(id)sender 
{
  [[self callController] toggleCallHold];
}

- (void)toggleMicrophoneMute:(id)sender
{
  [[self callController] toggleMicrophoneMute];
}

#ifdef SIP_CALL_TRANSFER
- (void)showCallTransferSheet:(id)sender 
{
  if (![[self callController] isCallOnHold]) 
  {
    [[self callController] toggleCallHold];
  }
  
  CallTransferController *callTransferController = [[self callController] callTransferController];
  
  // TODO: Display the view (modal ?)
  /*[NSApp beginSheet:[callTransferController window]
     modalForWindow:[[self callController] window]
      modalDelegate:nil
     didEndSelector:NULL
        contextInfo:NULL];*/
}
#endif /* SIP_CALL_TRANSFER */

- (void)startCallTimer 
{
  if ([self callTimer] != nil && [[self callTimer] isValid])
    return;
  
  [self setCallTimer:
   [NSTimer scheduledTimerWithTimeInterval:0.2
                                    target:self
                                  selector:@selector(callTimerTick:)
                                  userInfo:nil
                                   repeats:YES]];
}

- (void)stopCallTimer 
{
  if ([self callTimer] != nil) 
  {
    [[self callTimer] invalidate];
    [self setCallTimer:nil];
  }
}

- (void)callTimerTick:(NSTimer *)theTimer 
{
  NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
  NSInteger seconds = (NSInteger)(now - ([[self callController] callStartTime]));
  
  if (seconds < 3600) 
  {
    [[self callController] setStatus:[NSString stringWithFormat:@"%02d:%02d",
                                      (seconds / 60) % 60,
                                      seconds % 60]];
  } 
  else 
  {
    [[self callController] setStatus:[NSString stringWithFormat:@"%02d:%02d:%02d",
                                      (seconds / 3600) % 24,
                                      (seconds / 60) % 60,
                                      seconds % 60]];
  }
}

#pragma mark -
#pragma mark PhonePadDelegate
- (void)phonePad:(id)phonepad keyDown:(char)car
{
  NSString *aString = [NSString stringWithFormat:@"%c", car];
  if ([[self enteredDTMF] length] == 0) 
  {
    [[self enteredDTMF] appendString:aString];
    [[self callController] setDisplayedName:aString];
  } 
  else 
  {
    [[self enteredDTMF] appendString:aString];
    [[self callController] setDisplayedName:[self enteredDTMF]];
  }
  
  [[[self callController] call] sendDTMFDigits:aString];
}

#pragma mark -
#pragma mark MenuCallViewDelegate
- (void)menuButtonClicked:(NSInteger)num
{
  UIButton *button = [menuView_ buttonAtPosition:num];
  switch (num)
  {
    case 0: // Mute 
      [self toggleMicrophoneMute:button];
      [button setSelected:!button.selected];
      break;
    case 1: // Keypad
      [self showKeypad:YES animated:YES];
      break;
    case 2: // Speaker & bluetooth
			if (button.selected) // FIXME: not very nice
			{
				[menuView_ setTitle:NSLocalizedString(@"audio source", @"Call View")
											image:[UIImage imageNamed:@"route"] forPosition:2];
				[[SIPController sharedInstance] disableSpeakerPhone];
				[[SIPController sharedInstance] disableBluetoothHeadset];
			}
			else
			{
				[menuView_ setTitle:NSLocalizedString(@"speaker", @"Call View")
											image:[UIImage imageNamed:@"speaker"] forPosition:2];
				[[SIPController sharedInstance] enableSpeakerPhone];
			}
			[button setSelected:!button.selected];
			break;
    case 3: // Add call / Switch
#if MULTI_CALL
		{
			UIViewController *anAddCallViewController = [self newAddCallViewController];
			[self presentViewController:anAddCallViewController animated:YES];
			[anAddCallViewController release];
		}
#endif /* MULTI_CALL */
#ifdef SIP_CALL_TRANSFER
      if ([[[self callController] call] state] == kAKSIPCallConfirmedState &&
          ![[[self callController] call] isOnRemoteHold])
        [self showCallTransferSheet:button];
#endif /* SIP_CALL_TRANSFER */
      break;
    case 4: // Hold
      [self toggleCallHold:button];
      [button setSelected:!button.selected];
      break;
    case 5: // Contacts
#if MULTI_CALL
		{
			ContactViewController *contactsViewCtrl = [[ContactViewController alloc] init];
			contactsViewCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
			[self presentViewController:contactsViewCtrl animated:YES];
			[contactsViewCtrl release];
		}
#endif /* MULTI_CALL */
      break;
    default:
      break;
  }
}

- (void)menuButtonHeldDown:(NSInteger)num
{
	UIButton *button = [menuView_ buttonAtPosition:num];
  switch (num)
  {
		case 2: // Speaker & bluetooth
			if (!button.selected)
			{
				[[SIPController sharedInstance] enableBluetoothHeadset];
				[menuView_ setTitle:NSLocalizedString(@"bluetooth", @"Call View")
											image:[UIImage imageNamed:@"bluetooth"] forPosition:2];
				[button setSelected:YES];
				break;
			}
		default:
			[self menuButtonClicked:num];
	}
}

#pragma mark -
#pragma mark Add a new call
#if MULTI_CALL
- (void) presentViewController:(UIViewController *)viewController
											animated:(BOOL)animated
{
#if 0
	[viewController viewWillAppear:animated];
	[UIView animateWithDuration:animated ? kTransitionDuration : 0.
									 animations:^{ 
										 [self.view addSubview:viewController.view]; 
									 }
									 completion:^(BOOL finished){
										 [viewController viewDidAppear:animated];
										 addCallViewController_ = [viewController retain];
									 }];
#else
	CGRect rect = [[[self callController] view] bounds];
	UIView *view = [[UIView alloc] initWithFrame:rect];
	CGRect bRect = rect;
	bRect.size.height = 20;
	UIButton *button = [[UIButton alloc] initWithFrame:bRect];
	[button setBackgroundColor:[UIColor greenColor]];
	[button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
	[button setTitle:@"Touch to return to call" forState:UIControlStateNormal];
	[view addSubview:button];
	rect.origin.y = 20;
	rect.size.height -= 20;
	viewController.view.bounds = rect;
	[[self callController] pushViewController:viewController animated:YES];
	addCallViewController_ = [viewController retain];
#endif
}

- (void) dismissViewControllerAnimated:(BOOL)animated
{
#if 0	
	[addCallViewController_ viewWillDisappear:animated];
	/*[addCallViewController_.view removeFromSuperview];
	[addCallViewController_ viewDidDisappear:animated];
	[addCallViewController_ release];
	addCallViewController_ = nil;*/
	[UIView animateWithDuration:animated ? kTransitionDuration : 0.
									 animations:^{ 
										 [addCallViewController_.view removeFromSuperview]; 
									 }
									 completion:^(BOOL finished){
										 [addCallViewController_ viewDidDisappear:animated];
										 [addCallViewController_ release];
										 addCallViewController_ = nil;
									 }];
#else
	[[self callController] popViewControllerAnimated:YES];
	[addCallViewController_ release];
	addCallViewController_ = nil;
#endif
}

- (UIViewController *)newAddCallViewController
{
	UINavigationController *localNav = nil;

	// Create a tabBar controller and an array to contain the view controllers
	UITabBarController *tabBarController = [[UITabBarController alloc] init];
	NSMutableArray *localViewControllersArray = [[NSMutableArray alloc] initWithCapacity:5];
	
	// Setup the view controllers
	/* Favorites List*/
  FavoritesListController *favorites = [[[FavoritesListController alloc]
																				 initWithStyle:UITableViewStylePlain] autorelease];  
  localNav = [[UINavigationController alloc] initWithRootViewController: favorites];
  localNav.navigationBar.barStyle = UIBarStyleBlackOpaque;
	[localViewControllersArray addObject:localNav];
	[localNav release];
	
	/* Recents list */
  RecentsViewController *recents = [[[RecentsViewController alloc]
																		 initWithStyle:UITableViewStylePlain] autorelease];
  localNav = [[UINavigationController alloc] initWithRootViewController: recents];
  localNav.navigationBar.barStyle = UIBarStyleBlackOpaque;
	[localViewControllersArray addObject:localNav];
  [localNav release];
	
	/* Dial pad */
	PhoneViewController *phone = [[PhoneViewController alloc] init];
	[localViewControllersArray addObject:phone];
	[phone release];
	//phoneView_ = [[PhoneViewController alloc] init];
	//[localViewControllersArray addObject:phoneView_];
	
	/* Contacts */
#if 1
	ContactViewController *contactsViewCtrl = [[ContactViewController alloc] init];
	contactsViewCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
  [localViewControllersArray addObject:contactsViewCtrl];
	[contactsViewCtrl release];
#else
	PeoplePickerTableViewController *contacts = [[[PeoplePickerTableViewController alloc]
																								initWithStyle:UITableViewStylePlain] autorelease];  
  localNav = [[UINavigationController alloc] initWithRootViewController: contacts];
  localNav.navigationBar.barStyle = UIBarStyleBlackOpaque;
	[localViewControllersArray addObject:localNav];
	[localNav release];
#endif	

	// Set the tab bar controller view.
  tabBarController.viewControllers = localViewControllersArray;
	
	// The localViewControllersArray is now retained by the tabBarController
	// so we can release the local version
	[localViewControllersArray release];
  
  tabBarController.selectedIndex = 2;
  
	return tabBarController;
}
#endif /* MULTI_CALL */

@end
