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
#import "ABRecord.h"
#import "ABRecord+Private.h"
#import "ABSearchElement+Private.h"

@implementation SVRecord

@synthesize recordRef  = recordRef_;
@synthesize property   = property_;
@synthesize identifier = identifier_;

- (void)dealloc
{
	CFRelease(recordRef_);
	[super dealloc];
}

+ (SVSearchElement *)searchElementForProperty:(ABPropertyID)property 
																				label:(NSString *)label 
																					key:(NSString *)key 
																				value:(id)value 
																	 comparison:(SVSearchComparison)comparison
{
	//if (property == nil)
	//	return nil;
	
	SVSearchElement *searchElement = [[SVSearchElement alloc] initWithProperty:property
																																			 label:label
																																				 key:key
																																			 value:value
																																	comparison:comparison];
	
	return [searchElement autorelease]; // CLANG
	
}


@end

@implementation SVRecord (private)

- (void)setProperty:(ABPropertyID)property
{
	property_ = property;
}

- (void)setIdentifier:(ABMultiValueIdentifier)identifier
{
	identifier_ = identifier;
}

- (id) initWithRecord:(ABRecordRef)record
						 property:(ABPropertyID)property
					 identifier:(ABMultiValueIdentifier)identifier
{
	self = [super init];
	if (self)
	{
		recordRef_  = CFRetain(record);
		property_   = property;
		identifier_ = identifier;
	}
	return self;
}

- (id) initWithRecord:(ABRecordRef) record
{
	return [self initWithRecord:record
										 property:-1
									 identifier:kABMultiValueInvalidIdentifier];
}

@end
