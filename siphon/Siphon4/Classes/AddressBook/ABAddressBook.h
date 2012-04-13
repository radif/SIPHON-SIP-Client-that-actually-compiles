/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
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
#import <AddressBook/AddressBook.h>

@class SVSearchElement;

@interface SVAddressBook : NSObject 
{
@private
	ABAddressBookRef addressBook_; 
}

@property (nonatomic, readonly) ABAddressBookRef addressBook;

/**
 * Returns the unique shared instance of ABAddressBook
 * Returns nil if the address book database cannot be initialized
 */
+ (SVAddressBook *)sharedAddressBook;

/**
 * Destroy the shared instance and return the swizzled methods to their former 
 * selves.
 */
+ (void) purgeSharedAddressBook;

/**
 * Returns a new instance of ABAddressBook
 */
//+ (SVAddressBook *)addressBook;

/**
 * Returns an array of records matching the given search element
 * Raises if search is nil
 * Returns an empty array if no matches
 */
- (NSArray *)recordsMatchingSearchElement:(SVSearchElement *)search;

/**
 * Saves changes made since the last save
 * Return YES if successful (or there was no change)
 */
- (BOOL)save;



@end
