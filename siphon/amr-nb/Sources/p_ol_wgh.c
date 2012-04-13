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
*      File             : p_ol_wgh.c
*      Purpose          : Compute the open loop pitch lag with weighting      
*
*************************************************************************/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "p_ol_wgh.h"
const char p_ol_wgh_id[] = "@(#)$Id $" p_ol_wgh_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "corrwght.tab"
#include "gmed_n.h"
#include "inv_sqrt.h"
#include "vad.h"
#include "calc_cor.h"
#include "hp_max.h"
#include "copy.h"
                      
/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  Lag_max
 *
 *  PURPOSE: Find the lag that has maximum correlation of scal_sig[] in a
 *           given delay range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs the maximum correlation after normalization
 *      and the corresponding lag.
 *
 *************************************************************************/
static Word16 Lag_max ( /* o : lag found                               */
    vadState *vadSt,    /* i/o : VAD state struct                      */
    Word32 corr[],      /* i   : correlation vector.                   */
    Word16 scal_sig[],  /* i : scaled signal.                          */
    Word16 L_frame,     /* i : length of frame to compute pitch        */
    Word16 lag_max,     /* i : maximum lag                             */
    Word16 lag_min,     /* i : minimum lag                             */
    Word16 old_lag,     /* i : old open-loop lag                       */
    Word16 *cor_max,    /* o : normalized correlation of selected lag  */
    Word16 wght_flg,    /* i : is weighting function used              */ 
    Word16 *gain_flg,   /* o : open-loop flag                          */
    Flag dtx            /* i   : dtx flag; use dtx=1, do not use dtx=0 */
    )
{
    Word16 i, j;
    Word16 *p, *p1;
    Word32 max, t0;
    Word16 t0_h, t0_l;
    Word16 p_max;
    const Word16 *ww, *we;
    Word32 t1;
    
    ww = &corrweight[250];
    we = &corrweight[123 + lag_max - old_lag];

    max = MIN_32;
    p_max = lag_max;

    for (i = lag_max; i >= lag_min; i--)
    {
       t0 = corr[-i];
       
       /* Weighting of the correlation function.   */
       t0_h = (Word16) (t0 >> 16);
       t0_l = (Word16)((t0 >> 1) - (t0_h << 15));
       t0 = Mpy_32_16 (t0_h, t0_l, *ww);
       ww--;

       if (wght_flg > 0) {
          /* Weight the neighbourhood of the old lag. */
          t0_h = (Word16) (t0 >> 16);
          t0_l = (Word16)((t0 >> 1) - (t0_h << 15));
          t0 = Mpy_32_16 (t0_h, t0_l, *we);
          we--;
       }
       

       if (t0 >= max)
       {
          max = t0;
          p_max = i;
       }
    }
    
    p  = &scal_sig[0];
    p1 = &scal_sig[-p_max];
    t0 = 0;
    t1 = 0;
    
    for (j = 0; j < L_frame; j++, p++, p1++)
    {
      t0 += *p * *p1;
      t1 += *p1 * *p1;
    }
    t0 <<= 1;
    t1 <<= 1;

    if (dtx)
    {  /* no test() call since this if is only in simulation env */
#ifdef VAD2
       vadSt->L_Rmax = L_add(vadSt->L_Rmax, t0);   /* Save max correlation */
       vadSt->L_R0 =   L_add(vadSt->L_R0, t1);        /* Save max energy */
#else
       /* update and detect tone */
       vad_tone_detection_update (vadSt, 0);
       vad_tone_detection (vadSt, t0, t1);
#endif
    }
    
    /* gain flag is set according to the open_loop gain */
    /* is t2/t1 > 0.4 ? */    
    *gain_flg = round(L_msu(t0, round(t1), 13107));
    
    *cor_max = 0;

    return (p_max);
}

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   p_ol_wgh_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int p_ol_wgh_init (pitchOLWghtState *state)
{
    if (state == (pitchOLWghtState *) NULL){
        fprintf(stderr, "p_ol_wgh_init: invalid parameter\n");
        return -1;
    }

    p_ol_wgh_reset(state);

    return 0;
}
 
/*************************************************************************
*
*  Function:   p_ol_wgh_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int p_ol_wgh_reset (pitchOLWghtState *st)
{
   if (st == (pitchOLWghtState *) NULL){
      fprintf(stderr, "p_ol_wgh_reset: invalid parameter\n");
      return -1;
   }
   
   /* Reset pitch search states */
   st->old_T0_med = 40;
   st->ada_w = 0;
   st->wght_flg = 0; 
   
   return 0;
}
 
/*************************************************************************
*
*  Function:   p_ol_wgh
*  Purpose:    open-loop pitch search with weighting
*
**************************************************************************
*/
Word16 Pitch_ol_wgh (     /* o   : open loop pitch lag                            */
    pitchOLWghtState *st, /* i/o : State struct                                   */
    vadState *vadSt,      /* i/o : VAD state struct                               */
    Word16 signal[],      /* i   : signal used to compute the open loop pitch     */
                          /*       signal[-pit_max] to signal[-1] should be known */
    Word16 pit_min,       /* i   : minimum pitch lag                              */
    Word16 pit_max,       /* i   : maximum pitch lag                              */
    Word16 L_frame,       /* i   : length of frame to compute pitch               */
    Word16 old_lags[],    /* i   : history with old stored Cl lags                */
    Word16 ol_gain_flg[], /* i   : OL gain flag                                   */
    Word16 idx,           /* i   : index                                          */
    Flag dtx              /* i   : dtx flag; use dtx=1, do not use dtx=0          */
    )
{
    Word16 i;
    Word16 max1;
    Word16 p_max1;
    Word32 t0;
#ifndef VAD2
    Word16 corr_hp_max;
#endif
    Word32 corr[PIT_MAX+1], *corr_ptr;    

    /* Scaled signal */
    Word16 scaled_signal[PIT_MAX + L_FRAME];
    Word16 *scal_sig;

    scal_sig = &scaled_signal[pit_max];

    t0 = 0L;
    for (i = -pit_max; i < L_frame; i++)
    {
        /*t0 = L_mac (t0, signal[i], signal[i]);*/
      t0 += signal[i] * signal[i];
    }
    t0 <<= 1;
    /*--------------------------------------------------------*
     * Scaling of input signal.                               *
     *                                                        *
     *   if Overflow        -> scal_sig[i] = signal[i]>>2     *
     *   else if t0 < 1^22  -> scal_sig[i] = signal[i]<<2     *
     *   else               -> scal_sig[i] = signal[i]        *
     *--------------------------------------------------------*/

    /*--------------------------------------------------------*
     *  Verification for risk of overflow.                    *
     *--------------------------------------------------------*/


    if (t0 == MAX_32)               /* Test for overflow */
    {
        for (i = -pit_max; i < L_frame; i++)
        {
          scal_sig[i]  = signal[i] >> 3;
        }
    }
    else if (t0 < (Word32) 1048576L)
    {
        for (i = -pit_max; i < L_frame; i++)
        {
            scal_sig[i] = signal[i] << 3;
        }
    }
    else
    {
      scal_sig = signal;
    }

    /* calculate all coreelations of scal_sig, from pit_min to pit_max */
    corr_ptr = &corr[pit_max];
    comp_corr (scal_sig, L_frame, pit_max, pit_min, corr_ptr); 

    p_max1 = Lag_max (vadSt, corr_ptr, scal_sig, L_frame, pit_max, pit_min,
                      st->old_T0_med, &max1, st->wght_flg, &ol_gain_flg[idx],
                      dtx);



    if (ol_gain_flg[idx] > 0)
    {
       /* Calculate 5-point median of previous lags */
       for (i = 4; i > 0; i--) /* Shift buffer */
       {
          old_lags[i] = old_lags[i-1];
       }
       old_lags[0] = p_max1;
       st->old_T0_med = gmed_n (old_lags, 5);
       st->ada_w = 32767;                            /* Q15 = 1.0 */
    }	
    else
    {        
       st->old_T0_med = p_max1;
       st->ada_w = mult(st->ada_w, 29491);      /* = ada_w = ada_w * 0.9 */
    }
    

    if (st->ada_w < 9830)  /* ada_w - 0.3 */
    { 
       st->wght_flg = 0;
    } 
    else
    {
       st->wght_flg = 1;
    }

#ifndef VAD2
    if (dtx)
    {  /* no test() call since this if is only in simulation env */

       if (idx - 1)
       {
          /* calculate max high-passed filtered correlation of all lags */
          hp_max (corr_ptr, scal_sig, L_frame, pit_max, pit_min, &corr_hp_max); 
          
          /* update complex background detector */
          vad_complex_detection_update(vadSt, corr_hp_max); 
       }
    }
#endif
    
    return (p_max1);
}

