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

#import "TextViewController.h"

#import "AKSIPUserAgent.h"

@implementation TextViewController

@synthesize textView = textView_;
@synthesize filename = filename_;

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

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
*/
/*- (void)loadView 
{
	UITextView *textView = [[UITextView alloc] init];
	textView.editable = NO;
	
	[self.view addSubview:textView];
	[textView release];
}*/


/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
*/
- (void)viewDidLoad 
{
	[super viewDidLoad];
	
	self.textView = [[[UITextView alloc] init] autorelease];
	self.textView.editable = NO;
	//self.webView.backgroundColor = [UIColor whiteColor];
	//self.webView.scalesPageToFit = YES;
	//self.webView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
	//self.webView.delegate = self;
	self.view = self.textView;
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
	[filename_ release];
	[textView_ release];
	[super dealloc];
}

- (NSString *)loadContentsOfFile
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString *content = nil;
	if (self.filename && [fileManager fileExistsAtPath:self.filename])
	{
		NSStringEncoding stringEncoding;
		NSError *error = nil;
		content = [NSString stringWithContentsOfFile:self.filename
																		usedEncoding:&stringEncoding
																					 error:&error];
		if (error)
		{
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil
																											message:[error localizedDescription] 
																										 delegate:nil
																						cancelButtonTitle:NSLocalizedString(@"Ok", @"Close the alert view.")
																						otherButtonTitles:nil];
			[alert show];
			[alert release];
		}
	}
	
	return content;	
}

- (void)addActionButton
{
	UIBarButtonItem *actionButton = nil;
	if ([MFMailComposeViewController canSendMail])
	{
		actionButton = [[UIBarButtonItem alloc] 
																	 initWithBarButtonSystemItem:UIBarButtonSystemItemAction
																	 target:self 
																	 action:@selector(action:)];
	}
	self.navigationItem.rightBarButtonItem = actionButton;
	//actionButton.enabled = NO;
	[actionButton release];
}

#pragma mark -
#pragma mark UIViewControllerDelegate
- (void)viewWillAppear:(BOOL)animated
{
	[self addActionButton];
	// TODO block in background
	[[AKSIPUserAgent sharedUserAgent] flushLogFile];
	self.textView.text = [self loadContentsOfFile];
}

- (void)viewDidDisappear:(BOOL)animated
{
	/*if ([self.webView isLoading])
		[self.webView stopLoading];
	
	self.webView.delegate = nil;    // disconnect the delegate as the webview is hidden
	[UIApplication sharedApplication].networkActivityIndicatorVisible = NO;*/
	self.textView.text = nil;
}

#pragma mark -
#pragma mark Buttons
- (void)action:(id)sender 
{
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil 
                                                           delegate:self 
                                                  cancelButtonTitle:NSLocalizedString(@"Cancel",@"Close the action sheet") 
                                             destructiveButtonTitle:NSLocalizedString(@"Clear log file",@"Clear the log file") 
                                                  otherButtonTitles:NSLocalizedString(@"Send by mail",@"Send log file by mail"),
																																		//NSLocalizedString(@"Clear log file",@"Clear the log file"),
																																		//NSLocalizedString(@"Reload log file",@"Reload the log file"),
																																		nil];
	// TODO define the button to display
  actionSheet.actionSheetStyle = UIBarStyleBlackTranslucent;
	[actionSheet showFromBarButtonItem:(UIBarButtonItem *)sender animated:YES];
	[actionSheet release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet 
clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == actionSheet.cancelButtonIndex)
		return;
	
	if (buttonIndex == actionSheet.destructiveButtonIndex)
	{
		[[AKSIPUserAgent sharedUserAgent] clearLogFile];
		self.textView.text = [self loadContentsOfFile];
		/*NSFileManager *fileManager = [NSFileManager defaultManager];
		if (self.filename && [fileManager fileExistsAtPath:self.filename])
		{
			NSError *error = nil;
			BOOL status = [fileManager removeItemAtPath:self.filename 
																						error:&error];
			if (!status)
			{
				NSLog(@"Error delete file: %@", [error localizedDescription]);
				[error release];
			}
		}*/
	}
	
	if (buttonIndex == 1) // Send
	{
		MFMailComposeViewController *composeViewController = [[MFMailComposeViewController alloc] init];
		composeViewController.navigationBar.barStyle = UIBarStyleBlackOpaque;
		composeViewController.mailComposeDelegate = self;
		// Set up recipients
		/*NSArray *toRecipients = [NSArray arrayWithObject:@"first@example.com"]; 
		 NSArray *ccRecipients = [NSArray arrayWithObjects:@"second@example.com", @"third@example.com", nil]; 
		 NSArray *bccRecipients = [NSArray arrayWithObject:@"fourth@example.com"]; 
		 
		 [picker setToRecipients:toRecipients];
		 [picker setCcRecipients:ccRecipients];  
		 [picker setBccRecipients:bccRecipients];*/
		
		// Subject
		[composeViewController setSubject:@"Siphon log file"];
		
		// Attach an image to the email
		NSData *myData = [NSData dataWithContentsOfFile:[self filename]];
		[composeViewController addAttachmentData:myData mimeType:@"text/plain" fileName:@"log.txt"];
		
		// Fill out the email body text
		NSString *emailBody = @"Hello!\nHere is the Siphon log file";
		[composeViewController setMessageBody:emailBody isHTML:NO];
		
		[self presentModalViewController:composeViewController animated:YES];
		[composeViewController release];
	}
}

#pragma mark -
#pragma mark MFMailComposeViewControllerDelegate
// Dismisses the message composition interface when users tap Cancel or Send. Proceeds to update the 
// feedback message field with the result of the operation.
- (void)mailComposeController:(MFMailComposeViewController*)controller
					didFinishWithResult:(MFMailComposeResult)result 
												error:(NSError*)error
{
	//message.hidden = NO;
	// Notifies users about errors associated with the interface
	switch (result)
	{
		case MFMailComposeResultCancelled:
		case MFMailComposeResultSent:
			break;
		case MFMailComposeResultSaved:
			//message.text = @"Result: saved";
			break;
		case MFMailComposeResultFailed:
		{
			//message.text = @"Result: failed";
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil
																											message:[error localizedDescription] 
																										 delegate:nil
																						cancelButtonTitle:NSLocalizedString(@"Ok", @"Close alert view")
																						otherButtonTitles:nil];
			[alert show];
			[alert release];
		}
			break;
		default:
			//message.text = @"Result: not sent";
			break;
	}
	[self dismissModalViewControllerAnimated:YES];
}

@end
