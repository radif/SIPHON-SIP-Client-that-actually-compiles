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

#import "ABTypeDefs.h"

@class SVSearchElement;

@interface SVRecord : NSObject 
{
@private
	ABRecordRef  recordRef_;
	ABPropertyID property_;
	ABMultiValueIdentifier identifier_;
}

@property(nonatomic, readonly) ABRecordRef recordRef;
@property(nonatomic, readonly) ABPropertyID property;
@property(nonatomic, readonly) ABMultiValueIdentifier identifier;

/**
 * Returns a search element that will search people
 *					property: the name of the property to search on (cannot be nil)
 *          label: for multi-value properties an optional label (can be nil)
 *          key: for dictionary values an optional key (can be nil)
 *          value: value to match (can be nil)
 *          comparison: the type of search (see ABTypedefs.h)
 */
+ (SVSearchElement *)searchElementForProperty:(ABPropertyID)property 
																				label:(NSString *)label 
																					key:(NSString *)key 
																				value:(id)value 
																	 comparison:(SVSearchComparison)comparison;

@end

#define ABGroup  SVRecord
#define ABPerson SVRecord

