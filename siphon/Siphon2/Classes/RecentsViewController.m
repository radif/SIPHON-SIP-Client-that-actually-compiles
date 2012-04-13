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

#import "RecentsViewController.h"
#import "SiphonApplication.h"

// Private interface for RecentsViewController - internal only methods.
@interface RecentsViewController (Private)


- (void)createEditableCopyOfDatabaseIfNeeded;
- (void)initializeDatabase;
- (void)finalizeDatabase;
- (void)clearDatabase;
@end

@implementation RecentsViewController{
    NSDateFormatter *dateFormatter;

}

@synthesize phoneCallDelegate;
@synthesize calls;

- (id)initWithStyle:(UITableViewStyle)style 
{
	if (self = [super initWithStyle:style]) 
  {
    // The application ships with a default database in its bundle. If anything in the application
    // bundle is altered, the code sign will fail. We want the database to be editable by users, 
    // so we need to create a copy of it in the application's Documents directory.     
    [self createEditableCopyOfDatabaseIfNeeded];
    // Call internal method to initialize database connection
    [self initializeDatabase];
    
    self.title = NSLocalizedString(@"Recent Calls", @"Recents View");
#if defined(CYDIA) && (CYDIA == 1)
    self.tabBarItem = [[UITabBarItem alloc]
                       initWithTabBarSystemItem: UITabBarSystemItemRecents tag:1];
#else
    self.tabBarItem.title = NSLocalizedString(@"Recents", @"Recents View");
    self.tabBarItem.image = [UIImage imageNamed:@"Recents.png"];
#endif
    //self.navigationItem.leftBarButtonItem = [self editButtonItem];
    self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc]
                                              initWithTitle:NSLocalizedString(@"Clear", @"Recents")
                                               style:UIBarButtonItemStylePlain
                                              target:self action:@selector(clearAll:)]
                                              autorelease];
    
    // segmented control as the custom title view
#if 0
    NSArray *segmentTextContent = [NSArray arrayWithObjects:
                                   NSLocalizedString(@"All", @"Recents View"),
                                   NSLocalizedString(@"Missed", @"Recents View"),
                                   nil];
    UISegmentedControl* segmentedControl = [[UISegmentedControl alloc] initWithItems:segmentTextContent];
    segmentedControl.selectedSegmentIndex = 0;
    //	segmentedControl.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    segmentedControl.segmentedControlStyle = UISegmentedControlStyleBar;
    [segmentedControl addTarget:self action:@selector(segmentAction:) forControlEvents:UIControlEventValueChanged];
    
    self.navigationItem.titleView = segmentedControl;
    [segmentedControl release];
#endif   
    // Create a date formatter to convert the date to a string format.
    dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateStyle:NSDateFormatterShortStyle];
    [dateFormatter setTimeStyle:NSDateFormatterShortStyle];
	}
	return self;
}

/*
 Implement loadView if you want to create a view hierarchy programmatically
 */
//- (void)loadView 
//{
//}

/*
 If you need to do additional setup after loading the view, override viewDidLoad.
 */
//- (void)viewDidLoad 
//{
//}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
	// Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

//- (void)viewWillDisappear:(BOOL)animated
//{
//  [super viewWillDisappear:animated];
//}

- (void)didReceiveMemoryWarning 
{
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
}

- (void)dealloc 
{
  [self finalizeDatabase];
  [calls release];
	[super dealloc];
}

// Update the table before the view displays.
- (void)viewWillAppear:(BOOL)animated 
{
  [super viewWillAppear:animated];
  [self.tableView reloadData];
  if ([calls count])
  {
    self.navigationItem.rightBarButtonItem.enabled = YES;
    self.navigationItem.leftBarButtonItem = self.editButtonItem;
  }
  else
  {
    self.navigationItem.rightBarButtonItem.enabled = NO;
    self.navigationItem.leftBarButtonItem = nil;
  }
}

// Invoked when the user touches Edit.
- (void)setEditing:(BOOL)editing animated:(BOOL)animated 
{
  // Updates the appearance of the Edit|Done button as necessary.
  [super setEditing:editing animated:animated];
  [self.tableView setEditing:editing animated:YES];

  // Disable the clear button while editing.
  if (editing) 
  {
    self.navigationItem.rightBarButtonItem.enabled = NO;
  } 
  else 
  {
    if (![calls count])
    {
      self.navigationItem.leftBarButtonItem = nil;
      //self.navigationItem.rightBarButtonItem.enabled = NO;
    }
    else
      self.navigationItem.rightBarButtonItem.enabled = YES;
  }
}

- (void)segmentAction:(id)sender
{
	UISegmentedControl* segCtl = sender;
	// the segmented control was clicked, handle it here 
	NSLog(@"segment clicked %d", [segCtl selectedSegmentIndex]);
}

- (void)clearAll:(id)sender
{
  UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil 
                                                           delegate:self 
                                                  cancelButtonTitle:NSLocalizedString(@"Cancel",@"Recents View") 
                                             destructiveButtonTitle:NSLocalizedString(@"Clear All Recents",@"Recents View") 
                                                  otherButtonTitles:nil];
  actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
  //[actionSheet showFromTabBar:self.view.superview];
  //[actionSheet showFromTabBar:self.parentViewController.tabBarController.view];
  SiphonApplication *app = (SiphonApplication *)[SiphonApplication sharedApplication];
  [actionSheet showInView:[app window]];
}

- (void)actionSheet:(UIActionSheet *)actionSheet 
        clickedButtonAtIndex:(NSInteger)buttonIndex
{
  switch (buttonIndex) 
  {
    case 0: // Clear All Recent
      [self clearDatabase];
      break;
    case 1: // Cancel  
    default:
      break;
  }
}

// This table will always only have one section.
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tv 
{
  return 1;
}

// One row per call, the number of calls is the number of rows.
- (NSInteger)tableView:(UITableView *)tv numberOfRowsInSection:(NSInteger)section 
{
  return calls.count;
}

// The accessory type is the image displayed on the far right of each table cell. In order for the delegate method
// tableView:accessoryButtonClickedForRowWithIndexPath: to be called, you must return the "Detail Disclosure Button" type.
- (UITableViewCellAccessoryType)tableView:(UITableView *)tv 
         accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath 
{
  /*
  if (indexPath.row != NSNotFound) 
  {
    RecentCall *call = (RecentCall *)[calls objectAtIndex:indexPath.row];
    if (![call.number length])
      return UITableViewCellAccessoryNone;
  }
  */
  return UITableViewCellAccessoryDetailDisclosureButton;
}

- (void)setTitle:(CallType)callType 
   forUIViewController:(UIViewController *)ctrl
{
  switch (callType)
  {
      case Dialled:
        ctrl.title = NSLocalizedString(@"Outgoing Call", @"Recents View");
        break;
      case Received:
        ctrl.title = NSLocalizedString(@"Incoming Call", @"Recents View");
        break;
      case Missed:
        ctrl.title = NSLocalizedString(@"Missed Call", @"Recents View");
        break;
  }
}

- (void)tableView:(UITableView *)tableView 
accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
{
  NSUInteger row = indexPath.row;
  
  if (row == NSNotFound)
    return;

  RecentCall *call = (RecentCall *)[calls objectAtIndex:indexPath.row];
  if (call.uid != kABRecordInvalidID)
  {
    ABAddressBookRef addressBook = ABAddressBookCreate();
    ABRecordRef person = ABAddressBookGetPersonWithRecordID(addressBook,
                                                            call.uid);
    //CFRelease(addressBook);
    if (person)
    {
      ABPersonViewController *personCtrl = [[ABPersonViewController alloc] init];
      personCtrl.displayedPerson = person;
      personCtrl.allowsEditing = NO;
      personCtrl.personViewDelegate = self;
      [self setTitle:call.type forUIViewController:personCtrl];
      
      if (call.identifier != kABMultiValueInvalidIdentifier)
        [personCtrl setHighlightedItemForProperty:kABPersonPhoneProperty 
                                   withIdentifier:call.identifier];
      [self.navigationController pushViewController:personCtrl animated:YES];
      [personCtrl release];
      return;
    }
    call.uid = kABRecordInvalidID;
    call.identifier = kABMultiValueInvalidIdentifier;
  }
  
  CFErrorRef *error = NULL;
  ABRecordRef person = ABPersonCreate ();
  // Keep the call to update his fields if the user adds it to Address Book.
  unknownCall = call;
  
  // Add Name
  if ([call.compositeName length])
    ABRecordSetValue(person, kABPersonFirstNameProperty, call.compositeName, 
                     error);
  
  // Add Number
  if ([call.number length])
  {
    ABMutableMultiValueRef multiValue = ABMultiValueCreateMutable(kABStringPropertyType);
    ABMultiValueAddValueAndLabel(multiValue, call.number, kABPersonPhoneMainLabel, 
                                 NULL);
    ABRecordSetValue(person, kABPersonPhoneProperty, multiValue, error);
  }
  ABUnknownPersonViewController *unknownCtrl = [[ABUnknownPersonViewController alloc] init];
  unknownCtrl.displayedPerson = person;
  unknownCtrl.allowsActions = YES;
  unknownCtrl.allowsAddingToAddressBook = true;
  // TODO implémenter et mettre à jour le nom, l'uid et l'identifier dans l'historique
  unknownCtrl.unknownPersonViewDelegate = self;
  [self setTitle:call.type forUIViewController:unknownCtrl];
  CFRelease(person);
  [self.navigationController pushViewController:unknownCtrl animated:YES];
  [unknownCtrl release];
}

#define NAME_TAG 1
#define DATE_TAG 2

- (UITableViewCell *)tableviewCellWithReuseIdentifier:(NSString *)identifier 
{
  UITableViewCell *cell;
  UILabel *label;
  CGRect rect;
  
  // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
  cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero 
                                 reuseIdentifier:identifier] autorelease];

#define LEFT_COLUMN_OFFSET  10.0
#define LEFT_COLUMN_WIDTH  200.0

#define RIGHT_COLUMN_OFFSET 220.0
#define RIGHT_COLUMN_WITDH   60.0
  
#define LABEL_HEIGHT 25.0
  
  rect = CGRectMake(LEFT_COLUMN_OFFSET, LABEL_HEIGHT/2, LEFT_COLUMN_WIDTH, LABEL_HEIGHT);
  label = [[UILabel alloc] initWithFrame: rect];
  label.tag = NAME_TAG;
  label.font = [UIFont boldSystemFontOfSize:[UIFont labelFontSize]];
  label.adjustsFontSizeToFitWidth = YES;
  label.minimumFontSize = 10.;
  label.textAlignment = UITextAlignmentLeft;
  label.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
  label.highlightedTextColor = [UIColor whiteColor];
  [cell.contentView addSubview:label];
  [label release];
  
  rect = CGRectMake(RIGHT_COLUMN_OFFSET, LABEL_HEIGHT - [UIFont labelFontSize],
                    RIGHT_COLUMN_WITDH, 2*([UIFont labelFontSize])/*LABEL_HEIGHT*/);
  label = [[UILabel alloc] initWithFrame:rect];
  label.tag = DATE_TAG;
  label.font = [UIFont systemFontOfSize:[UIFont labelFontSize] - 4.];
  label.textAlignment = UITextAlignmentCenter;
  label.baselineAdjustment = UIBaselineAdjustmentAlignCenters;
  label.autoresizingMask = UIViewAutoresizingFlexibleRightMargin;// | UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleHeight;
  label.lineBreakMode = UILineBreakModeWordWrap;
  label.numberOfLines = 2;
  label.textColor = [UIColor blueColor];
  label.highlightedTextColor = [UIColor whiteColor];
  [cell.contentView addSubview:label];
  [label release];
  
  return cell;
}

- (UITableViewCell *)tableView:(UITableView *)tableView 
         cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
  static NSString *cellIdentifier = @"RecentsCell";
  UILabel *label;
  
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
  if (cell == nil) 
  {
    cell = [self tableviewCellWithReuseIdentifier:cellIdentifier]; 
  }
  // Retrieve the call object matching the row from the application delegate's array.
  RecentCall *call = (RecentCall *)[calls objectAtIndex:indexPath.row];

  label = (UILabel *)[cell.contentView viewWithTag:NAME_TAG];
  label.text = [call displayName];
  if (call.type == Dialled)
    label.textColor = [UIColor blackColor];
  else
    label.textColor = [UIColor redColor];
  label = (UILabel *)[cell.contentView viewWithTag:DATE_TAG];
  label.text = [dateFormatter stringFromDate:call.date];
  
  return cell;
}

- (void)tableView:(UITableView *)tableView 
didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
  NSUInteger row = indexPath.row;
  
  if (row != NSNotFound) 
  {
    RecentCall *call = [calls objectAtIndex:row];
    NSString *phoneNumber = [call number];
    if ([phoneNumber length] && 
        [phoneCallDelegate respondsToSelector:@selector(dialup:number:)])
    {
      // Don't maintain the selection.
      [tableView deselectRowAtIndexPath:indexPath animated:NO];
      [phoneCallDelegate dialup: phoneNumber number:NO];
    }
    else
      // This will give the user visual feedback that the cell was selected but 
      // fade out to indicate that no action is taken.
      [tableView deselectRowAtIndexPath:indexPath animated:YES];
  }
}

- (void)tableView:(UITableView *)tableView 
commitEditingStyle:(UITableViewCellEditingStyle)editingStyle 
forRowAtIndexPath:(NSIndexPath *)indexPath 
{
  // If row is deleted, remove it from the list.
  if (editingStyle == UITableViewCellEditingStyleDelete) 
  {
    NSUInteger row = indexPath.row;
    if (row != NSNotFound) 
    {
      // Find the book at the deleted row, and remove from application delegate's array.
      RecentCall *call = [calls objectAtIndex:indexPath.row];
      [self removeCall:call];
      // Animate the deletion from the table.
      [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] 
                       withRowAnimation:UITableViewRowAnimationLeft];
    }
  }
}

// Creates a writable copy of the bundled default database in the application Documents directory.
- (void)createEditableCopyOfDatabaseIfNeeded 
{
  // First, test for existence.
  BOOL success;
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSError *error;
#if defined(CYDIA) && (CYDIA == 1)
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [NSString stringWithFormat:@"%@/Siphon", [paths objectAtIndex:0]];
#else
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
#endif
  NSString *writableDBPath = [documentsDirectory stringByAppendingPathComponent:@"recentcalldb.sql"];
  success = [fileManager fileExistsAtPath:writableDBPath];
  if (success) 
    return;
  // The writable database does not exist, so copy the default to the appropriate location.
  NSString *defaultDBPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"recentcalldb.sql"];
  success = [fileManager copyItemAtPath:defaultDBPath toPath:writableDBPath error:&error];
  if (!success) 
  {
    NSAssert1(0, @"Failed to create writable database file with message '%@'.", 
              [error localizedDescription]);
  }
}

- (void)clearDatabase
{
  int success;
  const char *sql = "DELETE FROM recentCall";
  
  success = sqlite3_exec(database, sql, NULL, NULL, NULL);
  if (success != SQLITE_OK)
  {
    NSAssert1(0, @"Error: failed to clear database with message '%s'.", sqlite3_errmsg(database));
  }
  [self.calls removeAllObjects];
  //[self.tableView reloadData];
  [self viewWillAppear:YES];
}

// Open the database connection and retrieve minimal information for all objects.
- (void)initializeDatabase 
{
  NSMutableArray *recentCallArray = [[NSMutableArray alloc] init];
  self.calls = recentCallArray;
  [recentCallArray release];
  // The database is stored in the application bundle.
#if defined(CYDIA) && (CYDIA == 1)
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [NSString stringWithFormat:@"%@/Siphon", [paths objectAtIndex:0]];
#else
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
#endif
  NSString *path = [documentsDirectory stringByAppendingPathComponent:@"recentcalldb.sql"];
  // Open the database. The database was prepared outside the application.
  if (sqlite3_open([path UTF8String], &database) == SQLITE_OK) 
  {
    // Get the primary key for all calls.
    const char *sql = "SELECT pk FROM recentCall ORDER BY date DESC";
    sqlite3_stmt *statement;
    // Preparing a statement compiles the SQL query into a byte-code program in the SQLite library.
    // The third parameter is either the length of the SQL string or -1 to read up to the first null terminator.        
    if (sqlite3_prepare_v2(database, sql, -1, &statement, NULL) == SQLITE_OK) 
    {
      // We "step" through the results - once for each row.
      while (sqlite3_step(statement) == SQLITE_ROW) 
      {
        // The second parameter indicates the column index into the result set.
        int primaryKey = sqlite3_column_int(statement, 0);
        // We avoid the alloc-init-autorelease pattern here because we are in a tight loop and
        // autorelease is slightly more expensive than release. This design choice has nothing to do with
        // actual memory management - at the end of this block of code, all the recentCall objects allocated
        // here will be in memory regardless of whether we use autorelease or release, because they are
        // retained by the calls array.
        RecentCall *call = [[RecentCall alloc] initWithPrimaryKey:primaryKey database:database];
        [calls addObject:call];
        //[calls insertObject:call atIndex:0];
        [call release];
      }
    }
    // "Finalize" the statement - releases the resources associated with the statement.
    sqlite3_finalize(statement);
  } 
  else
  {
    // Even though the open failed, call close to properly clean up resources.
    sqlite3_close(database);
    NSAssert1(0, @"Failed to open database with message '%s'.", sqlite3_errmsg(database));
    // Additional error handling, as appropriate...
  }
}

- (void)finalizeDatabase
{
  [RecentCall finalizeStatements];
  // Close the database.
  if (sqlite3_close(database) != SQLITE_OK) 
  {
    NSAssert1(0, @"Error: failed to close database with message '%s'.", sqlite3_errmsg(database));
  }
}

// Remove a specific recentCall from the array of calls and also from the database.
- (void)removeCall:(RecentCall *)call 
{
  // Delete from the database first. The recentCall knows how to do this (see RecentCall.m)
  [call deleteFromDatabase];
  [calls removeObject:call];
}

// Insert a new recentCall into the database and add it to the array of books.
- (void)addCall:(RecentCall *)call 
{
  // Create a new record in the database and get its automatically generated primary key.
  [call insertIntoDatabase:database];
  //[calls addObject:call];
  [calls insertObject:call atIndex:0];
}

- (BOOL)personViewController:(ABPersonViewController *)personViewController 
shouldPerformDefaultActionForPerson:(ABRecordRef)person 
                    property:(ABPropertyID)property
                  identifier:(ABMultiValueIdentifier)identifier
{
  CFTypeRef multiValue;
  CFIndex valueIdx;
  
  if (kABPersonPhoneProperty == property)
  {
    multiValue = ABRecordCopyValue(person, property);
    valueIdx = ABMultiValueGetIndexForIdentifier(multiValue,identifier);
    NSString *phoneNumber = (NSString *)
    ABMultiValueCopyValueAtIndex(multiValue, valueIdx);
    
    if (phoneNumber && 
        [phoneCallDelegate respondsToSelector:@selector(dialup:number:)])
    {
      [phoneCallDelegate dialup: phoneNumber number:YES];
    }
    
    return NO;
  }
  return YES;
}

- (void)unknownPersonViewController:(ABUnknownPersonViewController *)unknownPersonView 
                 didResolveToPerson:(ABRecordRef)person
{
  if (person)
  {
    // FIXME: duplicate code in CallViewController:createCall
    CFTypeRef multiValue;
    CFIndex index;
    
    unknownCall.compositeName = (NSString *)ABRecordCopyCompositeName(person);
    unknownCall.uid = ABRecordGetRecordID(person);

    multiValue = ABRecordCopyValue(person, kABPersonPhoneProperty);
    index = ABMultiValueGetFirstIndexOfValue (multiValue, unknownCall.number);
    unknownCall.identifier = (index != kABMultiValueInvalidIdentifier ? 
                              ABMultiValueGetIdentifierAtIndex(multiValue, index) :
                              kABMultiValueInvalidIdentifier);
    CFRelease(multiValue);
  }
  unknownCall = nil;
}

@end
