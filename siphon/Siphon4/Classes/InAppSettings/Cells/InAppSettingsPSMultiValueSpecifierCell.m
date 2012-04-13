//
//  PSToggleSwitchSpecifier.m
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import "InAppSettingsPSMultiValueSpecifierCell.h"
#import "InAppSettingsConstants.h"

@implementation InAppSettingsPSMultiValueSpecifierCell

#pragma mark Datasource
- (NSArray *)values {
	NSArray *anArray = [self.setting valueForKey:InAppSettingsSpecifierValues];
	if (anArray)
		return anArray;
	
	NSString *aSelectorString = [self.setting valueForKey:InAppSettingsSpecifierInAppValuesDataSource];
	if ([aSelectorString length]) {
		SEL aSelector = NSSelectorFromString(aSelectorString);
		if ([self.setting.object respondsToSelector:aSelector])
			anArray = [self.setting.object performSelector:aSelector];
	}
	
	return anArray;
}

- (NSArray *)titles {
	NSArray *anArray = [self.setting valueForKey:InAppSettingsSpecifierTitles];
	if (anArray)
		return anArray;
	
	NSString *aSelectorString = [self.setting valueForKey:InAppSettingsSpecifierInAppTitlesDataSource];
	if ([aSelectorString length]) {
		SEL aSelector = NSSelectorFromString(aSelectorString);
		if ([self.setting.object respondsToSelector:aSelector])
			anArray = [self.setting.object performSelector:aSelector];
	}
	
	return anArray;
}

#pragma mark -
- (NSString *)getValueTitle{
		NSArray *titles = [self titles];
		NSArray *values = [self values];
    NSInteger valueIndex = [values indexOfObject:[self.setting getValue]];
    if((valueIndex >= 0) && (valueIndex < (NSInteger)[titles count])){
        return InAppSettingsLocalize([titles objectAtIndex:valueIndex], self.setting.stringsTable); 
    }
    return nil;
}

- (void)setUIValues{
    [super setUIValues];
    
    [self setTitle];
    [self setDetail:[self getValueTitle]];
}

- (void)setupCell{
    [super setupCell];
    
    [self setDisclosure:YES];
    self.canSelectCell = YES;
}

@end
