/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2010-2011 Samuel <samuelv0304@gmail.com>
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

#import "EndedCallViewController.h"

#import "SIPCallController.h"

@implementation EndedCallViewController

@synthesize callController = callController_;

#pragma mark -

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

- (id)initWithNibName:(NSString *)nibName
       callController:(CallController *)callController
{  
  self = [super initWithNibName:nibName
                         bundle:nil];
  if (self != nil) 
  {
		[self setWantsFullScreenLayout:YES];
    [self setCallController:callController];
  }
  return self;
}

- (id)init
{
  [self dealloc];
  NSString *reason = @"Initialize EndedCallViewController with initWithCallController:";
  @throw [NSException exceptionWithName:@"AKBadInitCall"
                                 reason:reason
                               userInfo:nil];
  return nil;
}

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
 */
- (void)loadView 
{
	CGRect frame = [[UIScreen mainScreen ] applicationFrame];
  UIView *view = [[UIView alloc] initWithFrame:frame];
  [view setAutoresizingMask:UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth];
  
	[view setBackgroundColor:[UIColor clearColor]];
  
  self.view = view;
  [view release];
}


/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc 
{  
  [super dealloc];
}

@end
