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

#import <UIKit/UIKit.h>
#import <AddressBookUI/AddressBookUI.h>

#import "PhoneCallDelegate.h"
#import "DialerPhonePad.h"
#import "AbsLCDView.h"

#define SPECIFIC_ADD_PERSON 1

@interface PhoneViewController : UIViewController <
          UITextFieldDelegate,
#if SPECIFIC_ADD_PERSON
          UIActionSheetDelegate,
          ABNewPersonViewControllerDelegate,
          ABPeoplePickerNavigationControllerDelegate,
#else         
          ABUnknownPersonViewControllerDelegate,
#endif
           PhonePadDelegate>
{
  UITextField *_label;
  UIView      *_container;
  AbsLCDView *_lcd;

  DialerPhonePad *_pad;
  
  UIButton *_addContactButton;
  UIButton *_gsmCallButton;
  UIButton *_callButton;
  UIButton *_deleteButton;
  
  int      _deletedChar;
  NSTimer *_deleteTimer;
  
  NSString *_lastNumber;
  
  id<PhoneCallDelegate> phoneCallDelegate;
#if SPECIFIC_ADD_PERSON
  ABPeoplePickerNavigationController *peoplePickerCtrl;
#endif
}

@property (nonatomic, retain)   id<PhoneCallDelegate> phoneCallDelegate;

@end
