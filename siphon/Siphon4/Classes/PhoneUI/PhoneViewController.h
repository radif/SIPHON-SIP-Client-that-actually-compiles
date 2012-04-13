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

#import <UIKit/UIKit.h>
#import <AddressBookUI/AddressBookUI.h>

#import "DialerPhonePad.h"
#import "LCDPhoneView.h"

#define GSM_BUTTON 1
#define POPOVER_CALL 0
#define HTTP_REQUEST 0

#if defined(POPOVER_CALL) && POPOVER_CALL!=0
#import "WEPopoverController.h"
#endif /* POPOVER_CALL */

#if HTTP_REQUEST
@class SiphonRequest;
#endif /* HTTP_REQUEST */

@interface PhoneViewController : UIViewController <
          UITextFieldDelegate,
					PhonePadDelegate,
          UIActionSheetDelegate,
          ABNewPersonViewControllerDelegate,
          ABPeoplePickerNavigationControllerDelegate>
{
	@private
  UITextField *_textfield;
  LCDPhoneView  *_lcd;

  DialerPhonePad *_pad;

	UIView      *_container;
  UIButton *_addContactButton;
#if GSM_BUTTON
  UIButton *_gsmCallButton;
#endif
  UIButton *_callButton;
	
  UIButton *_deleteButton;
  int      _deletedChar;
  NSTimer *_deleteTimer;
  
  NSString *_lastNumber;

  ABPeoplePickerNavigationController *peoplePickerController_;
	
#if defined(POPOVER_CALL) && POPOVER_CALL!=0
	BOOL consumedTap_;
	
	WEPopoverController *_callPickerPopover;
#endif /* POPOVER_CALL */
	
#if HTTP_REQUEST
	NSTimer        *_balanceTimer; 
	SiphonRequest  *_balanceRequest;
  SiphonRequest  *_rateRequest;
#endif /* HTTP_REQUEST */
}

// FIXME we use it to display the registration status. We should define a new view to manage the 
// different account.
@property(nonatomic, readonly) LCDPhoneView *lcd;

@end

