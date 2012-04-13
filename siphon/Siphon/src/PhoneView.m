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

#import <CoreGraphics/CGGeometry.h>
#import <OSServices/SystemSound.h>

#import <Message/NetworkController.h>
#import <iTunesStore/ISNetworkController.h>
#import <WebCore/WebFontCache.h>

#import "Siphon.h"
#import "PhoneView.h"

#include "call.h"
#include "dtmf.h"

@implementation PhoneView

- (BOOL)hasWiFiConnection 
{
    return ([[ISNetworkController sharedInstance] networkType] == 2);
}

-(id)initWithFrame:(struct CGRect)frame
{
  
  if ((self = [super initWithFrame:frame]) != nil)
  {
    UIImageView *lcd = [[UIImageView alloc] initWithFrame:
    CGRectMake(0.0f,0.0f, 320.0f, 74.0f)];
    [lcd setImage: [UIImage applicationImageNamed:@"lcd_top.png"]];
    [self addSubview:lcd];

    font = [NSClassFromString(@"WebFontCache") 
           createFontWithFamily:@"Helvetica" 
           traits:2 size:35];

    float fnt[] = {255, 255, 255, 1};
    struct CGColor *fntColor = CGColorCreate(CGColorSpaceCreateDeviceRGB(),fnt);
    float bg[] = {0, 0, 0, 0};
    struct CGColor *bgColor = CGColorCreate(CGColorSpaceCreateDeviceRGB(),bg);
    
    _lcd = [[UITextLabel alloc] initWithFrame:
      CGRectMake(0.0f, 0.0f, 320.0f, 65.0f)];
    [_lcd setCentersHorizontally:TRUE];
    [_lcd setFont: font];
    [_lcd setAlignment: 1]; // Center
    [_lcd setColor: fntColor];
    [_lcd setBackgroundColor: bgColor];
    [_lcd setTextAutoresizesToFit:YES];
    [_lcd setText: @""];

    _pad = [[DialerPhonePad alloc] initWithFrame:
          CGRectMake(0.0f, 74.0f, 320.0f, 274.0f)];
    [_pad setPlaysSounds:TRUE];
    [_pad setDelegate:self];

    _addContactButton = [[UIPushButton alloc] initWithFrame: 
      CGRectMake(0.0f, 348.0f, 107.0f, 64.0f)];
    [_addContactButton setImage:
      [UIImage applicationImageNamed:@"addcontact.png"] forState:0];
    [_addContactButton setImage:
      [UIImage applicationImageNamed:@"addcontact_pressed.png"] forState:1];
    [_addContactButton addTarget:self action:@selector(addButtonPressed:) 
      forEvents:1];
   
    _callButton = [[UIPushButton alloc] initWithFrame: 
      CGRectMake(107.0f, 348.0f, 107.0f, 64.0f)];
    [_callButton setImage:[UIImage applicationImageNamed:@"call.png"] 
      forState:0];
  //  [_callButton setImage:[UIImage applicationImageNamed:@"call_pressed.png"] forState:1];
    [_callButton addTarget:self action:@selector(callButtonPressed:) forEvents:1];

    _deleteButton = [[UIPushButton alloc] initWithFrame: 
      CGRectMake(214.0f, 348.0f, 107.0f, 64.0f)];
    [_deleteButton setImage:[UIImage applicationImageNamed:@"delete.png"] 
      forState:0];
    [_deleteButton setImage:
      [UIImage applicationImageNamed:@"delete_pressed.png"] forState:1];

    [_deleteButton addTarget:self action:@selector(deleteButtonPressed:) forEvents:1];
    [_deleteButton addTarget:self action:@selector(deleteButtonReleased:) forEvents:0x40];

    [self addSubview: _lcd];
    [self addSubview: _pad];
    [self addSubview: _addContactButton];
    [self addSubview: _callButton];
    [self addSubview: _deleteButton];
  }
  return self;
}

- (void)alertSheet:(UIAlertSheet*)sheet buttonClicked:(int)button
{
  if ( button == 1 )
    NSLog(@"Create New Contact");
  else if ( button == 2 )
  {
    NSLog(@"Add to Existing Contact");
    ABPeoplePicker *peoplePicker = [[ABPeoplePicker alloc] initWithFrame:
      CGRectMake(0.0f,0.0f, 320.0f, 74.0f)];
    [peoplePicker setBehavior:YES];
    [peoplePicker setAllowsCancel:YES];
    [peoplePicker setDelegate:self];
//    CFArrayRef _props = CFArrayCreate(nil, &kABCPhoneProperty, 1, nil);
    CFMutableArrayRef _props = CFArrayCreateMutable( NULL, 1, NULL );
    CFArrayAppendValue(_props, kABCPhoneProperty);
    [peoplePicker setDisplayedProperties:_props];
//    [_props release];
  }
//  else if ( button == 3 )
//      NSLog(@"Cancel");

  [sheet dismiss];
}

/*** Buttons callback ***/
- (void)phonePad:(TPPhonePad *)phonepad appendString:(NSString *)string
{
  NSString *curText = [_lcd text];
  [_lcd setText: [curText stringByAppendingString: string]];
}

- (void)addButtonPressed:(UIPushButton*)btn
{
  if ([[_lcd text] length] < 1)
    return;
  
#if 0 
  if ([[ABCGetSharedAddressBook] ABCGetPersonCount] == 0)
  {
    // Create New Contact
  }
  else
  {
    NSArray *array = [[NSArray alloc] initWithObjects: 
      NSLocalizedString(@"Create New Contact",@"Phone View"),
      NSLocalizedString(@"Add to Existing Contact",@"Phone View"),
      NSLocalizedString(@"Cancel",@"Phone View"),
      nil];
    
    UIAlertSheet *alertSheet = [[UIAlertSheet alloc] initWithTitle:nil 
                                buttons:array defaultButtonIndex:2 
                                delegate:self context:self];
    
    [alertSheet presentSheetInView: self];
  }
#endif  
}

- (void)callButtonPressed:(UIPushButton*)btn
{
  if (([[_lcd text] length] > 1) && 
      ([_delegate respondsToSelector:@selector(dialup:)])) 
    {
      [_delegate dialup: [_lcd text]];
      [_lcd setText:@""];
    }
}

- (void)stopTimer
{
  if (_deleteTimer)
  {
    [_deleteTimer invalidate];
    [_deleteTimer release];
    _deleteTimer = NULL;
  }
}

- (void)deleteRepeat
{
  NSString *curText = [_lcd text];
  if([curText length] > 0)
  {
    [_lcd setText: [curText substringToIndex:([curText length]-1)]];
  }
  else
  {
    [self stopTimer];
  }
}

- (void)deleteButtonPressed:(UIPushButton*)btn
{
  [self deleteRepeat];
  _deleteTimer = [[NSTimer scheduledTimerWithTimeInterval:0.4 target:self 
                   selector:@selector(deleteRepeat) userInfo:nil 
                   repeats:YES] retain];
  
}

- (void)deleteButtonReleased:(UIPushButton*)btn
{
  [self stopTimer];
}

/*** PeoplePicker ***/
- (void)peoplePickerDidEndPicking:(id)fp8
{
  NSLog(@"PhoneView peoplePickerDidEndPicking");
}

- (void)peoplePicker:(id)fp8 editedPerson:(struct CPRecord *)fp12 
    property:(int)fp16 identifier:(int)fp20
{
  NSLog(@"PhoneView peoplePicker");
}



/*** ***/
- (id)delegate 
{
  return _delegate;
}

- (void)setDelegate:(id)newDelegate 
{
  _delegate = newDelegate;
}


@end

