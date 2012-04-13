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

#import "JsonConnection.h"
#import "HTTPConnection+Additions.h"

#import "JSONKit.h"

@implementation JsonConnection

@synthesize result = _result;

- (id)initWithURLRequest:(NSURLRequest *)request
								receiver:(id)aReceiver
									action:(SEL)receiverAction
{
	self = [super initWithURLRequest:request receiver:aReceiver action:receiverAction];
	if (self)
	{
		_result = nil;
		_decoder = [[JSONDecoder decoder] retain];
	}
	return self;
}

- (id)initWithURL:(NSURL *)url
			 withObject:(id)anObject
				 receiver:(id)aReceiver
					 action:(SEL)receiverAction
{
	NSError *error = nil;
	
	NSData *data = [anObject JSONDataWithOptions:JKSerializeOptionNone
																				 error:&error];
	
	if (error)
	{
		NSLog(@"JsonSerialization error:%@", [error description]);
		[error release];
		return nil;
	}
	
	return [self initWithURLRequest:[self allocMutableURLRequestWithURL:url withData:data]
												 receiver:aReceiver
													 action:receiverAction];
}

- (void)dealloc
{
	[_decoder release];
	//_decoder = nil;
	[_result release];
	//_result = nil;
	[super dealloc];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	if ([self.receivedData length] > 0)
	{
		NSError *error = nil;
		
		//JSONDecoder *decoder = [JSONDecoder decoder];
		//NSLog(@"(%d) %@", [self.receivedData length], self.receivedData);
		_result = [[_decoder objectWithData:self.receivedData error:&error] retain];

		if (error)
		{
			NSLog(@"JsonDeserialization error:%@", [error description]);
			//[error release];
			_result = nil;
			//return;
		}
	}
	
	[super connectionDidFinishLoading:connection];
	
	[_result release];
	_result = nil;
}

@end
