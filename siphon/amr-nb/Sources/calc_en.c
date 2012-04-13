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
*      File             : calc_en.c
*      Purpose          : (pre-) quantization of pitch gain for MR795
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "calc_en.h"
const char calc_en_id[] = "@(#)$Id $" calc_en_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"
#include "cnst.h"
#include "log2.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
 *
 * FUNCTION: calc_unfilt_energies
 *
 * PURPOSE:  calculation of several energy coefficients for unfiltered
 *           excitation signals and the LTP coding gain
 *
 *       frac_en[0]*2^exp_en[0] = <res res>   // LP residual energy
 *       frac_en[1]*2^exp_en[1] = <exc exc>   // LTP residual energy
 *       frac_en[2]*2^exp_en[2] = <exc code>  // LTP/CB innovation dot product
 *       frac_en[3]*2^exp_en[3] = <lres lres> // LTP residual energy
 *                                            // (lres = res - gain_pit*exc)
 *       ltpg = log2(LP_res_en / LTP_res_en)
 *
 *************************************************************************/
void
calc_unfilt_energies(
    Word16 res[],     /* i  : LP residual,                               Q0  */
    Word16 exc[],     /* i  : LTP excitation (unfiltered),               Q0  */
    Word16 code[],    /* i  : CB innovation (unfiltered),                Q13 */
    Word16 gain_pit,  /* i  : pitch gain,                                Q14 */
    Word16 L_subfr,   /* i  : Subframe length                                */

    Word16 frac_en[], /* o  : energy coefficients (4), fraction part,    Q15 */
    Word16 exp_en[],  /* o  : energy coefficients (4), exponent part,    Q0  */
    Word16 *ltpg      /* o  : LTP coding gain (log2()),                  Q13 */
)
{
    Word32 s, L_temp;
    Word16 i, exp, tmp;
    Word16 ltp_res_en, pred_gain;
    Word16 ltpg_exp, ltpg_frac;

    /* Compute residual energy */
    s = L_mac((Word32) 0, res[0], res[0]);
    for (i = 1; i < L_subfr; i++)
        s = L_mac(s, res[i], res[i]);

    /* ResEn := 0 if ResEn < 200.0 (= 400 Q1) */
    /**/
    if (L_sub (s, 400L) < 0)
    {
        frac_en[0] = 0;
        exp_en[0] = -15;
    }
    else
    {
        exp = norm_l(s);
        frac_en[0] = extract_h(L_shl(s, exp));
        exp_en[0] = sub(15, exp);
    }
    
    /* Compute ltp excitation energy */
    s = L_mac((Word32) 0, exc[0], exc[0]);
    for (i = 1; i < L_subfr; i++)
        s = L_mac(s, exc[i], exc[i]);

    exp = norm_l(s);
    frac_en[1] = extract_h(L_shl(s, exp));
    exp_en[1] = sub(15, exp);

    /* Compute scalar product <exc[],code[]> */
    s = L_mac((Word32) 0, exc[0], code[0]);
    for (i = 1; i < L_subfr; i++)
        s = L_mac(s, exc[i], code[i]);

    exp = norm_l(s);
    frac_en[2] = extract_h(L_shl(s, exp));
    exp_en[2] = sub(16-14, exp);

    /* Compute energy of LTP residual */
    s = 0L;
    for (i = 0; i < L_subfr; i++)
    {
        L_temp = L_mult(exc[i], gain_pit);
        L_temp = L_shl(L_temp, 1);
        tmp = sub(res[i], round(L_temp));           /* LTP residual, Q0 */
        s = L_mac (s, tmp, tmp);
    }

    exp = norm_l(s);
    ltp_res_en = extract_h (L_shl (s, exp));
    exp = sub (15, exp);

    frac_en[3] = ltp_res_en;
    exp_en[3] = exp;
    
    /* calculate LTP coding gain, i.e. energy reduction LP res -> LTP res */

    if (ltp_res_en > 0 && frac_en[0] != 0)
    {
        /* gain = ResEn / LTPResEn */
        pred_gain = div_s (shr (frac_en[0], 1), ltp_res_en);
        exp = sub (exp, exp_en[0]);

        /* L_temp = ltpGain * 2^(30 + exp) */
        L_temp = L_deposit_h (pred_gain);
        /* L_temp = ltpGain * 2^27 */
        L_temp = L_shr (L_temp, add (exp, 3));

        /* Log2 = log2() + 27 */
        Log2(L_temp, &ltpg_exp, &ltpg_frac);

        /* ltpg = log2(LtpGain) * 2^13 --> range: +- 4 = +- 12 dB */
        L_temp = L_Comp (sub (ltpg_exp, 27), ltpg_frac);
        *ltpg = round (L_shl (L_temp, 13)); /* Q13 */
    }
    else
    {
        *ltpg = 0;
    }
}

/*************************************************************************
 *
 * FUNCTION: calc_filt_energies
 *
 * PURPOSE:  calculation of several energy coefficients for filtered
 *           excitation signals
 *
 *     Compute coefficients need for the quantization and the optimum
 *     codebook gain gcu (for MR475 only).
 *
 *      coeff[0] =    y1 y1
 *      coeff[1] = -2 xn y1
 *      coeff[2] =    y2 y2
 *      coeff[3] = -2 xn y2
 *      coeff[4] =  2 y1 y2
 *
 *
 *      gcu = <xn2, y2> / <y2, y2> (0 if <xn2, y2> <= 0)
 *
 *     Product <y1 y1> and <xn y1> have been computed in G_pitch() and
 *     are in vector g_coeff[].
 *
 *************************************************************************/
void
calc_filt_energies(
    enum Mode mode,     /* i  : coder mode                                   */
    Word16 xn[],        /* i  : LTP target vector,                       Q0  */
    Word16 xn2[],       /* i  : CB target vector,                        Q0  */
    Word16 y1[],        /* i  : Adaptive codebook,                       Q0  */
    Word16 Y2[],        /* i  : Filtered innovative vector,              Q12 */
    Word16 g_coeff[],   /* i  : Correlations <xn y1> <y1 y1>                 */
                        /*      computed in G_pitch()                        */

    Word16 frac_coeff[],/* o  : energy coefficients (5), fraction part,  Q15 */
    Word16 exp_coeff[], /* o  : energy coefficients (5), exponent part,  Q0  */
    Word16 *cod_gain_frac,/* o: optimum codebook gain (fraction part),   Q15 */
    Word16 *cod_gain_exp  /* o: optimum codebook gain (exponent part),   Q0  */
)
{
    Word32 s, ener_init, s1, s2, s3;
    Word16 i, exp, frac;
    Word16 y2[L_SUBFR];

    if (( mode == MR795 ) || ( mode == MR475))
    {
        ener_init = 0L; /**/
    }
    else
    {
        ener_init = 1L; /**/
    }
    
    /* Compute scalar product <y2[],y2[]> */
    /* Compute scalar product -2*<xn[],y2[]> */
    /* Compute scalar product 2*<y1[],y2[]> */
    s1 = s2 = s3 = 0;
    for (i = 0; i < L_SUBFR; i++) {
      y2[i] = Y2[i] >> 3;
      s1 += y2[i] * y2[i];
      s2 += y2[i] * xn[i];
      s3 += y2[i] * y1[i];
    }

    frac_coeff[0] = g_coeff[0];
    exp_coeff[0] = g_coeff[1];
    frac_coeff[1] = -g_coeff[2];     /* coeff[1] = -2 xn y1 */
    exp_coeff[1] = g_coeff[3] + 1;

    s1 <<= 1;
    s1 += ener_init;
    exp = norm_l(s1);
    frac_coeff[2] = extract_h(L_shl(s1, exp));
    exp_coeff[2] = sub(15 - 18, exp);

    s2 <<= 1;
    s2 += ener_init;
    exp = norm_l(s2);
    frac_coeff[3] = negate(extract_h(L_shl(s2, exp)));
    exp_coeff[3] = sub(15 - 9 + 1, exp);

    s3 <<= 1;
    s3 += ener_init;
    exp = norm_l(s3);
    frac_coeff[4] = extract_h(L_shl(s3, exp));
    exp_coeff[4] = sub(15 - 9 + 1, exp);

    if ((mode == MR475) || (mode == MR795))
    {
        /* Compute scalar product <xn2[],y2[]> */
        s = 0;
        for (i = 0; i < L_SUBFR; i++)
          s += xn2[i] * y2[i];
        s <<= 1;
        s += ener_init;
        
        exp = norm_l(s);
        frac = extract_h(L_shl(s, exp));
        exp = sub(15 - 9, exp);

        if (frac <= 0)
        {
            *cod_gain_frac = 0;
            *cod_gain_exp = 0;
        }
        else
        {
            /*
              gcu = <xn2, y2> / c[2]
                  = (frac>>1)/frac[2]             * 2^(exp+1-exp[2])
                  = div_s(frac>>1, frac[2])*2^-15 * 2^(exp+1-exp[2])
                  = div_s * 2^(exp-exp[2]-14)
             */  
            *cod_gain_frac = div_s (shr (frac,1), frac_coeff[2]);
            *cod_gain_exp = sub (sub (exp, exp_coeff[2]), 14);

        }
    }
}

/*************************************************************************
 *
 * FUNCTION: calc_target_energy
 *
 * PURPOSE:  calculation of target energy
 *
 *      en = <xn, xn>
 *
 *************************************************************************/
void
calc_target_energy(
    Word16 xn[],     /* i: LTP target vector,                       Q0  */
    Word16 *en_exp,  /* o: optimum codebook gain (exponent part),   Q0  */
    Word16 *en_frac  /* o: optimum codebook gain (fraction part),   Q15 */
)
{
    Word32 s;
    Word16 i, exp;

    /* Compute scalar product <xn[], xn[]> */
    s = 0;
    for (i = 0; i < L_SUBFR; i++)
      s += xn[i] * xn[i];
    s <<= 1;

    /* s = SUM 2*xn(i) * xn(i) = <xn xn> * 2 */
    exp = norm_l(s);
    *en_frac = extract_h(L_shl(s, exp));
    *en_exp = sub(16, exp);
}
