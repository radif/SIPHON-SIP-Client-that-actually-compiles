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

#import "DialerPhonePad.h"
#import <UIKit/UIImage.h>


@implementation DialerPhonePad : TPPhonePad

UIImage *_dialer = nil;
UIImage *_dialerPressed = nil;

- (id)_keypadImage
{
  if (_dialer == nil)
  {
     _dialer = [[UIImage applicationImageNamed: @"dialerkeypad.png"] retain];
  }

  return _dialer;
}

- (float)_yFudge
{
    return 0.0;
}

- (id)_pressedImage
{  
  if (_dialerPressed == nil)
  {
    _dialerPressed = [[UIImage applicationImageNamed: 
      @"dialerkeypad_pressed.png"] retain];
  }
  return _dialerPressed;
}

- (id)initWithFrame:(struct CGRect)rect
{
    if ((self = [super initWithFrame:rect]) != nil)
    {
      [self setOpaque: TRUE];
      _topHeight = 69.0;
      _midHeight = 68.0;
      _bottomHeight = 68.0;
      _leftWidth = 107.0;
      _midWidth = 105.0;
      _rightWidth = 108.0;
    }
    return self;
}
 
@end
