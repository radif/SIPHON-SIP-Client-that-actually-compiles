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
*      File             : qg475.c
*      Purpose          : Quantization of pitch and codebook gains for MR475.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "qgain475.h"
const char qgain475_id[] = "@(#)$Id $" qgain475_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "mode.h"
#include "cnst.h"
#include "pow2.h"
#include "log2.h"
#include "oper_32b.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/* minimum allowed gain code prediction error: 102.887/4096 = 0.0251189 */
#define MIN_QUA_ENER         ( -5443) /* Q10 <->    log2 (0.0251189) */
#define MIN_QUA_ENER_MR122   (-32768) /* Q10 <-> 20*log10(0.0251189) */

/* minimum allowed gain code prediction error: 32000/4096 = 7.8125 */
#define MAX_QUA_ENER         (  3037) /* Q10 <->    log2 (7.8125)    */
#define MAX_QUA_ENER_MR122   ( 18284) /* Q10 <-> 20*log10(7.8125)    */


#include "qgain475.tab"

/*
********************************************************************************
*                         PRIVATE PROGRAM CODE
********************************************************************************
*/
static void MR475_quant_store_results(

    gc_predState *pred_st, /* i/o: gain predictor state struct               */
    const Word16 *p,       /* i  : pointer to selected quantizer table entry */
    Word16 gcode0,         /* i  : predicted CB gain,     Q(14 - exp_gcode0) */
    Word16 exp_gcode0,     /* i  : exponent of predicted CB gain,        Q0  */
    Word16 *gain_pit,      /* o  : Pitch gain,                           Q14 */
    Word16 *gain_cod       /* o  : Code gain,                            Q1  */
)
{

    Word16 g_code, exp, frac, tmp;
    Word32 L_tmp;

    Word16 qua_ener_MR122; /* o  : quantized energy error, MR122 version Q10 */
    Word16 qua_ener;       /* o  : quantized energy error,               Q10 */

    /* Read the quantized gains */
    *gain_pit = *p++;
    g_code = *p++;

    /*------------------------------------------------------------------*
     *  calculate final fixed codebook gain:                            *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                            *
     *                                                                  *
     *   gc = gc0 * g                                                   *
     *------------------------------------------------------------------*/

    L_tmp = L_mult(g_code, gcode0);
    L_tmp = L_shr(L_tmp, sub(10, exp_gcode0));
    *gain_cod = extract_h(L_tmp);

    /*------------------------------------------------------------------*
     *  calculate predictor update values and update gain predictor:    *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    *
     *                                                                  *
     *   qua_ener       = log2(g)                                       *
     *   qua_ener_MR122 = 20*log10(g)                                   *
     *------------------------------------------------------------------*/

    Log2 (L_deposit_l (g_code), &exp, &frac); /* Log2(x Q12) = log2(x) + 12 */
    exp = sub(exp, 12);

    tmp = shr_r (frac, 5);
    qua_ener_MR122 = add (tmp, shl (exp, 10));

    L_tmp = Mpy_32_16(exp, frac, 24660); /* 24660 Q12 ~= 6.0206 = 20*log10(2) */
    qua_ener = round (L_shl (L_tmp, 13)); /* Q12 * Q0 = Q13 -> Q10 */

    gc_pred_update(pred_st, qua_ener_MR122, qua_ener);
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
 *
 * FUNCTION:  MR475_update_unq_pred()
 *
 * PURPOSE:   use optimum codebook gain and update "unquantized"
 *            gain predictor with the (bounded) prediction error
 *
 *************************************************************************/
void
MR475_update_unq_pred(
    gc_predState *pred_st, /* i/o: gain predictor state struct            */
    Word16 exp_gcode0,     /* i  : predicted CB gain (exponent MSW),  Q0  */
    Word16 frac_gcode0,    /* i  : predicted CB gain (exponent LSW),  Q15 */
    Word16 cod_gain_exp,   /* i  : optimum codebook gain (exponent),  Q0  */
    Word16 cod_gain_frac   /* i  : optimum codebook gain (fraction),  Q15 */
)
{
    Word16 tmp, exp, frac;
    Word16 qua_ener, qua_ener_MR122;
    Word32 L_tmp;

    /* calculate prediction error factor (given optimum CB gain gcu):
     *
     *   predErrFact = gcu / gcode0
     *   (limit to MIN_PRED_ERR_FACT <= predErrFact <= MAX_PRED_ERR_FACT
     *    -> limit qua_ener*)
     *
     * calculate prediction error (log):
     *
     *   qua_ener_MR122 = log2(predErrFact)
     *   qua_ener       = 20*log10(predErrFact)
     *
     */

    /*if (test(), cod_gain_frac <= 0)*/
    if (cod_gain_frac <= 0)
    {
        /* if gcu <= 0 -> predErrFact = 0 < MIN_PRED_ERR_FACT */
        /* -> set qua_ener(_MR122) directly                   */
        qua_ener = MIN_QUA_ENER;
        qua_ener_MR122 = MIN_QUA_ENER_MR122;
    }
    else
    {
        /* convert gcode0 from DPF to standard fraction/exponent format */
        /* with normalized frac, i.e. 16384 <= frac <= 32767            */
        /* Note: exponent correction (exp=exp-14) is done after div_s   */
        frac_gcode0 = extract_l (Pow2 (14, frac_gcode0));

        /* make sure cod_gain_frac < frac_gcode0  for div_s */
        /*if (test (), sub(cod_gain_frac, frac_gcode0) >= 0)*/
        if ( cod_gain_frac >= frac_gcode0 )
        {
            cod_gain_frac = shr (cod_gain_frac, 1);
            cod_gain_exp = add (cod_gain_exp, 1);
        }

        /*
          predErrFact
             = gcu / gcode0
             = cod_gain_frac/frac_gcode0 * 2^(cod_gain_exp-(exp_gcode0-14))
             = div_s (c_g_f, frac_gcode0)*2^-15 * 2^(c_g_e-exp_gcode0+14)
             = div_s * 2^(cod_gain_exp-exp_gcode0 - 1)
        */
        frac = div_s (cod_gain_frac, frac_gcode0);
        tmp = sub (sub (cod_gain_exp, exp_gcode0), 1);

        Log2 (L_deposit_l (frac), &exp, &frac);
        exp = add (exp, tmp);

        /* calculate prediction error (log2, Q10) */
        qua_ener_MR122 = shr_r (frac, 5);
        qua_ener_MR122 = add (qua_ener_MR122, shl (exp, 10));

        /*if (test (), sub(qua_ener_MR122, MIN_QUA_ENER_MR122) < 0)*/
        if ( qua_ener_MR122 < MIN_QUA_ENER_MR122 )
        {
            qua_ener = MIN_QUA_ENER;
            qua_ener_MR122 = MIN_QUA_ENER_MR122;
        }
        /*else if (test (), sub(qua_ener_MR122, MAX_QUA_ENER_MR122) > 0)*/
        else if ( qua_ener_MR122 > MAX_QUA_ENER_MR122 )
        {
            qua_ener = MAX_QUA_ENER;
            qua_ener_MR122 = MAX_QUA_ENER_MR122;
        }
        else
        {
            /* calculate prediction error (20*log10, Q10) */
            L_tmp = Mpy_32_16(exp, frac, 24660);
            /* 24660 Q12 ~= 6.0206 = 20*log10(2) */
            qua_ener = round (L_shl (L_tmp, 13));
            /* Q12 * Q0 = Q13 -> Q26 -> Q10     */
        }
    }

    /* update MA predictor memory */
    gc_pred_update(pred_st, qua_ener_MR122, qua_ener);
}


/*************************************************************************
 *
 * FUNCTION:  MR475_gain_quant()
 *
 * PURPOSE: Quantization of pitch and codebook gains for two subframes
 *          (using predicted codebook gain)
 *
 *************************************************************************/
Word16
MR475_gain_quant(              /* o  : index of quantization.                 */
    gc_predState *pred_st,     /* i/o: gain predictor state struct            */

                               /* data from subframe 0 (or 2) */
    Word16 sf0_exp_gcode0,     /* i  : predicted CB gain (exponent),      Q0  */
    Word16 sf0_frac_gcode0,    /* i  : predicted CB gain (fraction),      Q15 */
    Word16 sf0_exp_coeff[],    /* i  : energy coeff. (5), exponent part,  Q0  */
    Word16 sf0_frac_coeff[],   /* i  : energy coeff. (5), fraction part,  Q15 */
                               /*      (frac_coeff and exp_coeff computed in  */
                               /*       calc_filt_energies())                 */
    Word16 sf0_exp_target_en,  /* i  : exponent of target energy,         Q0  */
    Word16 sf0_frac_target_en, /* i  : fraction of target energy,         Q15 */

                               /* data from subframe 1 (or 3) */
    Word16 sf1_code_nosharp[], /* i  : innovative codebook vector (L_SUBFR)   */
                               /*      (whithout pitch sharpening)            */
    Word16 sf1_exp_gcode0,     /* i  : predicted CB gain (exponent),      Q0  */
    Word16 sf1_frac_gcode0,    /* i  : predicted CB gain (fraction),      Q15 */
    Word16 sf1_exp_coeff[],    /* i  : energy coeff. (5), exponent part,  Q0  */
    Word16 sf1_frac_coeff[],   /* i  : energy coeff. (5), fraction part,  Q15 */
                               /*      (frac_coeff and exp_coeff computed in  */
                               /*       calc_filt_energies())                 */
    Word16 sf1_exp_target_en,  /* i  : exponent of target energy,         Q0  */
    Word16 sf1_frac_target_en, /* i  : fraction of target energy,         Q15 */

    Word16 gp_limit,           /* i  : pitch gain limit                       */

    Word16 *sf0_gain_pit,      /* o  : Pitch gain,                        Q14 */
    Word16 *sf0_gain_cod,      /* o  : Code gain,                         Q1  */

    Word16 *sf1_gain_pit,      /* o  : Pitch gain,                        Q14 */
    Word16 *sf1_gain_cod       /* o  : Code gain,                         Q1  */
)
{
    const Word16 *p;
    Word16 i, index = 0;
    Word16 tmp;
    Word16 exp;
    Word16 sf0_gcode0, sf1_gcode0;
    Word16 g_pitch, g2_pitch, g_code, g2_code, g_pit_cod;
    Word16 coeff[10], coeff_lo[10], exp_max[10];  /* 0..4: sf0; 5..9: sf1 */
    Word32 L_tmp, dist_min;

    /*-------------------------------------------------------------------*
     *  predicted codebook gain                                          *
     *  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
     *  gc0     = 2^exp_gcode0 + 2^frac_gcode0                           *
     *                                                                   *
     *  gcode0 (Q14) = 2^14*2^frac_gcode0 = gc0 * 2^(14-exp_gcode0)      *
     *-------------------------------------------------------------------*/

    sf0_gcode0 = extract_l(Pow2(14, sf0_frac_gcode0));
    sf1_gcode0 = extract_l(Pow2(14, sf1_frac_gcode0));

    /*
     * For each subframe, the error energy (sum) to be minimized consists
     * of five terms, t[0..4].
     *
     *                      t[0] =    gp^2  * <y1 y1>
     *                      t[1] = -2*gp    * <xn y1>
     *                      t[2] =    gc^2  * <y2 y2>
     *                      t[3] = -2*gc    * <xn y2>
     *                      t[4] =  2*gp*gc * <y1 y2>
     *
     */

    /* sf 0 */
    /* determine the scaling exponent for g_code: ec = ec0 - 11 */
    exp = sub(sf0_exp_gcode0, 11);

    /* calculate exp_max[i] = s[i]-1 */
    exp_max[0] = sub(sf0_exp_coeff[0], 13);
    exp_max[1] = sub(sf0_exp_coeff[1], 14);
    exp_max[2] = add(sf0_exp_coeff[2], add(15, shl(exp, 1)));
    exp_max[3] = add(sf0_exp_coeff[3], exp);
    exp_max[4] = add(sf0_exp_coeff[4], add(1, exp));

    /* sf 1 */
    /* determine the scaling exponent for g_code: ec = ec0 - 11 */
    exp = sub(sf1_exp_gcode0, 11);

    /* calculate exp_max[i] = s[i]-1 */
    exp_max[5] = sub(sf1_exp_coeff[0], 13);
    exp_max[6] = sub(sf1_exp_coeff[1], 14);
    exp_max[7] = add(sf1_exp_coeff[2], add(15, shl(exp, 1)));
    exp_max[8] = add(sf1_exp_coeff[3], exp);
    exp_max[9] = add(sf1_exp_coeff[4], add(1, exp));



    /*-------------------------------------------------------------------*
     *  Gain search equalisation:                                        *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~                                        *
     *  The MSE for the two subframes is weighted differently if there   *
     *  is a big difference in the corresponding target energies         *
     *-------------------------------------------------------------------*/

    /* make the target energy exponents the same by de-normalizing the
       fraction of the smaller one. This is necessary to be able to compare
       them
     */
    exp = sf0_exp_target_en - sf1_exp_target_en;

    if (exp > 0)
    {
        sf1_frac_target_en = shr (sf1_frac_target_en, exp);
    }
    else
    {
        sf0_frac_target_en = shl (sf0_frac_target_en, exp);
    }

    /* assume no change of exponents */
    exp = 0;

    /* test for target energy difference; set exp to +1 or -1 to scale
     * up/down coefficients for sf 1
     */
    tmp = shr_r (sf1_frac_target_en, 1);   /* tmp = ceil(0.5*en(sf1)) */

    if (sub (tmp, sf0_frac_target_en) > 0) /* tmp > en(sf0)? */
    {
        /*
         * target_energy(sf1) > 2*target_energy(sf0)
         *   -> scale up MSE(sf0) by 2 by adding 1 to exponents 0..4
         */
        exp = 1;
    }
    else
    {
        tmp = shr (add (sf0_frac_target_en, 3), 2); /* tmp=ceil(0.25*en(sf0)) */

        if (sub (tmp, sf1_frac_target_en) > 0)      /* tmp > en(sf1)? */
        {
            /*
             * target_energy(sf1) < 0.25*target_energy(sf0)
             *   -> scale down MSE(sf0) by 0.5 by subtracting 1 from
             *      coefficients 0..4
             */
            exp = -1;
        }
    }
    
    for (i = 0; i < 5; i++)
    {
        exp_max[i] = add (exp_max[i], exp);
    }
                                                                       
    /*-------------------------------------------------------------------*
     *  Find maximum exponent:                                           *
     *  ~~~~~~~~~~~~~~~~~~~~~~                                           *
     *                                                                   *
     *  For the sum operation, all terms must have the same scaling;     *
     *  that scaling should be low enough to prevent overflow. There-    *
     *  fore, the maximum scale is determined and all coefficients are   *
     *  re-scaled:                                                       *
     *                                                                   *
     *    exp = max(exp_max[i]) + 1;                                     *
     *    e = exp_max[i]-exp;         e <= 0!                            *
     *    c[i] = c[i]*2^e                                                *
     *-------------------------------------------------------------------*/

    exp = exp_max[0];
    for (i = 1; i < 10; i++)
    {

        if (sub(exp_max[i], exp) > 0)
        {
            exp = exp_max[i];
        }
    }
    exp = add(exp, 1);      /* To avoid overflow */

    p = &sf0_frac_coeff[0];
    for (i = 0; i < 5; i++) {
        tmp = sub(exp, exp_max[i]);
        L_tmp = L_deposit_h(*p++);
        L_tmp = L_shr(L_tmp, tmp);
        L_Extract(L_tmp, &coeff[i], &coeff_lo[i]);
    }
    p = &sf1_frac_coeff[0];
    for (; i < 10; i++) {
        tmp = sub(exp, exp_max[i]);
        L_tmp = L_deposit_h(*p++);
        L_tmp = L_shr(L_tmp, tmp);
        L_Extract(L_tmp, &coeff[i], &coeff_lo[i]);
    }


    /*-------------------------------------------------------------------*
     *  Codebook search:                                                 *
     *  ~~~~~~~~~~~~~~~~                                                 *
     *                                                                   *
     *  For each pair (g_pitch, g_fac) in the table calculate the        *
     *  terms t[0..4] and sum them up; the result is the mean squared    *
     *  error for the quantized gains from the table. The index for the  *
     *  minimum MSE is stored and finally used to retrieve the quantized *
     *  gains                                                            *
     *-------------------------------------------------------------------*/

    /* start with "infinite" MSE */
    dist_min = MAX_32;

    p = &table_gain_MR475[0];

    for (i = 0; i < MR475_VQ_SIZE; i++)
    {
        /* subframe 0 (and 2) calculations */
        g_pitch = *p++;
        g_code = *p++;

        g_code = mult(g_code, sf0_gcode0);
        g2_pitch = mult(g_pitch, g_pitch);
        g2_code = mult(g_code, g_code);
        g_pit_cod = mult(g_code, g_pitch);
        
        L_tmp = Mpy_32_16(       coeff[0], coeff_lo[0], g2_pitch);
        L_tmp += Mpy_32_16(coeff[1], coeff_lo[1], g_pitch);
        L_tmp += Mpy_32_16(coeff[2], coeff_lo[2], g2_code);
        L_tmp += Mpy_32_16(coeff[3], coeff_lo[3], g_code);
        L_tmp += Mpy_32_16(coeff[4], coeff_lo[4], g_pit_cod);


        tmp = sub (g_pitch, gp_limit);

        /* subframe 1 (and 3) calculations */
        g_pitch = *p++;
        g_code = *p++;

        /*  */
        if (tmp <= 0 && sub(g_pitch, gp_limit) <= 0)
        {
            g_code = mult(g_code, sf1_gcode0);
            g2_pitch = mult(g_pitch, g_pitch);
            g2_code = mult(g_code, g_code);
            g_pit_cod = mult(g_code, g_pitch);

            L_tmp += Mpy_32_16( coeff[5], coeff_lo[5], g2_pitch);
            L_tmp += Mpy_32_16( coeff[6], coeff_lo[6], g_pitch);
            L_tmp += Mpy_32_16( coeff[7], coeff_lo[7], g2_code);
            L_tmp += Mpy_32_16( coeff[8], coeff_lo[8], g_code);
            L_tmp += Mpy_32_16( coeff[9], coeff_lo[9], g_pit_cod);
            
            /* store table index if MSE for this index is lower
               than the minimum MSE seen so far */

            if (L_sub(L_tmp, dist_min) < (Word32) 0)
            {
                dist_min = L_tmp;
                index = i;
            }
        }
    }

    /*------------------------------------------------------------------*
     *  read quantized gains and update MA predictor memories           *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~           *
     *------------------------------------------------------------------*/

    /* for subframe 0, the pre-calculated gcode0/exp_gcode0 are the same
       as those calculated from the "real" predictor using quantized gains */
    tmp = shl(index, 2);
    MR475_quant_store_results(pred_st,
                              &table_gain_MR475[tmp],
                              sf0_gcode0,
                              sf0_exp_gcode0,
                              sf0_gain_pit,
                              sf0_gain_cod);

    /* calculate new predicted gain for subframe 1 (this time using
       the real, quantized gains)                                   */
    gc_pred(pred_st, MR475, sf1_code_nosharp,
            &sf1_exp_gcode0, &sf1_frac_gcode0,
            &sf0_exp_gcode0, &sf0_gcode0); /* last two args are dummy */
    sf1_gcode0 = extract_l(Pow2(14, sf1_frac_gcode0));


    tmp = add (tmp, 2);
    MR475_quant_store_results(pred_st,
                              &table_gain_MR475[tmp],
                              sf1_gcode0,
                              sf1_exp_gcode0,
                              sf1_gain_pit,
                              sf1_gain_cod);

    return index;
}
