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

#import "ABTypedefs.h"

@class SVRecord;

@interface SVSearchElement : NSObject 
{
@private
	ABPropertyID property_;
	NSString *label_;
	NSString *key_;
  id value_;
  SVSearchComparison comparison_;
}

/**
 * Creates a search element combining several sub search elements.
 * conjunction can be kABSearchAnd or kABSearchOr.
 * Raises if children is nil or empty
 */
+ (SVSearchElement *)searchElementForConjunction:(SVSearchConjunction)conjuction
																				children:(NSArray *)children;


/**
 * Given a record returns YES if this record matches the search element
 * Raises if record is nil
 */
//- (BOOL)matchesRecord:(ABRecordRef)record;
- (BOOL)matchesRecord:(SVRecord *)record;

@end
