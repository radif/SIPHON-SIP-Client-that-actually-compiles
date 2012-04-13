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

#import "InAppSettingsPSButtonSpecifierCell.h"
#import "InAppSettingsConstants.h"


@implementation InAppSettingsPSButtonSpecifierCell

@synthesize valueButton;

- (void)didConfirm {
	//implement this per cell type
	NSString *anActionSelector = [self.setting valueForKey:InAppSettingsSpecifierInAppAction];
	if ([anActionSelector length]) {
		SEL aSelector = NSSelectorFromString(anActionSelector);
		if ([self.setting.object respondsToSelector:aSelector])
			[self.setting.object performSelector:aSelector 
													 withObject:self.setting 
													 withObject:nil/*self.navigationController*/];
	}
}

- (void)buttonAction {
	NSDictionary *confirmation = [self.setting valueForKey:InAppSettingsSpecifierInAppConfirmation];
	if (!confirmation)
    [self didConfirm];
	else	
		[self displayConfirmation:confirmation];
}

- (void)setUIValues{
	[super setUIValues];
	
	[self.valueButton setTitle:[self.setting localizedTitle] 
										forState:UIControlStateNormal];
	
	CGRect valueButtonFrame = [self.contentView frame];
	valueButtonFrame.origin.y = -1;
	valueButtonFrame.origin.x = -2;
	valueButtonFrame.size.height += 4;
	valueButtonFrame.size.width = InAppSettingsScreenWidth - 16;
	self.valueButton.frame = valueButtonFrame;
	
}

- (void)setupCell{
	[super setupCell];
	
	//create the button
	self.valueButton = [UIButton buttonWithType:UIButtonTypeCustom];

	self.valueButton.titleLabel.font = [UIFont boldSystemFontOfSize:[UIFont buttonFontSize]];
	[self.valueButton setTitleColor:[UIColor whiteColor]
												 forState:UIControlStateNormal];
	UIImage *background = [UIImage imageNamed:@"bottombarred"];
  NSInteger width = background.size.width;
  background = [background stretchableImageWithLeftCapWidth:width/2 topCapHeight:0.0];
	[self.valueButton setBackgroundImage:background forState:UIControlStateNormal];
	
	background = [UIImage imageNamed:@"bottombarred_pressed"];
  width = background.size.width;
  background = [background stretchableImageWithLeftCapWidth:width/2 topCapHeight:0.0];
	[self.valueButton setBackgroundImage:background forState:UIControlStateSelected];
	
	[self.valueButton addTarget:self action:@selector(buttonAction) forControlEvents:UIControlEventTouchUpInside];
	[self.contentView addSubview:self.valueButton];
}

- (void)dealloc {
	[valueButton release];
	[super dealloc];
}



@end
