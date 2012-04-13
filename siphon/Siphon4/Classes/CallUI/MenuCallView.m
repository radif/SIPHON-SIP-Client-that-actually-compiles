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

#import "MenuCallView.h"

#define LONG_TAP_TIME 0.8

@implementation MenuCallView

@synthesize delegate;

- (void)preloadButtons
{
  int i;
  CGRect rect = {0.0f, 0.0f, 0.0f, 0.0f};
  NSString *bg, *bgSel;
  UIImage *image, *selectedImage;
  
  for (i = 0; i < 6; ++i)
  {
    bg    = [NSString stringWithFormat:@"sixsqbutton_%d", i+1];
    bgSel = [NSString stringWithFormat:@"sixsqbuttonsel_%d", i+1];
    image = [UIImage imageNamed:bg];
    selectedImage = [UIImage imageNamed:bgSel];
    
    rect.size = [image size];
    PushButton *button = [[PushButton alloc] initWithFrame:rect];
    
    button.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
    
    [button setBackgroundImage:image forState:UIControlStateNormal];
    [button setBackgroundImage:image forState:UIControlStateDisabled];
    [button setBackgroundImage:selectedImage forState:UIControlStateHighlighted];
    [button setBackgroundImage:selectedImage forState:UIControlStateSelected];
    
    // in case the parent view draws with a custom color or gradient, use a transparent color
    //button.backgroundColor = [UIColor clearColor];
    
    [button setTag:i];

		[button addTarget:self action:@selector(buttonDown:) forControlEvents:UIControlEventTouchDown];
		[button addTarget:self action:@selector(buttonDidReleaseInside:)  forControlEvents:UIControlEventTouchUpInside];
		[button addTarget:self action:@selector(buttonDidReleaseOutside:) forControlEvents:UIControlEventTouchUpOutside];
    
    CGRect content = CGRectMake(11.0, 11.0, 72.0, 75.0);
    if (i == 0 || i == 3)
      content.origin.x += 5.;
    if (i < 3)
      content.origin.y += 2.;
    [button setContentRect: content];
    
    _buttons[i] = button;
    [self addSubview:_buttons[i]];

    if (i == 2)
    {
      //rect.origin.y += rect.size.height - 1.0f;
      rect.origin.y += rect.size.height - 9.0f;
      rect.origin.x = 0.0f;
    }
    else
      rect.origin.x += rect.size.width;
  }
}

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
    {
        // Initialization code
      // TODO display text maybe I need to derivate UIButton
      [self preloadButtons];
    }
    return self;
}

- (void)detecetedLongTap:(UIButton *)button
{
	consumedTap_ = YES;
	if ([delegate respondsToSelector:@selector(menuButtonHeldDown:)])
  {
    [delegate menuButtonHeldDown:[button tag]];
  }
	
}

- (void)buttonDown:(UIButton *)button
{
	consumedTap_ = NO;
	[self performSelector:@selector(detecetedLongTap:) withObject:button afterDelay:LONG_TAP_TIME];
}

- (void)buttonDidReleaseInside:(UIButton *)button
{
	[NSObject cancelPreviousPerformRequestsWithTarget:self 
																					 selector:@selector(detecetedLongTap:)
																						 object:button];

	if (!consumedTap_ && [delegate respondsToSelector:@selector(menuButtonClicked:)])
  {
    [delegate menuButtonClicked:[button tag]];
  }
}

- (void)buttonDidReleaseOutside:(UIButton *)button
{
	[NSObject cancelPreviousPerformRequestsWithTarget:self 
																					 selector:@selector(detecetedLongTap:)
																						 object:button];
}

- (void)dealloc 
{
  int i;
  for (i = 0; i < 6; ++i)
    [_buttons[i] release];
  [super dealloc];
}

- (PushButton *)buttonAtPosition:(NSInteger)pos
{
  if (pos < 0 || pos > 5)
    return nil;
  return _buttons[pos];
}

- (void)setTitle:(NSString *)title image:(UIImage *)image forPosition:(NSInteger)pos
{
  if (pos < 0 || pos > 5)
    return;
  if (image)
  {
    [_buttons[pos] setImage:image forState:UIControlStateNormal];
    [_buttons[pos] setImage:image forState:UIControlStateSelected];
  }
  if (title)
  {
#ifdef __IPHONE_3_0
    _buttons[pos].titleLabel.font = [UIFont systemFontOfSize:[UIFont buttonFontSize] - 5.];
	_buttons[pos].titleLabel.clipsToBounds = NO;
#else
    [_buttons[pos] setFont:[UIFont systemFontOfSize:[UIFont buttonFontSize] - 5.]];
#endif
    [_buttons[pos] setTitle:title forState:UIControlStateNormal];
		_buttons[pos].titleLabel.textAlignment = UITextAlignmentCenter;
  }
}

@end
