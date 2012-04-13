/**
 *  AKChain.m
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

#import "AKKeychain.h"

@implementation AKKeychain

+ (NSString *)passwordForServiceName:(NSString *)serviceName
                         accountName:(NSString *)accountName 
{
	NSData *passwordData = NULL;
	OSStatus findStatus;
	NSDictionary *query = [NSDictionary dictionaryWithObjectsAndKeys:
												 (id)kSecClassGenericPassword, (id)kSecClass, 
												 [NSNumber numberWithUnsignedInt:'voip'], (id)kSecAttrType, 
												 serviceName, (id)kSecAttrService, 
												 accountName, (id)kSecAttrAccount, 
												 // Use the proper search constants, 
												 // return only the attributes of the first match.
												 (id)kSecMatchLimitOne, (id)kSecMatchLimit, 
												 (id)kCFBooleanTrue, (id)kSecReturnData,
												 nil];
	
	// Acquire the password data from the attributes.
	findStatus = SecItemCopyMatching ((CFDictionaryRef)query,
																		(CFTypeRef *)&passwordData);
	if (findStatus != errSecSuccess)
		return nil;
	
	// Convert password from NSData to NSString.
	NSString *password = [[[NSString alloc] initWithBytes:[passwordData bytes] 
																								 length:[passwordData length] 
																							 encoding:NSUTF8StringEncoding] autorelease];
	
	[passwordData release];
	
	return password;
}

+ (BOOL)addItemWithServiceName:(NSString *)serviceName
                   accountName:(NSString *)accountName
                      password:(NSString *)password 
{
	NSDictionary *attributes;
	OSStatus addStatus;
  BOOL success = NO;

	attributes = [NSDictionary dictionaryWithObjectsAndKeys:
								(id)kSecClassGenericPassword, (id)kSecClass,
								[NSNumber numberWithUnsignedInt:'voip'], (id)kSecAttrType, 
								serviceName, (id)kSecAttrService, 	
								accountName, (id)kSecAttrAccount,
								[password dataUsingEncoding:NSUTF8StringEncoding], (id)kSecValueData,
								nil];
	
	// Add item to keychain.
	addStatus = SecItemAdd ((CFDictionaryRef)attributes, NULL);
	if (addStatus == errSecSuccess) 
    success = YES;
  else if (addStatus == errSecDuplicateItem) 
	{
		[AKKeychain removeItemWithServiceName:serviceName
															accountName:accountName];
		addStatus = SecItemAdd ((CFDictionaryRef)attributes, NULL);
		if (addStatus == errSecSuccess) 
			success = YES;
	}
	return success;
}

+ (BOOL)removeItemWithServiceName:(NSString *)serviceName
											accountName:(NSString *)accountName
{
	NSDictionary *query;
	OSStatus deleteStatus;
	BOOL success = NO;

	query = [NSDictionary dictionaryWithObjectsAndKeys:
					 (id)kSecClassGenericPassword, (id)kSecClass,
					 [NSNumber numberWithUnsignedInt:'voip'], (id)kSecAttrType, 
					 serviceName, (id)kSecAttrService, 
					 accountName, (id)kSecAttrAccount,
					 nil];

	deleteStatus = SecItemDelete((CFDictionaryRef)query);
	if (deleteStatus == errSecSuccess)
		success = YES;
	
	return success;
}

@end
