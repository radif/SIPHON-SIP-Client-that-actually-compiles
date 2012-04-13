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
*      File             : gc_pred.c
*      Purpose          : codebook gain MA prediction
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "gc_pred.h"
const char gc_pred_id[] = "@(#)$Id $" gc_pred_h;

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
#include "log2.h"
#include "copy.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

#define NPRED 4  /* number of prediction taps */

/* MA prediction coefficients (Q13) */
static const Word16 pred[NPRED] = {5571, 4751, 2785, 1556};

/* average innovation energy.                               */
/* MEAN_ENER  = 36.0/constant, constant = 20*Log10(2)       */
#define MEAN_ENER_MR122  783741L  /* 36/(20*log10(2)) (Q17) */

/* MA prediction coefficients (Q6)  */
static const Word16 pred_MR122[NPRED] = {44, 37, 22, 12};

/* minimum quantized energy: -14 dB */
#define MIN_ENERGY       -14336       /* 14                 Q10 */
#define MIN_ENERGY_MR122  -2381       /* 14 / (20*log10(2)) Q10 */ 
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   qua_gain_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int gc_pred_init (gc_predState *state)
{
  if (state == (gc_predState *) NULL){
      fprintf(stderr, "gc_pred_init: invalid parameter\n");
      return -1;
  }

  gc_pred_reset(state);

  return 0;
}

/*************************************************************************
*
*  Function:   gc_pred_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int gc_pred_reset (gc_predState *state)
{
   Word16 i;

   if (state == (gc_predState *) NULL){
      fprintf(stderr, "gc_pred_reset: invalid parameter\n");
      return -1;
   }

   for(i = 0; i < NPRED; i++)
   {
      state->past_qua_en[i] = MIN_ENERGY;
      state->past_qua_en_MR122[i] = MIN_ENERGY_MR122;
   }
  return 0;
}

/*************************************************************************
 *
 * FUNCTION:  gc_pred_copy()
 *
 * PURPOSE: Copy MA predictor state variable
 *
 *************************************************************************/
void
gc_pred_copy(
    gc_predState *st_src,  /* i : State struct                           */
    gc_predState *st_dest  /* o : State struct                           */
)
{
    Copy (st_src->past_qua_en, st_dest->past_qua_en, NPRED);
    Copy (st_src->past_qua_en_MR122, st_dest->past_qua_en_MR122, NPRED);
}

/*************************************************************************
 *
 * FUNCTION:  gc_pred()
 *
 * PURPOSE: MA prediction of the innovation energy
 *          (in dB/(20*log10(2))) with mean  removed).
 *
 *************************************************************************/
void
gc_pred(
    gc_predState *st,   /* i/o: State struct                           */
    enum Mode mode,     /* i  : AMR mode                               */
    Word16 *code,       /* i  : innovative codebook vector (L_SUBFR)   */
                        /*      MR122: Q12, other modes: Q13           */
    Word16 *exp_gcode0, /* o  : exponent of predicted gain factor, Q0  */
    Word16 *frac_gcode0,/* o  : fraction of predicted gain factor  Q15 */
    Word16 *exp_en,     /* o  : exponent of innovation energy,     Q0  */
                        /*      (only calculated for MR795)            */
    Word16 *frac_en     /* o  : fraction of innovation energy,     Q15 */
                        /*      (only calculated for MR795)            */
)
{
    Word16 i;
    Word32 ener_code;
    Word16 exp, frac;

    /*-------------------------------------------------------------------*
     *  energy of code:                                                  *
     *  ~~~~~~~~~~~~~~~                                                  *
     *  ener_code = sum(code[i]^2)                                       *
     *-------------------------------------------------------------------*/
    ener_code = L_mac((Word32) 0, code[0], code[0]);
                                                 /* MR122:  Q12*Q12 -> Q25 */
                                                 /* others: Q13*Q13 -> Q27 */
    for (i = 1; i < L_SUBFR; i++)
        ener_code = L_mac(ener_code, code[i], code[i]);
    

    /*if (sub (mode, MR122) == 0)*/
    if ( mode == MR122 )
    {
        Word32 ener;

        /* ener_code = ener_code / lcode; lcode = 40; 1/40 = 26214 Q20       */
        ener_code = L_mult (round (ener_code), 26214);   /* Q9  * Q20 -> Q30 */

        /*-------------------------------------------------------------------*
         *  energy of code:                                                  *
         *  ~~~~~~~~~~~~~~~                                                  *
         *  ener_code(Q17) = 10 * Log10(energy) / constant                   *
         *                 = 1/2 * Log2(energy)                              *
         *                                           constant = 20*Log10(2)  *
         *-------------------------------------------------------------------*/
        /* ener_code = 1/2 * Log2(ener_code); Note: Log2=log2+30 */
        Log2(ener_code, &exp, &frac);
        ener_code = L_Comp (sub (exp, 30), frac);     /* Q16 for log()    */
                                                    /* ->Q17 for 1/2 log()*/

        /*-------------------------------------------------------------------*
         *  predicted energy:                                                *
         *  ~~~~~~~~~~~~~~~~~                                                *
         *  ener(Q24) = (Emean + sum{pred[i]*past_en[i]})/constant           *
         *            = MEAN_ENER + sum(pred[i]*past_qua_en[i])              *
         *                                           constant = 20*Log10(2)  *
         *-------------------------------------------------------------------*/

        ener = MEAN_ENER_MR122;                   /* Q24 (Q17) */
        for (i = 0; i < NPRED; i++)
        {
            ener = L_mac (ener, st->past_qua_en_MR122[i], pred_MR122[i]);
                                                     /* Q10 * Q13 -> Q24 */
                                                     /* Q10 * Q6  -> Q17 */
        }

        /*-------------------------------------------------------------------*
         *  predicted codebook gain                                          *
         *  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
         *  gc0     = Pow10( (ener*constant - ener_code*constant) / 20 )     *
         *          = Pow2(ener-ener_code)                                   *
         *          = Pow2(int(d)+frac(d))                                   *
         *                                                                   *
         *  (store exp and frac for pow2())                                  *
         *-------------------------------------------------------------------*/

        ener = L_shr (L_sub (ener, ener_code), 1);                /* Q16 */
        L_Extract(ener, exp_gcode0, frac_gcode0);
    }
    else /* all modes except 12.2 */
    {
        Word32 L_tmp;
        Word16 exp_code, gcode0;
        
        /*-----------------------------------------------------------------*
         *  Compute: means_ener - 10log10(ener_code/ L_sufr)               *
         *-----------------------------------------------------------------*/

        exp_code = norm_l (ener_code);
        ener_code = L_shl (ener_code, exp_code);
        
        /* Log2 = log2 + 27 */
        Log2_norm (ener_code, exp_code, &exp, &frac);
        
        /* fact = 10/log2(10) = 3.01 = 24660 Q13 */
        L_tmp = Mpy_32_16(exp, frac, -24660); /* Q0.Q15 * Q13 -> Q14 */

        /*   L_tmp = means_ener - 10log10(ener_code/L_SUBFR)
         *         = means_ener - 10log10(ener_code) + 10log10(L_SUBFR)
         *         = K - fact * Log2(ener_code)
         *         = K - fact * log2(ener_code) - fact*27
         *
         *   ==> K = means_ener + fact*27 + 10log10(L_SUBFR)
         *
         *   means_ener =       33    =  540672    Q14  (MR475, MR515, MR59)
         *   means_ener =       28.75 =  471040    Q14  (MR67)
         *   means_ener =       30    =  491520    Q14  (MR74)
         *   means_ener =       36    =  589824    Q14  (MR795)
         *   means_ener =       33    =  540672    Q14  (MR102)         
         *   10log10(L_SUBFR) = 16.02 =  262481.51 Q14
         *   fact * 27                = 1331640    Q14
         *   -----------------------------------------
         *   (MR475, MR515, MR59)   K = 2134793.51 Q14 ~= 16678 * 64 * 2
         *   (MR67)                 K = 2065161.51 Q14 ~= 32268 * 32 * 2
         *   (MR74)                 K = 2085641.51 Q14 ~= 32588 * 32 * 2
         *   (MR795)                K = 2183945.51 Q14 ~= 17062 * 64 * 2
         *   (MR102)                K = 2134793.51 Q14 ~= 16678 * 64 * 2         
         */

        /*if (test (), sub (mode, MR102) == 0)*/
        if ( mode ==  MR102 )
        {
            /* mean = 33 dB */
            L_tmp = L_mac(L_tmp, 16678, 64);     /* Q14 */
        }
        /*else if (test (), sub (mode, MR795) == 0)*/
        else if ( mode == MR795 )
        {
            /* ener_code  = <xn xn> * 2^27*2^exp_code
               frac_en    = ener_code / 2^16
                          = <xn xn> * 2^11*2^exp_code
               <xn xn>    = <xn xn>*2^11*2^exp * 2^exp_en
                         := frac_en            * 2^exp_en

               ==> exp_en = -11-exp_code;
             */
            *frac_en = extract_h (ener_code);
            *exp_en = sub (-11, exp_code);

            /* mean = 36 dB */
            L_tmp = L_mac(L_tmp, 17062, 64);     /* Q14 */
        }
        /*else if (test (), sub (mode, MR74) == 0)*/
        else if ( mode == MR74 )
        {
            /* mean = 30 dB */
            L_tmp = L_mac(L_tmp, 32588, 32);     /* Q14 */
        }
        /*else if (test (), sub (mode, MR67) == 0)*/
        else if ( mode == MR67 )
        {
            /* mean = 28.75 dB */
            L_tmp = L_mac(L_tmp, 32268, 32);     /* Q14 */
        }
        else /* MR59, MR515, MR475 */
        {
            /* mean = 33 dB */
            L_tmp = L_mac(L_tmp, 16678, 64);     /* Q14 */
        }
        
        /*-----------------------------------------------------------------*
         * Compute gcode0.                                                 *
         *  = Sum(i=0,3) pred[i]*past_qua_en[i] - ener_code + mean_ener    *
         *-----------------------------------------------------------------*/

        L_tmp = L_shl(L_tmp, 10);                /* Q24 */
        for (i = 0; i < 4; i++)
            L_tmp = L_mac(L_tmp, pred[i], st->past_qua_en[i]);
                                                 /* Q13 * Q10 -> Q24 */

        gcode0 = extract_h(L_tmp);               /* Q8  */

        /*-----------------------------------------------------------------*
         * gcode0 = pow(10.0, gcode0/20)                                   *
         *        = pow(2, 3.3219*gcode0/20)                               *
         *        = pow(2, 0.166*gcode0)                                   *
         *-----------------------------------------------------------------*/

        /* 5439 Q15 = 0.165985                                        */
        /* (correct: 1/(20*log10(2)) 0.166096 = 5443 Q15)             */

        if (sub (mode, MR74) == 0) /* For IS641 bitexactness */
            L_tmp = L_mult(gcode0, 5439);  /* Q8 * Q15 -> Q24 */
        else
            L_tmp = L_mult(gcode0, 5443);  /* Q8 * Q15 -> Q24 */

        L_tmp = L_shr(L_tmp, 8);                   /*          -> Q16 */
        L_Extract(L_tmp, exp_gcode0, frac_gcode0); /*       -> Q0.Q15 */
    }
}


/*************************************************************************
 *
 * FUNCTION:  gc_pred_update()
 *
 * PURPOSE: update MA predictor with last quantized energy
 *
 *************************************************************************/
void gc_pred_update(
    gc_predState *st,      /* i/o: State struct                     */
    Word16 qua_ener_MR122, /* i  : quantized energy for update, Q10 */
                           /*      (log2(qua_err))                  */
    Word16 qua_ener        /* i  : quantized energy for update, Q10 */
                           /*      (20*log10(qua_err))              */
)
{
    Word16 i;

    for (i = 3; i > 0; i--)
    {
        st->past_qua_en[i] = st->past_qua_en[i - 1];
        st->past_qua_en_MR122[i] = st->past_qua_en_MR122[i - 1];
    }

    st->past_qua_en_MR122[0] = qua_ener_MR122;  /*    log2 (qua_err), Q10 */

    st->past_qua_en[0] = qua_ener;              /* 20*log10(qua_err), Q10 */

}

/*************************************************************************
 *
 * FUNCTION:  gc_pred_average_limited()
 *
 * PURPOSE: get average of MA predictor state values (with a lower limit)
 *          [used in error concealment]
 *
 *************************************************************************/
void gc_pred_average_limited(
    gc_predState *st,       /* i: State struct                    */
    Word16 *ener_avg_MR122, /* o: everaged quantized energy,  Q10 */
                            /*    (log2(qua_err))                 */
    Word16 *ener_avg        /* o: averaged quantized energy,  Q10 */
                            /*    (20*log10(qua_err))             */
)
{
    Word16 av_pred_en;
    Word16 i;

    /* do average in MR122 mode (log2() domain) */
    av_pred_en = 0;
    for (i = 0; i < NPRED; i++)
    {
        av_pred_en = add (av_pred_en, st->past_qua_en_MR122[i]);
    }

    /* av_pred_en = 0.25*av_pred_en */
    av_pred_en = mult (av_pred_en, 8192);

    /* if (av_pred_en < -14/(20Log10(2))) av_pred_en = .. */

    if (sub (av_pred_en, MIN_ENERGY_MR122) < 0)
    {
        av_pred_en = MIN_ENERGY_MR122;
    }
    *ener_avg_MR122 = av_pred_en;

    /* do average for other modes (20*log10() domain) */
    av_pred_en = 0;
    for (i = 0; i < NPRED; i++)
    {
        av_pred_en = add (av_pred_en, st->past_qua_en[i]);
    }

    /* av_pred_en = 0.25*av_pred_en */
    av_pred_en = mult (av_pred_en, 8192);

    /* if (av_pred_en < -14) av_pred_en = .. */

    if (sub (av_pred_en, MIN_ENERGY) < 0)
    {
        av_pred_en = MIN_ENERGY;
    }
    *ener_avg = av_pred_en;
}
