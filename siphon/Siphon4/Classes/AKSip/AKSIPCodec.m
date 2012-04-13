/**
 *  AKSIPCodec.m
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

#import "AKSIPCodec.h"
#import "AKNSString+PJSUA.h"

@implementation AKSIPCodec

@synthesize identifier = identifier_;
@synthesize priority = priority_;

- (void)setPriority:(AKSIPCodecPriority)priority
{
	pj_status_t status;
	pj_str_t identifier = [self.identifier pjString];
	
	status = pjsua_codec_set_priority(&identifier, priority);
	if (status != PJ_SUCCESS)
		NSLog(@"Error setting %@ codec priority (Err. %d)", self.identifier, status);
	else
		priority_ = priority;
}

#pragma mark -

- (id)initWithIdentifier:(NSString *)identifier 
								priority:(AKSIPCodecPriority)priority
{
	self = [super init];
	if (self == nil)
    return nil;
	
	identifier_ = [identifier retain];
	[self setPriority:priority];
	
	return self;
}

- (id)init 
{
  return [self initWithIdentifier:nil priority:kAKSIPCodecDisabledPriority];
}

- (void)dealloc
{
	[identifier_ release];
	[super dealloc];
}

@end
