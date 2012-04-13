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

#import "IncomingCallViewController.h"

#import "SIPController.h"
#import "SIPCallController.h"

#import "BottomButtonBar.h"
#import "BottomDualButtonBar.h"
#import "DualButtonView.h"

#import "SIPAccountController.h"
#import "AKSIPCall.h"

@implementation IncomingCallViewController

@synthesize callController = callController_;

@dynamic acceptCallButton;
@dynamic declineCallButton;

- (UIButton *)acceptCallButton
{
  return [dualButtonBar_ button2];  
}

- (UIButton *)declineCallButton
{
  return [dualButtonBar_ button];
}


#pragma mark -

- (id)initWithNibName:(NSString *)nibNameOrNil
       callController:(CallController *)callController
{
  self = [super initWithNibName:nibNameOrNil
                         bundle:nil];
  if (self != nil) 
  {
		[self setWantsFullScreenLayout:YES];
    [self setCallController:callController];
  }
  return self;
}

- (id)init 
{
  [self dealloc];
  NSString *reason = @"Initialize IncomingCallViewController with initWithCallController:";
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

  self.view = view;
  [view release];
}

- (void)incomingCallView
{
	if (dualButtonBar_)
		[dualButtonBar_ release];
	
	dualButtonBar_ = [[BottomDualButtonBar alloc] initForIncomingCallWaiting];
  [[dualButtonBar_ button] addTarget:self action:@selector(hangUpCall:)
                    forControlEvents:UIControlEventTouchUpInside];
  [[dualButtonBar_ button2] addTarget:self action:@selector(acceptCall:)
                     forControlEvents:UIControlEventTouchUpInside];
  [self.view addSubview:dualButtonBar_];
}

- (void)currentAndIncomingCallView
{
	if (bottomButtonBar_)
		[bottomButtonBar_ release];
	bottomButtonBar_ = [[BottomButtonBar alloc] initForIncomingCallWaiting];
	[[bottomButtonBar_ button] addTarget:self action:@selector(endActiveCallAndAcceptCall:)
											forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:bottomButtonBar_];
	
	if (dualButtonView_)
		[dualButtonView_ release];
	dualButtonView_ = [[DualButtonView alloc] initWithFrame: CGRectMake(18.0f, 122.0f, 285.0f, 222.0f)];
  //[_buttonView setDelegate:self];
  [dualButtonView_ setTitle:NSLocalizedString(@"Ignore", @"Call View") image:nil forPosition:0];
	[[dualButtonView_ buttonAtPosition:0] addTarget:self action:@selector(hangUpCall:)
																 forControlEvents:UIControlEventTouchUpInside];
  [dualButtonView_ setTitle:NSLocalizedString(@"Hold Call + Answer", @"Call View") image:nil forPosition:1];
	[[dualButtonView_ buttonAtPosition:1] addTarget:self action:@selector(holdActiveCallAndAcceptCall:)
																 forControlEvents:UIControlEventTouchUpInside];
	[self.view addSubview:dualButtonView_];
}

- (void)dealloc 
{
  [dualButtonBar_ release];
	[bottomButtonBar_ release];
	[dualButtonView_ release];

  [super dealloc];
}

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/
#pragma mark -
#pragma mark Responding to View Events
- (void)viewWillAppear:(BOOL)animated
{
	// TODO Configure the view
	// if activecall displays view with 3 buttons otherwise 2 buttons
	if ([[SIPController sharedInstance] hasActiveCallControllers])
		[self currentAndIncomingCallView];
	else
		[self incomingCallView];

	[super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated 
{
	[super viewDidAppear:animated];
	[[UIApplication sharedApplication] beginReceivingRemoteControlEvents];
	[self becomeFirstResponder];
}

- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];
	[[UIApplication sharedApplication] endReceivingRemoteControlEvents];
	[self resignFirstResponder];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
	NSArray *subviews = [self.view subviews];
	for (UIView *aView in subviews)
		[aView removeFromSuperview];
}

- (BOOL) canBecomeFirstResponder 
{	
	return YES;
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

/*- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}*/

/*- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}*/

/*- (void)removeObservations 
 {
 [[self displayedNameField] unbind:NSValueBinding];
 [[self statusField] unbind:NSValueBinding];
 [super removeObservations];
 }*/


// FIXME move from here

- (CallController *)activeCallController
{
	for (AccountController *anAccountController in [[SIPController sharedInstance] enabledAccountControllers]) 
	{
		for (CallController *aCallController in [anAccountController callControllers])
		{
			// FIXME incoming call
			if ([aCallController isCallActive] && ![aCallController isCallOnHold] &&
					[[aCallController call] state] == kAKSIPCallConfirmedState)
				return aCallController;
		}
	}
	return nil;
}

#pragma mark -
#pragma mark button actions
- (void)acceptCall:(id)sender 
{
  // FIXME: if this is a second call, we have to hold or hang-up the first call
  [[self callController] acceptCall];
}

- (void)hangUpCall:(id)sender 
{
  [[self callController] hangUpCall];
}

- (void)endActiveCallAndAcceptCall:(id)sender
{
	CallController *activeCallController = [self activeCallController];
	if (activeCallController)
			[activeCallController hangUpCall];
	[[self callController] acceptCall];
}

- (void)holdActiveCallAndAcceptCall:(id)sender
{
	CallController *activeCallController = [self activeCallController];
	if (activeCallController)
		[activeCallController toggleCallHold];
	
	[[self callController] acceptCall];
}

#pragma mark -
#pragma mark Handling remote-control events
- (void) remoteControlReceivedWithEvent: (UIEvent *) receivedEvent {
	
	if (receivedEvent.type == UIEventTypeRemoteControl)
	{
		switch (receivedEvent.subtype) 
		{
			case UIEventSubtypeRemoteControlTogglePlayPause:
				if ([[self callController] isCallActive])
					[self hangUpCall:nil];
				else
					[self acceptCall: nil];
				break;
				
			case UIEventSubtypeRemoteControlPreviousTrack:
				//[self previousTrack: nil];
				break;
				
			case UIEventSubtypeRemoteControlNextTrack:
				//[self nextTrack: nil];
				break;
				
			default:
				break;
		}
	}
}

@end
