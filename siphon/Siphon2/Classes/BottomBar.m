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

#import "BottomBar.h"

#define DEFAULT_POSX 0.0f
#define DEFAULT_POSY (460.0f - DEFAULT_HEIGHT)
#define DEFAULT_HEIGHT 96.0f
#define DEFAULT_WIDTH 320.0f

@implementation BottomBar

- (id)initWithDefaultSize
{
  CGRect rect = CGRectMake(DEFAULT_POSX, DEFAULT_POSY, 
                           DEFAULT_WIDTH, DEFAULT_HEIGHT);
  return [self initWithFrame:rect];
}

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame])
    {
        // Initialization code
      UIImage *background = [UIImage imageNamed:@"lcd_call_bottom.png"];
      self.backgroundColor = [UIColor colorWithPatternImage: background];
      self.alpha = 0.7f;
    }
    return self;
}


/*- (void)drawRect:(CGRect)rect {
    // Drawing code
}*/


- (void)dealloc 
{
    [super dealloc];
}


@end
