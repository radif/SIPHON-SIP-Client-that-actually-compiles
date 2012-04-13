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

#import "AboutView.h"

@implementation AboutView

- (id)initWithFrame:(CGRect)rect
{
  if ((self == [ super initWithFrame: rect ]) != nil)
  {
    FILE *file;
    char buffer[262144], buf[1024];

    _textView = [ [ UITextView alloc ] initWithFrame: rect ];
    [ _textView setTextSize: 12 ];
    [ _textView setEditable: NO ];



//    NSLog(@"language %@", [[NSUserDefaults standardUserDefaults] objectForKey: @"AppleLanguages"]);
//    NSLog(@"resource %@", [[NSBundle mainBundle] resourcePath]);
//    NSLog(@"bundle %@", [[NSBundle mainBundle] bundlePath]);
    NSString *path = [[NSString alloc] initWithString : 
      [[NSBundle mainBundle] bundlePath]];
    path = [path stringByAppendingString:@"/index.html"];
    NSLog(@"path %@", path);
    
    //file = fopen("/Applications/Preferences.app/English.lproj/legal-disclaimer-iphone.html", "r");
    file = fopen([path UTF8String], "r");

    if (!file)
    {
      CGColorSpaceRef colorSpace =
      CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
      float opaqueRed[4] =
      { 1.0, 0.0, 0.0, 1};
      CGColorRef red = CGColorCreate(colorSpace, opaqueRed);
      [ _textView setTextColor: red ];

      [ _textView setText: @"ERROR: File not found" ];
    }
    else
    {
      buffer[0] = 0;
      while((fgets(buf, sizeof(buf), file))!=NULL)
      {
        strlcat(buffer, buf, sizeof(buffer));
      }
      fclose(file);

      [ _textView setHTML:
      [ [ NSString alloc ] initWithCString: buffer ]];
    }

    [ self addSubview: _textView ];
  }

  return self;
}

- (void)dealloc
{
  [ self dealloc ];
  [ super dealloc ];
}

@end
