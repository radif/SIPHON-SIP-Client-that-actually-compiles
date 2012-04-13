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

#import "SiphonCoreDataStorage.h"
#import <UIKit/UIKit.h>

#import "NSManagedObject+Additions.h"


@implementation SiphonCoreDataStorage

#pragma mark -
#pragma mark Core Data stack

- (NSString *)applicationDocumentsDirectory
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *result = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
	
	NSFileManager *fileManager = [NSFileManager defaultManager];
	
	if(![fileManager fileExistsAtPath:result])
		[fileManager createDirectoryAtPath:result withIntermediateDirectories:YES attributes:nil error:nil];
	
	return result;
}

/**
 Returns the managed object context for the application.
 If the context doesn't already exist, it is created and bound to the persistent store coordinator for the application.
 */
- (NSManagedObjectContext *) managedObjectContext 
{	
	if (managedObjectContext_ == nil) 
	{
		NSPersistentStoreCoordinator *coordinator = [self persistentStoreCoordinator];
		if (coordinator != nil) 
		{
			managedObjectContext_ = [[NSManagedObjectContext alloc] init];
			[managedObjectContext_ setPersistentStoreCoordinator: coordinator];
		}
	}
	return managedObjectContext_;
}


/**
 Returns the managed object model for the application.
 If the model doesn't already exist, it is created by merging all of the models 
 found in the application bundle.
 */
- (NSManagedObjectModel *)managedObjectModel 
{	
	if (managedObjectModel_ == nil)
	{
		managedObjectModel_ = [[NSManagedObjectModel mergedModelFromBundles:nil] retain];
	}
	return managedObjectModel_;
}


/**
 Returns the persistent store coordinator for the application.
 If the coordinator doesn't already exist, it is created and the application's store added to it.
 */
- (NSPersistentStoreCoordinator *)persistentStoreCoordinator 
{
	static BOOL firstPass = YES;
	if (persistentStoreCoordinator_ == nil) 
	{
		NSString *storePath = [[self applicationDocumentsDirectory] stringByAppendingPathComponent: @"CoreDataSiphon.sqlite"];
		/*
		 Set up the store.
		 For the sake of illustration, provide a pre-populated default store.
		 */
		NSFileManager *fileManager = [NSFileManager defaultManager];
		// If the expected store doesn't exist, copy the default store.
		if (![fileManager fileExistsAtPath:storePath]) 
		{
			NSString *defaultStorePath = [[NSBundle mainBundle] pathForResource:@"CoreDataSiphon" ofType:@"sqlite"];
			if (defaultStorePath) 
			{
				[fileManager copyItemAtPath:defaultStorePath toPath:storePath error:NULL];
			}
		}
		
		NSURL *storeUrl = [NSURL fileURLWithPath:storePath];
		
		NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], NSMigratePersistentStoresAutomaticallyOption, [NSNumber numberWithBool:YES], NSInferMappingModelAutomaticallyOption, nil];	
		persistentStoreCoordinator_ = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel: [self managedObjectModel]];
		
		NSError *error;
		if (![persistentStoreCoordinator_ addPersistentStoreWithType:NSSQLiteStoreType 
																									 configuration:nil 
																														 URL:storeUrl 
																												 options:options 
																													 error:&error]) 
		{
			if (firstPass &&
					[fileManager fileExistsAtPath:storePath])
			{
				firstPass = NO;
				
				UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Database Changed", @"Database Changed")
																												message:NSLocalizedString(@"The local database metadata changed, we clean the previous version.", @"The local database metadata changed, we clean the previous versionx@")
																											 delegate:nil
																							cancelButtonTitle:@"OK"
																							otherButtonTitles:nil];
				[alert show];
				[alert release];
				
				/* Delete the data store and try again */
				[fileManager removeItemAtPath:storePath error:nil];
				
				[persistentStoreCoordinator_ release];
				persistentStoreCoordinator_ = nil;
				
				return [self persistentStoreCoordinator];
			}
			else 
			{
				// Update to handle the error appropriately.
				//NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
				//exit(-1);  // Fail
				UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Fatal database error", @"Fatal database error")
																												message:[NSString stringWithFormat:NSLocalizedString(@"Unable to initialize database: %@", @"Unable to initialize database"), [error localizedDescription]]
																											 delegate:nil
																							cancelButtonTitle:NSLocalizedString(@"OK", @"OK")
																							otherButtonTitles:nil];
				[alert performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
				[alert release];
			}
		} 
	}
	
	return persistentStoreCoordinator_;
}

#pragma mark -
#pragma mark Memory management

- (void)dealloc 
{	
	[managedObjectContext_ release];
	[managedObjectModel_ release];
	[persistentStoreCoordinator_ release];

	[super dealloc];
}

#pragma mark -

- (NSManagedObject *)insertNewRecentCallFromDictionary:(NSDictionary *)aDictionary
{
	NSManagedObject *managedObject = [NSEntityDescription
																		insertNewObjectForEntityForName:@"RecentCall"
																		inManagedObjectContext:[self managedObjectContext]];
	[self updateManagedObject:managedObject fromDictionary:aDictionary];
	
	return [managedObject retain];
}

- (void)updateManagedObject:(NSManagedObject *)object fromDictionary:(NSDictionary *)aDictionary
{
	[object updateValuesFromDictionary:aDictionary];

	NSError *error = nil;
	if (![[self managedObjectContext] save:&error])
	{
		NSLog(@"Unresolved error %@, %@", [error localizedDescription], [error localizedRecoverySuggestion]);
		[error release];
	}
}

@end
