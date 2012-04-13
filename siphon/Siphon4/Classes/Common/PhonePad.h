/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008-2011 Samuel <samuelv0304@gmail.com>
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
#import <AudioToolbox/AudioToolbox.h>

#define LONG_DTMF 0

@protocol PhonePadDelegate;

@interface PhonePad : UIControl
{  
  CGFloat _topHeight, _midHeight, _bottomHeight;
  CGFloat _leftWidth, _midWidth, _rightWidth;
  
@private
  id<PhonePadDelegate> _delegate;
  int _downKey;
  
  BOOL _soundsActivated;
#if LONG_DTMF
  void *_aqPlayer;
#endif
}

- (id)initWithFrame:(CGRect)rect;

- (UIImage*)keypadImage;
- (UIImage*)pressedImage;

- (void)handleKeyDown:(id)sender forEvent:(UIEvent *)event;
- (void)handleKeyUp:(id)sender forEvent:(UIEvent *)event;
- (void)handleKeyPressAndHold:(id)sender;
- (int)keyForPoint:(CGPoint)point;
- (CGRect)rectForKey:(int)key;
- (void)drawRect:(CGRect)rect;

- (void)setNeedsDisplayForKey:(int)key;

- (void)setPlaysSounds:(BOOL)activate;
- (void)playSoundForKey:(int)key;
#if LONG_DTMF
- (void)stopSoundForKey:(int)key;
#endif

@property (nonatomic, retain) id<PhonePadDelegate> delegate;

@end

@protocol PhonePadDelegate <NSObject>

@optional
- (void)phonePad:(id)phonepad appendString:(NSString *)string;
- (void)phonePad:(id)phonepad replaceLastDigitWithString:(NSString *)string;

- (void)phonePad:(id)phonepad keyDown:(char)car;
- (void)phonePad:(id)phonepad keyUp:(char)car;
@end
