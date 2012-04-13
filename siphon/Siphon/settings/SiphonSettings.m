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
 * 
 * Inspired by MobileScrobbler
 */

#import "SiphonSettings.h"

@implementation LocalizedListController
- (NSArray *)localizedSpecifiersForSpecifiers:(NSArray *)s
{
  int i;
  for(i=0; i<[s count]; i++) 
  {
    if([[s objectAtIndex: i] name]) 
    {
      [[s objectAtIndex: i] setName:[[self bundle] localizedStringForKey:[[s objectAtIndex: i] name] value:[[s objectAtIndex: i] name] table:nil]];
    }
    if([[s objectAtIndex: i] titleDictionary]) 
    {
      NSMutableDictionary *newTitles = [[NSMutableDictionary alloc] init];
      for(NSString *key in [[s objectAtIndex: i] titleDictionary]) 
      {
        [newTitles setObject: [[self bundle] localizedStringForKey:[[[s objectAtIndex: i] titleDictionary] objectForKey:key] value:[[[s objectAtIndex: i] titleDictionary] objectForKey:key] table:nil]
                      forKey: key];
      }
      [[s objectAtIndex: i] setTitleDictionary: [newTitles autorelease]];
    }
  }
  
  return s;
};
- (id)navigationTitle 
{
  return [[self bundle] localizedStringForKey:_title value:_title table:nil];
}
@end

@implementation LocalizedItemsController
- (NSArray *)specifiers 
{
  NSArray *s = [self itemsFromParent];
  s = [self localizedSpecifiersForSpecifiers:s];
  return s;
}
@end

@implementation AdvancedSettings
- (NSArray *)specifiers 
{
  NSArray *s = [self loadSpecifiersFromPlistName:@"Advanced" target: self];
  s = [self localizedSpecifiersForSpecifiers:s];
  return s;
}
@end

@implementation SiphonSettings
- (NSArray *)specifiers 
{
  NSArray *s = [self loadSpecifiersFromPlistName:@"Siphon" target: self];
  s = [self localizedSpecifiersForSpecifiers:s];
  return s;
}
@end


