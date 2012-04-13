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

#import "LCDView.h"
#import <QuartzCore/QuartzCore.h>

@implementation LCDView

@synthesize displayedName = _displayedName;
@synthesize status = _status;
@dynamic    image;

- (UIImage *)image
{
	return _imageView.image;
}

- (void) setImage: (UIImage *)image
{
  // TODO Resize text and label if image is defined
	if (image == _imageView.image)
		return ;

  _imageView.image = image;

  if (image == nil)
  {
    _displayedName.textAlignment = UITextAlignmentCenter;
    //_status.textAlignment = UITextAlignmentCenter;
		_imageView.hidden = YES;
		[_imageView removeFromSuperview];
  }
  else
  {
    _displayedName.textAlignment =  UITextAlignmentRight;
    //_status.textAlignment =  UITextAlignmentRight;
		_imageView.hidden = NO;
		CGRect iframe = _imageView.frame;
		CGRect frame = _status.frame;
		frame.origin.x += iframe.size.width;
		frame.size.width -= iframe.size.width;
		_status.frame = frame;
		//[self addSubview:_imageView];
  }
}


#pragma mark -
- (UILabel *)newLabel:(CGRect)rect size:(CGFloat)fontSize // CLANG rename
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
	//label.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleLeftMargin;
  
  return label;
}

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
    {
        // Initialization code
      //self.backgroundColor = [UIColor cyanColor];
      self.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcdbg.png"]];
      self.alpha = 0.7f;
       
      CGRect rect = frame;
			rect.origin.x += 5.0;
			rect.size.width -= 10.0;
      //rect.size.height = CGRectGetHeight(frame) / 2;
      rect.size.height = CGRectGetHeight(frame) - 30;
      _displayedName = [self newLabel:rect size:32];
      rect.origin.y = CGRectGetHeight(frame) / 2;
      rect.size.height = CGRectGetHeight(frame) / 2;
      _status = [self newLabel:rect size:20];
      
      _imageView = [[UIImageView alloc] initWithFrame:CGRectMake(5.0f, 5.0f, 
                                                                 frame.size.height - 10.0f, 
                                                                 frame.size.height - 10.0f)];
      _imageView.hidden = YES;
      _imageView.backgroundColor = [UIColor clearColor];
      _imageView.layer.cornerRadius = 10.0f;
      _imageView.layer.masksToBounds = YES;
      _imageView.layer.borderColor = [[UIColor darkGrayColor] CGColor];
      _imageView.layer.borderWidth = 1;
      //_image.layer.borderWidth = 0;
      
      [self addSubview:_imageView];
      [self addSubview:_displayedName];
      [self addSubview:_status];
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
	[_displayedName release];
	[_status release];
	[_imageView release];
	[super dealloc];
}


@end
