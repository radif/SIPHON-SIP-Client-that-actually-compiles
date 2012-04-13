//
//  PSToggleSwitchSpecifier.h
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "InAppSettingsTableCell.h"

#define CUSTOM_UI_SWITCH 1

#if CUSTOM_UI_SWITCH
#import "CustomUISwitch.h"
#endif /* CUSTOM_UI_SWITCH */

@interface InAppSettingsPSToggleSwitchSpecifierCell : InAppSettingsTableCell {
#if !CUSTOM_UI_SWITCH
	UISwitch *valueSwitch;
#else
	CustomUISwitch *valueSwitch;
#endif /* CUSTOM_UI_SWITCH */
}

#if !CUSTOM_UI_SWITCH
@property (nonatomic, retain) UISwitch *valueSwitch;
#else
@property (nonatomic, retain) CustomUISwitch *valueSwitch;
#endif /* CUSTOM_UI_SWITCH */

- (BOOL)getBool;
- (void)setBool:(BOOL)newValue;
- (void)switchAction;

@end
