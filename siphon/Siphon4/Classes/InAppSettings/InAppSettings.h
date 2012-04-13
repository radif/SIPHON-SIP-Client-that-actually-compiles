//
//  InAppSettingsViewController.h
//  InAppSettings
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "InAppSettingsTableCell.h"
#import "InAppSettingsReader.h"
#import "InAppSettingsSpecifier.h"
#import "InAppSettingsConstants.h"

#define InAppSettingsNotification InAppSettingsNotificationName

@interface InAppSettings : NSObject {}

+ (void)registerDefaults;
+ (id)sharedManager;

@end

@interface InAppSettingsModalViewController : UIViewController {}

@end

#if InAppSettingsUseUITableViewController
@interface InAppSettingsViewController : UITableViewController {
#else
@interface InAppSettingsViewController : UIViewController <UITableViewDelegate, UITableViewDataSource, UITextFieldDelegate> {
#endif
		NSString *file;
#if !InAppSettingsUseUITableViewController
    UITableView *settingsTableView;
    UIControl *firstResponder;
#endif
    InAppSettingsReader *settingsReader;
}

@property (nonatomic, copy) NSString *file;
#if !InAppSettingsUseUITableViewController
@property (nonatomic, retain) UITableView *settingsTableView;
@property (nonatomic, assign) UIControl *firstResponder;
#endif
@property (nonatomic, retain) InAppSettingsReader *settingsReader;

// modal view
- (void)dismissModalView;
- (void)addDoneButton;

// 
- (id)initWithFile:(NSString *)inputFile;

#if !InAppSettingsUseUITableViewController
//keyboard notification
- (void)registerForKeyboardNotifications;
- (void)keyboardWillShow:(NSNotification*)notification;
- (void)keyboardWillHide:(NSNotification*)notification;
#endif

@end

#if InAppSettingsDisplayPowered
@interface InAppSettingsLightningBolt : UIView {
    BOOL flip;
}

@property (nonatomic, assign) BOOL flip;

@end
#endif /* InAppSettingsDisplayPowered */
