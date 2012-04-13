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

#import <UIKit/UIKit.h>
#import <AddressBookUI/AddressBookUI.h>
#import <sqlite3.h>

#import "PhoneCallDelegate.h"
#import "RecentCall.h"

@interface RecentsViewController : UITableViewController <UIActionSheetDelegate,
  ABPersonViewControllerDelegate, ABUnknownPersonViewControllerDelegate>
//<UIActionSheetDelegate,
//    UITableViewDelegate, UITableViewDataSource>
{
  id<PhoneCallDelegate> phoneCallDelegate;
  
  NSMutableArray *calls;
  // Opaque reference to the SQLite database.
  sqlite3 *database;
  
  @private
  RecentCall *unknownCall;
}

@property (nonatomic, retain)  id<PhoneCallDelegate> phoneCallDelegate;
// Makes the main array of recentCall objects available to other objects in the application.
@property (nonatomic, retain) NSMutableArray *calls;

// Creates a new recentCall object with default data. 
- (void)addCall:(RecentCall *)call;
// Removes a recentCall from the array of calls, and also deletes it from the 
// database. There is no undo.
- (void)removeCall:(RecentCall *)call;

@end
