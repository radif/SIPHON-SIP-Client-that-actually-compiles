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

#include "ring.h"

#define THIS_FILE	"ring.m"

/* Ringtones                US	       UK  */
#define RINGBACK_FREQ1	    440	    /* 400 */
#define RINGBACK_FREQ2	    480	    /* 450 */
#define RINGBACK_ON         2000    /* 400 */
#define RINGBACK_OFF        4000    /* 200 */
#define RINGBACK_CNT        1       /* 2   */
#define RINGBACK_INTERVAL   4000    /* 2000 */

#define RING_FREQ1	  800
#define RING_FREQ2	  640
#define RING_ON		    200
#define RING_OFF	    100
#define RING_CNT	    3
#define RING_INTERVAL	3000

#if RING_FILE
static
void sip_ring_callback(CFRunLoopTimerRef timer, void *info)
{
  app_config_t *app_config = (app_config_t *)info;
  AudioServicesPlayAlertSound(app_config->ring_id);
}
#endif

void sip_ring_init(app_config_t *app_config)
{
  unsigned i, samples_per_frame;
	pjmedia_tone_desc tone[RING_CNT+RINGBACK_CNT];
	pj_str_t name;
  pj_status_t status;
  
  app_config->ringback_slot = PJSUA_INVALID_ID;
#if !RING_FILE
  app_config->ring_slot = PJSUA_INVALID_ID;
#else
  app_config->ring_timer = NULL;
#endif

	samples_per_frame = app_config->media_cfg.audio_frame_ptime * 
      app_config->media_cfg.clock_rate *
      app_config->media_cfg.channel_count / 1000;
  
	/* Ringback tone (call is ringing) */
	name = pj_str("ringback");
	status = pjmedia_tonegen_create2(app_config->pool, &name, 
                                   app_config->media_cfg.clock_rate,
                                   app_config->media_cfg.channel_count, 
                                   samples_per_frame,
                                   16, PJMEDIA_TONEGEN_LOOP, 
                                   &app_config->ringback_port);
	if (status != PJ_SUCCESS)
    //goto on_error;
    return; // FIXME
  
	pj_bzero(&tone, sizeof(tone));
	for (i=0; i<RINGBACK_CNT; ++i) 
  {
    tone[i].freq1 = RINGBACK_FREQ1;
    tone[i].freq2 = RINGBACK_FREQ2;
    tone[i].on_msec = RINGBACK_ON;
    tone[i].off_msec = RINGBACK_OFF;
	}
	tone[RINGBACK_CNT-1].off_msec = RINGBACK_INTERVAL;
  
	pjmedia_tonegen_play(app_config->ringback_port, RINGBACK_CNT, tone,
                       PJMEDIA_TONEGEN_LOOP);

	status = pjsua_conf_add_port(app_config->pool, app_config->ringback_port,
                               &app_config->ringback_slot);
	if (status != PJ_SUCCESS)
    //goto on_error;
    return; // FIXME
  
	/* Ring (to alert incoming call) */
#if !RING_FILE
	name = pj_str("ring");
	status = pjmedia_tonegen_create2(app_config->pool, &name, 
                                   app_config->media_cfg.clock_rate,
                                   app_config->media_cfg.channel_count, 
                                   samples_per_frame,
                                   16, PJMEDIA_TONEGEN_LOOP, 
                                   &app_config->ring_port);
	if (status != PJ_SUCCESS)
    //goto on_error;
    return; // FIXME
  
	for (i=0; i<RING_CNT; ++i) 
  {
    tone[i].freq1 = RING_FREQ1;
    tone[i].freq2 = RING_FREQ2;
    tone[i].on_msec = RING_ON;
    tone[i].off_msec = RING_OFF;
	}
	tone[RING_CNT-1].off_msec = RING_INTERVAL;
  
	pjmedia_tonegen_play(app_config->ring_port, RING_CNT, tone, 
                       PJMEDIA_TONEGEN_LOOP);
  
	status = pjsua_conf_add_port(app_config->pool, app_config->ring_port,
                               &app_config->ring_slot);
	if (status != PJ_SUCCESS)
    //goto on_error;
    return; // FIXME
#else
  /* It is easier to use pjsua_player_create/pjsua_player_destroy
   * but it is not possible to know if the user has configured the Settings 
   * application to vibrate on ring or not.
   */
  CFURLRef soundFileURLRef;
  SystemSoundID aSoundID;
  OSStatus oStatus;
  // Get the main bundle for the app
	CFBundleRef mainBundle = CFBundleGetMainBundle ();
  // Get the URL to the sound file to play
	soundFileURLRef  =	CFBundleCopyResourceURL (mainBundle, CFSTR ("phone"),
                                               CFSTR ("caf"), NULL);
  oStatus = AudioServicesCreateSystemSoundID (soundFileURLRef, &aSoundID);
  if (oStatus == kAudioServicesNoError)
    app_config->ring_id = aSoundID;
  else
    app_config->ring_id = kSystemSoundID_Vibrate;
  CFRelease(soundFileURLRef);

#endif
}

void sip_ring_start(app_config_t *app_config)
{
  //if (app_config.call_data[call_id].ring_on)
  //  return;
  //app_config.call_data[call_id].ring_on = PJ_TRUE;
  if (app_config->ring_on)
    return;
  app_config->ring_on = PJ_TRUE;
  
#if !RING_FILE
  if (++app_config->ring_cnt == 1 && 
      app_config->ring_slot != PJSUA_INVALID_ID) 
  {
    UInt32 route = kAudioSessionOverrideAudioRoute_Speaker;
    AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, 
                             sizeof(route), &route);
    pjsua_conf_connect(app_config->ring_slot, 0);
  }
#else
  if (++app_config->ring_cnt == 1) 
  {
    //AudioServicesPlayAlertSound(app_config->ring_id);
    //UInt32 route = kAudioSessionOverrideAudioRoute_Speaker;
    //AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, 
    //                         sizeof(route), &route);
    CFRunLoopTimerContext context = {0, (void *)app_config, NULL, NULL, NULL};
    app_config->ring_timer = CFRunLoopTimerCreate(kCFAllocatorDefault, 
                                                  CFAbsoluteTimeGetCurrent(),
                                                  2., 
                                                  0, 0, sip_ring_callback, 
                                                  &context);
    CFRunLoopAddTimer(CFRunLoopGetMain(), app_config->ring_timer, kCFRunLoopCommonModes);
  }
#endif
}

void sip_ringback_start(app_config_t *app_config)
{
  //if (app_config.call_data[call_id].ringback_on)
  //  return;
  //app_config.call_data[call_id].ringback_on = PJ_TRUE;
  if (app_config->ringback_on)
    return;
  app_config->ringback_on = PJ_TRUE;
  
  
  if (++app_config->ringback_cnt == 1 && 
      app_config->ringback_slot != PJSUA_INVALID_ID) 
  {
    pjsua_conf_connect(app_config->ringback_slot, 0);
  }
}

void sip_ring_stop(app_config_t *app_config)
{
  //if (app_config.call_data[call_id].ringback_on)
  if (app_config->ringback_on) 
  {
  //  app_config.call_data[call_id].ringback_on = PJ_FALSE;
    app_config->ringback_on = PJ_FALSE;
    
    pj_assert(app_config->ringback_cnt>0);
    if (--app_config->ringback_cnt == 0 && 
        app_config->ringback_slot != PJSUA_INVALID_ID) 
    {
	    pjsua_conf_disconnect(app_config->ringback_slot, 0);
	    pjmedia_tonegen_rewind(app_config->ringback_port);
    }
  }
  
  //if (app_config.call_data[call_id].ring_on)
  if (app_config->ring_on)
  {
  //  app_config.call_data[call_id].ring_on = PJ_FALSE;
    app_config->ring_on = PJ_FALSE;
    
    pj_assert(app_config->ring_cnt>0);
#if !RING_FILE
    if (--app_config->ring_cnt == 0 && 
        app_config->ring_slot != PJSUA_INVALID_ID) 
    {
	    pjsua_conf_disconnect(app_config->ring_slot, 0);
      UInt32 route = kAudioSessionOverrideAudioRoute_None;
      AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, 
                               sizeof(route), &route);
	    pjmedia_tonegen_rewind(app_config->ring_port);
    }
#else
    if (--app_config->ring_cnt == 0) 
    {
     // UInt32 route = kAudioSessionOverrideAudioRoute_None;
     // AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, 
     //                          sizeof(route), &route);
      CFRunLoopTimerInvalidate(app_config->ring_timer);
      CFRelease(app_config->ring_timer);
      app_config->ring_timer = NULL;
    }
#endif
  }
}

void sip_ring_deinit(app_config_t *app_config)
{
  /* Close ringback port */
  if (app_config->ringback_port && 
      app_config->ringback_slot != PJSUA_INVALID_ID) 
  {
    pjsua_conf_remove_port(app_config->ringback_slot);
    app_config->ringback_slot = PJSUA_INVALID_ID;
    pjmedia_port_destroy(app_config->ringback_port);
    app_config->ringback_port = NULL;
  }
  
  /* Close ring port */
#if !RING_FILE
  if (app_config->ring_port && app_config->ring_slot != PJSUA_INVALID_ID) 
  {
    pjsua_conf_remove_port(app_config->ring_slot);
    app_config->ring_slot = PJSUA_INVALID_ID;
    pjmedia_port_destroy(app_config->ring_port);
    app_config->ring_port = NULL;
  }
#else
  if (app_config->ring_timer)
  {
    CFRunLoopRemoveTimer (CFRunLoopGetMain(), app_config->ring_timer, 
                          kCFRunLoopCommonModes);
    CFRelease(app_config->ring_timer);
    app_config->ring_timer = NULL;
  }
  if (app_config->ring_id != kSystemSoundID_Vibrate)
    AudioServicesDisposeSystemSoundID (app_config->ring_id);
#endif
}
