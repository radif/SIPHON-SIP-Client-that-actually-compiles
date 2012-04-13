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
#import <objc/runtime.h> // For optimized singleton

#import "ABAddressBook.h"
#import "ABSearchElement.h"
#import "ABSearchElement+Private.h"
#import "ABRecord+Private.h"

static volatile SVAddressBook *sharedAddressBook_ = nil;

@implementation SVAddressBook

@synthesize addressBook = addressBook_;

#pragma mark -
#pragma mark SVAddressBook singleton instance

#ifndef __clang_analyzer__
+ (SVAddressBook *)sharedAddressBookSync {	
  @synchronized(self) {
    if (sharedAddressBook_ == nil)
      [[self alloc] init];  // Assignment not done here.
  }
  
  return (SVAddressBook *)sharedAddressBook_;
}
#endif // __clang_analyzer__

+ (SVAddressBook *)sharedAddressBookNoSync {
	return (SVAddressBook *)sharedAddressBook_;
}

+ (SVAddressBook *)sharedAddressBook {
	return [self sharedAddressBookSync];
}


+ (id)allocWithZone:(NSZone *)zone {
  @synchronized(self) {
    if (sharedAddressBook_ == nil) {
      sharedAddressBook_ = [super allocWithZone:zone];

			Method newSharedInstanceMethod = class_getClassMethod(self, @selector(sharedAddressBookNoSync));
			method_setImplementation(class_getClassMethod(self, @selector(sharedAddressBook)), method_getImplementation(newSharedInstanceMethod));
			method_setImplementation(class_getInstanceMethod(self, @selector(retainCount)), class_getMethodImplementation(self, @selector(retainCountDoNothing)));
			method_setImplementation(class_getInstanceMethod(self, @selector(release)), class_getMethodImplementation(self, @selector(releaseDoNothing)));
			method_setImplementation(class_getInstanceMethod(self, @selector(autorelease)), class_getMethodImplementation(self, @selector(autoreleaseDoNothing)));		
    }
  }
  
  return sharedAddressBook_;  // On subsequent allocation attempts return nil.
}

+ (void) purgeSharedAddressBook 
{
	@synchronized(self) {
		if (sharedAddressBook_ != nil) {
			Method newSharedInstanceMethod = class_getClassMethod(self, @selector(sharedInstanceSynch));
			method_setImplementation(class_getClassMethod(self, @selector(sharedAddressBook)), method_getImplementation(newSharedInstanceMethod));
			method_setImplementation(class_getInstanceMethod(self, @selector(retainCount)), class_getMethodImplementation(self, @selector(retainCountDoSomething)));
			method_setImplementation(class_getInstanceMethod(self, @selector(release)), class_getMethodImplementation(self, @selector(releaseDoSomething)));
			method_setImplementation(class_getInstanceMethod(self, @selector(autorelease)), class_getMethodImplementation(self, @selector(autoreleaseDoSomething)));
			[sharedAddressBook_ release];
			sharedAddressBook_ = nil; 
		}
	}
}

- (id)copyWithZone:(NSZone *)zone {
  return self;
}

- (id)retain {
  return self;
}

- (NSUInteger)retainCount {
	NSAssert1(1==0, @"SynthesizeSingleton: %@ ERROR: -(NSUInteger)retainCount method did not get swizzled.", self);
  return NSUIntegerMax;  // Denotes an object that cannot be released.
}

- (NSUInteger)retainCountDoNothing {
	return NSUIntegerMax;
}

- (NSUInteger)retainCountDoSomething {
	return [super retainCount];
}

- (void)release {
  // Do nothing.
	NSAssert1(1==0, @"SynthesizeSingleton: %@ ERROR: -(void)release method did not get swizzled.", self);
}

- (void)releaseDoNothing{
}

- (void)releaseDoSomething {
	@synchronized(self) {
		[super release];
	}
}

- (id)autorelease {
	NSAssert1(1==0, @"SynthesizeSingleton: %@ ERROR: -(id)autorelease method did not get swizzled.", self); \
	return self;
}

- (id)autoreleaseDoNothing {
	return self;
} 

- (id)autoreleaseDoSomething {
	return [super autorelease];
}

#pragma mark -

- (id)init 
{ 
  self = [super init];
  if (self)
  {
		addressBook_ = ABAddressBookCreate();
	}
	return self;
}

- (void)dealloc
{
	if (addressBook_)
		CFRelease(addressBook_);
	addressBook_ = nil;
	[super dealloc];
}

- (NSArray *)recordsMatchingSearchElement:(SVSearchElement *)search
{
	NSMutableArray *group = [NSMutableArray array];
	NSArray *people = (NSArray*)ABAddressBookCopyArrayOfAllPeople(addressBook_);
	
	ABRecordRef recordRef = nil;
	NSEnumerator *enumerator = [people objectEnumerator];
	while (recordRef = [enumerator nextObject])
  {
		SVRecord *record = [[SVRecord alloc] initWithRecord:recordRef];
		if ([search matchesRecord:record])
		{
			//SVRecord *person = [[SVRecord alloc] initWithRecord:record];
			[record setProperty:[search property]];
			[group addObject:record];
		}
		[record release];
	}
	[people release]; // CLANG
	
	return [NSArray arrayWithArray:group];
}

- (BOOL)save
{
	NSError *error;
	BOOL ret = ABAddressBookSave(addressBook_, (CFErrorRef *)&error);
	if (!ret)
	{
		NSLog(@"ABAddressBookSave error: %@", [error localizedDescription]); 
		[error release];
	}
	
	return ret;
}

@end
