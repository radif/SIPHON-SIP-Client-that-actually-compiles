/**
 *  AKChain.h
 *  Siphon
 *  Copyright (C) 2011 Samuel <samuelv0304@gmail.com>
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


// A Keychain Services wrapper.
@interface AKKeychain : NSObject {
  
}

// Returns password for the first Keychain item with a specified service name
// and account name.
+ (NSString *)passwordForServiceName:(NSString *)serviceName
                         accountName:(NSString *)accountName;

// Adds an item to the Keychain with a specified service name, account name,
// and password. If the same item already exists, its password will be replaced
// with the new one.
+ (BOOL)addItemWithServiceName:(NSString *)serviceName
                   accountName:(NSString *)accountName
                      password:(NSString *)password;


// Removes the first Keychain item with a specified service name
// and account name. 
+ (BOOL)removeItemWithServiceName:(NSString *)serviceName
											accountName:(NSString *)accountName;

@end
