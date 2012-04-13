/**
 *  AKSIPCodec.h
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
#import <pjsua-lib/pjsua.h>

enum  {
	/**
	 * This priority makes the codec the highest in the order.
	 * The last codec specified with this priority will get the
	 * highest place in the order, and will change the priority
	 * of previously highest priority codec to NEXT_HIGHER.
	 */
	kAKSIPCodecHighestPriority = PJMEDIA_CODEC_PRIO_HIGHEST,
	
	/**
	 * This priority will put the codec as the next codec after
	 * codecs with this same priority.
	 */
	kAKSIPCodecNextHigherPriority = PJMEDIA_CODEC_PRIO_NEXT_HIGHER,
	
	/**
	 * This is the initial codec priority when it is registered to
	 * codec manager by codec factory.
	 */
	kAKSIPCodecNormalPriority = PJMEDIA_CODEC_PRIO_NORMAL,
	
	/**
	 * This priority makes the codec the lowest in the order.
	 * The last codec specified with this priority will be put
	 * in the last place in the order.
	 */
	kAKSIPCodecLowestPriority = PJMEDIA_CODEC_PRIO_LOWEST,
	
	/**
	 * This priority will prevent the codec from being listed in the
	 * SDP created by media endpoint, thus should prevent the codec
	 * from being used in the sessions. However, the codec will still
	 * be listed by #pjmedia_codec_mgr_enum_codecs() and other codec
	 * query functions.
	 */
	kAKSIPCodecDisabledPriority = PJMEDIA_CODEC_PRIO_DISABLED
};
typedef NSUInteger AKSIPCodecPriority;

@interface AKSIPCodec : NSObject 
{
@private
	NSString   *identifier_;
	NSUInteger  priority_;
}

// Codec unique identifier.
@property(nonatomic, readonly) NSString  *identifier;

// Codec priority (integer 0-255).
@property(nonatomic, assign) AKSIPCodecPriority priority;

// 
- (id)initWithIdentifier:(NSString *)identifier 
								priority:(AKSIPCodecPriority)priority;

@end
