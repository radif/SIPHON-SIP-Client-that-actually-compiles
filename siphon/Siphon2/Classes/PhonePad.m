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

#import "PhonePad.h"
#import <AudioToolbox/AudioToolbox.h>

@implementation PhonePad

static NSString *_keyStrs[] = {nil, @"1", @"2", @"3", @"4", @"5", @"6", @"7", @"8", @"9", @"*", @"0", @"#"};
const static char _keyValues[] = {0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'};
static SystemSoundID sounds[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

@synthesize delegate = _delegate;

- (id)initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];
	if (self) 
  {    
    [self setOpaque:FALSE];
    self.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
    self.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;

    [self addTarget:self action:@selector(handleKeyDown:forEvent:) 
          forControlEvents:UIControlEventTouchDown];
    [self addTarget:self action:@selector(handleKeyUp:forEvent:) 
          forControlEvents:UIControlEventTouchUpInside|UIControlEventTouchUpOutside];
    
    // Init
    [self keypadImage];
    [self pressedImage];
    _topHeight = 58.0;
    _midHeight = 56.0;
    _bottomHeight = 59.0;
    _leftWidth = 95.0;
    _midWidth = 92.0;
    _rightWidth = 93.0;
	}
	return self;
}

- (UIImage*)keypadImage;
{
  if (_keypadImage == nil)
  {
    _keypadImage = [[UIImage imageNamed: @"keypad.png"] retain];
  }
  return _keypadImage;
}

- (UIImage*)pressedImage
{
  if (_pressedImage == nil)
  {
    _pressedImage = [[UIImage imageNamed: @"keypad_pressed.png"] retain];
  }
  return _pressedImage;
}

- (void)cancelTrackingWithEvent:(UIEvent *)event
{
  if (_keyValues[_downKey] == '0' || _keyValues[_downKey] == '*')
  {
    [NSObject cancelPreviousPerformRequestsWithTarget:self 
                                             selector:@selector(handleKeyPressAndHold:)
                                               object:nil];
  }
  _downKey = 0;
  [self setNeedsDisplayForKey:0];
}

- (void)handleKeyDown:(id)sender forEvent:(UIEvent *)event
{

  NSSet *set = [event touchesForView:self];
  NSEnumerator *enumerator = [set objectEnumerator];
  UITouch *touch;
  
  while ((touch = [enumerator nextObject])) 
  {
    CGPoint point = [touch locationInView:self];
    _downKey = [self keyForPoint: point];
    if (_downKey == 0)
      continue;
      //return;
  
    [self setNeedsDisplayForKey:_downKey];
    [self playSoundForKey:_downKey];
    
    if ([_delegate respondsToSelector:@selector(phonePad:appendString:)])
    {
      [_delegate phonePad:self appendString: _keyStrs[_downKey]];
    }
    if (_keyValues[_downKey] == '0' || _keyValues[_downKey] ==  '*')
    {
      [self performSelector:@selector(handleKeyPressAndHold:) 
                 withObject:nil afterDelay:0.5];
    }
    if ([_delegate respondsToSelector:@selector(phonePad:keyDown:)])
    {
      [_delegate phonePad:self keyDown: _keyValues[_downKey]];
    }
  }
}

- (void)handleKeyUp:(id)sender forEvent:(UIEvent *)event
{
  
  NSSet *set = [event touchesForView:self];
  NSEnumerator *enumerator = [set objectEnumerator];
  UITouch *touch;
  
  while ((touch = [enumerator nextObject])) 
  {    
    if (_downKey == 0)
      //return;
      continue;

    if ([_delegate respondsToSelector:@selector(phonePad:keyUp:)])
    {
      [_delegate phonePad:self keyUp: _keyValues[_downKey]];
    }

    [self cancelTrackingWithEvent:nil];
  }
}

- (void)handleKeyPressAndHold:(id)fp8
{
  NSString *key;
  if (_keyValues[_downKey] == '0')
    key = @"+";
  else if (_keyValues[_downKey] == '*')
    key = @",";
  else
    return;
  
  if ([_delegate respondsToSelector:@selector(phonePad:replaceLastDigitWithString:)])
  {
    [_delegate phonePad:self replaceLastDigitWithString: key];
  }
}

- (int)keyForPoint:(CGPoint)point
{
  int pos = 0;
  CGSize size = [_keypadImage size];
  CGRect bounds = [self bounds];
  
  point.x = point.x - (CGRectGetMidX(bounds) - size.width/2.);
  point.y = point.y - (CGRectGetMidY(bounds) - size.height/2.);
  
  if (point.x < 0 || point.y < 0)
    return 0;
  
  if (point.x < _leftWidth)
    pos = 1;
  else if (point.x < _leftWidth + _midWidth)
    pos = 2;
  else if (point.x < _leftWidth + _midWidth + _rightWidth)
    pos = 3;
  else
    return 0;

  
  if (point.y < _topHeight)
    ;
  else if (point.y < _topHeight + _midHeight)
    pos += 3;
  else if (point.y < _topHeight + 2 *_midHeight)
    pos += 6;
  else if (point.y < _topHeight + 2. * _midHeight + _bottomHeight)
    pos += 9;
  else
    return 0;

  return pos;
}

-(CGRect)rectForKey:(int)key
{
  CGFloat width, height, x,y;
  x = y = 0.0;
  switch (key % 3) 
  {
    case 1:
      width = _leftWidth;
      break;
    case 2:
      width = _midWidth;
      x += _leftWidth;
      break;
    case 0:
      width = _rightWidth;
      x += _leftWidth + _midWidth;
      break;
    default:
      //break;
      return CGRectZero;
  }
  
  switch ((key - 1)/ 3) 
  {
    case 0:
      height = _topHeight;
      break;
    case 2:
      y += _topHeight;
    case 1:
      y += _midHeight;
      height = _midHeight;
      break;
    case 3:
      y += _topHeight + 2.0 * _midHeight;
      height = _bottomHeight;
      break;
    default:
      //break;
      return CGRectZero;
  }

  return CGRectMake(x, y, width, height);
}

- (void)drawRect:(CGRect)rect
{
  CGRect r, b;

  //NSLog(@"drawRect");
  r.size = [_keypadImage size];
  b = [self bounds];
  r.origin.x = (b.size.width - r.size.width) / 2;
  r.origin.y = (b.size.height - r.size.height) / 2;
  [_keypadImage drawInRect:r];

  if (_downKey != 0)
  {
    //NSLog(@"drawButton %d", _downKey);
    CGRect ri = [self rectForKey:_downKey];
    CGImageRef cgImg = CGImageCreateWithImageInRect([_pressedImage CGImage], ri);
    UIImage *img = [UIImage imageWithCGImage:cgImg];
    ri.origin.x += r.origin.x;
    ri.origin.y += r.origin.y;
    [img drawInRect:ri];
    CGImageRelease (cgImg);
  }
}

- (void)setNeedsDisplayForKey:(int)key
{
  //CGRect rect = [self rectForKey:key];
  //[self setNeedsDisplayInRect:rect];
  [self setNeedsDisplay];
}

- (void)setPlaysSounds:(BOOL)activate
{
  _soundsActivated = activate;
}

- (void)playSoundForKey:(int)key
{
  if (!_soundsActivated)
    return;

  if (!sounds[key])
  {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *filename = [NSString stringWithFormat:@"dtmf-%c", 
                          (key == 10 ? 's' : _keyValues[key])];
    NSString *path = [mainBundle pathForResource:filename ofType:@"aif"];
    if (!path)
      return;
    
    NSURL *aFileURL = [NSURL fileURLWithPath:path isDirectory:NO];
    if (aFileURL != nil)  
    {
      SystemSoundID aSoundID;
      OSStatus error = AudioServicesCreateSystemSoundID((CFURLRef)aFileURL, 
                                                        &aSoundID);
      if (error != kAudioServicesNoError)
        return;
      
      sounds[_downKey] = aSoundID;
    }
  }
  
  AudioServicesPlaySystemSound(sounds[_downKey]);
}

-(void)dealloc
{
  int i;
  for (i = 1; i < 13; ++i)
    if (sounds[i])
    {
      AudioServicesDisposeSystemSoundID(sounds[i]);
      sounds[i] = 0;
    }
  [_keypadImage release];
  [_pressedImage release];
  [super dealloc];
}

@end
