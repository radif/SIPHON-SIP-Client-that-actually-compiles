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
*      File             : sid_sync.c
*
*****************************************************************************
*/
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "sid_sync.h"
const char sid_sync_id[] = "@(#)$Id $" sid_sync_h;
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "mode.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
int sid_sync_memSize()
{
  return sizeof(sid_syncState);
}

int sid_sync_init (sid_syncState *state)
{
    if (state == (sid_syncState *) NULL){
        fprintf(stderr, "sid_sync_init:invalid state parameter\n");
        return -1;
    }

    state->sid_update_rate = 8;

    return sid_sync_reset(state);
}

int sid_sync_reset (sid_syncState *st)
{
    st->sid_update_counter = 3;
    st->sid_handover_debt = 0;
    st->prev_ft = TX_SPEECH_GOOD;
    return 0;
}

int sid_sync_set_handover_debt (sid_syncState *st,
                                Word16 debtFrames) 
{
   /* debtFrames >= 0 */ 
   st->sid_handover_debt = debtFrames;
   return 0;
}


void sid_sync (sid_syncState *st, enum Mode mode,
               enum TXFrameType *tx_frame_type)
{
 
    if ( mode == MRDTX){

       st->sid_update_counter--;
       
        if (st->prev_ft == TX_SPEECH_GOOD) 
        {
           *tx_frame_type = TX_SID_FIRST;
           st->sid_update_counter = 3;
        } 
        else 
        {
           /* TX_SID_UPDATE or TX_NO_DATA */
           if( (st->sid_handover_debt > 0) &&
               (st->sid_update_counter > 2) )
           {
              /* ensure extra updates are  properly delayed after 
                 a possible SID_FIRST */
              *tx_frame_type = TX_SID_UPDATE;
              st->sid_handover_debt--;
           }
           else 
           {
              if (st->sid_update_counter == 0)
              {
                 *tx_frame_type = TX_SID_UPDATE;
                 st->sid_update_counter = st->sid_update_rate;
              } else {
                 *tx_frame_type = TX_NO_DATA;
              }
           }
        }
    }
    else
    {
       st->sid_update_counter = st->sid_update_rate ;
       *tx_frame_type = TX_SPEECH_GOOD;
    }
    st->prev_ft = *tx_frame_type;
}

