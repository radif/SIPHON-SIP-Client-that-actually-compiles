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

#import "BottomButtonBar.h"


@implementation BottomButtonBar

@synthesize button;
@synthesize smallTitle, bigTitle;



+ (UIButton *)createButtonWithTitle:(NSString *)title
                              image:(UIImage *)image
                              frame:(CGRect)frame
                         background:(UIImage *)backgroundImage
                  backgroundPressed:(UIImage *)backgroundImagePressed
{	
	UIButton *button = [[UIButton alloc] initWithFrame:frame];
	
	button.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
	button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
	
	[button setTitle:title forState:UIControlStateNormal];
#ifdef __IPHONE_3_0
  button.titleLabel.font = [UIFont boldSystemFontOfSize:[UIFont buttonFontSize]]; 
#else
  button.font = [UIFont boldSystemFontOfSize:[UIFont buttonFontSize]]; 
#endif
  [button setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
  //button.titleEdgeInsets = UIEdgeInsetsMake (0.0f, 2.0f, 0.0f, 0.0f);
  //button.font = [UIFont systemFontOfSize: 30.0];
  if (image)
  {
    [button setImage:image forState:UIControlStateNormal];
    button.imageEdgeInsets = UIEdgeInsetsMake (0., 0., 0., 5.);
  }
	//if (darkTextColor)
//	{
//		[button setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
//	}
//	else
//	{
//		[button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
//	}
	
	UIImage *newImage = [backgroundImage stretchableImageWithLeftCapWidth:12.0 topCapHeight:0.0];
	[button setBackgroundImage:newImage forState:UIControlStateNormal];
	
	UIImage *newPressedImage = [backgroundImagePressed stretchableImageWithLeftCapWidth:12.0 topCapHeight:0.0];
	[button setBackgroundImage:newPressedImage forState:UIControlStateHighlighted];

  // in case the parent view draws with a custom color or gradient, use a transparent color
	button.backgroundColor = [UIColor clearColor];
  
	return button;
}

#define kStdButtonWidth			284.0
#define kStdButtonHeight		48.0    

- (id) initForEndCall
{
  self = [super initWithDefaultSize];
  if (self)
  {
    UIImage *buttonBackground = [UIImage imageNamed:@"bottombarred.png"];
    UIImage *buttonBackgroundPressed = [UIImage imageNamed:@"bottombarred_pressed.png"];
    UIImage *image = [UIImage imageNamed:@"decline.png"];
    [self setSmallTitle:NSLocalizedString(@"Small end call", @"PhoneView")];
    [self setBigTitle:NSLocalizedString(@"End call", @"PhoneView")];
    UIButton *endCall = [BottomButtonBar createButtonWithTitle: NSLocalizedString(@"End call", @"PhoneView")
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
  self = [super initWithDefaultSize];
  if (self)
  {
    UIImage *buttonBackground = [UIImage imageNamed:@"bottombarred.png"];
    UIImage *buttonBackgroundPressed = [UIImage imageNamed:@"bottombarred_pressed.png"];
    UIImage *image = [UIImage imageNamed:@"decline.png"];
    //[self setSmallTitle:NSLocalizedString(@"Decline", @"PhoneView")];
    //[self setBigTitle:NSLocalizedString(@"Decline", @"PhoneView")];
    //UIButton *declineCall = [BottomButtonBar createButtonWithTitle: NSLocalizedString(@"Decline", @"PhoneView")
    UIButton *declineCall = [BottomButtonBar createButtonWithTitle: NSLocalizedString(@"End Call + Answer", @"PhoneView")
                                                         image: image
                                                         frame: CGRectZero
                                                    background: buttonBackground
                                             backgroundPressed: buttonBackgroundPressed];
    [self setButton:declineCall];
  }
  return self;
}

/*- (void)drawRect:(CGRect)rect 
{
	// Drawing code
}*/


- (void)dealloc 
{
  [button release];
	[super dealloc];
}

- (void)setButton:(UIButton *)newButton
{
  [newButton retain];
  [newButton setFrame:CGRectMake(18.0, 24.0, kStdButtonWidth, kStdButtonHeight)];
  
  [button removeFromSuperview];
  [button release];
  button = newButton;
  [self addSubview:button];
}

@end
