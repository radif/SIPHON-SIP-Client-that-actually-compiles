//
//  PSMultiValueSpecifierTable.m
//  InAppSettings
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/3/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import "InAppSettingsPSMultiValueSpecifierTable.h"
#import "InAppSettingsConstants.h"

@implementation InAppSettingsPSMultiValueSpecifierTable

@synthesize setting;

- (id)initWithStyle:(UITableViewStyle)style{
    return [super initWithStyle:UITableViewStyleGrouped];
}

- (id)initWithSetting:(InAppSettingsSpecifier *)inputSetting{
    self = [super init];
    if (self != nil){
        self.setting = inputSetting;
    }
    return self;
}

- (void)viewDidLoad{
    [super viewDidLoad];
    
    self.title = [self.setting localizedTitle];
}

- (void)dealloc{
    [setting release];
    [super dealloc];
}

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

#pragma mark Value

- (id)getValue{
    id value = [[NSUserDefaults standardUserDefaults] valueForKey:[self.setting getKey]];
    if(value == nil){
        value = [self.setting valueForKey:InAppSettingsSpecifierDefaultValue];
    }
    return value;
}

- (void)setValue:(id)newValue{
    //[[NSUserDefaults standardUserDefaults] setObject:newValue forKey:[self.setting getKey]];
		[self.setting setValue:newValue];
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [[self values] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString *CellIdentifier = @"PSMultiValueSpecifierTableCell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil){
        #if InAppSettingsUseNewCells
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
        #else
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
        #endif
    }
	
    NSString *cellTitle = InAppSettingsLocalize([[self titles] objectAtIndex:indexPath.row], self.setting.stringsTable);
    id cellValue = [[self values] objectAtIndex:indexPath.row];
    #if InAppSettingsUseNewCells
    cell.textLabel.text = cellTitle;
    #else
    cell.text = cellTitle;
    #endif
	if([cellValue isEqual:[self getValue]]){
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
        #if InAppSettingsUseNewCells
        cell.textLabel.textColor = InAppSettingsBlue;
        #else
        cell.textColor = InAppSettingsBlue;
        #endif
    }
    else{
        cell.accessoryType = UITableViewCellAccessoryNone;
        #if InAppSettingsUseNewCells
        cell.textLabel.textColor = [UIColor blackColor];
        #else
        cell.textColor = [UIColor blackColor];
        #endif
    }
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    [tableView deselectRowAtIndexPath:[tableView indexPathForSelectedRow] animated:YES];
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    id cellValue = [[self values] objectAtIndex:indexPath.row];
    [self setValue:cellValue];
    [self.tableView reloadData];
    return indexPath;
}

#pragma mark -
#pragma mark Responding to View Events
- (void)viewWillAppear:(BOOL)animated
{
	// Alloc object
	// load Values and Titles arrays
	[super viewWillAppear:animated];
}

@end

