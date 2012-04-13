//
//  PopupViewController.m
//  WEPopover
//
//  Modified by Samuel Vinson on 10/02/11.
//  Created by Werner Altewischer on 02/09/10.
//  Copyright 2010 Werner IT Consultancy. All rights reserved.
//

#import "WEPopoverController.h"
#import "WEPopoverParentView.h"

#define FADE_DURATION 0.25

@interface WEPopoverController(Private)

@property (nonatomic, retain) UIView *popoverView;
- (CGRect)displayAreaForView:(UIView *)theView;
- (WEPopoverContainerViewProperties *)defaultContainerViewProperties;
- (void) selfDismissPopover;

@end


@implementation WEPopoverController

@synthesize contentViewController;
@synthesize popoverVisible = popoverVisible_;
@synthesize popoverArrowDirection;
@synthesize delegate = delegate_;
@synthesize popoverView = popoverView_;
@synthesize containerViewProperties;
@synthesize context;

- (id)initWithContentViewController:(UIViewController *)viewController 
{
	if (self = [self init]) 
	{
		self.contentViewController = viewController;
	}
	return self;
}

- (void)dealloc {
	[self dismissPopoverAnimated:NO];
	[contentViewController release];
	[containerViewProperties release];
	self.context = nil;
	[super dealloc];
}

- (void)dismissPopoverAnimated:(BOOL)animated {
	
	if (self.popoverView) {
		[contentViewController viewWillDisappear:animated];
		
		if (animated) {
			[UIView animateWithDuration:FADE_DURATION
											 animations:^{
												 self.popoverView.alpha = 0.0;
											 } 
											 completion:^(BOOL finished){
												 popoverVisible_ = NO;
												 [contentViewController viewDidDisappear:YES];
												 [self.popoverView removeFromSuperview];
												 self.popoverView = nil;
											 }];
		} else {
			popoverVisible_ = NO;
			[contentViewController viewDidDisappear:animated];
			[self.popoverView removeFromSuperview];
			self.popoverView = nil;
		}
	}
}

- (void)presentPopoverFromRect:(CGRect)rect 
						inView:(UIView *)theView 
	  permittedArrowDirections:(UIPopoverArrowDirection)arrowDirections 
					  animated:(BOOL)animated {
	
	
	[self dismissPopoverAnimated:NO];
	
	CGRect displayArea = [self displayAreaForView:theView];
		
	// add a button to dismiss the popover
	UIButton* fakeButton = [UIButton buttonWithType:UIButtonTypeCustom];
	fakeButton.backgroundColor = [UIColor clearColor];
	fakeButton.frame = displayArea;
	fakeButton.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	[fakeButton addTarget:self action:@selector(selfDismissPopover)
			 forControlEvents:UIControlEventTouchUpInside];
	[theView addSubview:fakeButton];
	
	WEPopoverContainerViewProperties *props = self.containerViewProperties ? self.containerViewProperties : [self defaultContainerViewProperties];
	
	WEPopoverContainerView *containerView = [[[WEPopoverContainerView alloc] 
																						initWithSize:self.contentViewController.contentSizeForViewInPopover/*self.popoverContentSize*/ 
																						anchorRect:rect 
																						displayArea:displayArea 
																						permittedArrowDirections:arrowDirections 
																						properties:props] autorelease];
	popoverArrowDirection = containerView.arrowDirection;
	//[theView addSubview:containerView];
	[fakeButton addSubview:containerView];

	containerView.contentView = contentViewController.view;
	
	//self.popoverView = containerView;
	self.popoverView = fakeButton;
	
	[contentViewController viewWillAppear:animated];
	
	if (animated) {
		//self.popoverView.userInteractionEnabled = NO;
		self.popoverView.alpha = 0.0;
		[UIView animateWithDuration:FADE_DURATION
										 animations:^{
											 self.popoverView.alpha = 1.0;
										 } 
										 completion:^(BOOL finished){
											 //self.popoverView.userInteractionEnabled = YES;
											 popoverVisible_ = YES;
											 [contentViewController viewDidAppear:YES];
										 }];
	} else {
		self.popoverView.userInteractionEnabled = YES;
		popoverVisible_ = YES;
		[contentViewController viewDidAppear:animated];
	}
}

- (void)repositionPopoverFromRect:(CGRect)rect
	   permittedArrowDirections:(UIPopoverArrowDirection)arrowDirections {
	[(WEPopoverContainerView *)self.popoverView updatePositionWithAnchorRect:rect
														displayArea:[self displayAreaForView:self.popoverView.superview]
										   permittedArrowDirections:arrowDirections];
	popoverArrowDirection = ((WEPopoverContainerView *)self.popoverView).arrowDirection;
}


@end


@implementation WEPopoverController(Private)

- (void)setPopoverView:(UIView *)pv 
{
	if (popoverView_ != pv) 
	{
		[popoverView_ release];
		popoverView_ = [pv retain];
	}
}

- (CGRect)displayAreaForView:(UIView *)theView {
	CGRect displayArea = CGRectZero;
	if ([theView conformsToProtocol:@protocol(WEPopoverParentView)] && [theView respondsToSelector:@selector(displayAreaForPopover)]) {
		displayArea = [(id <WEPopoverParentView>)theView displayAreaForPopover];
	} else if ([theView isKindOfClass:[UIScrollView class]]) {
		CGPoint contentOffset = [(UIScrollView *)theView contentOffset];
		displayArea = CGRectMake(contentOffset.x, contentOffset.y, theView.frame.size.width, theView.frame.size.height);
	} else {
		displayArea = CGRectMake(0, 0, theView.frame.size.width, theView.frame.size.height);
	}
	return displayArea;
}

- (WEPopoverContainerViewProperties *)defaultContainerViewProperties {
	WEPopoverContainerViewProperties *ret = [[WEPopoverContainerViewProperties new] autorelease];
	
	static const NSInteger CAP_SIZE = 188 / 2;
	static const CGFloat BG_IMAGE_MARGIN = 39.0;
	
	CGSize theSize = self.contentViewController.contentSizeForViewInPopover;
	NSString *bgImageName = nil;
	CGFloat bgMargin = 0.0;
	CGFloat bgCapSize = 0.0;
	CGFloat contentMargin = 3.0;
	if (theSize.width < (CAP_SIZE + 1) || theSize.height < (CAP_SIZE + 1)) {
		bgImageName = @"popoverBgSmall.png";
		bgMargin = BG_IMAGE_MARGIN / 2;
		bgCapSize = CAP_SIZE / 2;
	} else {
		bgImageName = @"popoverBg.png";
		bgMargin = BG_IMAGE_MARGIN;
		bgCapSize = CAP_SIZE;
	}
	
	ret.leftBgMargin = bgMargin;
	ret.rightBgMargin = bgMargin;
	ret.topBgMargin = bgMargin;
	ret.bottomBgMargin = bgMargin;
	ret.leftBgCapSize = bgCapSize;
	ret.topBgCapSize = bgCapSize;
	ret.bgImageName = bgImageName;
	ret.leftContentMargin = contentMargin;
	ret.rightContentMargin = contentMargin;
	ret.topContentMargin = contentMargin;
	ret.bottomContentMargin = contentMargin;
	
	ret.upArrowImageName = @"popoverArrowUp.png";
	ret.downArrowImageName = @"popoverArrowDown.png";
	ret.leftArrowImageName = @"popoverArrowLeft.png";
	ret.rightArrowImageName = @"popoverArrowRight.png";
	return ret;
}


- (void) selfDismissPopover
{
	BOOL shouldDismiss = YES;
	if ([self.delegate respondsToSelector:@selector(popoverControllerShouldDismissPopover:)])
		shouldDismiss = [self.delegate popoverControllerShouldDismissPopover:self];
	
	if (shouldDismiss)
	{
		[UIView animateWithDuration:FADE_DURATION
										 animations:^{
											 self.popoverView.alpha = 0.0;
										 } 
										 completion:^(BOOL finished){
											 popoverVisible_ = NO;
											 [contentViewController viewDidDisappear:YES];
											 [self.popoverView removeFromSuperview];
											 self.popoverView = nil;
											 if ([self.delegate respondsToSelector:@selector(popoverControllerDidDismissPopover:)])
												 [self.delegate popoverControllerDidDismissPopover:self];

										 }];
	}
}

@end
