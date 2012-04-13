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

#import "PhoneViewController.h"
#import "SiphonAppDelegate.h"

#import "UIScreen+ConvertRect.h"
#if GSM_BUTTON
#import "UIDevice+Capabilities.h"
#import "SettingsController.h"
#endif /* GSM_BUTTON */

#import "SIPController.h"
#import "SIPAccountController.h"

#import "AKSIPURI.h"
#import "AKTelephoneNumberFormatter.h"

#import "CallPickerController.h"

#import "AddressBook.h"
#import "ABRecord+Private.h"

#if HTTP_REQUEST
#import "SiphonRequest.h"
#endif /* HTTP_REQUEST */

static const NSString *forbiddenChars = @",;/?:&=+$";

@interface PhoneViewController ()

#if defined(POPOVER_CALL) && POPOVER_CALL!=0
@property (nonatomic, retain) WEPopoverController  *callPickerPopover;
#endif /* POPOVER_CALL */

#if HTTP_REQUEST
@property (nonatomic, retain) NSTimer *balanceTimer; 
@property (nonatomic, retain) SiphonRequest *balanceRequest;
@property (nonatomic, retain) SiphonRequest *rateRequest;
#endif /* HTTP_REQUEST */

- (void)addNewPerson;

@end

@implementation PhoneViewController

@synthesize lcd = _lcd;

#if defined(POPOVER_CALL) && POPOVER_CALL!=0
@synthesize callPickerPopover = _callPickerPopover;
#endif /* POPOVER_CALL */

#if HTTP_REQUEST
@synthesize balanceTimer   = _balanceTimer;
@synthesize balanceRequest = _balanceRequest;
@synthesize rateRequest    = _rateRequest;
#endif /* HTTP_REQUEST */

- (ABPeoplePickerNavigationController *)peoplePickerController
{
	if (peoplePickerController_ == nil)
	{
		peoplePickerController_ = [[ABPeoplePickerNavigationController alloc] init];
    peoplePickerController_.navigationBar.barStyle = UIBarStyleBlackOpaque;
    peoplePickerController_.peoplePickerDelegate = self;
	}
	return peoplePickerController_;
}

#pragma mark -
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil 
{
  self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	if (self) 
  {
		// Initialization code
    self.title = NSLocalizedString(@"Keypad", @"Keypad to dial");
    self.tabBarItem.image = [UIImage imageNamed:@"Dial"];
    
    _lcd = [[LCDPhoneView alloc] initWithFrame:
            CGRectMake(0.0f, 0.0f, 320.0f, 74.0f)];
    _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top_buttons"]];
    //_lcd.backgroundColor = [UIColor clearColor];
    
		//AccountController *firstEnabledAccountController = [[[SIPController sharedInstance] 
		//																										 enabledAccountControllers] objectAtIndex:0];
		//[_lcd leftText:firstEnabledAccountController.account.registrar];
		
    //[_lcd leftText: [[NSUserDefaults standardUserDefaults] stringForKey: 
    //                 @"server"]];
    //[_lcd rightText:NSLocalizedString(@"Disconnected", 
		//																	@"Initialize with disconnected status because here we don't know the status.")];
		
    /*peoplePickerController_ = [[ABPeoplePickerNavigationController alloc] init];
    peoplePickerController_.navigationBar.barStyle = UIBarStyleBlackOpaque;
    peoplePickerController_.peoplePickerDelegate = self;*/
	}
	return self;
}

/*
 Implement loadView if you want to create a view hierarchy programmatically
*/
- (void)loadView 
{  
  UIView *view = [[UIView alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];
  [view setAutoresizingMask:UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth];
//  //[view setBackgroundColor:_color];
  
  _textfield = [[UITextField alloc] initWithFrame: CGRectMake(0.0f, 0.0f, 320.0f, 74.0f)];
  //_textField.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top"]];
  _textfield.autocorrectionType = UITextAutocorrectionTypeNo;
  _textfield.autocapitalizationType = UITextAutocapitalizationTypeNone;
  _textfield.keyboardType = UIKeyboardTypeURL;
  _textfield.returnKeyType = UIReturnKeyDone;
  _textfield.borderStyle = UITextBorderStyleNone;
  _textfield.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
  _textfield.delegate = self;
  _textfield.textColor = [UIColor lightGrayColor];
  _textfield.backgroundColor = [UIColor clearColor];
  _textfield.font = [UIFont fontWithName:@"Helvetica" size:35];
  _textfield.minimumFontSize = 15;
  _textfield.adjustsFontSizeToFitWidth = YES;
  _textfield.textAlignment = UITextAlignmentCenter;
  _textfield.text = @"";
  [_lcd addSubview:_textfield];
  
  _pad = [[DialerPhonePad alloc] initWithFrame: 
          CGRectMake(0.0f, 74.0f, 320.0f, 273.0f)];

  [_pad setPlaysSounds:YES];
  /*[_pad setPlaysSounds:[[NSUserDefaults standardUserDefaults] 
                        boolForKey:@"keypadPlaySound"]];*/
  [_pad setDelegate:self];
  
	_addContactButton = [[UIButton alloc] initWithFrame:
                         CGRectMake(0.0f, 0.0f, 107.0f, 64.0f)];
	[_addContactButton setImage: [UIImage imageNamed:@"addcontact"]
                       forState:UIControlStateNormal];
	[_addContactButton setImage: [UIImage imageNamed:@"addcontact_pressed"] 
                       forState:UIControlStateHighlighted];
	[_addContactButton addTarget:self action:@selector(addButtonPressed:) 
                       forControlEvents:UIControlEventTouchDown];

#if GSM_BUTTON
    _gsmCallButton =[[UIButton alloc] initWithFrame:
                     CGRectMake(0.0f, 0.0f, 107.0f, 64.0f)];
    [_gsmCallButton setImage:[UIImage imageNamed:@"answer"] 
                 forState: UIControlStateNormal];
    _gsmCallButton.imageEdgeInsets = UIEdgeInsetsMake (0., 0., 0., 5.);
    [_gsmCallButton setTitle:@"GSM" forState:UIControlStateNormal];
#ifdef __IPHONE_3_0
    _gsmCallButton.titleLabel.shadowOffset = CGSizeMake(0,-1);
    _gsmCallButton.titleLabel.font = [UIFont boldSystemFontOfSize:26];
#else
    _gsmCallButton.titleShadowOffset = CGSizeMake(0,-1);
    _gsmCallButton.font = [UIFont boldSystemFontOfSize:26];
#endif
    [_gsmCallButton setTitleShadowColor:[UIColor darkGrayColor] forState:UIControlStateNormal];
    [_gsmCallButton setTitleShadowColor:[UIColor colorWithWhite:0. alpha:0.2]  forState:UIControlStateDisabled];
    [_gsmCallButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [_gsmCallButton setTitleColor:[UIColor colorWithWhite:1.0 alpha:0.5]  forState:UIControlStateDisabled];

    _gsmCallButton.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"callblue"]];
    
    [_gsmCallButton addTarget:self action:@selector(gsmCallButtonPressed:) 
          forControlEvents:UIControlEventTouchDown];
#endif /* GSM_BUTTON */
	
  _callButton =[[UIButton alloc] initWithFrame:
                CGRectMake(107.0f, 0.0f, 107.0f, 64.0f)];
	_callButton.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"call"]];
  //_callButton.enabled = NO;

  [_callButton setImage:[UIImage imageNamed:@"answer"] 
               forState: UIControlStateNormal];
  _callButton.imageEdgeInsets = UIEdgeInsetsMake (0., 0., 0., 5.);
  [_callButton setTitle:@"Call" forState:UIControlStateNormal];
#if __IPHONE_3_0
  _callButton.titleLabel.shadowOffset = CGSizeMake(0,-1);
  _callButton.titleLabel.font = [UIFont boldSystemFontOfSize:26];
#else
  _callButton.titleShadowOffset = CGSizeMake(0,-1);
  _callButton.font = [UIFont boldSystemFontOfSize:26];
#endif
  [_callButton setTitleShadowColor:[UIColor darkGrayColor] forState:UIControlStateNormal];
  [_callButton setTitleShadowColor:[UIColor colorWithWhite:0. alpha:0.2]  forState:UIControlStateDisabled];
  [_callButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
  [_callButton setTitleColor:[UIColor colorWithWhite:1.0 alpha:0.5]  forState:UIControlStateDisabled];

#if defined(POPOVER_CALL) && POPOVER_CALL!=0
	[_callButton addTarget:self action:@selector(callButtonDidPress:) 
				forControlEvents:UIControlEventTouchDown];
	[_callButton addTarget:self action:@selector(callButtonDidReleaseOutside:) 
				forControlEvents:UIControlEventTouchUpOutside];
#endif /* POPOVER_CALL */

  [_callButton addTarget:self action:@selector(callButtonDidReleaseInside:) 
               forControlEvents:UIControlEventTouchUpInside];
  
  _deleteButton = [[UIButton alloc] initWithFrame:
                   CGRectMake(214.0f, 0.0f, 107.0f, 64.0f)];
  [_deleteButton setImage:[UIImage imageNamed:@"delete"] 
                 forState:UIControlStateNormal];
  [_deleteButton setImage: [UIImage imageNamed:@"delete_pressed"] 
                 forState:UIControlStateHighlighted];
  [_deleteButton addTarget:self action:@selector(deleteButtonPressed:) 
                 forControlEvents:UIControlEventTouchDown];
  [_deleteButton addTarget:self action:@selector(deleteButtonReleased:) 
                 forControlEvents:UIControlEventValueChanged| 
                 UIControlEventTouchUpInside|UIControlEventTouchUpOutside];
	
  _container = [[UIView alloc] initWithFrame:
                CGRectMake(0.0f, 347.0f, 320.0f, 64.0f)];
  
	[view addSubview:_lcd];
  [view addSubview:_pad];

#if !GSM_BUTTON
	[_container addSubview:_addContactButton];
#endif /* GSM_BUTTON */

  [_container addSubview:_callButton];
  [_container addSubview:_deleteButton];
  
  [view addSubview:_container];

  self.view = view;
  [view release];
}

/*
 If you need to do additional setup after loading the view, override viewDidLoad
 */
- (void)viewDidLoad 
{
  [super viewDidLoad];
  _callButton.enabled = NO;
#if GSM_BUTTON
  if (_gsmCallButton)
    _gsmCallButton.enabled = NO;
#endif /* GSM_BUTTON */
}

- (void)keyboardWillShow:(NSNotification*)aNotification
{ 
  _pad.enabled = NO;
  
#if GSM_BUTTON
  if (_gsmCallButton)
    _gsmCallButton.enabled = NO;
#endif /* GSM_BUTTON */
	
  _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top"]];
  
  NSDictionary* info = [aNotification userInfo];
  
  // Get the size of the keyboard.
	NSValue* aValue = [info valueForKey:UIKeyboardFrameEndUserInfoKey];
	CGRect keyboardEndFrame = [UIScreen convertRect:[aValue CGRectValue] 
																					 toView:self.view];

	CGRect rect = _container.frame;
	rect.origin.y = keyboardEndFrame.origin.y - 64.0f;

	[UIView animateWithDuration:0.3
									 animations:^{
										 _container.frame = rect;
									 }
	 ];
}

- (void)keyboardWillHide:(NSNotification*)aNotification
{
  //[_scrollView setContentOffset:CGPointZero animated:YES];
  _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top_buttons"]];
  
	CGRect rect = _container.frame;
  rect.origin.y = 347.0f;
	
	[UIView animateWithDuration:0.3
									 animations:^{
										 _container.frame = rect;
									 } 
									 completion:^(BOOL finished){
										 [_pad setEnabled:YES];
									 }];
}

- (void)viewWillAppear:(BOOL)animated 
{
#if GSM_BUTTON
	if (![[UIDevice currentDevice] supportsTelephony] || 
      ![[NSUserDefaults standardUserDefaults] boolForKey:kCellularButton])
	{
		[_gsmCallButton removeFromSuperview];
    [_container addSubview:_addContactButton];
	}
  else
  { 
		[_addContactButton removeFromSuperview];
		[_container addSubview:_gsmCallButton];
	}
#endif /* GSM_BUTTON */
	
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(keyboardWillShow:)
                                               name:UIKeyboardWillShowNotification
                                             object:nil];
  
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(keyboardWillHide:)
                                               name:UIKeyboardWillHideNotification 
                                             object:nil];
	
#if HTTP_REQUEST
	self.balanceRequest = [[SiphonRequest alloc] initWithReceiver:_lcd action:@selector(rightText:)];
  self.balanceTimer = [NSTimer scheduledTimerWithTimeInterval:600 target:self.balanceRequest 
																										 selector:@selector(requestBalance) 
																										 userInfo:nil 
																											repeats:YES];
	[self.balanceTimer fire];
#endif /* HTTP_REQUEST */
}

- (void)viewWillDisappear:(BOOL)animated 
{
  [_textfield resignFirstResponder];
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:UIKeyboardWillShowNotification 
                                                object:nil];
  
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:UIKeyboardWillHideNotification 
                                                object:nil];
#if defined(POPOVER_CALL) && POPOVER_CALL!=0
	if ([self.callPickerPopover isPopoverVisible])
	{
		[self.callPickerPopover dismissPopoverAnimated:NO];
	}
#endif /* POPOVER_CALL */
	
#if HTTP_REQUEST
	if (_balanceTimer)
  {
    [self.balanceTimer invalidate];
    self.balanceTimer = nil;
  }
	self.balanceRequest = nil;
	self.rateRequest = nil;
#endif /* HTTP_REQUEST */
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
	[peoplePickerController_ release];
	peoplePickerController_ = nil;
}


- (void)dealloc 
{
  [peoplePickerController_ release];
	peoplePickerController_ = nil;

  [_textfield release];
  [_lcd release];
  [_pad release];

  [_addContactButton release];
#if GSM_BUTTON
  [_gsmCallButton release];
#endif /* GSM_BUTTON */
#if defined(POPOVER_CALL) && POPOVER_CALL!=0
	[_callPickerPopover release];
#endif /* POPOVER_CALL */
  [_callButton release];

	[_deleteTimer invalidate];
	[_deleteTimer release];
  [_deleteButton release];


  [_container release];

  //NSTimer *_deleteTimer;
  //NSString *_lastNumber;
  
#if HTTP_REQUEST
	[self.balanceTimer invalidate];
	[_balanceTimer release];
	
  [_rateRequest release];
  [_balanceRequest release];
#endif /* HTTP_REQUEST */
	
	[super dealloc];
}

#pragma mark -
#pragma mark PhonePadDelegate
- (void)phonePad:(id)phonepad appendString:(NSString *)string
{
  NSString *curText = [_textfield text];
  [_textfield setText: [curText stringByAppendingString: string]];
  
  _callButton.enabled = YES;
#if GSM_BUTTON
  if (_gsmCallButton)
    _gsmCallButton.enabled = YES;
#endif /* GSM_BUTTON */
  _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top"]];
	
#if HTTP_REQUEST
	//if ([curText length] >= 3 && _rateRequest == nil)
	if ([curText length] >= 3 && ![_rateRequest isActive])
	{
		self.rateRequest = [[SiphonRequest alloc] initWithReceiver:_lcd action:@selector(topText:)];
		[self.rateRequest requestRate:[_textfield text]];
	}
#endif /* HTTP_REQUEST */
}

- (void)phonePad:(id)phonepad replaceLastDigitWithString:(NSString *)string
{
  NSString *curText = [_textfield text];
  curText = [curText substringToIndex:([curText length] - 1)];
  [_textfield setText: [curText stringByAppendingString: string]];
}


#pragma mark -
#pragma mark Buttons callback
#if defined(POPOVER_CALL) && POPOVER_CALL!=0
- (void)callButtonDidLongPress:(UIButton *)button
{
	consumedTap_ = YES;
	if (_callPickerPopover == nil)
	{
		CallPickerController *callPicker = [[CallPickerController alloc] initWithStyle:UITableViewStylePlain];
		self.callPickerPopover = [[WEPopoverController alloc]
															initWithContentViewController:callPicker];
		//CGRect rect = _callButton.frame;
		//rect = _container.frame;
	}
	[self.callPickerPopover presentPopoverFromRect:_container.frame/*(CGRect)rect*/
																					inView:self.view /*self.view/*button*/ 
												permittedArrowDirections:UIPopoverArrowDirectionDown 
																				animated:YES];
}

- (void)callButtonDidPress:(UIButton *)button
{
	if ([[_textfield text] length] > 0)
	{
		consumedTap_ = NO;
		[self performSelector:@selector(callButtonDidLongPress:)
							 withObject:button 
							 afterDelay:0.5];
	}
}

- (void)callButtonDidReleaseOutside:(UIButton *)button
{
	[NSObject cancelPreviousPerformRequestsWithTarget:self
																					 selector:@selector(callButtonDidLongPress:)
																						 object:button];
}
#endif /* POPOVER_CALL */

- (void)callButtonDidReleaseInside:(UIButton*)button
{
#if defined(POPOVER_CALL) && POPOVER_CALL!=0
	[NSObject cancelPreviousPerformRequestsWithTarget:self
																					 selector:@selector(callButtonDidLongPress:)
																						 object:button];
	if (!consumedTap_ && ![self.callPickerPopover isPopoverVisible])
	{
#endif /* POPOVER_CALL */
	
	if ([[_textfield text] length] > 0)
	{
		_lastNumber = [[NSString alloc] initWithString: [_textfield text]];
    [_textfield setText:@""];
		_lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top_buttons"]];

#if 0
		// Search Address Book for caller's name.
		ABAddressBook *AB = [ABAddressBook sharedAddressBook];
		NSArray *records = nil;
		NSString *finalDisplayedName = nil;
		NSString *localizedLabel = nil;
		BOOL recordFound = NO;
		// Look for the whole phone number match first.
    ABSearchElement *phoneNumberMatch = [ABPerson searchElementForProperty:kABPersonPhoneProperty
																																		 label:nil
																																			 key:nil
																																		 value:_lastNumber
																																comparison:kABEqual];
		
		records = [AB recordsMatchingSearchElement:phoneNumberMatch];
		if ([records count] > 0) 
		{
			recordFound = YES;
			ABRecord *theRecord = [records objectAtIndex:0];
			finalDisplayedName = (NSString *)ABRecordCopyCompositeName([theRecord recordRef]);
			// Find the exact phone number match.
			ABMultiValueRef multiValue = ABRecordCopyValue([theRecord recordRef],
																										 kABPersonPhoneProperty);
			NSArray* phoneNumbers = (NSArray*)ABMultiValueCopyArrayOfAllValues(multiValue);
			for ( NSString *aNumber in phoneNumbers)
			{
				if ([aNumber isEqualToString:_lastNumber])
				{
					CFIndex valueIdx = ABMultiValueGetFirstIndexOfValue (multiValue, aNumber);
					CFStringRef label = ABMultiValueCopyLabelAtIndex(multiValue, valueIdx);
					localizedLabel = (NSString *)ABAddressBookCopyLocalizedLabel(label);
					CFRelease(label);
					break;
				}
			}
			CFRelease(multiValue);
		}
		
		[[SIPController sharedInstance] makeCall:_lastNumber
																 displayName:finalDisplayedName
																	phoneLabel:localizedLabel
																			 image:nil];
#else
		// Search Address Book for caller's name.
		ABAddressBook *AB = [ABAddressBook sharedAddressBook];
		NSArray *records = nil;

		// Look for the whole phone number match first.
    ABSearchElement *phoneNumberMatch = [ABPerson searchElementForProperty:kABPersonPhoneProperty
																																		 label:nil
																																			 key:nil
																																		 value:_lastNumber
																																comparison:kABEqual];
		
		records = [AB recordsMatchingSearchElement:phoneNumberMatch];
		ABRecord *theRecord = nil;
		if ([records count] > 0)
			theRecord = [records objectAtIndex:0];
		
		NSUInteger significantPhoneNumberLength = 6;
		//= [defaults integerForKey:kSignificantPhoneNumberLength];
		
		// Get the significant phone suffix if the phone number length is greater
    // than we defined.
    NSString *significantPhoneSuffix;
    if ([_lastNumber length] > significantPhoneNumberLength) 
		{
      significantPhoneSuffix = [_lastNumber substringFromIndex:
																([_lastNumber length] - significantPhoneNumberLength)];
      
      // If the the record hasn't been found with the whole number, look for
      // significant suffix match.
      if (!theRecord) 
			{
        ABSearchElement *phoneNumberSuffixMatch =
					[ABPerson searchElementForProperty:kABPersonPhoneProperty
																			 label:nil
																				 key:nil
																			 value:significantPhoneSuffix
																	comparison:kABSuffixMatch];
        
        records = [AB recordsMatchingSearchElement:phoneNumberSuffixMatch];
        if ([records count] > 0) 
					// TODO check the correct number!!!
					theRecord = [records objectAtIndex:0];
      }
    }
		
		// If still not found, search phone numbers that contain spaces, dashes, etc.
		if (!theRecord)
		{
			AKTelephoneNumberFormatter *telephoneNumberFormatter = [[AKTelephoneNumberFormatter alloc] init];
			
			NSArray *people = (NSArray*)ABAddressBookCopyArrayOfAllPeople([AB addressBook]);

			ABRecordRef record = nil;
			NSEnumerator *enumerator = [people objectEnumerator];
			while (record = [enumerator nextObject])
			{
				ABMultiValueRef phoneNumberProperty = ABRecordCopyValue(record, kABPersonPhoneProperty);
				NSArray* phoneNumbers = (NSArray*)ABMultiValueCopyArrayOfAllValues(phoneNumberProperty);
				//for ( NSString *aNumber in phoneNumbers)
				for (int i = [phoneNumbers count] - 1; i >=0 ; --i)
				{
					NSString *aNumber = [phoneNumbers objectAtIndex:i];
					NSString *normalizedNumber = [telephoneNumberFormatter telephoneNumberFromString:aNumber];
					//NSLog(@"number %@", normalizedNumber);
					if ([_lastNumber compare:normalizedNumber] == NSOrderedSame)
					{
						theRecord = [[[ABRecord alloc] initWithRecord:record] autorelease];
						[theRecord setProperty:kABPersonPhoneProperty];
						ABMultiValueIdentifier identifier = ABMultiValueGetIdentifierAtIndex(phoneNumberProperty, i);
						[theRecord setIdentifier:identifier];
						break;
					}
				}
				CFRelease(phoneNumberProperty);
				[phoneNumbers release];
				if (theRecord)
					break;
			}
			[people release]; // CLANG
			[telephoneNumberFormatter release];
		}
		
		[[SIPController sharedInstance] makeCall:_lastNumber
															 displayPerson:[theRecord recordRef]
																		property:[theRecord property]
																	identifier:[theRecord identifier]];
#endif		
	}
	else
  {
    _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top"]];
    [_textfield setText:_lastNumber];
		
#if HTTP_REQUEST
		//if ([_lastNumber length] >= 3 && _rateRequest == nil)
		if ([_lastNumber length] >= 3)
		{
			self.rateRequest = [[SiphonRequest alloc] initWithReceiver:_lcd action:@selector(topText:)];
			[self.rateRequest requestRate:_lastNumber];
		}
#endif /* HTTP_REQUEST */
		
    [_lastNumber release];
  }
#if defined(POPOVER_CALL) && POPOVER_CALL!=0
	}
#endif /* POPOVER_CALL */
}

- (void)addButtonPressed:(UIButton*)unused
{
  if ([[_textfield text] length] < 1) 
    return;

	ABAddressBookRef AB = [[ABAddressBook sharedAddressBook] addressBook];
	CFIndex count = ABAddressBookGetPersonCount(AB);
	if (count == 0)
  {
    // Create a new contact
    [self addNewPerson];
  }
  else
  {
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil 
                                  delegate:self 
                                  cancelButtonTitle:NSLocalizedString(@"Cancel",@"Phone View") 
                                  destructiveButtonTitle:nil 
                                  otherButtonTitles:NSLocalizedString(@"Create New Contact",@"Phone View"),
                                  NSLocalizedString(@"Add to Existing Contact",@"Phone View"), nil];
    actionSheet.actionSheetStyle = UIActionSheetStyleBlackTranslucent;

		SiphonAppDelegate *appDelegate = (SiphonAppDelegate *)[[UIApplication sharedApplication] delegate];
		[actionSheet showFromTabBar:appDelegate.tabBarController.tabBar];
		[actionSheet release];
  }
}

#if GSM_BUTTON
- (void)gsmCallButtonPressed:(UIButton*)button
{
  NSURL *url;
  NSString *urlStr;
  if ([[_textfield text] length] > 0)
  {
    urlStr = [NSString stringWithFormat:@"tel://%@",[_textfield text],nil];
    url = [NSURL URLWithString:urlStr];
    [[UIApplication sharedApplication] openURL: url]; 
    //_lastNumber = [NSString stringWithString: [_textfield text]];
    //[_textfield setText:@""];
  }
  else
  {
    _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top"]];
    [_textfield setText:_lastNumber];
    [_lastNumber release];
  }
}
#endif /* GSM_BUTTON */

- (void)stopTimer
{
  if (_deleteTimer)
  {
    [_deleteTimer invalidate];
    [_deleteTimer release];
    _deleteTimer = nil;
  }
  if ([[_textfield text] length] == 0)
  {
    _callButton.enabled = NO;
#if GSM_BUTTON
    if (_gsmCallButton)
      _gsmCallButton.enabled = NO;
#endif /* GSM_BUTTON */
    if (!_textfield.editing)
      _lcd.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"lcd_top_buttons"]];
  }
}

- (void)deleteRepeat
{
  NSString *curText = [_textfield text];
  int length = [curText length];
  if(length > 0)
  {
    _deletedChar++;
    if (_deletedChar == 6)
    {
      [_textfield setText:@""];
    }
    else
    {
      [_textfield setText: [curText substringToIndex:(length-1)]];
    }
#if HTTP_REQUEST
		if (length <= 4)
		{
			self.rateRequest = nil;
			[_lcd topText:@""];
		}
#endif /* HTTP_REQUEST */
  }
  else
  {
    [self stopTimer];
  }
}

- (void)deleteButtonPressed:(UIButton*)unused
{
  _deletedChar = 0;
  [self deleteRepeat];
  _deleteTimer = [[NSTimer scheduledTimerWithTimeInterval:0.2 target:self 
                                                selector:@selector(deleteRepeat) 
                                                userInfo:nil 
                                                repeats:YES] retain];
}

- (void)deleteButtonReleased:(UIButton*)unused
{
  [self stopTimer];
}

- (void)addNewPerson
{
  CFErrorRef error = NULL;
  // Create New Contact
  ABRecordRef person = ABPersonCreate ();
  
  // Add phone number
  ABMutableMultiValueRef multiValue = ABMultiValueCreateMutable(kABStringPropertyType);
  
  ABMultiValueAddValueAndLabel(multiValue, [_textfield text], kABPersonPhoneMainLabel, 
                               NULL);  
  
  ABRecordSetValue(person, kABPersonPhoneProperty, multiValue, &error);
  
	CFRelease(multiValue); // CLANG
  
  ABNewPersonViewController *newPersonCtrl = [[ABNewPersonViewController alloc] init];
	newPersonCtrl.addressBook = [[ABAddressBook sharedAddressBook] addressBook];
  newPersonCtrl.newPersonViewDelegate = self;
  newPersonCtrl.displayedPerson = person;
  CFRelease(person); // TODO check
  
  UINavigationController *navCtrl = [[UINavigationController alloc] 
                                     initWithRootViewController:newPersonCtrl];
  navCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
  [self.parentViewController presentModalViewController:navCtrl animated:YES];
  [newPersonCtrl release];
  [navCtrl release];
}

#pragma mark -
#pragma mark ABNewPersonViewControllerDelegate
- (void)newPersonViewController:(ABNewPersonViewController *)newPersonViewController 
       didCompleteWithNewPerson:(ABRecordRef)person
{
  [newPersonViewController dismissModalViewControllerAnimated:YES];
}

#pragma mark -
#pragma mark ABPeoplePickerNavigationControllerDelegate
- (BOOL)peoplePickerNavigationController:(ABPeoplePickerNavigationController *)peoplePicker
      shouldContinueAfterSelectingPerson:(ABRecordRef)person
{
  CFErrorRef error = NULL;
  //BOOL status;
  ABMutableMultiValueRef multiValue;
  // Inserer le numéro dans la fiche de la personne
  // Add phone number
  CFTypeRef typeRef = ABRecordCopyValue(person, kABPersonPhoneProperty);
  if (ABMultiValueGetCount(typeRef) == 0)
    multiValue = ABMultiValueCreateMutable(kABStringPropertyType);
  else
    multiValue = ABMultiValueCreateMutableCopy (typeRef);
	CFRelease(typeRef); // CLANG
  
  // TODO type (mobile, main...)
  // TODO manage URI
  /*status = */ABMultiValueAddValueAndLabel(multiValue, [_textfield text], kABPersonPhoneMainLabel, 
                               NULL);  
  
  /*status = */ABRecordSetValue(person, kABPersonPhoneProperty, multiValue, &error);
  /*status = */ABAddressBookSave(peoplePicker.addressBook, &error);
	CFRelease(multiValue); // CLANG
  [peoplePicker dismissModalViewControllerAnimated:YES];
  return NO;
}

- (BOOL)peoplePickerNavigationController:(ABPeoplePickerNavigationController *)peoplePicker 
      shouldContinueAfterSelectingPerson:(ABRecordRef)person 
                                property:(ABPropertyID)property 
                              identifier:(ABMultiValueIdentifier)identifier
{
  return NO;
}

- (void)peoplePickerNavigationControllerDidCancel:(ABPeoplePickerNavigationController *)peoplePicker
{
  [peoplePicker dismissModalViewControllerAnimated:YES];
}

#pragma mark -
#pragma mark UIActionSheetDelegate
- (void)actionSheet:(UIActionSheet *)actionSheet 
                    clickedButtonAtIndex:(NSInteger)buttonIndex
{
  switch (buttonIndex) 
  {
    case 0: // Create new contact
      [self addNewPerson];
      break;
    case 1: // Add to existing Contact
      [self presentModalViewController:[self peoplePickerController] animated:YES];
			break;
    default:
      break;
  }
}

#pragma mark -
#pragma mark TextFieldDelegate
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
  [textField resignFirstResponder];
  [_textfield setText:@""];
  _callButton.enabled = NO;
  return YES;
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
  NSRange r = [forbiddenChars rangeOfString: string];
  if (r.location != NSNotFound)
    return NO;
  
  _callButton.enabled = ([[textField text] length] + [string length] - range.length > 0);
  
  return YES;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
  return ([[textField text] length] == 0);
}

@end
