/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008-2010 Samuel <samuelv0304@gmail.com>
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

#import "FavoritesCell.h"


@implementation FavoritesCell

@synthesize name, label;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier 
{
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) 
    {
        // Initialization code
      name = [[UILabel alloc] initWithFrame:CGRectZero];
      name.font = [UIFont boldSystemFontOfSize:[UIFont labelFontSize]];
      name.backgroundColor = [UIColor clearColor];
      
      label = [[UILabel alloc] initWithFrame:CGRectZero];
      label.font = [UIFont boldSystemFontOfSize:[UIFont labelFontSize] - 2.];
      label.textColor = [UIColor darkGrayColor];
      label.textAlignment = UITextAlignmentRight;
      label.backgroundColor = [UIColor clearColor];
      
      // Add the labels to the content view of the cell.
      
      // Important: although UITableViewCell inherits from UIView, you should add subviews to its content view
      // rather than directly to the cell so that they will be positioned appropriately as the cell transitions 
      // into and out of editing mode.
      
      [self.contentView addSubview:name];
      [self.contentView addSubview:label];
    }
    return self;
}

- (void)layoutSubviews 
{
  [super layoutSubviews];
  // Start with a rect that is inset from the content view by 10 pixels on all sides.
  CGRect baseRect = CGRectInset(self.contentView.bounds, 10, 10);
  CGRect rect = baseRect;
  // Position each label with a modified version of the base rect.
  if (baseRect.size.width - 70 < 120)
   rect.size.width = baseRect.size.width;
  else
    rect.size.width = baseRect.size.width - 70;
  name.frame = rect;

  if (baseRect.size.width - 70 < 120) // Display button delete
  {
    rect.origin.x = label.frame.origin.x;
    rect.size.width = 0;
  }
  else
  {
    rect.origin.x = baseRect.size.width - 70;
    rect.size.width = 70;
  }
  label.frame = rect;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
  [super setSelected:selected animated:animated];

  // Configure the view for the selected state
  if (selected) 
  {
    name.textColor = [UIColor whiteColor];
    label.textColor = [UIColor whiteColor];
  }
  else
  {
    name.textColor = [UIColor blackColor];
    label.textColor = [UIColor darkGrayColor];
  }
}


- (void)dealloc 
{
  [name release];
  [label release];
  [super dealloc];
}


@end
