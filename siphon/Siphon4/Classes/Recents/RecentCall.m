/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008-2011 Samuel <samuelv0304@gmail.com>
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

#import "RecentCall.h"


@implementation RecentCall

@dynamic incoming;
@dynamic phoneNumber;
@dynamic date; // TODO en lecture uniquement
@dynamic compositeName;
@dynamic uid;
@dynamic identifier;
@dynamic duration;

- (BOOL)isIncoming
{
	return [[self incoming] boolValue];
}

- (NSString *)displayName
{
  if ([self.compositeName length])
		return self.compositeName;
  else if ([self.phoneNumber length])
		return self.phoneNumber;
  else
    return NSLocalizedString(@"Unknown", @"Recents View");
}

- (BOOL) isMissed
{
	return ([self isIncoming] && [[self duration] intValue] == 0);
}

@end
