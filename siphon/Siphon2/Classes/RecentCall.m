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

#import "RecentCall.h"


// Static variables for compiled SQL queries. This implementation choice is to be able to share a one time
// compilation of each query across all instances of the class. Each time a query is used, variables may be bound
// to it, it will be "stepped", and then reset for the next usage. When the application begins to terminate,
// a class method will be invoked to "finalize" (delete) the compiled queries - this must happen before the database
// can be closed.
static sqlite3_stmt *insert_statement = nil;
static sqlite3_stmt *init_statement = nil;
static sqlite3_stmt *delete_statement = nil;

@implementation RecentCall


@synthesize number;
@synthesize date;

// FIXME: on pourrait avoir plus d'info, le numéro de téléphone, le type d'appel
// la date, le nom
-(id)init
{
  self = [super init];
  if (self)
  {
    self.type = Undefined;
    self.number = nil;
    self.date = [NSDate date];
    self.compositeName = nil;
    self.identifier = kABMultiValueInvalidIdentifier;
    self.uid = kABRecordInvalidID;
    dirty = YES;
  }
  return self;
}

- (void)dealloc
{
  [compositeName release];
  [number release];
  [date release];
  [super dealloc];
}

// Finalize (delete) all of the SQLite compiled queries.
+ (void)finalizeStatements
{
  if (insert_statement) 
  {
    sqlite3_finalize(insert_statement);
    insert_statement = nil;
  }
  if (init_statement) 
  {
    sqlite3_finalize(init_statement);
    init_statement = nil;
  }
  if (delete_statement) 
  {
    sqlite3_finalize(delete_statement);
    delete_statement = nil;
  }
}

// Creates the object with primary key and title is brought into memory.
- (id)initWithPrimaryKey:(NSInteger)pk database:(sqlite3 *)db
{
  if (self = [super init]) 
  {
    primaryKey = pk;
    database = db;
    // Compile the query for retrieving book data. See insertIntoDatabase: for more detail.
    if (init_statement == nil) 
    {
      // Note the '?' at the end of the query. This is a parameter which can be replaced by a bound variable.
      // This is a great way to optimize because frequently used queries can be compiled once, then with each
      // use new variable values can be bound to placeholders.
      const char *sql = "SELECT number,date, compositeName, type, uid, identifier FROM recentCall WHERE pk=?";

      if (sqlite3_prepare_v2(database, sql, -1, &init_statement, NULL) != SQLITE_OK) 
      {
        NSAssert1(0, @"Error: failed to prepare statement with message '%s'.", sqlite3_errmsg(database));
      }
    }
    // For this query, we bind the primary key to the first (and only) placeholder in the statement.
    // Note that the parameters are numbered from 1, not from 0.
    sqlite3_bind_int(init_statement, 1, primaryKey);
    if (sqlite3_step(init_statement) == SQLITE_ROW) 
    {
      char *tmp = (char *)sqlite3_column_text(init_statement, 0);
      self.number = (tmp ? [NSString stringWithUTF8String:tmp] : @"");
      self.date = [NSDate dateWithTimeIntervalSince1970:sqlite3_column_double(init_statement, 1)];
      tmp = (char *)sqlite3_column_text(init_statement, 2);
      self.compositeName = (tmp) ? [NSString stringWithUTF8String:tmp] : @"";
      self.type = sqlite3_column_int(init_statement, 3);
      self.uid = sqlite3_column_int(init_statement, 4);
      self.identifier = sqlite3_column_int(init_statement, 5);
    }
    else 
    {
      // impossible normalement
      self.number = nil;
      self.date   = nil;
      self.compositeName = @"";
      self.type = 0;
      self.identifier = kABMultiValueInvalidIdentifier;
      self.uid = kABRecordInvalidID;      
    }
    // Reset the statement for future reuse.
    sqlite3_reset(init_statement);
    dirty = NO;
  }
  return self;  
}

// Inserts the book into the database and stores its primary key.
- (void)insertIntoDatabase:(sqlite3 *)db
{
  database = db;
  // This query may be performed many times during the run of the application. 
  // As an optimization, a static variable is used to store the SQLite compiled 
  // byte-code for the query, which is generated one time - the first time the 
  // method is executed by any RecentCall object.
  if (insert_statement == nil) 
  {
    static char *sql = "INSERT INTO recentCall (date, number,compositeName, type, uid, identifier) VALUES(?,?,?,?,?,?)";
    if (sqlite3_prepare_v2(database, sql, -1, &insert_statement, NULL) != SQLITE_OK) 
    {
      NSAssert1(0, @"Error: failed to prepare statement with message '%s'.", sqlite3_errmsg(database));
    }
  }
  
  sqlite3_bind_double(insert_statement, 1, [date timeIntervalSince1970]);
  sqlite3_bind_text(insert_statement, 2, [number UTF8String], -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(insert_statement, 3, [compositeName UTF8String], -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(insert_statement, 4, type);
  sqlite3_bind_int(insert_statement, 5, uid);
  sqlite3_bind_int(insert_statement, 6, identifier);

  int success = sqlite3_step(insert_statement);
  // Because we want to reuse the statement, we "reset" it instead of "finalizing" it.
  sqlite3_reset(insert_statement);
  if (success == SQLITE_ERROR) 
  {
    NSAssert1(0, @"Error: failed to insert into the database with message '%s'.", sqlite3_errmsg(database));
  }
  else
  {
    // SQLite provides a method which retrieves the value of the most recently 
    // auto-generated primary key sequence in the database. To access this 
    // functionality, the table should have a column declared of type 
    // "INTEGER PRIMARY KEY"
    primaryKey = sqlite3_last_insert_rowid(database);
  }
}

// Remove the recent call complete from the database. In memory deletion to 
//follow...
- (void)deleteFromDatabase
{
  // Compile the delete statement if needed.
  if (delete_statement == nil) 
  {
    const char *sql = "DELETE FROM recentCall WHERE pk=?";
    if (sqlite3_prepare_v2(database, sql, -1, &delete_statement, NULL) != SQLITE_OK) 
    {
      NSAssert1(0, @"Error: failed to prepare statement with message '%s'.", 
                sqlite3_errmsg(database));
    }
  }
  // Bind the primary key variable.
  sqlite3_bind_int(delete_statement, 1, primaryKey);
  // Execute the query.
  int success = sqlite3_step(delete_statement);
  // Reset the statement for future use.
  sqlite3_reset(delete_statement);
  // Handle errors.
  if (success != SQLITE_DONE) 
  {
    NSAssert1(0, @"Error: failed to delete from database with message '%s'.", sqlite3_errmsg(database));
  }
}

- (NSString *)displayName
{
  if ([compositeName length])
     return compositeName;
  else if ([number length])
     return number;
  else
    return NSLocalizedString(@"Unknown", @"Recents View");
}

-(void)setType:(CallType)newType
{
  if (newType == type)
    return;
  dirty = YES;
  type = newType;
}

-(CallType)type
{
  return type;
}

- (void)setUid:(ABRecordID)newUid
{
  if (newUid == uid)
    return;
  dirty = YES;
  uid = newUid;
}

- (ABRecordID)uid
{
  return uid;
}

- (void)setIdentifier:(ABMultiValueIdentifier)newIdentifier
{
  if (newIdentifier == identifier)
    return;
  dirty = YES;
  identifier = newIdentifier;
}

- (ABMultiValueIdentifier)identifier
{
  return identifier;
}

- (void)setCompositeName:(NSString *)newName
{
  if ((!compositeName && !newName) || 
      (compositeName && newName && [compositeName isEqualToString:newName])) 
    return;

  dirty = YES;
  [compositeName release];
  compositeName = [newName copy];
}

- (NSString *)compositeName
{
  return compositeName;
}

@end
