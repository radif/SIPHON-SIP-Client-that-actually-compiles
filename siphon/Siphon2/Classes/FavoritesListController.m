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

#import "FavoritesListController.h"
#import <AddressBook/AddressBook.h>
#import <AddressBookUI/ABFavoritesList.h>
#import <AddressBookUI/ABFavoritesEntry.h>

#import "FavoritesCell.h"

// define to 1, if you want to allow reodering of favorites.
#define REORDER 1


@implementation FavoritesListController

@synthesize phoneCallDelegate;

- (id)initWithStyle:(UITableViewStyle)style
{
	if (self = [super initWithStyle:style])
  {
    //[self setEditing:YES animated:YES];
    self.title = NSLocalizedString(@"Favorites", @"Favorites View");
#if defined(CYDIA) && (CYDIA == 1)
    self.tabBarItem = [[UITabBarItem alloc] initWithTabBarSystemItem:
                       UITabBarSystemItemFavorites tag:0];
#else
    self.tabBarItem.title = NSLocalizedString(@"Favorites", @"Favorites View");
    self.tabBarItem.image = [UIImage imageNamed:@"Favorites.png"];
#endif
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc]
                                              initWithBarButtonSystemItem:UIBarButtonSystemItemAdd
                                              target:self action:@selector(addContact:)];

    peopleCtrl = [[ABPeoplePickerNavigationController alloc] init];
    peopleCtrl.navigationBar.barStyle = UIBarStyleBlackOpaque;
    peopleCtrl.peoplePickerDelegate = self;
  }
	return self;
}

- (void) addContact: (id)unused
{
  [self presentModalViewController:peopleCtrl animated:YES];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
	return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
  return [[[ABFavoritesList sharedInstance] entries] count];
}

#if 0
#define NAME_TAG 1
#define LABEL_TAG 2

- (UITableViewCell *)tableviewCellWithReuseIdentifier:(NSString *)identifier
{
	/*
	 Create an instance of UITableViewCell and add tagged subviews for the name, local time, and quarter image of the time zone.
	 */
	CGRect rect;
  UITableViewCell *cell;
  UILabel *label;

  // Create a new cell. CGRectZero allows the cell to determine the appropriate size.
  cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero
                                 reuseIdentifier:identifier] autorelease];
  cell.autoresizingMask = UIViewAutoresizingNone;

#define LEFT_COLUMN_OFFSET 10.0
#define LEFT_COLUMN_WIDTH 160.0

#define RIGHT_COLUMN_OFFSET 180.0
#define RIGHT_COLUMN_WIDTH   80.0

#define LABEL_HEIGHT 25.0

	/*
	 Create labels for the text fields; set the highlight color so that when the cell is selected it changes appropriately.
   */
	rect = CGRectMake(LEFT_COLUMN_OFFSET, LABEL_HEIGHT / 2,
                    LEFT_COLUMN_WIDTH, LABEL_HEIGHT);
	label = [[UILabel alloc] initWithFrame:rect];
	label.tag = NAME_TAG;
	label.font = [UIFont boldSystemFontOfSize:[UIFont labelFontSize]];
  //label.baselineAdjustment = UIBaselineAdjustmentAlignCenters;
	label.adjustsFontSizeToFitWidth = YES;
  label.minimumFontSize = [UIFont labelFontSize] - 6;
  label.autoresizingMask = UIViewAutoresizingFlexibleWidth;
  label.highlightedTextColor = [UIColor whiteColor];
	[cell.contentView addSubview:label];
	[label release];

	rect = CGRectMake(RIGHT_COLUMN_OFFSET, LABEL_HEIGHT / 2,
                    RIGHT_COLUMN_WIDTH, LABEL_HEIGHT);
	label = [[UILabel alloc] initWithFrame:rect];
	label.tag = LABEL_TAG;
	label.font = [UIFont boldSystemFontOfSize:[UIFont labelFontSize] - 2.];
	label.textAlignment = UITextAlignmentRight;
  label.autoresizingMask = UIViewAutoresizingNone;
  //label.baselineAdjustment = UIBaselineAdjustmentAlignCenters;
  label.textColor = [UIColor darkGrayColor];
	label.highlightedTextColor = [UIColor whiteColor];
	[cell.contentView addSubview:label];
	[label release];

  return cell;
}
#endif

- (void)tableView:(UITableView *)tableView
didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
  NSUInteger row = indexPath.row;

  if (row != NSNotFound)
  {

    ABFavoritesEntry *entry = [[[ABFavoritesList sharedInstance] entries]
                               objectAtIndex: row];
#if 0
    // person can be deleted from Address Book
    ABRecordRef person = [entry ABPerson];
    multiValue = ABRecordCopyValue(person, [entry property]);
    valueIdx = ABMultiValueGetIndexForIdentifier(multiValue, [entry identifier]);
    NSString *phoneNumber = (NSString *) ABMultiValueCopyValueAtIndex(multiValue,
                                                                      valueIdx);
#endif
    NSString *phoneNumber = [entry value];
    if ([phoneNumber length] &&
        [phoneCallDelegate respondsToSelector:@selector(dialup:number:)])
    {
      // Don't maintain the selection.
      [tableView deselectRowAtIndexPath:indexPath animated:NO];
      [phoneCallDelegate dialup: phoneNumber number:YES];
    }
    else
      // This will give the user visual feedback that the cell was selected but
      // fade out to indicate that no action is taken.
      [tableView deselectRowAtIndexPath:indexPath animated:YES];
  }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	static NSString *cellIdentifier = @"FavoritesCell";
#if 0
  UILabel *label;

	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
	if (cell == nil)
  {
    cell = [self tableviewCellWithReuseIdentifier:cellIdentifier];
    //cell.hidesAccessoryWhenEditing = NO;
    cell.showsReorderControl = YES;
	}
	// Configure the cell
  ABFavoritesEntry *entry = [[[ABFavoritesList sharedInstance] entries]
                             objectAtIndex: indexPath.row];
  label = (UILabel *)[cell viewWithTag:NAME_TAG];
  label.text = [entry displayName];
  label = (UILabel *)[cell viewWithTag:LABEL_TAG];
  label.text = [entry label];
#else
  FavoritesCell *cell = (FavoritesCell *)[tableView dequeueReusableCellWithIdentifier:cellIdentifier];
  if (cell == nil)
  {
    cell = [[[FavoritesCell alloc] initWithFrame:CGRectZero reuseIdentifier:cellIdentifier] autorelease];
    cell.hidesAccessoryWhenEditing = YES;
  }
  // Configure the cell
  ABFavoritesEntry *entry = [[[ABFavoritesList sharedInstance] entries]
                             objectAtIndex: indexPath.row];

  cell.name.text = [entry displayName];
  cell.label.text = [entry label];
#endif
	return cell;
}

- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView
         accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath

{
  return UITableViewCellAccessoryDetailDisclosureButton;
}

- (void)tableView:(UITableView *)tableView
         accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
{
  NSUInteger row = indexPath.row;

  if (row != NSNotFound)
  {
    ABFavoritesEntry *entry = [[[ABFavoritesList sharedInstance] entries]
                               objectAtIndex: row];
    ABRecordRef person = [entry ABPerson];
    if (person)
    {
      ABPersonViewController *personCtrl = [[ABPersonViewController alloc] init];
      personCtrl.displayedPerson = person;
      personCtrl.allowsEditing = NO;
      personCtrl.personViewDelegate = self;
      [personCtrl setHighlightedItemForProperty:[entry property]
                                 withIdentifier:[entry identifier]];
      [self.navigationController pushViewController:personCtrl animated:YES];
      [personCtrl release];
    }
    else
    {
      CFErrorRef *error = NULL;
      person = ABPersonCreate ();

      // Add Name
      if ([[entry displayName] length])
        ABRecordSetValue(person, kABPersonFirstNameProperty, [entry displayName],
                         error);

      // Add Number
      if ([[entry value] length])
      {
        ABMutableMultiValueRef multiValue = ABMultiValueCreateMutable(kABStringPropertyType);
        ABMultiValueAddValueAndLabel(multiValue, [entry value], kABPersonPhoneMainLabel,
                                     NULL);
        ABRecordSetValue(person, kABPersonPhoneProperty, multiValue, error);
      }
      ABUnknownPersonViewController *unknownCtrl = [[ABUnknownPersonViewController alloc] init];
      unknownCtrl.displayedPerson = person;
      unknownCtrl.allowsActions = NO;
      unknownCtrl.allowsAddingToAddressBook = true;
      // TODO implémenter et mettre à jour le nom, l'uid et l'identifier dans l'historique
      //unknownCtrl.unknownPersonViewDelegate = self;
      unknownCtrl.title = [entry displayName];
      CFRelease(person);
      [self.navigationController pushViewController:unknownCtrl animated:YES];
      [unknownCtrl release];

    }
  }
}


- (void)tableView:(UITableView *)tableView
      commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
      forRowAtIndexPath:(NSIndexPath *)indexPath
{

	if (editingStyle == UITableViewCellEditingStyleDelete)
  {
    NSUInteger row = indexPath.row;
    NSArray *array = [NSArray arrayWithObjects: indexPath, nil];

    [[ABFavoritesList sharedInstance] removeEntryAtIndex:row];
    [[ABFavoritesList sharedInstance] save];

    [tableView deleteRowsAtIndexPaths:array
                     withRowAnimation:UITableViewRowAnimationLeft];
	}
//	if (editingStyle == UITableViewCellEditingStyleInsert) {
//	}
}

- (BOOL)tableView:(UITableView *)tableView
canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
	return YES;
}

- (void)tableView:(UITableView *)tableView
moveRowAtIndexPath:(NSIndexPath *)fromIndexPath
      toIndexPath:(NSIndexPath *)toIndexPath
{
  [[ABFavoritesList sharedInstance] moveEntryAtIndex:fromIndexPath.row
                                             toIndex:toIndexPath.row];
  [[ABFavoritesList sharedInstance] save];
}

- (void)dealloc
{
	[super dealloc];
}

// Set up the user interface.
//- (void)viewDidLoad
//{
//  [super viewDidLoad];
//}

// Update the table before the view displays.
- (void)viewWillAppear:(BOOL)animated
{
  [super viewWillAppear:animated];
  [self.tableView reloadData];
  if ([[[ABFavoritesList sharedInstance] entries] count])
    self.navigationItem.leftBarButtonItem = self.editButtonItem;
  else
    self.navigationItem.leftBarButtonItem = nil;
}

//- (void)viewDidAppear:(BOOL)animated
//{
//	[super viewDidAppear:animated];
//}

//- (void)viewWillDisappear:(BOOL)animated
//{
//}

//- (void)viewDidDisappear:(BOOL)animated
//{
//}

// Invoked when the user touches Edit.
- (void)setEditing:(BOOL)editing animated:(BOOL)animated
{
  // Updates the appearance of the Edit|Done button as necessary.
  [super setEditing:editing animated:animated];

  // Disable the add button while editing.
  if (editing)
  {
    self.navigationItem.rightBarButtonItem.enabled = NO;
  }
  else
  {
    self.navigationItem.rightBarButtonItem.enabled = YES;
    if (![[[ABFavoritesList sharedInstance] entries] count])
      self.navigationItem.leftBarButtonItem = nil;
  }
}

//- (void)didReceiveMemoryWarning {
//	[super didReceiveMemoryWarning];
//}

- (BOOL)insertPerson:(ABRecordRef)person
          identifier:(ABMultiValueIdentifier)identifier
{
  ABFavoritesList *list = [ABFavoritesList sharedInstance];

  if ([list containsEntryWithIdentifier: identifier forPerson: person] == NO)
  {
    [list addEntryForPerson:person property:kABPersonPhoneProperty
             withIdentifier:identifier];
    [list save];
    [self.tableView reloadData];

    return YES;
  }
  return NO;
}

- (BOOL)peoplePickerNavigationController:(ABPeoplePickerNavigationController *)peoplePicker
      shouldContinueAfterSelectingPerson:(ABRecordRef)person
{
  // Si un seul numéro de téléphone associer à la liste des favoris.
  // sinon continuer
  CFTypeRef multiValue;

  if (person)
  {
    multiValue = ABRecordCopyValue(person, kABPersonPhoneProperty);
    if (ABMultiValueGetCount (multiValue) == 1)
    {
      ABMultiValueIdentifier identifier =
          ABMultiValueGetIdentifierAtIndex (multiValue, 0);
      [self insertPerson:person identifier:identifier];
      [self dismissModalViewControllerAnimated:YES];
      return NO;
    }
  }

  return YES;
}

- (BOOL)peoplePickerNavigationController:(ABPeoplePickerNavigationController *)peoplePicker
      shouldContinueAfterSelectingPerson:(ABRecordRef)person
                                property:(ABPropertyID)property
                              identifier:(ABMultiValueIdentifier)identifier
{
  if (kABPersonPhoneProperty == property)
  {
    [self insertPerson:person identifier:identifier];
    [self dismissModalViewControllerAnimated:YES];
    return NO;
  }
  return YES;
}

- (void)peoplePickerNavigationControllerDidCancel:(ABPeoplePickerNavigationController *)peoplePicker;
{
  [self dismissModalViewControllerAnimated:YES];
}

#pragma mark ABPersonViewControllerDelegate
- (BOOL)personViewController:(ABPersonViewController *)personViewController
shouldPerformDefaultActionForPerson:(ABRecordRef)person
                    property:(ABPropertyID)property
                  identifier:(ABMultiValueIdentifier)identifier
{
  CFTypeRef multiValue;
  CFIndex valueIdx;

  // FIXME duplicate code from ContactViewController.peoplePickerNavigationController
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

@end

