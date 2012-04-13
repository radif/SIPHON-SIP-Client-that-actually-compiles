/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008 Samuel <samuelv@users.sourceforge.org>
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

#ifndef __CONTACTS_VIEW_H__
#define __CONTACTS_VIEW_H__

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UIKit/UIView.h>
#import <AddressBookUI/ABPeoplePicker.h>

#ifdef __cplusplus
extern "C" {
#endif

extern NSString* const kABCPhoneProperty;
extern NSString* const kABCEmailProperty;

extern NSString* ABCCopyLocalizedPropertyOrLabel(NSString*);

extern NSString* ABCMultiValueGetLabelAtIndex(int,int);
extern NSString* ABCMultiValueCopyValueAtIndex(int, int);
extern int       ABCMultiValueIndexForIdentifier(int,int);
extern void      ABCMultiValueDestroy(int);

extern NSString* ABCRecordCopyCompositeName(struct CPRecord*);
extern int       ABCRecordCopyValue(struct CPRecord *,int ); 

#ifdef __cplusplus
};
#endif

@protocol ContactView
-(void) dialup:(NSString *)phoneNumber;
@end

@interface ContactView : UIView
{
  ABPeoplePicker *_peoplepicker;
  NSString *_phoneNumber;
  id _delegate;
}

- (id)initWithFrame:(struct CGRect)rect;
- (void)setDelegate:(id)delegate;

@end

#endif /* __CONTACTS_VIEW_H__ */
