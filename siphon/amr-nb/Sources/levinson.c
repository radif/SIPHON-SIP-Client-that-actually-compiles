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
*      File             : levinson.c
*      Purpose          : Levinson-Durbin algorithm in double precision.
*                       : To compute the LP filter parameters from the
*                       : speech autocorrelations.
*
*****************************************************************************
*/
 
 
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "levinson.h"
const char levinson_id[] = "@(#)$Id $" levinson_h;
 
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
#include "count.h"
#include "cnst.h"
 
/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*---------------------------------------------------------------*
 *    Constants (defined in "cnst.h")                            *
 *---------------------------------------------------------------*
 * M           : LPC order
 *---------------------------------------------------------------*/
 
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   Levinson_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int Levinson_init (LevinsonState *state)
{
  if (state == (LevinsonState *) NULL){
      fprintf(stderr, "Levinson_init: invalid parameter\n");
      return -1;
  }

  Levinson_reset(state);

  return 0;
}
 
/*************************************************************************
*
*  Function:   Levinson_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int Levinson_reset (LevinsonState *state)
{
  Word16 i;
  
  if (state == (LevinsonState *) NULL){
      fprintf(stderr, "Levinson_reset: invalid parameter\n");
      return -1;
  }
  
  state->old_A[0] = 4096;
  for(i = 1; i < M + 1; i++)
      state->old_A[i] = 0;
 
  return 0;
}
 
/*************************************************************************
 *
 *   FUNCTION:  Levinson()
 *
 *   PURPOSE:  Levinson-Durbin algorithm in double precision. To compute the
 *             LP filter parameters from the speech autocorrelations.
 *
 *   DESCRIPTION:
 *       R[i]    autocorrelations.
 *       A[i]    filter coefficients.
 *       K       reflection coefficients.
 *       Alpha   prediction gain.
 *
 *       Initialisation:
 *               A[0] = 1
 *               K    = -R[1]/R[0]
 *               A[1] = K
 *               Alpha = R[0] * (1-K**2]
 *
 *       Do for  i = 2 to M
 *
 *            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]
 *
 *            K = -S / Alpha
 *
 *            An[j] = A[j] + K*A[i-j]   for j=1 to i-1
 *                                      where   An[i] = new A[i]
 *            An[i]=K
 *
 *            Alpha=Alpha * (1-K**2)
 *
 *       END
 *
 *************************************************************************/
int Levinson (
    LevinsonState *st,
    Word16 Rh[],       /* i : Rh[m+1] Vector of autocorrelations (msb) */
    Word16 Rl[],       /* i : Rl[m+1] Vector of autocorrelations (lsb) */
    Word16 A[],        /* o : A[m]    LPC coefficients  (m = 10)       */
    Word16 rc[]        /* o : rc[4]   First 4 reflection coefficients  */
)
{
    Word16 i, j;
    Word16 hi, lo;
    Word16 Kh, Kl;                /* reflexion coefficient; hi and lo      */
    Word16 alp_h, alp_l, alp_exp; /* Prediction gain; hi lo and exponent   */
    Word16 Ah[M + 1], Al[M + 1];  /* LPC coef. in double prec.             */
    Word16 Anh[M + 1], Anl[M + 1];/* LPC coef.for next iteration in double
                                     prec. */
    Word32 t0, t1, t2;            /* temporary variable                    */

    /* K = A[1] = -R[1] / R[0] */

    t1 = L_Comp (Rh[1], Rl[1]);
    t2 = L_abs (t1);                    /* abs R[1]         */
    t0 = Div_32 (t2, Rh[0], Rl[0]);     /* R[1]/R[0]        */

    if (t1 > 0)
       t0 = L_negate (t0);             /* -R[1]/R[0]       */
    L_Extract (t0, &Kh, &Kl);           /* K in DPF         */
    
    rc[0] = round (t0);

    t0 = L_shr (t0, 4);                 /* A[1] in          */
    L_Extract (t0, &Ah[1], &Al[1]);     /* A[1] in DPF      */

    /*  Alpha = R[0] * (1-K**2) */

    t0 = Mpy_32 (Kh, Kl, Kh, Kl);       /* K*K             */
    t0 = L_abs (t0);                    /* Some case <0 !! */
    t0 = L_sub ((Word32) 0x7fffffffL, t0); /* 1 - K*K        */
    L_Extract (t0, &hi, &lo);           /* DPF format      */
    t0 = Mpy_32 (Rh[0], Rl[0], hi, lo); /* Alpha in        */

    /* Normalize Alpha */

    alp_exp = norm_l (t0);
    t0 = L_shl (t0, alp_exp);
    L_Extract (t0, &alp_h, &alp_l);     /* DPF format    */

    /*--------------------------------------*
     * ITERATIONS  I=2 to M                 *
     *--------------------------------------*/

    for (i = 2; i <= M; i++)
    {
       /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */
       
       t0 = 0;
       for (j = 1; j < i; j++)
       {
          t0 = L_add (t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));
       }
       t0 = L_shl (t0, 4);
       
       t1 = L_Comp (Rh[i], Rl[i]);
       t0 = L_add (t0, t1);            /* add R[i]        */
       
       /* K = -t0 / Alpha */
       
       t1 = L_abs (t0);
       t2 = Div_32 (t1, alp_h, alp_l); /* abs(t0)/Alpha              */

       if (t0 > 0)
          t2 = L_negate (t2);         /* K =-t0/Alpha                */
       t2 = L_shl (t2, alp_exp);       /* denormalize; compare to Alpha */
       L_Extract (t2, &Kh, &Kl);       /* K in DPF                      */
       

       if (sub (i, 5) < 0)
       {
          rc[i - 1] = round (t2);
       }
       /* Test for unstable filter. If unstable keep old A(z) */
       

       if (sub (abs_s (Kh), 32750) > 0)
       {
          for (j = 0; j <= M; j++)
          {
             A[j] = st->old_A[j];
          }
          
          for (j = 0; j < 4; j++)
          {
             rc[j] = 0;
          }
          
          return 0;
       }
       /*------------------------------------------*
        *  Compute new LPC coeff. -> An[i]         *
        *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
        *  An[i]= K                                *
        *------------------------------------------*/
       
       for (j = 1; j < i; j++)
       {
          t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
          t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
          L_Extract (t0, &Anh[j], &Anl[j]);
       }
       t2 = L_shr (t2, 4);
       L_Extract (t2, &Anh[i], &Anl[i]);
       
       /*  Alpha = Alpha * (1-K**2) */
       
       t0 = Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
       t0 = L_abs (t0);                        /* Some case <0 !! */
       t0 = L_sub ((Word32) 0x7fffffffL, t0);  /* 1 - K*K        */
       L_Extract (t0, &hi, &lo);               /* DPF format      */
       t0 = Mpy_32 (alp_h, alp_l, hi, lo);
       
       /* Normalize Alpha */
       
       j = norm_l (t0);
       t0 = L_shl (t0, j);
       L_Extract (t0, &alp_h, &alp_l);         /* DPF format    */
       alp_exp = add (alp_exp, j);             /* Add normalization to
                                                  alp_exp */
       
       /* A[j] = An[j] */
       
       for (j = 1; j <= i; j++)
       {
          Ah[j] = Anh[j];
          Al[j] = Anl[j];
       }
    }
    
    A[0] = 4096;
    for (i = 1; i <= M; i++)
    {
       t0 = L_Comp (Ah[i], Al[i]);
       st->old_A[i] = A[i] = round (L_shl (t0, 1));
    }
    
    return 0;
}
