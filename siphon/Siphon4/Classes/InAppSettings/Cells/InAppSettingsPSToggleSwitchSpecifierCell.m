//
//  PSToggleSwitchSpecifier.m
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Copyright 2011 Samuel Vinson. All rights reserved.
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import "InAppSettingsPSToggleSwitchSpecifierCell.h"
#import "InAppSettingsConstants.h"

#import "NSDictionary+Additions.h"

@implementation InAppSettingsPSToggleSwitchSpecifierCell

@synthesize valueSwitch;

//    The value associated with the preference when the toggle switch 
//    is in the ON position. The value type for this key can be any 
//    scalar type, including Boolean, String, Number, Date, or Data. 
//    If this key is not present, the default value type is a Boolean.

- (BOOL)getBool{
    id value = [self.setting getValue];
    id trueValue = [self.setting valueForKey:@"TrueValue"];
    id falseValue = [self.setting valueForKey:@"FalseValue"];
    
    if([value isEqual:trueValue]){
        return YES;
    }
    
    if([value isEqual:falseValue]){
        return NO;
    }
    
    //if there is no true or false values the value has to be a bool
    return [value boolValue];
}

- (void)setBool:(BOOL)newValue{
    id value = [NSNumber numberWithBool:newValue];
    if(newValue){
        id trueValue = [self.setting valueForKey:@"TrueValue"];
        if(trueValue){
            value = trueValue;
        }
    }
    else{
        id falseValue = [self.setting valueForKey:@"FalseValue"];
        if(falseValue){
            value = falseValue;
        }
    }
    [self.setting setValue:value];
}

- (BOOL)getAlternateColors
{
	id value = [self.setting valueForKey:InAppSettingsSpecifierInAppAlternateColor];
	return [value boolValue];
}

- (void)switchAction{
	NSDictionary *confirmation = [self.setting valueForKey:InAppSettingsSpecifierInAppConfirmation];
	if (!confirmation)
    [self setBool:[self.valueSwitch isOn]];
	else	
		[self displayConfirmation:confirmation];
}

- (void)setUIValues{
    [super setUIValues];
    
    [self setTitle];
    self.valueSwitch.on = [self getBool];
		[self.valueSwitch setAlternateColors:[self getAlternateColors]];
}

- (void)setupCell{
    [super setupCell];
    
    //create the switch
#if !CUSTOM_UI_SWITCH
    self.valueSwitch = [[UISwitch alloc] initWithFrame:CGRectZero];
#else 
		self.valueSwitch = [[CustomUISwitch alloc] initWithFrame:CGRectZero];
#endif /* CUSTOM_UI_SWITCH */
    CGRect valueSwitchFrame = self.valueSwitch.frame;
    valueSwitchFrame.origin.y = (CGFloat)round((self.contentView.frame.size.height*0.5f)-(valueSwitchFrame.size.height*0.5f))-InAppSettingsOffsetY;
    valueSwitchFrame.origin.x = (CGFloat)round((InAppSettingsScreenWidth-(InAppSettingsTotalTablePadding+InAppSettingsCellPadding))-valueSwitchFrame.size.width);
    self.valueSwitch.frame = valueSwitchFrame;
		
    [self.valueSwitch addTarget:self action:@selector(switchAction) forControlEvents:UIControlEventValueChanged];
    [self.contentView addSubview:self.valueSwitch];
}

- (void)dealloc{
    [valueSwitch release];
    [super dealloc];
}

#pragma mark -
- (void)didCancel {
	self.valueSwitch.on = ![self.valueSwitch isOn];
}

- (void)didConfirm {
	[self setBool:[self.valueSwitch isOn]];
}

@end
