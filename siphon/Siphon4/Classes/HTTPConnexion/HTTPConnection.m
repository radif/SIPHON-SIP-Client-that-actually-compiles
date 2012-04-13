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

#import "HTTPConnection.h"
#import <UIKit/UIKit.h>

@interface HTTPConnection (private)

-(NSMutableURLRequest *)allocMutableURLRequestWithURL:(NSURL *)url withBody:(NSString *)body;

@end


@implementation HTTPConnection

@synthesize receivedData = _receivedData;
@synthesize showNetworkActivityIndicator = _showNetworkActivityIndicator;
@synthesize showErrors = _showErrors;

- (id)initWithURLRequest:(NSURLRequest *)request
								receiver:(id)aReceiver
									action:(SEL)receiverAction
{
	self = [super init];
  if (self)
  {
		_receiver = aReceiver;
		_action   = receiverAction;
		
		_showNetworkActivityIndicator = YES;
		_showErrors = YES;
		
		_connection = [[NSURLConnection alloc] initWithRequest:request
																									delegate:self
																					startImmediately:NO];
	}
	
	return self;
}

- (id)initWithURL:(NSURL *)url 
						 body:(NSString *)body 
				 receiver:(id)aReceiver
					 action:(SEL)receiverAction
{
	if (url == nil)
    return nil;

	return [self initWithURLRequest:[self allocMutableURLRequestWithURL:url withBody:body]
												 receiver:aReceiver
													 action:receiverAction];
}

- (id)initWithURL:(NSURL *)url body:(NSString *)body
{
	return [self initWithURL:url body:body receiver:nil action:nil];
}

- (void) dealloc
{
	[self cancel];
	
  [_connection release];
  [_receivedData release];
  [super dealloc];
}

-(NSMutableURLRequest *)allocMutableURLRequestWithURL:(NSURL *)url
																						 withData:(NSData *)data
{
	NSMutableURLRequest *request;
  
  request = [[NSMutableURLRequest alloc] initWithURL:url 
                                         cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
                                     timeoutInterval:60];
  
	if ([data length] > 0)
	{
		[request setHTTPMethod:@"POST"];
		[request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
		[request setHTTPBody:data];
		[request setValue:[NSString stringWithFormat:@"%d", [data length]] forHTTPHeaderField:@"Content-Length"];
	}
	else
		[request setHTTPMethod:@"GET"];
	
  return request;
}

-(NSMutableURLRequest *)allocMutableURLRequestWithURL:(NSURL *)url 
																						 withBody:(NSString *)body
{
	return [self allocMutableURLRequestWithURL:url withData:[body dataUsingEncoding:NSASCIIStringEncoding]];
}

- (void)start
{
  if (_connection)
    [_connection start];

	if ([self showNetworkActivityIndicator])
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
}

- (void)cancel
{
  if (_connection)
    [_connection cancel];
	
	if ([self showNetworkActivityIndicator])
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
	
	[_receivedData release];
	_receivedData = nil;
}

#pragma mark -
#pragma mark NSURLConnection delegate
- (NSCachedURLResponse *)connection:(NSURLConnection *)connection 
									willCacheResponse:(NSCachedURLResponse *)cachedResponse
{
  return nil;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	assert(connection == _connection);
  if (connection != _connection)
		return;
	
	if ([response isKindOfClass:[NSHTTPURLResponse self]]) 
	{
		NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
		if ([httpResponse statusCode] >= 400)
		{
			if ([self showErrors])
			{
				UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil
																												message:[NSHTTPURLResponse localizedStringForStatusCode:[httpResponse statusCode]]
																											 delegate:nil
																							cancelButtonTitle:NSLocalizedString(@"OK", @"Close the alert view.")
																							otherButtonTitles:nil];
				[alert show];
				[alert release];
			}
			return;
		}
		
		NSDictionary *headers = [httpResponse allHeaderFields];
		
		/* create the NSMutableData instance that will hold the received data */
    long long contentLength = [response expectedContentLength];
		if (contentLength == NSURLResponseUnknownLength)
			contentLength = [[headers objectForKey:@"Content-Length"] integerValue];
			
		if (contentLength > 0)
			_receivedData = [[NSMutableData alloc] initWithCapacity:contentLength];
		else
			_receivedData = [[NSMutableData alloc] init];
	}
	else 
	{
		_receivedData = [[NSMutableData alloc] init];
	}
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
  assert(connection == _connection);
  if (connection == _connection)
    [_receivedData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
  assert(connection == _connection);
  
  [connection cancel];
  //if (connection == _connection)
  {
		[_connection release];
    _connection = nil;
		
    [_receivedData release];
    _receivedData = nil;
  }
  
	if ([self showErrors])
	{
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil
																										message:[error localizedDescription] 
																									 delegate:nil 
																					cancelButtonTitle:NSLocalizedString(@"OK", @"Close the alert view.") 
																					otherButtonTitles:nil];
		[alert show];
		[alert release];
	}
	
	if ([self showNetworkActivityIndicator])
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
  assert(connection == _connection);
  
	[_connection release];
  _connection = nil;
	
  //str = [[NSString alloc] initWithData:_receivedData encoding:NSASCIIStringEncoding];
  //str = [str stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
  
	if (_receiver && [_receiver respondsToSelector:_action])
	{
		[_receiver performSelector:_action withObject:self];
		_receiver = nil;
	}

  [_receivedData release];
  _receivedData = nil;
	
	if ([self showNetworkActivityIndicator])
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
}

@end
