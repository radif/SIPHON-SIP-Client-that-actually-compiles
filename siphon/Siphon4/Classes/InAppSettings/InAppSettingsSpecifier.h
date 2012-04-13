//
//  InAppSetting.h
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface InAppSettingsSpecifier : NSObject {
	NSString *stringsTable;
	NSDictionary *settingDictionary;
	@package
	id object;
}

@property (nonatomic, copy) NSString *stringsTable;
@property (nonatomic, retain) id object;

- (NSString *)getKey;
- (NSString *)getType;
- (BOOL)isType:(NSString *)type;
- (id)getValue;
- (void)setValue:(id)newValue;
- (id)valueForKey:(NSString *)key;

- (NSString *)localizedTitle;
- (NSString *)localizedFooter;
- (NSString *)cellName;

- (BOOL)hasTitle;
- (BOOL)hasKey;
- (BOOL)hasDefaultValue;
- (BOOL)isValid;

- (id)initWithDictionary:(NSDictionary *)dictionary andStringsTable:(NSString *)table;

@end
