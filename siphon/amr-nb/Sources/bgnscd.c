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
/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : bgnscd.c
*      Purpose          : Background noise source charateristic detector (SCD)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "bgnscd.h"
const char bgnscd_id[] = "@(#)$Id $" bgnscd_h;

#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"
#include "set_zero.h"
#include "gmed_n.h"
#include "sqrt_l.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*-----------------------------------------------------------------*
 *   Decoder constant parameters (defined in "cnst.h")             *
 *-----------------------------------------------------------------*
 *   L_FRAME       : Frame size.                                   *
 *   L_SUBFR       : Sub-frame size.                               *
 *-----------------------------------------------------------------*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Bgn_scd_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
Word16 Bgn_scd_init (Bgn_scdState *state)
{
   if (state == (Bgn_scdState *) NULL){
      fprintf(stderr, "Bgn_scd_init: invalid parameter\n");
      return -1;
   }

   Bgn_scd_reset(state);

   return 0;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
Word16 Bgn_scd_reset (Bgn_scdState *state)
{
   if (state == (Bgn_scdState *) NULL){
      fprintf(stderr, "Bgn_scd_reset: invalid parameter\n");
      return -1;
   }

   /* Static vectors to zero */
   Set_zero (state->frameEnergyHist, L_ENERGYHIST);

   /* Initialize hangover handling */
   state->bgHangover = 0;
   
   return 0;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = no bgn, 1 = bgn
*
**************************************************************************
*/
Word16 Bgn_scd (Bgn_scdState *st,      /* i : State variables for bgn SCD */
                Word16 ltpGainHist[],  /* i : LTP gain history            */
                Word16 speech[],       /* o : synthesis speech frame      */
                Word16 *voicedHangover /* o : # of frames after last 
                                              voiced frame                */
                )
{
   Word16 i;
   Word16 prevVoiced, inbgNoise;
   Word16 temp;
   Word16 ltpLimit, frameEnergyMin;
   Word16 currEnergy, noiseFloor, maxEnergy, maxEnergyLastPart;
   Word32 s;
   
   /* Update the inBackgroundNoise flag (valid for use in next frame if BFI) */
   /* it now works as a energy detector floating on top                      */ 
   /* not as good as a VAD.                                                  */

   currEnergy = 0;
   s = (Word32) 0;

   for (i = 0; i < L_FRAME; i++)
   {
       s = L_mac (s, speech[i], speech[i]);
   }

   s = L_shl(s, 2);  

   currEnergy = extract_h (s);

   frameEnergyMin = 32767;

   for (i = 0; i < L_ENERGYHIST; i++)
   {

      if (sub(st->frameEnergyHist[i], frameEnergyMin) < 0)
         frameEnergyMin = st->frameEnergyHist[i];
   }

   noiseFloor = shl (frameEnergyMin, 4); /* Frame Energy Margin of 16 */

   maxEnergy = st->frameEnergyHist[0];
   for (i = 1; i < L_ENERGYHIST-4; i++)
   {

      if ( sub (maxEnergy, st->frameEnergyHist[i]) < 0)
      {
         maxEnergy = st->frameEnergyHist[i];
      }
   }
   
   maxEnergyLastPart = st->frameEnergyHist[2*L_ENERGYHIST/3];
   for (i = 2*L_ENERGYHIST/3+1; i < L_ENERGYHIST; i++)
   {

      if ( sub (maxEnergyLastPart, st->frameEnergyHist[i] ) < 0)
      {
         maxEnergyLastPart = st->frameEnergyHist[i];
      }
   }

   inbgNoise = 0;        /* false */

   /* Do not consider silence as noise */
   /* Do not consider continuous high volume as noise */
   /* Or if the current noise level is very low */
   /* Mark as noise if under current noise limit */
   /* OR if the maximum energy is below the upper limit */


   if ( (sub(maxEnergy, LOWERNOISELIMIT) > 0) &&
        (sub(currEnergy, FRAMEENERGYLIMIT) < 0) &&
        (sub(currEnergy, LOWERNOISELIMIT) > 0) &&
        ( (sub(currEnergy, noiseFloor) < 0) ||
          (sub(maxEnergyLastPart, UPPERNOISELIMIT) < 0)))
   {

      if (sub(add(st->bgHangover, 1), 30) > 0)
      {
         st->bgHangover = 30;
      } else
      {
         st->bgHangover = add(st->bgHangover, 1);
      }
   }
   else
   {
      st->bgHangover = 0;
   }
   
   /* make final decision about frame state , act somewhat cautiosly */

   if (sub(st->bgHangover,1) > 0)
      inbgNoise = 1;       /* true  */

   for (i = 0; i < L_ENERGYHIST-1; i++)
   {
      st->frameEnergyHist[i] = st->frameEnergyHist[i+1];
   }
   st->frameEnergyHist[L_ENERGYHIST-1] = currEnergy;
   
   /* prepare for voicing decision; tighten the threshold after some 
      time in noise */
   ltpLimit = 13926;             /* 0.85  Q14 */

   if (sub(st->bgHangover, 8) > 0)
   {
      ltpLimit = 15565;          /* 0.95  Q14 */
   }

   if (sub(st->bgHangover, 15) > 0)
   {
      ltpLimit = 16383;          /* 1.00  Q14 */
   }

   /* weak sort of voicing indication. */
   prevVoiced = 0;        /* false */


   if (sub(gmed_n(&ltpGainHist[4], 5), ltpLimit) > 0)
   {
      prevVoiced = 1;     /* true  */
   }

   if (sub(st->bgHangover, 20) > 0) {
      if (sub(gmed_n(ltpGainHist, 9), ltpLimit) > 0)
      {
         prevVoiced = 1;  /* true  */
      }
      else
      {
         prevVoiced = 0;  /* false  */
      }
   }
   

   if (prevVoiced)
   {
      *voicedHangover = 0;
   }
   else
   {
      temp = add(*voicedHangover, 1);

      if (sub(temp, 10) > 0)
      {
         *voicedHangover = 10;
      }
      else
      {
         *voicedHangover = temp;
      }
   }

   return inbgNoise;
}
