/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2009-2011 Samuel <samuelv0304@gmail.com>
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

#import "DualButtonView.h"
#import "BottomButtonBar.h"

#define kStdButtonWidth			241.0
#define kStdButtonHeight		48.0  

@implementation DualButtonView

- (void)preloadButtons
{
  CGRect rect = CGRectMake(22.0f,  33.0f, kStdButtonWidth, kStdButtonHeight);
  UIImage *buttonBackground = [UIImage imageNamed:@"bottombardarkgray"];
  UIImage *buttonBackgroundPressed = [UIImage imageNamed:@"bottombardarkgray_pressed"];
  
  UIButton *button = [BottomButtonBar createButtonWithTitle:nil
                                                      image:nil
                                                      frame:rect
                                                 background:buttonBackground
                                          backgroundPressed:buttonBackgroundPressed];
  button.opaque = YES;
  [button setTag:0];
  //[button addTarget:self action:@selector(clicked:) forControlEvents:UIControlEventTouchUpInside];
  _buttons[0] = [button retain]; // CLANG
  [self addSubview:_buttons[0]];
  
  rect = CGRectMake(22.0f,  137.0f, kStdButtonWidth, kStdButtonHeight);
  button = [BottomButtonBar createButtonWithTitle:nil
                                            image:nil
                                            frame:rect
                                       background:buttonBackground
                                backgroundPressed:buttonBackgroundPressed];
  [button setTag:1];
  //[button addTarget:self action:@selector(clicked:) forControlEvents:UIControlEventTouchUpInside];
  _buttons[1] = [button retain]; // CLANG
  [self addSubview:_buttons[1]];
}

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
    {
      UIImage *background = [UIImage imageNamed:@"dualbutton"];
      background = [background stretchableImageWithLeftCapWidth:background.size.width/2 topCapHeight:0.0];
      //UIImageView *imageView = [[UIImageView alloc] initWithImage:background];
      CGRect rect = CGRectMake(0.0, 0.0, frame.size.width, frame.size.height);
      UIImageView *imageView = [[UIImageView alloc] initWithFrame:rect];
      imageView.image = background;
      imageView.contentMode = UIViewContentModeScaleToFill;
      self.backgroundColor = [UIColor clearColor];
      [self addSubview:imageView];
      //[imageView sizeToFit];
      [imageView release];
      //self.backgroundColor = [UIColor colorWithPatternImage: background];
      self.opaque = NO;
      [self preloadButtons];
    }
    return self;
}


/*- (void)drawRect:(CGRect)rect {
    // Drawing code
}*/

/*- (void)clicked:(UIButton *)button
{
  if ([delegate respondsToSelector:@selector(buttonClicked:)])
  {
    [delegate buttonClicked:[button tag]];
  }
}*/

- (void)dealloc 
{
  [_buttons[0] release];
  [_buttons[1] release];
  [super dealloc];
}

- (UIButton *)buttonAtPosition:(NSInteger)pos
{
  if (pos < 0 || pos > 1)
    return nil;
  return _buttons[pos];
}

- (void)setTitle:(NSString *)title image:(UIImage *)image forPosition:(NSInteger)pos
{
  if (pos < 0 || pos > 1)
    return;
  if (image)
  {
    [_buttons[pos] setImage:image forState:UIControlStateNormal];
    [_buttons[pos] setImage:image forState:UIControlStateSelected];
  }
  if (title)
  {
    //[_buttons[pos] setFont:[UIFont systemFontOfSize:[UIFont buttonFontSize] - 4.]];
    [_buttons[pos] setTitle:title forState:UIControlStateNormal];
  }
}

@end
