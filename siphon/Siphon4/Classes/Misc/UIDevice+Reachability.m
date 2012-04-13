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

#import "UIDevice+Reachability.h"
#import "Apple Sample Code/wwanconnect.h"

@implementation UIDevice (Reachability)

#pragma mark -
#pragma mark Forcing WWAN connection. Courtesy of Apple. Thank you Apple.
MyStreamInfoPtr	myInfoPtr;
static void myClientCallback(void *refCon)
{
	int  *val = (int*)refCon;
	printf("myClientCallback entered - value from refCon is %d\n", *val);
}

//- (BOOL) forceWWAN
- (BOOL) wakeUpWWAN
{
	int value = 0;
	myInfoPtr = (MyStreamInfoPtr) StartWWAN(myClientCallback, &value);
	NSLog(@"%@", myInfoPtr ? @"Started WWAN" : @"Failed to start WWAN");
	return (!(myInfoPtr == NULL));
}

- (void) shutdownWWAN
{
	if (myInfoPtr) StopWWAN((MyInfoRef) myInfoPtr);
}

@end
