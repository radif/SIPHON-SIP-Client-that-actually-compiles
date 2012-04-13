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

#import "PropertyConnection.h"
#import "HTTPConnection+Additions.h"


@implementation PropertyConnection

@synthesize result = _result;

- (id)initWithURLRequest:(NSURLRequest *)request
								receiver:(id)aReceiver
									action:(SEL)receiverAction
{
	_result = nil;
	return [super initWithURLRequest:request receiver:aReceiver action:receiverAction];
}

- (id)initWithURL:(NSURL *)url
 withPropertyList:(id)plist
				 receiver:(id)aReceiver
					 action:(SEL)receiverAction
{
	NSError *error = nil;
	
	NSData *data = [NSPropertyListSerialization dataWithPropertyList:plist
																														format:kCFPropertyListXMLFormat_v1_0 
																													 options:0 
																														 error:&error];
	if (error)
	{
		NSLog(@"PropertyListSerialization error:%@", [error description]);
		[error release];
		return nil;
	}
	
	return [self initWithURLRequest:[self allocMutableURLRequestWithURL:url withData:data]
																	receiver:aReceiver
																		action:receiverAction];
}

- (void)dealloc
{
	[_result release];
	_result = nil;
	[super dealloc];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	if ([self.receivedData length] > 0)
	{
		NSError *error = nil;
		_result =[NSPropertyListSerialization propertyListWithData:self.receivedData
																											 options:kCFPropertyListImmutable
																												format:nil
																												 error:&error];
		if (error)
		{
			NSLog(@"PropertyListSerialization error:%@", [error description]);
			[error release];
			_result = nil;
			//return;
		}
	}
	
	[super connectionDidFinishLoading:connection];
	
	[_result release];
	_result = nil;
}

@end
