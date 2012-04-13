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

#import "BottomDualButtonBar.h"
#import "BottomButtonBar.h"
#import "BottomBar.h"


@implementation BottomDualButtonBar

@synthesize button = _button;
@synthesize button2 = _button2;

- (id)initWithDefaultSize
{
  CGRect rect = CGRectMake(DEFAULT_POSX, DEFAULT_POSY, 
                           DEFAULT_WIDTH, DEFAULT_HEIGHT);
  self = [super initWithFrame:rect];
  if (self)
  {
		self.opaque = NO;
    _background = nil;
    //[self backgroundImage];
  }
  return self;
}

- (id) initForEndCall
{
  self = [self initWithDefaultSize];
  if (self)
  {
    UIImage *buttonBackground = [UIImage imageNamed:@"bottombarred"];
    UIImage *buttonBackgroundPressed = [UIImage imageNamed:@"bottombarred_pressed"];
    UIImage *image = [UIImage imageNamed:@"decline"];
    
    UIButton *endCall = [BottomButtonBar createButtonWithTitle: NSLocalizedString(@"End", @"PhoneView")
                                                         image: image
                                                         frame: CGRectZero
                                                    background: buttonBackground
                                             backgroundPressed: buttonBackgroundPressed];
    [self setButton:endCall];
  }
  return self;
}

- (id) initForIncomingCallWaiting
{
  self = [self initWithDefaultSize];
  if (self)
  {
    UIImage *buttonBackground = [UIImage imageNamed:@"bottombarred"];
    UIImage *buttonBackgroundPressed = [UIImage imageNamed:@"bottombarred_pressed"];
    UIImage *image = [UIImage imageNamed:@"decline"];
    
    UIButton *decline = [BottomButtonBar createButtonWithTitle: NSLocalizedString(@"Decline", @"PhoneView")
                                                         image: image
                                                         frame: CGRectZero
                                                    background: buttonBackground
                                             backgroundPressed: buttonBackgroundPressed];
    [self setButton:decline];
    
    buttonBackground = [UIImage imageNamed:@"bottombargreen"];
    buttonBackgroundPressed = [UIImage imageNamed:@"bottombargreen_pressed"];
    image = [UIImage imageNamed:@"answer"];
    
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
  [_button  release];
  [_button2 release];
	[super dealloc];
}

- (void)setButton:(UIButton *)newButton
{
  if (newButton == _button)
    return;
  
  [newButton retain];
  CGRect aRect = CGRectMake(kStdButtonPosX, kStdButtonPosY,
                            kDoubleButtonWidth, kStdButtonHeight);
  [newButton setFrame:aRect];
  
  [_button removeFromSuperview];
  [_button release];
  _button = newButton;
  [self addSubview:_button];
}

- (void)setButton2:(UIButton *)newButton
{
  if (newButton == _button2)
    return;
  
  [newButton retain];
  CGRect aRect = CGRectMake(kStdButtonPosX + kDoubleButtonWidth + 20.0, 
                            kStdButtonPosY,
                            kDoubleButtonWidth, kStdButtonHeight);
  [newButton setFrame:aRect];
  
  [_button2 removeFromSuperview];
  [_button2 release];
  _button2 = newButton;
  [self addSubview:_button2];
}

- (void)backgroundImage
{
  if (_background == nil)
  {
    UIImage *bg = [BottomButtonBar backgroundImage];
    NSInteger width = bg.size.width;
    bg = [bg stretchableImageWithLeftCapWidth:width/2 topCapHeight:0.0];

    CGRect buttonRect = CGRectMake(0.0, 0.0, 160.0 + 12.0, bg.size.height);
    //buttonRect.size.width = rect.size.width / 2 + 12.0;
		
    // Create a graphics context with the target size
    // On iOS 4 and later, use UIGraphicsBeginImageContextWithOptions to take 
		// the scale into consideration
    // On iOS prior to 4, fall back to use UIGraphicsBeginImageContext
		if (NULL != UIGraphicsBeginImageContextWithOptions)
			//UIGraphicsBeginImageContextWithOptions(buttonRect.size, NO, 0.);
			UIGraphicsBeginImageContextWithOptions(buttonRect.size, NO, 1.);
    else
			UIGraphicsBeginImageContext(buttonRect.size);
		
		CGContextRef context = UIGraphicsGetCurrentContext();
		
		// Flip the context because UIKit coordinate system is upside down to 
		// Quartz coordinate system
		CGContextTranslateCTM(context, 0.0, buttonRect.size.height);
		CGContextScaleCTM(context, 1.0, -1.0);
		
		// Draw the original image to the context
    [bg drawInRect:buttonRect];

    _background = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
  }
}

- (void)drawRect:(CGRect)rect 
{
  CGImageRef cgImg;

  [self backgroundImage];
  
  CGRect buttonRect = rect;
  buttonRect.size.width = rect.size.width / 2;
  
  CGContextRef context = UIGraphicsGetCurrentContext();

  cgImg = CGImageCreateWithImageInRect([_background CGImage], buttonRect);
  CGContextDrawImage(context, buttonRect,cgImg);
  CGImageRelease (cgImg);
  
  buttonRect.origin.x = 12.0;
  cgImg = CGImageCreateWithImageInRect([_background CGImage], buttonRect);
  buttonRect.origin.x = buttonRect.size.width;
  CGContextDrawImage(context, buttonRect,cgImg);
  CGImageRelease (cgImg);
}

@end
