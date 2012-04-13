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

#import "InAppSettingsWebViewController.h"


@implementation InAppSettingsWebViewController

@synthesize webView = webView_;
@synthesize unloaded = unloaded_;
@synthesize URL = url_;
@synthesize filename = filename_;

- (void)setURL:(NSURL *)url
{
	if (url != url_)
		[url_ release];
	
	url_ = [url retain];
	self.unloaded = YES;
}

#pragma mark -
- (id)initWithFile:(NSString *)filename
{
	self = [super init];
	if (self)
	{
		self.filename = filename;
	}
	return self;
}

- (id)initWithURL:(NSURL *)url
{
	self = [super init];
	if (self)
	{
		self.URL = url;
	}
	return self;
}

// The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
/*
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization.
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
 */
- (void)viewDidLoad 
{
	[super viewDidLoad];
	
	self.webView = [[[UIWebView alloc] init] autorelease];
	self.webView.backgroundColor = [UIColor whiteColor];
	//self.webView.scalesPageToFit = YES;
	//self.webView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
	self.webView.delegate = self;
	self.view = self.webView;
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations.
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc 
{
	webView_.delegate = nil;
	[webView_ release];
	[super dealloc];
}

#pragma mark -
#pragma mark UIViewControllerDelegate
- (void)viewWillAppear:(BOOL)animated
{
	self.webView.delegate = self; // setup the delegate as the web view is shown
	// FIXME: reload
	if ([self filename])
	{
		NSString *thePath = [[NSBundle mainBundle] pathForResource:self.filename ofType:nil];
		if (thePath) {
			NSString *html = [NSString stringWithContentsOfFile:thePath 
																								 encoding:NSUTF8StringEncoding 
																										error:NULL];
			[self.webView loadHTMLString:html baseURL:nil];
		}
	}
	if ([self URL])
		[self.webView loadRequest:[NSURLRequest requestWithURL:[self URL]]];
	
}

- (void)viewWillDisappear:(BOOL)animated
{
	if ([self.webView isLoading])
		[self.webView stopLoading];
	
	self.webView.delegate = nil;    // disconnect the delegate as the webview is hidden
	[UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
}

#pragma mark -
#pragma mark UIWebViewDelegate
- (void)webViewDidStartLoad:(UIWebView *)webView
{
	// starting the load, show the activity indicator in the status bar
	[UIApplication sharedApplication].networkActivityIndicatorVisible = YES;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
	// finished loading, hide the activity indicator in the status bar
	[UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
	// load error, hide the activity indicator in the status bar
	[UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
	
	// report the error inside the webview
	NSString* errorString = [NSString stringWithFormat:
													 @"<html><center><font size=+5 color='red'>An error occurred:<br>%@</font></center></html>",
													 error.localizedDescription];
	[self.webView loadHTMLString:errorString baseURL:nil];
}

@end
