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

#import "LCDView.h"

@implementation LCDView

//@synthesize image;

+ (UILabel *)createLabel:(CGRect)rect size:(CGFloat)fontSize
{
  UILabel *label;
  
  label = [[UILabel alloc] initWithFrame:rect];
  label.backgroundColor = [UIColor clearColor];
  label.adjustsFontSizeToFitWidth = YES;
  label.minimumFontSize = 15;
  label.lineBreakMode = UILineBreakModeHeadTruncation;
  label.font = [UIFont fontWithName:@"Helvetica" size:fontSize];
  label.textAlignment = UITextAlignmentCenter;
  label.textColor = [UIColor whiteColor];
  
  return label;
}

- (id)initWithFrame:(CGRect)frame
{
  self = [super initWithFrame:frame];
	if (self)
  {
		// Initialization code
    //self.backgroundColor = [[UIColor alloc] initWithWhite:0.0f alpha:0.5f];
    //UIImage *background = [UIImage imageNamed:@"lcd_call_top.png"];
    //self.backgroundColor = [[UIColor colorWithPatternImage:background] 
    //                        colorWithAlphaComponent:0.5f];
    UIImage *background = [UIImage imageNamed:@"lcd_call_bottom.png"];
    self.backgroundColor = [UIColor colorWithPatternImage: background];
    self.alpha = 0.7f;
    //self.backgroundColor = [[UIColor colorWithPatternImage:background] 
    //                         colorWithAlphaComponent:0.5f];
    // FIXME: dimension !!!
    CGRect rect = frame;
    //rect.size.height = CGRectGetHeight(frame) / 2;
    rect.size.height = CGRectGetHeight(frame) - 30;
    _text = [LCDView createLabel:rect size:32];
    rect.origin.y = CGRectGetHeight(frame) / 2;
    rect.size.height = CGRectGetHeight(frame) / 2;
    _label = [LCDView createLabel:rect size:20];

    _image = [[UIImageView alloc] initWithFrame:CGRectMake(frame.size.width - frame.size.height, 
                                                          5.0f, 
                                                          frame.size.height - 10.0f, 
                                                          frame.size.height - 10.0f)];
    
    [self addSubview:_image];
    [self addSubview:_label];
    [self addSubview:_text];
  }
	return self;
}

- (id) initWithDefaultSize
{
  CGRect rect = CGRectMake(0.0f, 0.0f, 320.0f, 96.0f);
  return [self initWithFrame: rect];
}

/*- (void)drawRect:(CGRect)rect {
	// Drawing code
}*/


- (void)dealloc 
{
  [_label release];
  [_text release];
  [_image release];
	[super dealloc];
}

- (void) setLabel: (NSString *)label
{
  _label.text = label;
}
- (void) setText: (NSString *)text
{
  _text.text = text;
}

- (void) setSubImage: (UIImage *)image
{
  // TODO Resize text and label if image is defined
  [_image.image release];
  _image.image = image;
  [_image.image retain];
  if (image == nil)
  {
    _label.textAlignment = UITextAlignmentCenter;
    _text.textAlignment = UITextAlignmentCenter;
  }
  else
  {
    _label.textAlignment =  UITextAlignmentLeft;
    _text.textAlignment =  UITextAlignmentLeft;
  }
}

@end
