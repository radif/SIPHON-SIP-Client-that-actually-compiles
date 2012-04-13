//
//  PopupViewController.h
//  WEPopover
//
//  Created by Werner Altewischer on 02/09/10.
//  Copyright 2010 Werner IT Consultancy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WEPopoverContainerView.h"

@class WEPopoverController;

@protocol WEPopoverControllerDelegate <NSObject>
@optional
- (void)popoverControllerDidDismissPopover:(WEPopoverController *)popoverController;
- (BOOL)popoverControllerShouldDismissPopover:(WEPopoverController *)popoverController;

@end

@interface WEPopoverController : NSObject {
	@private
	UIViewController *contentViewController;
	UIView           *popoverView_;
	BOOL              popoverVisible_;
	UIPopoverArrowDirection popoverArrowDirection;
	id <WEPopoverControllerDelegate> delegate_;
	WEPopoverContainerViewProperties *containerViewProperties;
	id <NSObject> context;
}

@property(nonatomic, retain) UIViewController *contentViewController;

@property (nonatomic, readonly) UIView *popoverView;
@property (nonatomic, readonly, getter=isPopoverVisible) BOOL popoverVisible;
@property (nonatomic, readonly) UIPopoverArrowDirection popoverArrowDirection;
@property (nonatomic, assign) id <WEPopoverControllerDelegate> delegate;
@property (nonatomic, retain) WEPopoverContainerViewProperties *containerViewProperties;
@property (nonatomic, retain) id <NSObject> context;

- (id)initWithContentViewController:(UIViewController *)theContentViewController;

- (void)dismissPopoverAnimated:(BOOL)animated;

- (void)presentPopoverFromRect:(CGRect)rect 
						inView:(UIView *)view 
	  permittedArrowDirections:(UIPopoverArrowDirection)arrowDirections 
					  animated:(BOOL)animated;

- (void)repositionPopoverFromRect:(CGRect)rect
		 permittedArrowDirections:(UIPopoverArrowDirection)arrowDirections;

@end
