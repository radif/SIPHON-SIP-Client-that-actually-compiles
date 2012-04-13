//
//  InAppSettingsViewController.m
//  InAppSettings
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import "InAppSettings.h"
#import "InAppSettingsPSMultiValueSpecifierTable.h"
#import "InAppSettingsWebViewController.h"

#import <objc/runtime.h> // For optimized singleton

@implementation InAppSettings

static InAppSettings *sharedInstance_ = nil;

+ (void)registerDefaults{
    [[[InAppSettingsReaderRegisterDefaults alloc] init] release];
}

#pragma mark -
#pragma mark Singleton

#pragma mark -
#pragma mark InAppSettings singleton instance

#ifndef __clang_analyzer__
+ (InAppSettings *)sharedManagerSync {
  @synchronized(self) {
    if (sharedInstance_ == nil)
      [[self alloc] init];  // Assignment not done here.
  }
  
  return (InAppSettings *)sharedInstance_;
}
#endif // __clang_analyzer__

+ (InAppSettings *)sharedManagerNoSync {
	return (InAppSettings *)sharedInstance_;
}

+ (InAppSettings *)sharedManager {
	return [self sharedManagerSync];
}

+ (id)allocWithZone:(NSZone *)zone {
  @synchronized(self) {
    if (sharedInstance_ == nil) {
      sharedInstance_ = [super allocWithZone:zone];
			Method newSharedInstanceMethod = class_getClassMethod(self, @selector(sharedManagerNoSync));
			method_setImplementation(class_getClassMethod(self, @selector(sharedManager)), method_getImplementation(newSharedInstanceMethod));
    }
  }
  
  return sharedInstance_;  // On subsequent allocation attempts return nil.
}

- (id)copyWithZone:(NSZone *)zone {
  return self;
}

- (id)retain {
  return self;
}

- (NSUInteger)retainCount {
  return NSUIntegerMax;  // Denotes an object that cannot be released.
}

- (void)release {
  // Do nothing.
}

- (id)autorelease {
  return self;
}

@end

#pragma mark -
@implementation InAppSettingsModalViewController

- (id)init{
#if InAppSettingsUseUITableViewController
	InAppSettingsViewController *settings = [[InAppSettingsViewController alloc] initWithStyle:UITableViewStyleGrouped];
#else
    InAppSettingsViewController *settings = [[InAppSettingsViewController alloc] init];
#endif
		self = [[UINavigationController alloc] initWithRootViewController:settings];
    [settings addDoneButton];
    [settings release];
    return self;
}

@end

@implementation InAppSettingsViewController

@synthesize file;
#if !InAppSettingsUseUITableViewController
@synthesize settingsTableView;
@synthesize firstResponder;
#endif
@synthesize settingsReader;

#pragma mark modal view

- (void)dismissModalView{
    [self.navigationController dismissModalViewControllerAnimated:YES];
}

- (void)addDoneButton{
    UIBarButtonItem *doneButton = [[UIBarButtonItem alloc] 
                                   initWithBarButtonSystemItem:UIBarButtonSystemItemDone 
                                   target:self 
                                   action:@selector(dismissModalView)];
    self.navigationItem.rightBarButtonItem = doneButton;
    [doneButton release];
}

#pragma mark setup view

- (id)initWithFile:(NSString *)inputFile{
#if InAppSettingsUseUITableViewController
	self = [super initWithStyle:UITableViewStyleGrouped];
#else
    self = [super init];
#endif
    if (self != nil){
        self.file = inputFile;
    }
    return self;
}

- (void)viewDidLoad{
#if InAppSettingsUseUITableViewController
	[super viewDidLoad];
#else
    //setup the table
    self.settingsTableView = [[UITableView alloc] initWithFrame:self.view.bounds style:UITableViewStyleGrouped];
    self.settingsTableView.autoresizingMask = (UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight);
    self.settingsTableView.delegate = self;
    self.settingsTableView.dataSource = self;
    [self.view addSubview:self.settingsTableView];
#endif
    
    //if the title is nil set it to Settings
    if(!self.title){
        self.title = NSLocalizedString(@"Settings", nil);
    }
    
    //load settigns plist
    if(!self.file){
        self.file = InAppSettingsRootFile;
    }
    
    self.settingsReader = [[InAppSettingsReader alloc] initWithFile:self.file];
    
#if !InAppSettingsUseUITableViewController
    //setup keyboard notification
    self.firstResponder = nil;
    [self registerForKeyboardNotifications];
#endif
}

#if InAppSettingsUseUITableViewController
- (void)viewWillAppear:(BOOL)animated 
{
	[self.tableView reloadData];
	[super viewWillAppear:animated];
}
#else
- (void)viewWillAppear:(BOOL)animated {
    self.firstResponder = nil;
    
    self.settingsTableView.contentInset = UIEdgeInsetsZero;
    self.settingsTableView.scrollIndicatorInsets = UIEdgeInsetsZero;
    
    [self.settingsTableView reloadData];
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated{
    self.firstResponder = nil;
    [super viewWillDisappear:animated];
}
#endif

- (void)dealloc{
	
#if !InAppSettingsUseUITableViewController
    self.firstResponder = nil;
#endif
    [file release];
#if !InAppSettingsUseUITableViewController
    [settingsTableView release];
#endif
    [settingsReader release];
    [super dealloc];
}

#if InAppSettingsUseUITableViewController
#pragma mark text field cell delegate
- (BOOL)textFieldShouldReturn:(UITextField *)cellTextField{
	[cellTextField resignFirstResponder];
	return YES;
}
#else /* InAppSettingsUseUITableViewController */
#pragma mark text field cell delegate

- (void)textFieldDidBeginEditing:(UITextField *)cellTextField{
    self.firstResponder = cellTextField;
    
    //TODO: find a better way to get the cell from the text view
    NSIndexPath *indexPath = [self.settingsTableView indexPathForCell:(UITableViewCell *)[[cellTextField superview] superview]];
    [self.settingsTableView scrollToRowAtIndexPath:indexPath atScrollPosition:UITableViewScrollPositionMiddle animated:YES];
}

- (BOOL)textFieldShouldReturn:(UITextField *)cellTextField{
    self.firstResponder = nil;
    [cellTextField resignFirstResponder];
    return YES;
}

#pragma mark keyboard notification

- (void)registerForKeyboardNotifications{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillShow:)
                                                 name:UIKeyboardWillShowNotification object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillHide:)
                                                 name:UIKeyboardWillHideNotification object:nil];
}

- (void)keyboardWillShow:(NSNotification*)notification{
    if(self.firstResponder == nil){
        // get the keybaord rect
#if InAppSettingsUseNewKeyboard
        CGRect keyboardRect = [[[notification userInfo] objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
#else
        CGRect keyboardRect = [[[notification userInfo] objectForKey:UIKeyboardBoundsUserInfoKey] CGRectValue];
#endif
        // determin the bottom inset for the table view
        UIEdgeInsets settingsTableInset = self.settingsTableView.contentInset;
        CGPoint tableViewScreenSpace = [self.settingsTableView.superview convertPoint:self.settingsTableView.frame.origin toView:nil];
        CGFloat tableViewBottomOffset = InAppSettingsScreenHeight-(tableViewScreenSpace.y+self.settingsTableView.frame.size.height);
        settingsTableInset.bottom = keyboardRect.size.height-tableViewBottomOffset;
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:InAppSettingsKeyboardAnimation];
        [UIView setAnimationBeginsFromCurrentState:YES];
        self.settingsTableView.contentInset = settingsTableInset;
        self.settingsTableView.scrollIndicatorInsets = settingsTableInset;
        [UIView commitAnimations];
    }
}

- (void)keyboardWillHide:(NSNotification*)notification{
    if(self.firstResponder == nil){
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:InAppSettingsKeyboardAnimation];
        [UIView setAnimationBeginsFromCurrentState:YES];
        self.settingsTableView.contentInset = UIEdgeInsetsZero;
        self.settingsTableView.scrollIndicatorInsets = UIEdgeInsetsZero;
        [UIView commitAnimations];
    }
}

#endif /* InAppSettingsUseUITableViewController */

#pragma mark Table view methods
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
- (NSArray *)dynamicContentsFromSettings:(InAppSettingsSpecifier *)settingsSpecifier {

	NSString *aSelectorString = [settingsSpecifier valueForKey:InAppSettingsSpecifierInAppContentsDataSource];
	
	if ([aSelectorString length]) {
		SEL aSelector = NSSelectorFromString(aSelectorString);
		
		if ([settingsSpecifier.object respondsToSelector:aSelector])
			return [settingsSpecifier.object performSelector:aSelector];
	}
	return nil;
}
#endif /* DYNAMIC_CONTENT_CELLS */


- (InAppSettingsSpecifier *)settingAtIndexPath:(NSIndexPath *)indexPath{
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
	id obj = [self.settingsReader.hasDynamicContents objectAtIndex:indexPath.section];
	if (![obj boolValue])
		return [[self.settingsReader.settings objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
	
	int row = indexPath.row;
	NSArray *settings = [self.settingsReader.settings objectAtIndex:indexPath.section];

	int num = 0;
	for (InAppSettingsSpecifier *settingsSpecifier in settings) {
		if ([settingsSpecifier isType:InAppSettingsPSDynamicPaneSpecifier]) {
			NSArray *anArray = [self dynamicContentsFromSettings:settingsSpecifier];
			if (row < num + [anArray count])
				return [anArray objectAtIndex:row-num];
			num += [anArray count];
			//[anArray release]; // CLANG
		}
		else {
			if (num == row)
				return settingsSpecifier;
			num += 1;
		}
	}

	return nil;
#else
	return [[self.settingsReader.settings objectAtIndex:indexPath.section] objectAtIndex:indexPath.row];
#endif
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return [self.settingsReader.headers count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
	return [self.settingsReader.headers objectAtIndex:section];
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section{
	return [self.settingsReader.footers objectAtIndex:section];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
	int count;
	NSArray *theSettings = [self.settingsReader.settings objectAtIndex:section];
	count = [theSettings count];
	for (InAppSettingsSpecifier *settingSpecifier in theSettings) {
		//NSString *type = [settingSpecifier getType];
		if ([settingSpecifier isType:InAppSettingsPSDynamicPaneSpecifier]) {
			NSArray *anArray = [self dynamicContentsFromSettings:settingSpecifier];
			count -= 1; // FIXME What happen if anArray is undefined or empty
			count += [anArray count];
			//[anArray release]; // CLANG
		}
	}
	return count;
#else /* DYNAMIC_CONTENT_CELLS */
	return [[self.settingsReader.settings objectAtIndex:section] count];
#endif /* DYNAMIC_CONTENT_CELLS */
}

#if InAppSettingsDisplayPowered
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    if(/*InAppSettingsDisplayPowered &&*/ [self.file isEqualToString:InAppSettingsRootFile] && section == (NSInteger)[self.settingsReader.headers count]-1){
        return InAppSettingsPowerFooterHeight;
    }
	return 0.0f;
}


- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section {
    if(/*InAppSettingsDisplayPowered &&*/ [self.file isEqualToString:InAppSettingsRootFile] && section == (NSInteger)[self.settingsReader.headers count]-1){
        UIView *powerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, InAppSettingsScreenWidth, InAppSettingsPowerFooterHeight)];
        
        //InAppSettings label
        CGSize InAppSettingsSize = [InAppSettingsProjectName sizeWithFont:InAppSettingsFooterFont];
        CGPoint InAppSettingsPos = CGPointMake((CGFloat)round((InAppSettingsScreenWidth*0.5f)-(InAppSettingsSize.width*0.5f)), 
                                               (CGFloat)round((InAppSettingsPowerFooterHeight*0.5f)-(InAppSettingsSize.height*0.5f))-1);
        UILabel *InAppLabel = [[UILabel alloc] initWithFrame:CGRectMake(InAppSettingsPos.x, InAppSettingsPos.y, InAppSettingsSize.width, InAppSettingsSize.height)];
        InAppLabel.text = InAppSettingsProjectName;
        InAppLabel.font = InAppSettingsFooterFont;
        InAppLabel.backgroundColor = [UIColor clearColor];
        InAppLabel.textColor = InAppSettingsFooterBlue;
        InAppLabel.shadowColor = [UIColor whiteColor];
        InAppLabel.shadowOffset = CGSizeMake(0.0f, 1.0f);
        [powerView addSubview:InAppLabel];
        [InAppLabel release];
        
        //lighting bolts
        CGPoint leftLightningBoltPos = CGPointMake(InAppSettingsPos.x-InAppSettingsLightingBoltSize,
                                               (CGFloat)round((InAppSettingsPowerFooterHeight*0.5f)-(InAppSettingsLightingBoltSize*0.5f)));
        InAppSettingsLightningBolt *leftLightningBolt = [[InAppSettingsLightningBolt alloc] 
                                                         initWithFrame:CGRectMake(leftLightningBoltPos.x, leftLightningBoltPos.y, 
                                                                                  InAppSettingsLightingBoltSize, InAppSettingsLightingBoltSize)];
        [powerView addSubview:leftLightningBolt];
        [leftLightningBolt release];
        
        CGPoint rightLightningBoltPos = CGPointMake((CGFloat)round(InAppSettingsPos.x+InAppSettingsSize.width), leftLightningBoltPos.y);
        InAppSettingsLightningBolt *rightLightningBolt = [[InAppSettingsLightningBolt alloc] 
                                                          initWithFrame:CGRectMake(rightLightningBoltPos.x, rightLightningBoltPos.y, 
                                                                                   InAppSettingsLightingBoltSize, InAppSettingsLightingBoltSize)];
        rightLightningBolt.flip = YES;
        [powerView addSubview:rightLightningBolt];
        [rightLightningBolt release];
        
        return [powerView autorelease];
    }
    return nil;
}
#endif /* InAppSettingsDisplayPowered */

- (UITableViewCell *)tableView:(UITableView *)tableView 
				 cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    InAppSettingsSpecifier *setting = [self settingAtIndexPath:indexPath];
    
    //get the NSClass for a specifier, if there is none use the base class InAppSettingsTableCell
    NSString *cellType = [setting cellName];
    Class nsclass = NSClassFromString(cellType);
    if(!nsclass){
        cellType = @"InAppSettingsTableCell";
        nsclass = NSClassFromString(cellType);
    }
    
    InAppSettingsTableCell *cell = ((InAppSettingsTableCell *)[tableView dequeueReusableCellWithIdentifier:cellType]);
    if (cell == nil){
        cell = [[[nsclass alloc] initWithReuseIdentifier:cellType] autorelease];
        //setup the cells controlls
        [cell setupCell];
    }
    
    //set the values of the cell, this is separated from setupCell for reloading the table
    cell.setting = setting;
    [cell setValueDelegate:self];
    [cell setUIValues];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    InAppSettingsSpecifier *setting = [self settingAtIndexPath:indexPath];
		
		NSString *anActionSelector = [setting valueForKey:InAppSettingsSpecifierInAppAction];
		if ([anActionSelector length]) {
				SEL aSelector = NSSelectorFromString(anActionSelector);
				if ([setting.object respondsToSelector:aSelector])
					[setting.object performSelector:aSelector 
															 withObject:setting 
															 withObject:self.navigationController];
				return;
		}
	
    if([setting isType:InAppSettingsPSMultiValueSpecifier]){
        InAppSettingsPSMultiValueSpecifierTable *multiValueSpecifier = [[InAppSettingsPSMultiValueSpecifierTable alloc] initWithSetting:setting];
        [self.navigationController pushViewController:multiValueSpecifier animated:YES];
        [multiValueSpecifier release];
    }else if([setting isType:InAppSettingsPSChildPaneSpecifier]){
        InAppSettingsViewController *childPane = [[InAppSettingsViewController alloc] initWithFile:[setting valueForKey:InAppSettingsSpecifierFile]];
        childPane.title = [setting localizedTitle];
        [self.navigationController pushViewController:childPane animated:YES];
        [childPane release];
    }else if([setting isType:InAppSettingsPSTitleValueSpecifier]){
			InAppSettingsWebViewController *webViewController = nil;
			if ([[setting valueForKey:InAppSettingsSpecifierInAppHTMLFile] length])
				webViewController = [[InAppSettingsWebViewController alloc] initWithFile:
															 [setting valueForKey:InAppSettingsSpecifierInAppHTMLFile]];
			if ([[setting valueForKey:InAppSettingsSpecifierInAppURL] length])
			{
				NSURL *url = [NSURL URLWithString:[setting valueForKey:InAppSettingsSpecifierInAppURL]];
				if (webViewController)
					[webViewController setURL:url];
				else
					webViewController = [[InAppSettingsWebViewController alloc] initWithURL:url];
			}
			
			if (webViewController)
			{
				webViewController.title = [setting localizedTitle];
				[self.navigationController pushViewController:webViewController animated:YES];
				[webViewController release]; // CLANG
			}
    }
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    InAppSettingsTableCell *cell = ((InAppSettingsTableCell *)[tableView cellForRowAtIndexPath:indexPath]);
    
    if([cell.setting isType:@"PSTextFieldSpecifier"]){
        [cell.valueInput becomeFirstResponder];
    }else if(cell.canSelectCell){
#if !InAppSettingsUseUITableViewController
        [self.firstResponder resignFirstResponder];
#endif
        return indexPath;
    }
    return nil;
}

@end

#if InAppSettingsDisplayPowered
@implementation InAppSettingsLightningBolt

@synthesize flip;

- (id)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    if (self != nil) {
        self.flip = NO;
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

- (void)drawRect:(CGRect)rect{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, [InAppSettingsFooterBlue CGColor]);
    #if __IPHONE_3_2
    CGContextSetShadowWithColor(context, CGSizeMake(0.0f, 1.0f), 0.0f, [[UIColor whiteColor] CGColor]);
    #else
    CGContextSetShadowWithColor(context, CGSizeMake(0.0f, -1.0f), 0.0f, [[UIColor whiteColor] CGColor]);
    #endif
    if(self.flip){
        CGContextMoveToPoint(context, 4.0f, 1.0f);
        CGContextAddLineToPoint(context, 13.0f, 1.0f);
        CGContextAddLineToPoint(context, 10.0f, 5.0f);
        CGContextAddLineToPoint(context, 12.0f, 7.0f);
        CGContextAddLineToPoint(context, 2.0f, 15.0f);
        CGContextAddLineToPoint(context, 5.0f, 7.0f);
        CGContextAddLineToPoint(context, 3.0f, 5.0f);
    }else{
        CGContextMoveToPoint(context, 3.0f, 1.0f);
        CGContextAddLineToPoint(context, 12.0f, 1.0f);
        CGContextAddLineToPoint(context, 13.0f, 5.0f);
        CGContextAddLineToPoint(context, 11.0f, 7.0f);
        CGContextAddLineToPoint(context, 14.0f, 15.0f);
        CGContextAddLineToPoint(context, 4.0f, 7.0f);
        CGContextAddLineToPoint(context, 6.0f, 5.0f); 
    }
    CGContextFillPath(context);
}

@end
#endif /* InAppSettingsDisplayPowered */
