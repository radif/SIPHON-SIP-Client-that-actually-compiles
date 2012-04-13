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
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : ol_ltp.c
*      Purpose          : Compute the open loop pitch lag.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "ol_ltp.h"
const char ol_ltp_id[] = "@(#)$Id $" ol_ltp_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include "typedef.h"
#include "cnst.h"
#include "pitch_ol.h"
#include "p_ol_wgh.h"
#include "count.h"
#include "basic_op.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
void ol_ltp(
    pitchOLWghtState *st, /* i/o : State struct                            */
    vadState *vadSt,      /* i/o : VAD state struct                        */
    enum Mode mode,       /* i   : coder mode                              */
    Word16 wsp[],         /* i   : signal used to compute the OL pitch, Q0 */
                          /*       uses signal[-pit_max] to signal[-1]     */
    Word16 *T_op,         /* o   : open loop pitch lag,                 Q0 */
    Word16 old_lags[],    /* i   : history with old stored Cl lags         */
    Word16 ol_gain_flg[], /* i   : OL gain flag                            */
    Word16 idx,           /* i   : index                                   */
    Flag dtx              /* i   : dtx flag; use dtx=1, do not use dtx=0   */
    )
{
   if (mode != MR102)
   {
      ol_gain_flg[0] = 0;
      ol_gain_flg[1] = 0;
   }
   

   if (mode == MR475 || mode == MR515)
   {
      *T_op = Pitch_ol(vadSt, mode, wsp, PIT_MIN, PIT_MAX, L_FRAME, idx, dtx);

   }
   else
   {
     if ( mode <= MR795 )
      {

         *T_op = Pitch_ol(vadSt, mode, wsp, PIT_MIN, PIT_MAX, L_FRAME_BY2,
                          idx, dtx);

      }
      else if ( mode == MR102 )
      {

         *T_op = Pitch_ol_wgh(st, vadSt, wsp, PIT_MIN, PIT_MAX, L_FRAME_BY2,
                              old_lags, ol_gain_flg, idx, dtx);

      }
      else
      {

         *T_op = Pitch_ol(vadSt, mode, wsp, PIT_MIN_MR122, PIT_MAX,
                          L_FRAME_BY2, idx, dtx);

      }
   }
}
