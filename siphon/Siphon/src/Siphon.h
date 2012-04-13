/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008 Samuel <samuelv@users.sourceforge.org>
 *  Copyright (C) 2008 Christian Toepp <chris.touchmods@googlemail.com>
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

#import <UIKit/UIKit.h>
#import <UIKit/UIAlertSheet.h>
#import <Celestial/AVSystemController.h>

#import "PhoneView.h"
#import "ContactView.h"
#import "CallView.h"
#import "AboutView.h"
#import "FavoritesView.h"

#import "call.h"

@interface Siphon : UIApplication
{
  UIView           *_mainView;  
  UIWindow         *_window;
  UITransitionView *_transition;

  UIButtonBar      *_buttonBar;
  int               _currentView;
//  int               _previousView;

  PhoneView     *_phoneView;
  ContactView   *_contactView;
  CallView      *_callView;
  FavoritesView *_favoritesView;
#ifdef ABOUT  
  AboutView     *_aboutView;
#endif  
  AVSystemController *_avs;
  
  app_config_t _app_config; // pointer ???
  pjsua_acc_id  _sip_acc_id;
}

- (void)processCallState:(NSNotification *)notification;
- (void)hideButtonBar:(UIView *)view;
- (void)showButtonBar:(UIView *)view;

@end

extern NSString *kUIButtonBarButtonAction;
extern NSString *kUIButtonBarButtonInfo;
extern NSString *kUIButtonBarButtonInfoOffset;
extern NSString *kUIButtonBarButtonSelectedInfo;
extern NSString *kUIButtonBarButtonStyle;
extern NSString *kUIButtonBarButtonTag;
extern NSString *kUIButtonBarButtonTarget;
extern NSString *kUIButtonBarButtonTitle;
extern NSString *kUIButtonBarButtonTitleVerticalHeight;
extern NSString *kUIButtonBarButtonTitleWidth;
extern NSString *kUIButtonBarButtonType;
