/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008 Samuel <samuelv@users.sourceforge.org>
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

#import "FavoritesView.h"
#import <AddressBookUI/ABFavoritesList.h>
#import <AddressBookUI/ABFavoritesEntry.h>

#if 0
extern NSString* const ABFavoritesEntryChangedNotification;
extern NSString* const ABFavoritesListChangedNotification;
#endif

extern NSString* const kABCPhoneProperty;

@implementation FavoritesView

- (id) initWithFrame:(struct CGRect)rect
{
  self = [super initWithFrame:rect];
  if(self != nil) 
  {
//    struct CGSize size = [UINavigationBar defaultSize];
    _navBar = [[UINavigationBar alloc] 
                initWithFrame: CGRectMake(0.0f, 0.0f, 320.0f, 48.0f)];
    [_navBar setBarStyle: 0];
    [_navBar setDelegate:self];
    
    UINavigationItem *navItem = [[UINavigationItem alloc] initWithTitle:
        NSLocalizedString(@"Favorites", @"Favorites view")];
    [_navBar pushNavigationItem: navItem ];
    [navItem release];
    
    _table = [[UITable alloc] initWithFrame: 
        CGRectMake(0.0f, 48.0f, 320.0f, rect.size.height - 48.0f)];
    [_table setDelegate: self];
    [_table setAllowSelectionDuringRowDeletion:NO];
    [_table setShowScrollerIndicators: YES];
    [_table setEventMode:NO];
    [_table setAllowsReordering:YES];
    [_table setAutoresizingMask:NO];
    [_table setReusesTableCells:YES];
    [_table setSeparatorStyle:1];

    [_table setDataSource: self ];
        
    UITableColumn *column = [[UITableColumn alloc]
                              initWithTitle: nil identifier:nil
                              width: rect.size.width];
    [_table addTableColumn: column];
    [_table reloadData];
    
    [self addSubview: _navBar];
    [self addSubview: _table];
    
#if 0    
    [[NSNotificationCenter defaultCenter] addObserver: self 
        selector:@selector(favoritesEntryChanged:) 
        name: ABFavoritesEntryChangedNotification
        object: nil ];
    
    [[NSNotificationCenter defaultCenter] addObserver: self 
        selector:@selector(favoritesListChanged:) 
        name: ABFavoritesListChangedNotification
        object: nil ];
#endif    
  }
  return self;
}


- (void)dealloc
{
  [ _navBar release ];
  [ self dealloc ];
  [ super dealloc ];
}

/*** ***/
- (int)numberOfRowsInTable:(UITable *)table
{
  NSLog(@"numberOfRowsInTable %d", [[[ABFavoritesList sharedInstance] entries] count]);
  return [[[ABFavoritesList sharedInstance] entries] count];
}

- (UITableCell *)table:(UITable *)table cellForRow:(int)row 
    column:(UITableColumn *)col reusing:(id)fp
{
  ABFavoritesEntry *entry;
  UIImageAndTextTableCell *cell = (UIImageAndTextTableCell *)fp;
  if (cell == NULL)
  {
    cell = [[UIImageAndTextTableCell alloc] init];
    [cell autorelease];
  }
  entry = [[[ABFavoritesList sharedInstance] entries] 
                                  objectAtIndex: row];
  [cell setTitle: [entry displayName]];
//    [cell setLabel: [entry label]];
  
  [ cell setShowDisclosure: YES animated: YES];
  [ cell setDisclosureStyle: 1];
      
  return cell;
}

/*** Disclosure ***/
- (void)table:(id)fp8 disclosureClickedForRow:(int)row
{
  ABFavoritesEntry *entry;
  struct CPRecord  *record;
  entry = [[[ABFavoritesList sharedInstance] entries] 
                                    objectAtIndex: row];
  record = [entry ABPerson];
}

- (BOOL)table:(id)fp8 showDisclosureForRow:(int)row
{
  if ([[[[ABFavoritesList sharedInstance] entries] objectAtIndex: row] 
        ABPerson])
  {
    return TRUE;
  }
  return FALSE;
}

- (BOOL)table:(id)fp8 disclosureClickableForRow:(int)fp12
{
  return TRUE;
}

/*** ***/
- (BOOL)table:(id)fp8 canDeleteRow:(int)fp12
{
  return TRUE;
}

- (void)tableSelectionDidChange:(NSNotification *)notification
{
  int row;
  ABFavoritesEntry *entry;

  row = [[notification object] selectedRow];
  entry = [[[ABFavoritesList sharedInstance] entries] objectAtIndex: row];
  if ((int)kABCPhoneProperty == [entry property])
  {
    if ( [_delegate respondsToSelector:@selector(dialup:)]) 
    {
      [_delegate dialup:[entry value]];
    }
  }
  // Unselect current row
  [[notification object] selectRow:-1 byExtendingSelection:FALSE];
}

/*** ***/
#if 0
- (void)favoritesEntryChanged:(NSNotification *)notification 
{
  
}

- (void)favoritesListChanged:(NSNotification *)notification 
{
  
}
#endif    

- (void)setDelegate:(id)delegate 
{
  _delegate = delegate;
}

@end