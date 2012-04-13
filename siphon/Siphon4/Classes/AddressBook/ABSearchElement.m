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
#import "ABSearchElement.h"
#import "ABRecord.h"
#import "ABRecord+Private.h"

@interface SVConjunctionSearchElement : SVSearchElement
{
@private
	SVSearchConjunction conjuction_;
	NSArray *children_;
}

- (id)initWithConjunction:(SVSearchConjunction)conjuction
								 children:(NSArray *)children;
- (BOOL)matchesRecord:(SVRecord *)record;

@end


@implementation SVSearchElement

+ (SVSearchElement *)searchElementForConjunction:(SVSearchConjunction)conjuction
																				children:(NSArray *)children
{
	return [[[SVConjunctionSearchElement alloc] initWithConjunction:conjuction
																												children:children] autorelease]; // CLANG
}

- (void)dealloc
{
	[label_ release];
	[key_ release];
	[value_ release];
	[super dealloc];
}

+ (NSString *) normalizePhoneNumber:(NSString *)number 
{
	// Becareful international prefix '+' is removed.
	return [[number componentsSeparatedByCharactersInSet:
					 [[NSCharacterSet decimalDigitCharacterSet] invertedSet]] 
					componentsJoinedByString:@""];
}

#pragma mark -
- (BOOL)matchesValue:(CFTypeRef)value
{
	if (CFGetTypeID(value) == CFStringGetTypeID())
	{
		if (CFGetTypeID(value_) != CFStringGetTypeID())
			return NO; // Can't compare
		
		//value = (CFStringRef)[SVSearchElement normalizePhoneNumber:(NSString *)value];
		
		switch (comparison_)
		{
			case kABEqual:
				return (CFStringCompare(value, (CFStringRef)value_, 0) == kCFCompareEqualTo);
			case kABNotEqual:
				return (CFStringCompare(value, (CFStringRef)value_, 0) != kCFCompareEqualTo);
			case kABLessThan:
				return (CFStringCompare(value, (CFStringRef)value_, 0) ==  kCFCompareLessThan);
			case kABLessThanOrEqual:
				return (CFStringCompare(value, (CFStringRef)value_, 0) <=  kCFCompareEqualTo /*kCFCompareLessThan*/);
			case kABGreaterThan:
				return (CFStringCompare(value, (CFStringRef)value_, 0) ==  kCFCompareGreaterThan);
			case kABGreaterThanOrEqual:
				return (CFStringCompare(value, (CFStringRef)value_, 0) >=  kCFCompareEqualTo /*kCFCompareGreaterThan*/);
				
			case kABEqualCaseInsensitive:
				return (CFStringCompare(value, (CFStringRef)value_, kCFCompareCaseInsensitive) == kCFCompareEqualTo);
			case kABNotEqualCaseInsensitive:
				return (CFStringCompare(value, (CFStringRef)value_, kCFCompareCaseInsensitive) != kCFCompareEqualTo);

			case kABContainsSubString:
			{
				CFRange range = CFStringFind (value, (CFStringRef)value_, 0);
				return range.location != kCFNotFound;
			}
			case kABContainsSubStringCaseInsensitive:
			{
				CFRange range = CFStringFind (value, (CFStringRef)value_, kCFCompareCaseInsensitive);
				return range.location != kCFNotFound;
			}
			case kABDoesNotContainSubString:
			{	
				CFRange range = CFStringFind (value, (CFStringRef)value_, 0);
				return range.location == kCFNotFound;
			}
			case kABDoesNotContainSubStringCaseInsensitive:
			{	
				CFRange range = CFStringFind (value, (CFStringRef)value_, kCFCompareCaseInsensitive);
				return range.location == kCFNotFound;
			}
			case kABPrefixMatch:
				return CFStringHasPrefix(value, (CFStringRef)value_);
			/*{	
				CFRange range = CFStringFind (value, (CFStringRef)value_, 0);
				return range.location == 0;
			}*/
			case kABPrefixMatchCaseInsensitive:
			{
				CFRange range = CFStringFind (value, (CFStringRef)value_, kCFCompareCaseInsensitive);
				return range.location == 0;
			}
			case kABSuffixMatch:
				return CFStringHasSuffix(value, (CFStringRef)value_);
			case kABSuffixMatchCaseInsensitive:
			{
				CFRange range = CFStringFind (value, (CFStringRef)value_, kCFCompareCaseInsensitive);
				return range.location + range.length == CFStringGetLength(value);
			}
			default:
				return NO; // Unknown comparison
		}
	}
	else if (CFGetTypeID(value) == CFDateGetTypeID())
	{
		if (CFGetTypeID(value_) != CFDateGetTypeID())
			return NO; // Can't compare
		
		switch (comparison_)
		{
			case kABEqual:
				return (CFDateCompare(value, (CFDateRef)value_, 0) == kCFCompareEqualTo);
			case kABNotEqual:
				return (CFDateCompare(value, (CFDateRef)value_, 0) != kCFCompareEqualTo);
			case kABLessThan:
				return (CFDateCompare(value, (CFDateRef)value_, 0) ==  kCFCompareLessThan);
			case kABLessThanOrEqual:
				return (CFDateCompare(value, (CFDateRef)value_, 0) <=  kCFCompareEqualTo /*kCFCompareLessThan*/);
			case kABGreaterThan:
				return (CFDateCompare(value, (CFDateRef)value_, 0) ==  kCFCompareGreaterThan);
			case kABGreaterThanOrEqual:
				return (CFDateCompare(value, (CFDateRef)value_, 0) >=  kCFCompareEqualTo /*kCFCompareGreaterThan*/);
			default:
				return NO; // Unknown or impossible comparison
		}
	}
	
	return NO;
}

//- (BOOL)matchesRecord:(ABRecordRef)record
- (BOOL)matchesRecord:(SVRecord *)record
{
	BOOL ret = NO;
	ABMultiValueIdentifier identifier = kABMultiValueInvalidIdentifier;
	CFTypeRef value = ABRecordCopyValue(record.recordRef, property_);
	if (value == NULL)
		return ret;
	
	
	if (CFGetTypeID(value) & kABMultiValueMask)
	{
		CFTypeRef value2 = NULL;
		for (CFIndex i = 0; i < ABMultiValueGetCount(value); ++i) 
		{
			value2 = NULL;
			if (label_)
			{
				CFStringRef label = ABMultiValueCopyLabelAtIndex(value, i);
				if (CFStringCompare ((CFStringRef)label_, label,0) == kCFCompareEqualTo)
					value2 = ABMultiValueCopyValueAtIndex(value, i);
				CFRelease(label);
			}
			else
				value2 = ABMultiValueCopyValueAtIndex(value, i);

			if (!value2)
				continue;
			
			if (CFGetTypeID(value2) == CFDictionaryGetTypeID())
			{
				if (key_)
				{
					ret = [self matchesValue:CFDictionaryGetValue(value2, key_)];
					CFRelease(value2);
					break;
				}
				else
				{
					CFTypeRef *allValues = 0;
					CFIndex count, j;
					
					count = CFDictionaryGetCount(value2);
					allValues = (CFTypeRef *)malloc (sizeof(CFTypeRef) * count);
					CFDictionaryGetKeysAndValues (value2, NULL, allValues);
					for(j = 0; j < count; ++j)
					{
						ret = [self matchesValue:allValues[i]];
						if (ret)
							break;
					}
					free(allValues);
					CFRelease(value2);
					if (ret)
						break;
				}
			}
			else
			{
				ret = [self matchesValue:value2];
				if (ret)
				{
					identifier = ABMultiValueGetIdentifierAtIndex(value, i);
					CFRelease(value2);
					break;
				}
			}
			//CFRelease(value2);
		}
	}
	else 
	{
		ret = [self matchesValue:value];
	}
	CFRelease(value);
	[record setIdentifier:identifier];
	return ret;
}

@end

#pragma mark -
@implementation SVSearchElement (private)

- (ABPropertyID) property
{
	return property_;
}

- initWithProperty:(ABPropertyID)property 
						 label:(NSString *)label 
							 key:(NSString *)key 
						 value:(id)value 
				comparison:(SVSearchComparison)comparison
{
	self = [super init];
	if (self)
	{
		property_   = property;
		label_      = [label copy];
		key_        = [key copy];
		value_      = [value copy];
		comparison_ = comparison;
	}
	
	return self;
}
@end

#pragma mark -
@implementation SVConjunctionSearchElement

- (id)initWithConjunction:(SVSearchConjunction)conjuction
								 children:(NSArray *)children
{
	self = [super init];
	if (self)
	{
		conjuction_ = conjuction;
		children_ = [NSArray arrayWithArray:children];
	}
	return self;
}

- (void)dealloc
{
	[children_ release];
	[super dealloc];
}

//- (BOOL)matchesRecord:(ABRecordRef)record
- (BOOL)matchesRecord:(SVRecord *)record
{
	for (SVSearchElement *search in children_)
	{
		BOOL ret = [search matchesRecord:record];
		if (conjuction_ == kABSearchOr && ret)
			return YES;
		else if (conjuction_ == kABSearchAnd && !ret)
			return NO;
	}

	return (conjuction_ == kABSearchAnd);
}

@end

