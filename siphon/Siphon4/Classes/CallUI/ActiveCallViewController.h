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

#import <UIKit/UIKit.h>

#import "PhonePad.h"
#import "MenuCallView.h"

#define MULTI_CALL 0

@class CallController;
@class BottomButtonBar, BottomDualButtonBar, PhonePad, MenuCallView;

@interface ActiveCallViewController : UIViewController /*<AKActiveCallViewDelegate>*/ 
  <PhonePadDelegate, MenuCallViewDelegate>
{
@private
  CallController *callController_;
  NSTimer *callTimer_;
  NSMutableString *enteredDTMF_;

  UIView              *containerView_;
  PhonePad            *phonePad_;
  MenuCallView        *menuView_;
  
  BottomButtonBar     *singleButtonBar_;
  BottomDualButtonBar *dualButtonBar_;
	
#if MULTI_CALL
	UIViewController *addCallViewController_;
#endif
}

// Call controller the receiver belongs to.
@property(nonatomic, assign) CallController *callController;

// Timer to present call duration time.
@property(nonatomic, assign) NSTimer *callTimer;

// DTMF digits entered by a user.
@property(nonatomic, retain) NSMutableString *enteredDTMF;

// Hang-up button outlet.
@property(nonatomic, readonly) UIButton *hangUpButton;

// Menu button outlet.
@property(nonatomic, readonly) UIButton *menuButton;

#if MULTI_CALL
// The view that is temporarily displayed on top of the active call view controller.
@property(nonatomic, readonly) UIViewController *addCallViewController;
#endif /* MULTI_CALL */

//
- (void) setOnHoldState:(BOOL)state;

// Designated initializer.
// Initializes an ActiveCallViewController object with a given nib file and call
// controller.
- (id)initWithNibName:(NSString *)nibName
       callController:(CallController *)callController;

// Hangs up call.
- (void)hangUpCall:(id)sender;

// Toggles call hold.
- (void)toggleCallHold:(id)sender;

// Toggles microphone mute.
- (void)toggleMicrophoneMute:(id)sender;

#ifdef SIP_CALL_TRANSFER
// Shows call transfer sheet.
- (void)showCallTransferSheet:(id)sender;
#endif /* SIP_CALL_TRANSFER */

// Starts call timer.
- (void)startCallTimer;

// Stops call timer.
- (void)stopCallTimer;

// Method to be called when call timer fires.
- (void)callTimerTick:(NSTimer *)theTimer;

@end
