/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008-2010 Samuel <samuelv0304@gmail.com>
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

#import "RecentMultiCall.h"


@implementation RecentMultiCall

+ (id)create:(NSDate *)date withPreviousCall:(RecentCall *)call
{
  RecentMultiCall *multiCall;
  
  multiCall = [RecentMultiCall alloc];
  if (call)
  {
    multiCall.type = call.type;
    date = [NSDate date];
    multiCall.number = call.number;
    multiCall.compositeName = call.compositeName;
    multiCall.uid = call.uid;
    multiCall.identifier = call.identifier;
    // TODO
    //_dates = [[NSMutableArray alloc] initWithObjects: call.date, 
    //                   multiCall.date, nil];
  }
  return multiCall;
}

- (void)dealloc
{
  [_dates release];
  [super dealloc];
}

- (void)addOccurrence:(id)dat
{
  [_dates addObject:dat];
}

- (int)numberOfOccurrences
{
  return [_dates count];
}

- (id)occurrenceAtIndex:(NSUInteger)index
{
  return [_dates objectAtIndex:index];
}

- (id)dates
{
  return _dates;
}

@end
