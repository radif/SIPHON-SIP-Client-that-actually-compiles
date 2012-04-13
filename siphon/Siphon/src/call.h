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

#ifndef __SIPHON_CALL_H__
#define __SIPHON_CALL_H__

#include <pjsua-lib/pjsua.h>
#import <Foundation/NSString.h>

#define VOLUME_MULT 8.0f

typedef struct app_config_t
{
  pj_pool_t             *pool;

  pjsua_config           cfg;
  pjsua_logging_config   log_cfg;
  pjsua_media_config     media_cfg;
  
  pjsua_transport_config udp_cfg;
  pjsua_transport_config rtp_cfg;
  
//  pjsua_acc_config       acc_cfg;
  
//  float mic_level;
//  float speaker_level;
} app_config_t;

extern NSString *kSIPCallState;

PJ_BEGIN_DECL

pj_status_t sip_startup    (app_config_t *app_config);
pj_status_t sip_cleanup    (app_config_t *app_config);

pj_status_t sip_connect    (pj_pool_t *pool, pjsua_acc_id *acc_id);
pj_status_t sip_disconnect (pjsua_acc_id *acc_id);

pj_status_t sip_dial       (pjsua_acc_id acc_id, const char *number, 
                            pjsua_call_id *call_id);
pj_status_t sip_answer     ();
pj_status_t sip_hangup     (pjsua_call_id *call_id);

PJ_END_DECL

#endif /* __SIPHON_CALL_H__ */
