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

#import "PhonePad.h"

#if LONG_DTMF
#import "AQPlayer.h"
#endif

@implementation PhonePad

static NSString *_keyStrs[] = {nil, @"1", @"2", @"3", @"4", @"5", @"6", @"7", @"8", @"9", @"*", @"0", @"#"};
const static char _keyValues[] = {0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'};

#if !LONG_DTMF
static SystemSoundID sounds[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

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
    _topHeight = 58.0;
    _midHeight = 56.0;
    _bottomHeight = 59.0;
    _leftWidth = 95.0;
    _midWidth = 92.0; // 91
    _rightWidth = 93.0; // 94
    
#if LONG_DTMF
    AQPlayer *player = new AQPlayer();
    _aqPlayer = (void *)player;
    
    // initialize the audio player. No idea why this is necessary, but if I don't do it, it takes a while before audio play begins the first time. -CK	
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *path = [mainBundle pathForResource:@"silence" ofType:@"wav"];

    if (path)
    {
      player->DisposeQueue(true);
      player->CreateQueueForFile((CFStringRef)path);
      player->SetLooping(false);
      player->StartQueue(false);
    }
#endif
	}
	return self;
}

- (UIImage*)keypadImage
{
	return [UIImage imageNamed:@"keypad"];
}

- (UIImage*)pressedImage
{
	return [UIImage imageNamed: @"keypad_pressed.png"];
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

  for (int i = [set count] ; i ; --i) 
  {    
    if (_downKey == 0)
      //return;
      continue;
    
#if LONG_DTMF
    [self stopSoundForKey:_downKey];
#endif
    
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
  CGSize size = [[self keypadImage] size];
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
      y = _midHeight;
    case 1:
      y += _topHeight;
      height = _midHeight;
      break;
    case 3:
      y = _topHeight + 2.0 * _midHeight;
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
  r.size = [[self keypadImage] size];
  b = [self bounds];
  r.origin.x = (b.size.width - r.size.width) / 2;
  r.origin.y = (b.size.height - r.size.height) / 2;
  [[self keypadImage] drawInRect:r];
  
  if (_downKey != 0)
  {
    //NSLog(@"drawButton %d", _downKey);
    CGRect ri = [self rectForKey:_downKey];
    CGFloat scale = [[self pressedImage] scale];
    CGRect ris = CGRectMake(ri.origin.x * scale,  ri.origin.y * scale,
														ri.size.width * scale, ri.size.height * scale);
    CGImageRef cgImg = CGImageCreateWithImageInRect([[self pressedImage] CGImage], ris);
    UIImage *img = [UIImage imageWithCGImage:cgImg scale:scale orientation:UIImageOrientationUp];
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

#if LONG_DTMF
  NSString *keyStr = _keyStrs[key];
  
  if ([keyStr isEqualToString:@"*"]) {
		keyStr = @"star";
	} else 	if ([keyStr isEqualToString:@"#"]) {
		keyStr = @"pound";
	} else 	if ([keyStr isEqualToString:@"+"]) {
		return;
	}
	
  NSBundle *mainBundle = [NSBundle mainBundle];
  NSString *filename = [NSString stringWithFormat:@"dtmf-%@", keyStr];
  NSString *path = [mainBundle pathForResource:filename ofType:@"aif"];
  if (!path)
    return;
  
  
  AQPlayer *player = (AQPlayer *)_aqPlayer;
  
	player->DisposeQueue(true);
	player->CreateQueueForFile((CFStringRef)path);
	player->SetLooping(true);
	player->StartQueue(false);
#else
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
#endif
}

#if LONG_DTMF
- (void)stopSoundForKey:(int)key
{
  AQPlayer *player = (AQPlayer *)_aqPlayer;
  player->SetLooping(false);
}
#endif

-(void)dealloc
{
#if LONG_DTMF
  AQPlayer *player = (AQPlayer *)_aqPlayer;
  delete player;
#else
  int i;
  for (i = 1; i < 13; ++i)
    if (sounds[i])
    {
      AudioServicesDisposeSystemSoundID(sounds[i]);
      sounds[i] = 0;
    }
#endif
  [super dealloc];
}

@end
