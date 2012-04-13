/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2008 Samuel <samuelv@users.sourceforge.org>
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
#import "CallView.h"
#import "call.h"
#import "dtmf.h"

/**
 * The iPhone stores photos in three (?) different sizes.
 */
enum {
    IPHONE_PHOTO_SIZE_THUMBNAIL,
    IPHONE_PHOTO_SIZE_MEDIUM,
    IPHONE_PHOTO_SIZE_ORIGINAL
};

typedef struct __ABAddressBookRef *ABAddressBookRef;
typedef const struct __CFData *CFDataRef;
typedef void *ABRecordRef;
typedef struct __ABPerson *ABPersonRef;

extern NSString* const kABCFirstNameProperty;
extern NSString* const kABCLastNameProperty;

extern ABAddressBookRef ABCGetSharedAddressBook();
extern ABRecordRef ABCFindPersonMatchingPhoneNumber(ABAddressBookRef addressBook, 
    NSString *phoneNumber, int identifier, int uid);
extern int         ABCRecordGetUniqueId(ABRecordRef record);
extern ABRecordRef ABCPersonGetRecordForUniqueID(ABAddressBookRef addressBook, int uid);
extern CFDataRef   ABCPersonCopyImageData(ABPersonRef person, int format);
extern NSString   *ABCRecordCopyCompositeName(ABRecordRef record);

@implementation CallView  : UIView

//****************************************************************************************
//                                GUI Control
//****************************************************************************************

- (void)phonePad:(TPPhonePad *)phonepad keyDown:(char)car
{
  NSLog(@"keyDown %@ %c", phonepad, car);
  // DTMF
  sip_call_play_digit(_call_id, car);
}

- (void)endCallUpInside:(id)fp8
{
  NSLog(@"endCallUpInside %@", fp8);
  // Hold
  sip_hangup(&_call_id);
}

- (void)answerCallDown:(id)fp8
{
  NSLog(@"answerCallDown %@", fp8);
  sip_answer(&_call_id);
}

- (void)declineCallDown:(id)fp8
{
  NSLog(@"declineCallDown %@", fp8);
  sip_hangup(&_call_id);
}

//** **
- (id)init
{
  struct CGRect hwRect, appRect;
 
  _call_id = PJSUA_INVALID_ID;
 
  hwRect  = [UIHardware fullScreenApplicationContentRect];
  appRect = hwRect;
  appRect.origin.x = appRect.origin.y = 0.0f;
 
  if ((self = [super initWithFrame: appRect]) != nil)
  {
    /** Background **/
    UIImageView *background = [[UIImageView alloc]
     initWithFrame:CGRectMake(0.0f, (-hwRect.origin.y), hwRect.size.width, 
      hwRect.size.height + hwRect.origin.y)];
    [background setImage:[UIImage defaultDesktopImage]];
    [self addSubview:background];
    
    /** Phone Pad **/
    _phonePad = [[TPPhonePad alloc] initWithFrame: CGRectMake(0.0f, 70.0f, 320.0f, 320.0f)];
    [_phonePad setPlaysSounds: TRUE];
    [_phonePad setNeedsDisplayForKey: TRUE];
    [_phonePad setDelegate: self];
   
    /** End call **/
    _bottomBar = [[TPBottomButtonBar alloc] initForEndCallWithFrame: 
      CGRectMake(0.0f, 460.0f - 96.0f, 320.0f, 96.0f)];
    [[_bottomBar button] addTarget:self action:@selector(endCallUpInside:) 
      forEvents:kUIControlEventMouseUpInside/*kUIControlEventMouseDown*/];

    /** Decline or Answer **/
    _dualBottomBar = [[TPBottomDualButtonBar alloc] initForIncomingCallWithFrame:
        CGRectMake(0.0f, 460.0f - 96.0f, 320.0f, 96.0f)];
  
    [[_dualBottomBar button] addTarget:self action:@selector(declineCallDown:) 
      forEvents:kUIControlEventMouseUpInside/*kUIControlEventMouseDown*/];
    [[_dualBottomBar button2] addTarget:self action:@selector(answerCallDown:) 
      forEvents:kUIControlEventMouseUpInside/*kUIControlEventMouseDown*/];
    
    /** LCD **/
    _lcd = [[TPLCDView alloc] initWithDefaultSize];
    [_lcd setLabel:@""]; // name or number of callee
    [_lcd setText:@""];   // timer, call state for example
//    [_lcd setSubImage:];  // image/avatar
    [self addSubview: _lcd];
  }
  
  return self;
}

/*** ***/
- (id)delegate 
{
  return _delegate;
}

- (void)setDelegate:(id)newDelegate 
{
  _delegate = newDelegate;
}

/*** ***/
- (void)timeout:(id)unused
{
  pjsua_call_info ci;
  
  pjsua_call_get_info(_call_id, &ci);
  
  if (ci.connect_duration.sec >= 3600)
  {
    long sec = ci.connect_duration.sec % 3600;
    [_lcd setLabel:[NSString stringWithFormat:@"%d:%02d:%02d", 
                     ci.connect_duration.sec / 3600,
                     sec/60, sec%60]];
  }
  else
  {
    [_lcd setLabel:[NSString stringWithFormat:@"%02d:%02d", 
                     (ci.connect_duration.sec)/60,
                     (ci.connect_duration.sec)%60]];
  }
}

/*** ***/
- (ABRecordRef)findRecord:(NSString *)phoneNumber
{
  ABAddressBookRef addressBook = ABCGetSharedAddressBook();
  ABRecordRef record = ABCFindPersonMatchingPhoneNumber(addressBook, 
      phoneNumber,0, 0);
  
  return record;
}

- (UIImage *)findImage:(ABRecordRef)record
{
  UIImage *image = NULL;
  
  if (record)
  {
    CFDataRef data;
    
    data = ABCPersonCopyImageData(record, IPHONE_PHOTO_SIZE_THUMBNAIL);
    if (!data)
    {
      data = ABCPersonCopyImageData(record, IPHONE_PHOTO_SIZE_MEDIUM);
    }
    if (data)
      image = [[UIImage alloc] initWithData: data cache:YES];
  }
  return image;
}

- (void)displayUserInfo:(pjsua_call_id)call_id
{
  pjsua_call_info ci;
  pjsip_name_addr *url;
  pjsip_sip_uri *sip_uri;
  pj_str_t tmp, dst;
  pj_pool_t     *pool;
  
  pool = pjsua_pool_create("call", 128, 128);

  if (pool)
  {
    pjsua_call_get_info(call_id, &ci);
    pj_strdup_with_null(pool, &tmp, &ci.remote_info);
  
    url = (pjsip_name_addr*)pjsip_parse_uri(pool, tmp.ptr, tmp.slen,
                  PJSIP_PARSE_URI_AS_NAMEADDR);
    if (url != NULL) 
    {
      NSString *phoneNumber = NULL;
      sip_uri = (pjsip_sip_uri*) pjsip_uri_get_uri(url->uri);
      pj_strdup_with_null(pool, &dst, &sip_uri->user);

      ABRecordRef record = [self findRecord:[NSString stringWithUTF8String: 
                                             pj_strbuf(&dst)]];
      if (record)
        phoneNumber = ABCRecordCopyCompositeName(record);
      if (!phoneNumber)
      {
        if (url->display.slen)
        {
          pj_strdup_with_null(pool, &dst, &url->display);
        }
        phoneNumber = [NSString stringWithUTF8String: pj_strbuf(&dst)];
      }
      [_lcd setText: phoneNumber];
      UIImage *image = [self findImage: record];
      [_lcd setSubImage: image];
    }
    else
    {
      [_lcd setText: @""];
      [_lcd setSubImage: nil];
    }
    
    pj_pool_release(pool);
  } 
}

/*** ***/
- (void)setState:(int)state callId:(pjsua_call_id)call_id
{
  _call_id = call_id;
  switch(state)
  {
    case PJSIP_INV_STATE_CALLING: // After INVITE is sent.
      [self addSubview: _bottomBar];
      [self displayUserInfo: call_id];
      [_lcd setLabel: NSLocalizedString(@"CALLING", @"Call view")];
      break;
    case PJSIP_INV_STATE_INCOMING: // After INVITE is received.
      [self addSubview: _dualBottomBar];
      [self displayUserInfo: call_id];
      [_lcd setLabel: @""];
      break;
    case PJSIP_INV_STATE_EARLY: // After response with To tag.
    case PJSIP_INV_STATE_CONNECTING: // After 2xx is sent/received.
      break;
    case PJSIP_INV_STATE_CONFIRMED: // After ACK is sent/received.
      [_dualBottomBar removeFromSuperview];
      [self addSubview: _bottomBar];
      [self addSubview: _phonePad];
      _timer = [[NSTimer scheduledTimerWithTimeInterval:1.0
               target:self
               selector:@selector(timeout:)
               userInfo:nil
               repeats:YES] retain];
      [_timer fire];
      break;
    case PJSIP_INV_STATE_DISCONNECTED:
      if (_timer) 
      {
        [_timer invalidate];
        [_timer release];
        _timer = NULL;
      }
      [_lcd setLabel: NSLocalizedString(@"CALL_ENDED", @"Call view")];
      [_lcd setText:@""];
      _call_id = PJSUA_INVALID_ID;
      [_bottomBar removeFromSuperview];
      [_dualBottomBar removeFromSuperview];
      [_phonePad removeFromSuperview];
      break;
  }
}


@end
