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

#import <Foundation/NSUserDefaults.h>
#import <Celestial/AVSystemController.h>
#import "NSNotificationAdditions.h"

#include <pjsua-lib/pjsua.h>

#import "Siphon.h"
#import "CallView.h"

#include "call.h"
#include "ring.h"
#include "dtmf.h"

#define THIS_FILE "call.m"

NSString *kSIPStartOfCall = @"StartOfCall";
NSString *kSIPEndOfCall   = @"EndOfCall";

NSString *kSIPCallState         = @"CallState";

NSString *kSIPStateNull         = @"StateNull";
NSString *kSIPStateCalling      = @"StateCalling";
NSString *kSIPStateIncoming     = @"StateIncoming";
NSString *kSIPStateEarly        = @"StateEarly";
NSString *kSIPStateConnecting   = @"StateConnecting";
NSString *kSIPStateConfirmed    = @"StateConfirmed";
NSString *kSIPStateDisconnected = @"StateDisconnected";

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
  pjsua_call_info ci;
//  NSNumber *value;
  
  PJ_UNUSED_ARG(e);
  
  pjsua_call_get_info(call_id, &ci);
  
  PJ_LOG(1,(THIS_FILE, "Call %d state=%.*s", call_id,
    (int)ci.state_text.slen, ci.state_text.ptr));
  
  NSAutoreleasePool *autoreleasePool = [[ NSAutoreleasePool alloc ] init];
  
  NSDictionary *userinfo = [NSDictionary dictionaryWithObjectsAndKeys:
    [NSNumber numberWithInt: call_id], @"CallID",
    [NSNumber numberWithInt: ci.state], @"CallState",
    nil];
  switch(ci.state)
  {
  case PJSIP_INV_STATE_NULL: // Before INVITE is sent or received.
    break;
  case PJSIP_INV_STATE_CALLING:      // After INVITE is sent.
  case PJSIP_INV_STATE_INCOMING:     // After INVITE is received.
	  pjsua_set_snd_dev(-1, -1); 	
  case PJSIP_INV_STATE_EARLY:        // After response with To tag.
  case PJSIP_INV_STATE_CONNECTING:   // After 2xx is sent/received.
  case PJSIP_INV_STATE_CONFIRMED:    // After ACK is sent/received.
    [[NSNotificationCenter defaultCenter] 
      postNotificationOnMainThreadWithName:kSIPCallState object:nil
      userInfo: userinfo];
    break;
  case PJSIP_INV_STATE_DISCONNECTED: // Session is terminated.
    sip_call_deinit_tonegen(call_id);
    [[NSNotificationCenter defaultCenter] 
      postNotificationOnMainThreadWithName:kSIPCallState object:nil
      userInfo: userinfo];
    pjsua_set_null_snd_dev();
    break;
  }
  [ autoreleasePool release ];
}

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
           pjsip_rx_data *rdata)
{
  pjsua_call_info ci;

  PJ_UNUSED_ARG(acc_id);
  PJ_UNUSED_ARG(rdata);

  pjsua_call_get_info(call_id, &ci);

  PJ_LOG(1,(THIS_FILE, "Incoming call from %.*s!!",
     (int)ci.remote_info.slen,
     ci.remote_info.ptr));
  
  /* Automatically answer incoming calls with 180/RINGING */
  pjsua_call_answer(call_id, 180, NULL, NULL);

  sip_ring_startup(call_id);
  /*PJ_LOG(3,(THIS_FILE,
      "Incoming call for account %d!\n"
      "From: %s\n"
      "To: %s\n"
      "Press a to answer or h to reject call",
      acc_id,
      ci.remote_info.ptr,
      ci.local_info.ptr));*/
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    AVSystemController *avs;
    NSString *audioDeviceName;
    float     volume;

    pjsua_call_get_info(call_id, &ci);
//    PJ_LOG(3,(THIS_FILE,"on_call_media_state status %d count %d",
//      ci.media_status
//      pjmedia_conf_get_connect_count()));

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) 
    {
      // When media is active, connect call to sound device.
      pjsua_conf_connect(ci.conf_slot, 0);
      pjsua_conf_connect(0, ci.conf_slot);

      pjsua_conf_adjust_rx_level(0, 3.0);
      // TODO: deplace somewhere but I don't know where for the moment !!! 
      {
        NSAutoreleasePool *autoreleasePool = [[ NSAutoreleasePool alloc ] init];
        avs = [ AVSystemController sharedAVSystemController ];
        [ avs getActiveCategoryVolume: &volume andName: &audioDeviceName ];
        [ autoreleasePool release ];
      }
      pjsua_conf_adjust_tx_level(0, VOLUME_MULT * volume);
    }
}

/*
 * Handler registration status has changed.
 */
static void on_reg_state(pjsua_acc_id acc_id)
{
  pjsua_acc_info info;

  pjsua_acc_get_info(acc_id, &info);

  // Log already written.
  NSLog(@"Status changed acc %d %.*s", acc_id, (int)info.status_text.slen, 
      info.status_text.ptr);
}

/* */
pj_status_t sip_startup(app_config_t *app_config)
{
  pj_status_t status;
  long val;
  char tmp[80];

  /* Create pjsua first! */
  status = pjsua_create();
  if (status != PJ_SUCCESS)
    return status;

  /* Create pool for application */
  app_config->pool = pjsua_pool_create("pjsua", 1000, 1000);
  
  /* Initialize default config */
  pjsua_config_default(&(app_config->cfg));
  pj_ansi_snprintf(tmp, 80, "Siphon PjSip v%s/%s", pj_get_version(), PJ_OS_NAME);
  pj_strdup2_with_null(app_config->pool, &(app_config->cfg.user_agent), tmp);
  
  pjsua_logging_config_default(&(app_config->log_cfg));
  
  val = [[NSUserDefaults standardUserDefaults] integerForKey: 
    @"sip_loggingEnabled"];

  app_config->log_cfg.msg_logging = (val ? PJ_TRUE : PJ_FALSE);
  app_config->log_cfg.console_level = val;
//  app_config->log_cfg.console_level = 0;
  app_config->log_cfg.level = val;
  if (val != 0)
  {
//    NSString *path = [[[NSBundle mainBundle] bundlePath] 
//      stringByAppendingString: @"/log.txt"];
//    app_config->log_cfg.log_filename = pj_strdup3(app_config->pool, 
//      [path UTF8String]);
  }

  pjsua_media_config_default(&(app_config->media_cfg));
  app_config->media_cfg.clock_rate = 8000;
  app_config->media_cfg.snd_clock_rate = 8000;
  app_config->media_cfg.ec_tail_len = 0;
//  app_config.media_cfg.quality = 2;
//  app_config->media_cfg.channel_count = 2;
  
  pjsua_transport_config_default(&(app_config->udp_cfg));
  val = [[NSUserDefaults standardUserDefaults] integerForKey: @"sip_localport"];
  if (val < 0 || val > 65535)
  {
    PJ_LOG(1,(THIS_FILE, 
      "Error: local-port argument value (expecting 0-65535"));
    return PJ_EINVAL;
  }
  app_config->udp_cfg.port = val;
  
  pjsua_transport_config_default(&(app_config->rtp_cfg));
  app_config->rtp_cfg.port = [[NSUserDefaults standardUserDefaults] 
    integerForKey: @"sip_rtpport"];  
  if (app_config->rtp_cfg.port == 0) 
  {
    enum { START_PORT=4000 };
    unsigned range;

    range = (65535-START_PORT-PJSUA_MAX_CALLS*2);
    app_config->rtp_cfg.port = START_PORT + 
            ((pj_rand() % range) & 0xFFFE);
  }

  if (app_config->rtp_cfg.port < 1 || app_config->rtp_cfg.port > 65535) 
  {
    PJ_LOG(1,(THIS_FILE,
        "Error: rtp-port argument value (expecting 1-65535"));
    return PJ_EINVAL;
  }

  /* Initialize application callbacks */
  app_config->cfg.cb.on_incoming_call = &on_incoming_call;
  app_config->cfg.cb.on_call_media_state = &on_call_media_state;
  app_config->cfg.cb.on_call_state = &on_call_state;
  app_config->cfg.cb.on_reg_state = &on_reg_state;
  
//  [[[NSUserDefaults standardUserDefaults] stringForKey: 
//      @"sip_stunDomain"] UTF8String];
//  app_config->cfg.stun_domain = pj_str(pj_optarg); /* STUN domain */
//  [[[NSUserDefaults standardUserDefaults] stringForKey: 
//      @"sip_stunServer"] UTF8String];
//  app_config->cfg.stun_host = pj_str(pj_optarg);   /* STUN server */
  
  /* Initialize pjsua */
  status = pjsua_init(&app_config->cfg, &app_config->log_cfg, 
    &app_config->media_cfg);
  if (status != PJ_SUCCESS)
    goto error;

  /* Add UDP transport. */
  status = pjsua_transport_create(PJSIP_TRANSPORT_UDP,
          &app_config->udp_cfg, NULL/*&transport_id*/);
  if (status != PJ_SUCCESS)
    goto error;
      
  /* Add RTP transports */
  status = pjsua_media_transports_create(&app_config->rtp_cfg);
  if (status != PJ_SUCCESS)
    goto error;
 
  /* Initialization is done, now start pjsua */
  status = pjsua_start();

  return status;

error:
  pj_pool_release(app_config->pool);
  app_config->pool = NULL;
  return status;
}


/* */
pj_status_t sip_cleanup(app_config_t *app_config)
{
  pj_status_t status;

  if (app_config->pool) 
  {
    pj_pool_release(app_config->pool);
    app_config->pool = NULL;
  }

  /* Destroy pjsua */
  status = pjsua_destroy();
  
  pj_bzero(app_config, sizeof(app_config_t));
  
  return status;
  
}

/* */
pj_status_t sip_connect(pj_pool_t *pool, pjsua_acc_id *acc_id)
{
  pj_status_t status;
  pjsua_acc_config acc_cfg;
  const char *uname;
  const char *passwd;
  const char *server;
  
  // TODO Verify if wifi is connected, if not verify if user wants edge connection 
  
  uname  = [[[NSUserDefaults standardUserDefaults] stringForKey: 
    @"sip_user"] UTF8String];
  passwd = [[[NSUserDefaults standardUserDefaults] stringForKey: 
    @"sip_password"] UTF8String];
  server = [[[NSUserDefaults standardUserDefaults] stringForKey: 
    @"sip_server"] UTF8String];
  
  pjsua_acc_config_default(&acc_cfg);

  // ID
  acc_cfg.id.ptr = (char*) pj_pool_alloc(/*app_config.*/pool, PJSIP_MAX_URL_SIZE);
  acc_cfg.id.slen = pj_ansi_snprintf(acc_cfg.id.ptr, PJSIP_MAX_URL_SIZE, 
    "sip:%s@%s", uname, server);
  if (pjsua_verify_sip_url(acc_cfg.id.ptr) != 0) 
  {
    PJ_LOG(1,(THIS_FILE, "Error: invalid SIP URL '%s' in local id argument", 
      acc_cfg.id));
    return PJ_EINVAL;
  }
  
  // Registar
  acc_cfg.reg_uri.ptr = (char*) pj_pool_alloc(/*app_config.*/pool, 
    PJSIP_MAX_URL_SIZE);
  acc_cfg.reg_uri.slen = pj_ansi_snprintf(acc_cfg.reg_uri.ptr, 
    PJSIP_MAX_URL_SIZE, "sip:%s", server);
  if (pjsua_verify_sip_url(acc_cfg.reg_uri.ptr) != 0) 
  {
    PJ_LOG(1,(THIS_FILE,  "Error: invalid SIP URL '%s' in registrar argument",
      acc_cfg.reg_uri));
    return PJ_EINVAL;
  }

  //acc_cfg.id = pj_str(id);
  //acc_cfg.reg_uri = pj_str(registrar);
  acc_cfg.cred_count = 1;
  acc_cfg.cred_info[0].scheme = pj_str("Digest");
  acc_cfg.cred_info[0].realm = pj_str("*");//pj_str(realm);
  acc_cfg.cred_info[0].username = pj_str((char *)uname);
  acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  acc_cfg.cred_info[0].data = pj_str((char *)passwd);
  
  acc_cfg.publish_enabled = PJ_TRUE;
  acc_cfg.allow_contact_rewrite = PJ_FALSE; // FIXME pb with SIP provider Free !!
  
  // FIXME: gestion du message 423 dans pjsip
  acc_cfg.reg_timeout = [[NSUserDefaults standardUserDefaults] integerForKey: 
    @"sip_regtimeout"];
  if (acc_cfg.reg_timeout < 1 || acc_cfg.reg_timeout > 3600) 
  {
    PJ_LOG(1,(THIS_FILE, 
      "Error: invalid value for timeout (expecting 1-3600)"));
    return PJ_EINVAL;
  }

  pjsua_set_null_snd_dev();
  
  status = pjsua_acc_add(&acc_cfg, PJ_TRUE, acc_id);
  if (status != PJ_SUCCESS) 
  {
      pjsua_perror(THIS_FILE, "Error adding new account", status);
  }
  
  return status;
}

/* */
pj_status_t sip_disconnect(pjsua_acc_id *acc_id)
{
  pj_status_t status = PJ_SUCCESS;
  
  if (pjsua_acc_is_valid(*acc_id))
  {
    status = pjsua_acc_del(*acc_id);
    if (status == PJ_SUCCESS)
      *acc_id = PJSUA_INVALID_ID;
  }
  
  return status;
}

/* */
pj_status_t sip_dial(pjsua_acc_id acc_id, const char *number, 
  pjsua_call_id *call_id)
{
  // FIXME: récupérer le domain à partir du compte (acc_id);
  // TODO be careful app already mustn't be in communication!
  // TODO if not SIP connected, use GSM ? NSURL with 'tel' protocol
  pj_status_t status = PJ_SUCCESS;
  char uri[256];
  pj_str_t pj_uri;
  const char *sip_domain;
  
  sip_domain = [[[NSUserDefaults standardUserDefaults] stringForKey: 
    @"sip_server"] UTF8String];
  
  pj_ansi_snprintf(uri, 256, "sip:%s@%s", number, sip_domain);
  PJ_LOG(5,(THIS_FILE,  "Calling URI \"%s\".", uri));

  status = pjsua_verify_sip_url(uri);
  if (status != PJ_SUCCESS) 
  {
    PJ_LOG(1,(THIS_FILE,  "Invalid URL \"%s\".", uri));
    pjsua_perror(THIS_FILE, "Invalid URL", status);
    return status;
  }
  
  pj_uri = pj_str(uri);
  
  status = pjsua_call_make_call(acc_id, &pj_uri, 0, NULL, NULL, call_id);
  if (status != PJ_SUCCESS)
  {
    pjsua_perror(THIS_FILE, "Error making call", status);
  }

  return status;
}

/* */
pj_status_t sip_answer(pjsua_call_id *call_id)
{
  pj_status_t status;
  
  sip_ring_cleanup(*call_id);

  status = pjsua_call_answer(*call_id, 200, NULL, NULL);
  if (status != PJ_SUCCESS)
  {
    *call_id = PJSUA_INVALID_ID;
  }

  return status;
}

/* */
pj_status_t sip_hangup(pjsua_call_id *call_id)
{
  pj_status_t status = PJ_SUCCESS;
  
  pjsua_call_hangup_all();
  /* TODO Hangup current calls */
  //status = pjsua_call_hangup(*call_id, 0, NULL, NULL);
  *call_id = PJSUA_INVALID_ID;
  
  return status;
}

