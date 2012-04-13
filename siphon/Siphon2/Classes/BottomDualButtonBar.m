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

#import "BottomDualButtonBar.h"


@implementation BottomDualButtonBar

@synthesize button2;

#define kSingleButtonWidth	284.0
#define kStdButtonWidth			132.0
#define kStdButtonHeight		48.0

- (id) initForIncomingCallWaiting
{
  //self = [super initForEndCall];
  self = [super initForIncomingCallWaiting];
  if (self)
  {
    //[[super button] setTitle:NSLocalizedString(@"Decline", @"PhoneView")
    //                forState:UIControlStateNormal];
    [super setSmallTitle:NSLocalizedString(@"Decline", @"PhoneView")];
    [super setBigTitle:NSLocalizedString(@"Decline", @"PhoneView")];
    [[super button] setTitle:NSLocalizedString(@"Decline", @"PhoneView") 
                    forState:UIControlStateNormal];

    UIImage *buttonBackground = [UIImage imageNamed:@"bottombargreen.png"];
    UIImage *buttonBackgroundPressed = [UIImage imageNamed:@"bottombargreen_pressed.png"];
    UIImage *image = [UIImage imageNamed:@"answer.png"];

    UIButton * answer = [BottomButtonBar createButtonWithTitle: NSLocalizedString(@"Answer", @"PhoneView")
                                                         image: image
                                                         frame: CGRectZero
                                                    background: buttonBackground
                                             backgroundPressed: buttonBackgroundPressed];
    [self setButton2: answer];
  }
  return self;
}

- (void)dealloc
{
  [button2 release];
	[super dealloc];
}

- (void)setButton2:(UIButton *)newButton
{
  [newButton retain];
  [newButton setFrame:CGRectMake(18.0 + kStdButtonWidth + 20.0, 24.0, 
                                 kStdButtonWidth, kStdButtonHeight)];
  
  [button2 removeFromSuperview];
  [button2 release];
  button2 = newButton;
  [self addSubview:button2];
  
  if (newButton == nil)
  {
    if ([self.bigTitle length])
      [button setTitle:self.bigTitle forState:UIControlStateNormal];
    CGRect rect = [button frame];
    rect.size.width = kSingleButtonWidth;
    [button setFrame:rect];
  }
  else
  {
    if ([self.smallTitle length])
      [button setTitle:self.smallTitle forState:UIControlStateNormal];
    CGRect rect = [button frame];
    rect.size.width = kStdButtonWidth;
    [button setFrame:rect];
  }
}

@end
