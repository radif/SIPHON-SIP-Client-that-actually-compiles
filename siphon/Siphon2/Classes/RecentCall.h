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
#import <AddressBook/AddressBook.h>
#import <sqlite3.h>

ABRecordRef ABCFindPersonMatchingPhoneNumber(ABAddressBookRef addressBook,
                                             NSString *phoneNumber,
                                             int, int);
//ABRecordRef ABAddressBookFindPersonMatchingPhoneNumber(ABAddressBookRef addressBook,
//                                                       NSString *phoneNumber,???);
//ABRecordRef ABAddressBookFindPersonMatchingURL(ABAddressBookRef addressBook,
//                                               NSString *url,???);

typedef enum 
{
  Undefined,
  Dialled,
  Received,
  Missed
} CallType;

@interface RecentCall : NSObject
{
  // Opaque reference to the underlying database.
  sqlite3 *database;
  // Primary key in the database.
  NSInteger primaryKey;

  // Dirty tracks whether there are in-memory changes to data which have no been written to the database.
  BOOL dirty;

  CallType  type;
  NSString *number;
  NSDate   *date;
  NSString *compositeName;
  
  ABRecordID             uid;
  ABMultiValueIdentifier identifier;
}
#if 0
- (id)initWithCallNumber:(NSString *)phoneNumber;
#endif
// Finalize (delete) all of the SQLite compiled queries.
+ (void)finalizeStatements;
// Creates the object with primary key and title is brought into memory.
- (id)initWithPrimaryKey:(NSInteger)pk database:(sqlite3 *)db;
// Inserts the book into the database and stores its primary key.
- (void)insertIntoDatabase:(sqlite3 *)db;

// Remove the recent call complete from the database. In memory deletion to 
//follow...
- (void)deleteFromDatabase;


@property (assign)  CallType   type;
@property (nonatomic, retain)  NSString  *number;
@property (nonatomic, retain/*, readonly*/)  NSDate    *date; // TODO en lecture uniquement
@property (nonatomic, retain)  NSString  *compositeName;
@property (assign)  ABRecordID uid;
@property (assign)  ABMultiValueIdentifier identifier;

- (NSString *)displayName;

@end
