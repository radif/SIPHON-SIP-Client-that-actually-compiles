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

#import "PushButton.h"


@implementation PushButton

- (CGRect)imageRectForContentRect:(CGRect)contentRect
{
  return _contentRect;
}

- (CGRect)titleRectForContentRect:(CGRect)contentRect
{
	CGRect rect = _contentRect;
	
	rect.origin.x -= 9.0;
	rect.size.width += 18.0;
	
	rect.origin.y = rect.size.height;
	rect.size.height = [UIFont buttonFontSize];
	
	return rect;
}

- (CGRect)contentRectForBounds:(CGRect)bounds
{
  return _contentRect;
}

- (void)dealloc 
{
  //[_contentRect release];
  [super dealloc];
}

- (void)setContentRect:(CGRect)rect
{
  //[_contentRect release];
  _contentRect = rect;
}

@end
