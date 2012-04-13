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
*      File             : c_g_aver.c
*      Purpose          : 
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "c_g_aver.h"
const char c_g_aver_id[] = "@(#)$Id $" c_g_aver_h;

#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "mode.h"
#include "basic_op.h"
#include "cnst.h"
#include "set_zero.h"

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
*  Function    : Cb_gain_average_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
Word16 Cb_gain_average_init (Cb_gain_averageState *state)
{
   if (state == (Cb_gain_averageState *) NULL){
      fprintf(stderr, "Cb_gain_average_init: invalid parameter\n");
      return -1;
   }

   Cb_gain_average_reset(state);

   return 0;
}

/*
**************************************************************************
*
*  Function    : Cb_gain_average_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
Word16 Cb_gain_average_reset (Cb_gain_averageState *state)
{
   if (state == (Cb_gain_averageState *) NULL){
      fprintf(stderr, "Cb_gain_average_reset: invalid parameter\n");
      return -1;
   }

   /* Static vectors to zero */
   Set_zero (state->cbGainHistory, L_CBGAINHIST);

   /* Initialize hangover handling */
   state->hangVar = 0;
   state->hangCount= 0;
   
   return 0;
}

/*
**************************************************************************
*
*  Function    : Cb_gain_average
*  Purpose     : 
*  Returns     : The mix cb gains for MR475, MR515, MR59, MR67, MR102; gain_code other modes 
*
**************************************************************************
*/
Word16 Cb_gain_average (    
   Cb_gain_averageState *st, /* i/o : State variables for CB gain avergeing   */
   enum Mode mode,           /* i   : AMR mode                                */
   Word16 gain_code,         /* i   : CB gain                              Q1 */
   Word16 lsp[],             /* i   : The LSP for the current frame       Q15 */
   Word16 lspAver[],         /* i   : The average of LSP for 8 frames     Q15 */
   Word16 bfi,               /* i   : bad frame indication flag               */
   Word16 prev_bf,           /* i   : previous bad frame indication flag      */
   Word16 pdfi,              /* i   : potential degraded bad frame ind flag   */
   Word16 prev_pdf,          /* i   : prev pot. degraded bad frame ind flag   */
   Word16 inBackgroundNoise, /* i   : background noise decision               */
   Word16 voicedHangover     /* i   : # of frames after last voiced frame     */
   )
{
   /*---------------------------------------------------------*
    * Compute mixed cb gain, used to make cb gain more        *
    * smooth in background noise for modes 5.15, 5.9 and 6.7  *
    * states that needs to be updated by all                  *
    *---------------------------------------------------------*/
   Word16 i;
   Word16 cbGainMix, diff, tmp_diff, bgMix, cbGainMean;
   Word32 L_sum;
   Word16 tmp[M], tmp1, tmp2, shift1, shift2, shift;

   /* set correct cbGainMix for MR74, MR795, MR122 */
   cbGainMix = gain_code;
   
   /*-------------------------------------------------------*
    *   Store list of CB gain needed in the CB gain         *
    *   averaging                                           *
    *-------------------------------------------------------*/
   for (i = 0; i < (L_CBGAINHIST-1); i++)
   {
      st->cbGainHistory[i] = st->cbGainHistory[i+1];
   }
   st->cbGainHistory[L_CBGAINHIST-1] = gain_code;
   
   /* compute lsp difference */
   for (i = 0; i < M; i++) {
      tmp1 = abs_s(sub(lspAver[i], lsp[i]));  /* Q15       */
      shift1 = sub(norm_s(tmp1), 1);          /* Qn        */
      tmp1 = shl(tmp1, shift1);               /* Q15+Qn    */
      shift2 = norm_s(lspAver[i]);            /* Qm        */
      tmp2 = shl(lspAver[i], shift2);         /* Q15+Qm    */
      tmp[i] = div_s(tmp1, tmp2);             /* Q15+(Q15+Qn)-(Q15+Qm) */

      shift = sub(add(2, shift1), shift2);

      if (shift >= 0)
      {
         tmp[i] = shr(tmp[i], shift);           /* Q15+Qn-Qm-Qx=Q13 */
      }
      else
      {
         tmp[i] = shl(tmp[i], negate(shift));   /* Q15+Qn-Qm-Qx=Q13 */
      }
   }
   
   diff = tmp[0];
   for (i = 1; i < M; i++) { 
      diff = add(diff, tmp[i]);       /* Q13 */
   }   

   /* Compute hangover */

   if (diff > 5325)  /* 0.65 in Q11 */
   {
      st->hangVar += 1;
   }
   else
   {
      st->hangVar = 0;
   }


   if (st->hangVar > 10)
   {
      st->hangCount = 0;  /* Speech period, reset hangover variable */
   }

   /* Compute mix constant (bgMix) */   
   bgMix = 8192;    /* 1 in Q13 */

   if (mode <= MR67 || mode == MR102)
      /* MR475, MR515, MR59, MR67, MR102 */
   {
      /* if errors and presumed noise make smoothing probability stronger */

      if (((((pdfi != 0) && (prev_pdf != 0)) || (bfi != 0) || (prev_bf != 0)) &&
          (voicedHangover > 1) && (inBackgroundNoise != 0) &&
          (mode < MR67/*(sub(mode, MR475) == 0) ||
           (sub(mode, MR515) == 0) ||
           (sub(mode, MR59) == 0)*/) ))
      {
         /* bgMix = min(0.25, max(0.0, diff-0.55)) / 0.25; */
         tmp_diff = sub(diff, 4506);   /* 0.55 in Q13 */

         /* max(0.0, diff-0.55) */

         if (tmp_diff > 0)
         {
            tmp1 = tmp_diff;
         }
         else
         {
            tmp1 = 0;
         }

         /* min(0.25, tmp1) */

         if (tmp1 > 2048)
         {
            bgMix = 8192;
         }
         else
         {
            bgMix = tmp1 << 2; /*shl(tmp1, 2);*/
         }
      }
      else
      {
         /* bgMix = min(0.25, max(0.0, diff-0.40)) / 0.25; */        
         tmp_diff = sub(diff, 3277); /* 0.4 in Q13 */

         /* max(0.0, diff-0.40) */

         if (tmp_diff > 0)
         {
            tmp1 = tmp_diff;
         }
         else
         {
            tmp1 = 0;
         }

         /* min(0.25, tmp1) */

         if (tmp1 > 2048)
         {
            bgMix = 8192;
         }
         else
         {
            bgMix = tmp1 << 2; /*shl(tmp1, 2);*/
         }
      }


      if ((st->hangCount < 40) || (diff > 5325)) /* 0.65 in Q13 */
      {
         bgMix = 8192;  /* disable mix if too short time since */
      }

      /* Smoothen the cb gain trajectory  */
      /* smoothing depends on mix constant bgMix */
      L_sum = L_mult(6554, st->cbGainHistory[2]);     /* 0.2 in Q15; L_sum in Q17 */   
      for (i = 3; i < L_CBGAINHIST; i++)
      {
         L_sum = L_mac(L_sum, 6554, st->cbGainHistory[i]);
      }
      cbGainMean = round(L_sum);                      /* Q1 */
      
      /* more smoothing in error and bg noise (NB no DFI used  here) */

      if (((bfi != 0) || (prev_bf != 0)) && (inBackgroundNoise != 0) &&
          (mode < MR67/*(sub(mode, MR475) == 0) ||
           (sub(mode, MR515) == 0) ||
           (sub(mode, MR59) == 0)*/) )
      {
         L_sum = L_mult(4681, st->cbGainHistory[0]);  /* 0.143 in Q15; L_sum in Q17 */     
         for (i = 1; i < L_CBGAINHIST; i++)
         {
            L_sum = L_mac(L_sum, 4681, st->cbGainHistory[i]);
         }
         cbGainMean = round(L_sum);                   /* Q1 */
      }
      
      /* cbGainMix = bgMix*cbGainMix + (1-bgMix)*cbGainMean; */
      L_sum = L_mult(bgMix, cbGainMix);               /* L_sum in Q15 */
      L_sum = L_mac(L_sum, 8192, cbGainMean);         
      L_sum = L_msu(L_sum, bgMix, cbGainMean);
      cbGainMix = round(L_shl(L_sum, 2));             /* Q1 */
   }
   
   st->hangCount = add(st->hangCount, 1);
   return cbGainMix;
}
