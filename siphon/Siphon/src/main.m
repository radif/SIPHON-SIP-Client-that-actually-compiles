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

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "Siphon.h"

static
void insertPrefBundle(NSString *settingsFile) 
{
  int i;
  NSMutableDictionary *settings = 
    [NSMutableDictionary dictionaryWithContentsOfFile: settingsFile];
  for(i = 0; i < [[settings objectForKey:@"items"] count]; i++) 
  {
    NSDictionary *entry = [[settings objectForKey:@"items"] objectAtIndex: i];
    if([[entry objectForKey:@"bundle"] isEqualToString:@"SiphonSettings"]) 
    {
      return;
    }
  }
  [[settings objectForKey:@"items"] insertObject:
   [NSDictionary dictionaryWithObjectsAndKeys:
    @"PSLinkCell", @"cell",
    @"SiphonSettings", @"bundle",
    @"Siphon", @"label",
    [NSNumber numberWithInt: 1], @"isController",
    [NSNumber numberWithInt: 1], @"hasIcon",
    nil] atIndex: [[settings objectForKey:@"items"] count] - 1];
  [settings writeToFile:settingsFile atomically:YES];
}

static
void removePrefBundle(NSString *settingsFile) 
{
  int i;
  NSMutableDictionary *settings = 
    [NSMutableDictionary dictionaryWithContentsOfFile: settingsFile];
    
  for(i = 0; i < [[settings objectForKey:@"items"] count]; i++) 
  {
    NSDictionary *entry = [[settings objectForKey:@"items"] objectAtIndex: i];
    if([[entry objectForKey:@"bundle"] isEqualToString:@"SiphonSettings"]) 
    {
      [[settings objectForKey:@"items"] removeObjectAtIndex: i];
    }
  }
  [settings writeToFile:settingsFile atomically:YES];
}
  


int main(int argc, char **argv)
{
  int returnCode = 0;
  
  NSAutoreleasePool *autoreleasePool = [[ NSAutoreleasePool alloc ] init];

  if(argc > 1 && !strcmp(argv[1],"--installPrefBundle")) 
  {
    insertPrefBundle(@"/Applications/Preferences.app/Settings-iPhone.plist");
    insertPrefBundle(@"/Applications/Preferences.app/Settings-iPod.plist");
    return 0;
  } 
  else if(argc > 1 && !strcmp(argv[1],"--removePrefBundle")) 
  {
    removePrefBundle(@"/Applications/Preferences.app/Settings-iPhone.plist");
    removePrefBundle(@"/Applications/Preferences.app/Settings-iPod.plist");
    return 0;
  }

  returnCode = UIApplicationMain( argc, argv, [ Siphon class ] );
  [ autoreleasePool release ];
  return returnCode;
}

