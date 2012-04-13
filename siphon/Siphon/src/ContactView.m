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

#import "ContactView.h"

@implementation ContactView

- (id) initWithFrame:(struct CGRect)rect
{
  self = [super initWithFrame:rect];
  if(self != nil) 
  {
    _peoplepicker =[[ABPeoplePicker alloc] initAsAddressBookWithFrame: rect];
    [_peoplepicker setDelegate: self];
   
    [self addSubview:_peoplepicker];
  }

  return self;
}

-  (void) dealloc
{
  [_peoplepicker release];
  [super dealloc];
}

/***************************************************************
 * 
 * cpRecord - the high-level ContactPerson record object
 * propertyId - value of "property" field in table ABMultiValue
 * identifier - value of "identifier" field in table ABMultiValue
 *  
 ***************************************************************/
- (void)peoplePicker:(id)peoplePicker
      selectedPerson:(struct CPRecord *)cpRecord 
      property:(int)propertyId 
      identifier:(int)propertyIdentifier 
{
  int multiValue,valueIdx;
  
  if ((int)kABCPhoneProperty == propertyId)
  {
    multiValue = ABCRecordCopyValue(cpRecord, propertyId);
    valueIdx=ABCMultiValueIndexForIdentifier(multiValue,propertyIdentifier);
    _phoneNumber = ABCMultiValueCopyValueAtIndex(multiValue, valueIdx);
    ABCMultiValueDestroy(multiValue);
  }
  else
  {
    [peoplePicker performDefaultActionForPerson:cpRecord 
                  property:propertyId identifier:propertyIdentifier];
  }
}

- (void)peoplePickerDidEndPicking:(id)iself 
{
  //[_peoplepicker saveState];
  [_peoplepicker resume];
  if(_delegate == nil) 
  {
    NSLog(@"ERROR: delegate is nil!");
    //TODO throw exception!!
    return;
  }
  if (_phoneNumber != nil &&
  	  [_delegate respondsToSelector:@selector(dialup:)]) 
  {
    [_delegate dialup:_phoneNumber];
    [_phoneNumber release];
    _phoneNumber = nil;
  }
  else 
  {
    NSLog(@"WARN delegate would not respond to message @selector(contactSelected:phoneNumber:)");
  }
  //TODO send out notification ?
}

- (void)peoplePicker:(id)fp8 willTransitionToFullScreenViewFromView:(id)view
{
  [UIApp hideButtonBar: view];
}
- (void)peoplePicker:(id)fp8 willTransitionFromFullScreenViewToView:(id)view
{
  [UIApp showButtonBar: view];
}

//- (void)peoplePicker:(id)fp8 didTransitionFromFullScreenViewToView:(id)view
//{
//  [UIApp showButtonBar: view];
//}

- (void)setDelegate:(id)delegate 
{
  _delegate = delegate;
}

@end