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
*      File             : cl_ltp.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "cl_ltp.h"
const char cl_ltp_id[] = "@(#)$Id $" cl_ltp_h;
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "convolve.h"
#include "g_pitch.h"
#include "pred_lt.h"
#include "pitch_fr.h"
#include "enc_lag3.h"
#include "enc_lag6.h"
#include "q_gain_p.h"
#include "ton_stab.h"

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
/*************************************************************************
*
*  Function:   cl_ltp_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int cl_ltp_init (clLtpState *state)
{
    if (state == (clLtpState *) NULL){
        fprintf(stderr, "cl_ltp_init: invalid parameter\n");
        return -1;
    }

    /* init the sub state */
    if (Pitch_fr_init(&state->pitchSt)) {
        cl_ltp_reset(state);
        return -1;
    }

    cl_ltp_reset(state);

    return 0;
}
 
/*************************************************************************
*
*  Function:   cl_ltp_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int cl_ltp_reset (clLtpState *state)
{
    if (state == (clLtpState *) NULL){
        fprintf(stderr, "cl_ltp_reset: invalid parameter\n");
        return -1;
    }
    
    /* Reset pitch search states */
    Pitch_fr_reset (&state->pitchSt);
    
    return 0;
}

/*************************************************************************
*
*  Function:   cl_ltp
*  Purpose:    closed-loop fractional pitch search
*
**************************************************************************
*/
void cl_ltp (
    clLtpState *clSt,    /* i/o : State struct                              */
    tonStabState *tonSt, /* i/o : State struct                              */
    enum Mode mode,      /* i   : coder mode                                */
    Word16 frameOffset,  /* i   : Offset to subframe                        */
    Word16 T_op[],       /* i   : Open loop pitch lags                      */
    Word16 *h1,          /* i   : Impulse response vector               Q12 */
    Word16 *exc,         /* i/o : Excitation vector                      Q0 */
    Word16 res2[],       /* i/o : Long term prediction residual          Q0 */
    Word16 xn[],         /* i   : Target vector for pitch search         Q0 */
    Word16 lsp_flag,     /* i   : LSP resonance flag                        */
    Word16 xn2[],        /* o   : Target vector for codebook search      Q0 */
    Word16 y1[],         /* o   : Filtered adaptive excitation           Q0 */
    Word16 *T0,          /* o   : Pitch delay (integer part)                */
    Word16 *T0_frac,     /* o   : Pitch delay (fractional part)             */
    Word16 *gain_pit,    /* o   : Pitch gain                            Q14 */
    Word16 g_coeff[],    /* o   : Correlations between xn, y1, & y2         */
    Word16 **anap,       /* o   : Analysis parameters                       */
    Word16 *gp_limit     /* o   : pitch gain limit                          */
)
{
    Word16 i;
    Word16 index;
    Word32 L_temp;     /* temporarily variable */
    Word16 resu3;      /* flag for upsample resolution */
    Word16 gpc_flag;
    
   /*----------------------------------------------------------------------*
    *                 Closed-loop fractional pitch search                  *
    *----------------------------------------------------------------------*/
   *T0 = Pitch_fr(&clSt->pitchSt,
                  mode, T_op, exc, xn, h1,
                  L_SUBFR, frameOffset,
                  T0_frac, &resu3, &index);
   
   *(*anap)++ = index;
   
   /*-----------------------------------------------------------------*
    *   - find unity gain pitch excitation (adapitve codebook entry)  *
    *     with fractional interpolation.                              *
    *   - find filtered pitch exc. y1[]=exc[] convolve with h1[])     *
    *   - compute pitch gain and limit between 0 and 1.2              *
    *   - update target vector for codebook search                    *
    *   - find LTP residual.                                          *
    *-----------------------------------------------------------------*/
   
   Pred_lt_3or6(exc, *T0, *T0_frac, L_SUBFR, resu3);
   
   Convolve(exc, h1, y1, L_SUBFR);
   
   /* gain_pit is Q14 for all modes */
   *gain_pit = G_pitch(mode, xn, y1, g_coeff, L_SUBFR);

   
   /* check if the pitch gain should be limit due to resonance in LPC filter */
   gpc_flag = 0;
   *gp_limit = MAX_16;

   if (lsp_flag != 0 && *gain_pit > GP_CLIP)
   {
       gpc_flag = check_gp_clipping(tonSt, *gain_pit);
   }

   /* special for the MR475, MR515 mode; limit the gain to 0.85 to */
   /* cope with bit errors in the decoder in a better way.         */

   if (mode == MR475 || mode == MR515) {

      if (*gain_pit > 13926) {
         *gain_pit = 13926;   /* 0.85 in Q14 */
      }


      if (gpc_flag != 0) {
          *gp_limit = GP_CLIP;
      }
   }
   else
   {

       if (gpc_flag != 0)
       {
           *gp_limit = GP_CLIP;
           *gain_pit = GP_CLIP;
       }           
       /* For MR122, gain_pit is quantized here and not in gainQuant */
       if ( mode ==  MR122 )
       {
           *(*anap)++ = q_gain_pitch(MR122, *gp_limit, gain_pit,
                                     NULL, NULL);
       }
   }

   /* update target vector und evaluate LTP residual */
   for (i = 0; i < L_SUBFR; i++) {
       L_temp = ((Word32)y1[i] * *gain_pit) >> 14;
       xn2[i] = xn[i] - (Word16)L_temp;

       L_temp   = ((Word32)exc[i] * *gain_pit) >> 14;
       res2[i] -= (Word16)L_temp;
   }
}
