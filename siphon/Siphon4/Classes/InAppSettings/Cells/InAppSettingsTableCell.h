//
//  InAppSettingsTableCell.h
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 11/21/09.
//  Copyright 2009 InScopeApps{+}. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "InAppSettingsSpecifier.h"

@interface InAppSettingsTableCell : UITableViewCell <UIActionSheetDelegate> {
    InAppSettingsSpecifier *setting;
    UILabel *titleLabel, *valueLabel;
    UIControl *valueInput;
    BOOL canSelectCell;
}

@property (nonatomic, retain) InAppSettingsSpecifier *setting;
@property (nonatomic, retain) UILabel *titleLabel, *valueLabel;
@property (nonatomic, assign) UIControl *valueInput;
@property (nonatomic, assign) BOOL canSelectCell;

- (void)setTitle;
- (void)setDetail;
- (void)setDetail:(NSString *)detail;
- (void)setDisclosure:(BOOL)disclosure;

- (void)setValueDelegate:(id)delegate;

- (void)setupCell;
- (void)setUIValues;
- (id)initWithReuseIdentifier:(NSString *)reuseIdentifier;

- (void)displayConfirmation:(NSDictionary *)confirmation;

@end
