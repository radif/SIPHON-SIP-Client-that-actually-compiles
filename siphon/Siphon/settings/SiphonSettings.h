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

#ifndef __SIPHON_SETTINGS_H__
#define __SIPHON_SETTINGS_H__

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UIKit/UIProgressHUD.h>
#import <Preferences/PSListController.h>
#import <Preferences/PSListItemsController.h>

@interface LocalizedListController : PSListController 
{
}
-(NSArray *)localizedSpecifiersForSpecifiers:(NSArray *)s;
-(id)navigationTitle;
@end

@interface LocalizedItemsController : PSListItemsController 
{
}
-(NSArray *)specifiers;
@end

@interface AdvancedSettings : LocalizedListController 
{
}
-(NSArray *)specifiers;
@end

@interface SiphonSettings : LocalizedListController 
{
}
-(NSArray *)specifiers;
@end

#endif /* __SIPHON_SETTINGS_H__ */
