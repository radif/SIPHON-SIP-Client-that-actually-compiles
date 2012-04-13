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

#ifndef __SIPHON_RING_H__
#define __SIPHON_RING_H__

//#include <pjsua-lib/pjsua.h>
#include "call.h"

PJ_BEGIN_DECL

void sip_ring_init(app_config_t *app_config);
void sip_ring_deinit(app_config_t *app_config);
void sip_ring_start(app_config_t *app_config);
void sip_ringback_start(app_config_t *app_config);
void sip_ring_stop(app_config_t *app_config);

PJ_END_DECL

#endif /* __SIPHON_RING_H__ */
