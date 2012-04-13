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
 *
 * Inspired by MobileScrobbler
 */

#import "SiphonSettings.h"

@implementation CodecSettings

- (id) specifiers 
{
	if (!_specifiers)
  {
    _specifiers = [[self loadSpecifiersFromPlistName:@"Codec"  target: self] retain];
	}
  
	return _specifiers;
}
@end

@implementation AdvancedSettings

- (id) specifiers 
{
	if (!_specifiers)
  {
    _specifiers = [[self loadSpecifiersFromPlistName:@"Advanced"  target: self] retain];
	}
  
	return _specifiers;
}
@end

@implementation PhoneSettings
- (id) specifiers 
{
	if (!_specifiers)
  {
    _specifiers = [[self loadSpecifiersFromPlistName:@"Phone"  target: self] retain];
	}
  
	return _specifiers;
}

-(void)setCellularButton:(id)value specifier:(id)specifier
{
  [self setPreferenceValue:value specifier:specifier];
	[[NSUserDefaults standardUserDefaults] synchronize];

  unlink("/Applications/Siphon.app/Default.png");
	if(value == kCFBooleanTrue)
  {
    symlink("/Applications/Siphon.app/default-iphone.png", 
            "/Applications/Siphon.app/Default.png");
	} 
  else 
  {
    symlink("/Applications/Siphon.app/default-ipod.png", 
            "/Applications/Siphon.app/Default.png");

	}
}

@end

@implementation NetworkSettings
- (id) specifiers 
{
	if (!_specifiers)
  {
    _specifiers = [[self loadSpecifiersFromPlistName:@"Network"  target: self] retain];
	}
  
	return _specifiers;
}
@end

@implementation SiphonSettings
- (id) specifiers 
{
	if (!_specifiers)
  {
    _specifiers = [[self loadSpecifiersFromPlistName:@"Siphon"  target: self] retain];
	}
  
	return _specifiers;
}

-(void)donate:(id)param 
{
	/*Add code to be executed here.  Anything goes, so don’t feel limited by simply being in Settings */
  NSURL *url = [NSURL URLWithString:@"https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=samuelv0304%40gmail%2ecom&item_name=Siphon&no_shipping=0&no_note=1&tax=0&currency_code=EUR&lc=EN&bn=PP%2dDonationsBF&charset=UTF%2d8"];
  [[UIApplication sharedApplication] openURL:url];
}
@end


