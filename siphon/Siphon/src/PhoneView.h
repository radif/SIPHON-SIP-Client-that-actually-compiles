/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008 Samuel <samuelv@users.sourceforge.org>
 *  Copyright (C) 2008 Christian Toepp <chris.touchmods@googlemail.com>
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
#import <UIKit/UIView.h>

#import <pjsua-lib/pjsua.h>

#import "DialerPhonePad.h"

@protocol PhoneView
-(void) dialup:(NSString *)phoneNumber;
@end

@interface PhoneView : UIView
{
  UITextLabel    *_lcd;

  DialerPhonePad *_pad;

  UIPushButton   *_addContactButton;
  UIPushButton   *_callButton;
  UIPushButton   *_deleteButton;

  NSTimer *_deleteTimer;
  
  struct __GSFont *font;

  id _delegate;
}

- (id)initWithFrame:(struct CGRect)rect;
- (id)delegate;
- (void)setDelegate:(id)newDelegate;

- (void)peoplePickerDidEndPicking:(id)fp8;
- (void)peoplePicker:(id)fp8 editedPerson:(struct CPRecord *)fp12 
    property:(int)fp16 identifier:(int)fp20;

@end

