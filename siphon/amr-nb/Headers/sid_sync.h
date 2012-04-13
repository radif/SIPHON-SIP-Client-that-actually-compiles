/**
 *  AMR codec for iPhone and iPod Touch
 *  Copyright (C) 2009 Samuel <samuelv0304@gmail.com>
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
/*******************************************************************************
 Portions of this file are derived from the following 3GPP standard:

    3GPP TS 26.073
    ANSI-C code for the Adaptive Multi-Rate (AMR) speech codec
    Available from http://www.3gpp.org

 (C) 2004, 3GPP Organizational Partners (ARIB, ATIS, CCSA, ETSI, TTA, TTC)
 Permission to distribute, modify and use this file under the standard license
 terms listed above has been obtained from the copyright holder.
*******************************************************************************/
/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : sid_sync.h
*      Purpose          : To ensure that the mode only switches to a
*                         neighbouring mode
*
*****************************************************************************
*/
#ifndef sid_sync_h
#define sid_sync_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "mode.h"
#include "frame.h"
 
/*
******************************************************************************
*                         CONSTANTS
******************************************************************************
*/

/*
******************************************************************************
*                         DEFINITION OF DATA TYPES
******************************************************************************
*/
typedef struct {
    Word16 sid_update_rate;  /* Send SID Update every sid_update_rate frame */
    Word16 sid_update_counter; /* Number of frames since last SID          */
    Word16 sid_handover_debt;  /* Number of extra SID_UPD frames to schedule*/
    enum TXFrameType prev_ft;
} sid_syncState;
 
/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
 
/*
*****************************************************************************
*                         DECLARATION OF PROTOTYPES
*****************************************************************************
*/
int sid_sync_memSize();
int sid_sync_init (sid_syncState *st);
/* initialize one instance of the sid_sync module
   Stores pointer to state struct in *st. This pointer has to
   be passed to sid_sync in each call.
   returns 0 on success
 */
 
int sid_sync_reset (sid_syncState *st);
/* reset of sid_sync module (i.e. set state memory to zero)
   returns 0 on success
 */

int sid_sync_set_handover_debt (sid_syncState *st, /* i/o: sid_sync state  */
                                Word16 debtFrames);
/*  update handover debt 
    debtFrames extra SID_UPD are scheduled .
    to update remote decoder CNI states, right after an handover.
    (primarily for use on MS UL side )  
*/ 

 
void sid_sync(sid_syncState *st , /* i/o: sid_sync state      */
              enum Mode mode,
              enum TXFrameType *tx_frame_type); 
#endif
