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

#import <UIKit/UIKit.h>
#import <AddressBookUI/AddressBookUI.h>
#import <CoreData/CoreData.h>

@interface RecentsViewController : UITableViewController <ABPersonViewControllerDelegate,
	ABUnknownPersonViewControllerDelegate, NSFetchedResultsControllerDelegate,
	UIActionSheetDelegate>
{
	@private
	NSFetchedResultsController *currentFetchedResultsController_;
	NSManagedObjectContext     *managedObjectContext_;
	
	NSDateFormatter *dateFormatter_;
	NSDateFormatter *weekdayFormatter_;
	NSDateFormatter *hourFormatter_;
}

@property (nonatomic, retain) NSFetchedResultsController *currentFetchedResultsController;
@property (nonatomic, retain) NSManagedObjectContext *managedObjectContext;

@property (nonatomic, readonly) NSDateFormatter *dateFormatter;
@property (nonatomic, readonly) NSDateFormatter *weekdayFormatter;
@property (nonatomic, readonly) NSDateFormatter *hourFormatter;

@end
