/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : qua_gain.c
*      Purpose          : Quantization of pitch and codebook gains.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "qua_gain.h"
const char qua_gain_id[] = "@(#)$Id $" qua_gain_h;

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
#include "mode.h"
#include "count.h"
#include "cnst.h"
#include "pow2.h"
#include "gc_pred.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "qua_gain.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
 *
 * FUNCTION:  Qua_gain()
 *
 * PURPOSE: Quantization of pitch and codebook gains.
 *          (using predicted codebook gain)
 *
 *************************************************************************/
Word16
Qua_gain(                   /* o  : index of quantization.                 */   
    enum Mode mode,         /* i  : AMR mode                               */
    Word16 exp_gcode0,      /* i  : predicted CB gain (exponent),      Q0  */
    Word16 frac_gcode0,     /* i  : predicted CB gain (fraction),      Q15 */
    Word16 frac_coeff[],    /* i  : energy coeff. (5), fraction part,  Q15 */
    Word16 exp_coeff[],     /* i  : energy coeff. (5), exponent part,  Q0  */
                            /*      (frac_coeff and exp_coeff computed in  */
                            /*       calc_filt_energies())                 */
    Word16 gp_limit,        /* i  : pitch gain limit                       */
    Word16 *gain_pit,       /* o  : Pitch gain,                        Q14 */
    Word16 *gain_cod,       /* o  : Code gain,                         Q1  */
    Word16 *qua_ener_MR122, /* o  : quantized energy error,            Q10 */
                            /*      (for MR122 MA predictor update)        */
    Word16 *qua_ener        /* o  : quantized energy error,            Q10 */
                            /*      (for other MA predictor update)        */
)
{
    const Word16 *p;
    Word16 i, j, index = 0;
    Word16 gcode0, e_max, exp_code;
    Word16 g_pitch, g2_pitch, g_code, g2_code, g_pit_cod;
    Word16 coeff[5], coeff_lo[5];
    Word16 exp_max[5];
    Word32 L_tmp, dist_min;
    const Word16 *table_gain;
    Word16 table_len;
    

    if ( sub (mode, MR102) == 0 || sub (mode, MR74) == 0 || sub (mode, MR67) == 0)
    {
       table_len = VQ_SIZE_HIGHRATES;
       table_gain = table_gain_highrates;
    }
    else
    {
       table_len = VQ_SIZE_LOWRATES;
       table_gain = table_gain_lowrates;
    }
    
    /*-------------------------------------------------------------------*
     *  predicted codebook gain                                          *
     *  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
     *  gc0     = 2^exp_gcode0 + 2^frac_gcode0                           *
     *                                                                   *
     *  gcode0 (Q14) = 2^14*2^frac_gcode0 = gc0 * 2^(14-exp_gcode0)      *
     *-------------------------------------------------------------------*/

    gcode0 = extract_l(Pow2(14, frac_gcode0));

    /*-------------------------------------------------------------------*
     *  Scaling considerations:                                          *
     *  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
     *-------------------------------------------------------------------*/

    /*
     * The error energy (sum) to be minimized consists of five terms, t[0..4].
     *
     *                      t[0] =    gp^2  * <y1 y1>
     *                      t[1] = -2*gp    * <xn y1>
     *                      t[2] =    gc^2  * <y2 y2>
     *                      t[3] = -2*gc    * <xn y2>
     *                      t[4] =  2*gp*gc * <y1 y2>
     *
     */

    /* determine the scaling exponent for g_code: ec = ec0 - 11 */
    exp_code = sub(exp_gcode0, 11);

    /* calculate exp_max[i] = s[i]-1 */
    exp_max[0] = sub(exp_coeff[0], 13);
    exp_max[1] = sub(exp_coeff[1], 14);
    exp_max[2] = add(exp_coeff[2], add(15, shl(exp_code, 1)));
    exp_max[3] = add(exp_coeff[3], exp_code);
    exp_max[4] = add(exp_coeff[4], add(1, exp_code));


    /*-------------------------------------------------------------------*
     *  Find maximum exponent:                                           *
     *  ~~~~~~~~~~~~~~~~~~~~~~                                           *
     *                                                                   *
     *  For the sum operation, all terms must have the same scaling;     *
     *  that scaling should be low enough to prevent overflow. There-    *
     *  fore, the maximum scale is determined and all coefficients are   *
     *  re-scaled:                                                       *
     *                                                                   *
     *    e_max = max(exp_max[i]) + 1;                                   *
     *    e = exp_max[i]-e_max;         e <= 0!                          *
     *    c[i] = c[i]*2^e                                                *
     *-------------------------------------------------------------------*/

    e_max = exp_max[0];
    for (i = 1; i < 5; i++)
    {

        if (sub(exp_max[i], e_max) > 0)
        {
            e_max = exp_max[i];
        }
    }

    e_max = add(e_max, 1);      /* To avoid overflow */

    for (i = 0; i < 5; i++) {
        j = sub(e_max, exp_max[i]);
        L_tmp = L_deposit_h(frac_coeff[i]);
        L_tmp = L_shr(L_tmp, j);
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

    p = &table_gain[0];

    for (i = 0; i < table_len; i++)
    {
        g_pitch = *p++;
        g_code = *p++;         /* this is g_fac        */
        p++;                             /* skip log2(g_fac)     */
        p++;                             /* skip 20*log10(g_fac) */
            

        if (sub(g_pitch, gp_limit) <= 0)
        {
            g_code = mult(g_code, gcode0);
            g2_pitch = mult(g_pitch, g_pitch);
            g2_code = mult(g_code, g_code);
            g_pit_cod = mult(g_code, g_pitch);

            L_tmp = Mpy_32_16(coeff[0], coeff_lo[0], g2_pitch);
            L_tmp = L_add(L_tmp, Mpy_32_16(coeff[1], coeff_lo[1], g_pitch));
            L_tmp = L_add(L_tmp, Mpy_32_16(coeff[2], coeff_lo[2], g2_code));
            L_tmp = L_add(L_tmp, Mpy_32_16(coeff[3], coeff_lo[3], g_code));
            L_tmp = L_add(L_tmp, Mpy_32_16(coeff[4], coeff_lo[4], g_pit_cod));

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
     *  read quantized gains and new values for MA predictor memories   *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   *
     *------------------------------------------------------------------*/

    /* Read the quantized gains */
    p = &table_gain[shl (index, 2)];
    *gain_pit = *p++;
    g_code = *p++;
    *qua_ener_MR122 = *p++;
    *qua_ener = *p;

    /*------------------------------------------------------------------*
     *  calculate final fixed codebook gain:                            *
     *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                            *
     *                                                                  *
     *   gc = gc0 * g                                                   *
     *------------------------------------------------------------------*/

    L_tmp = L_mult(g_code, gcode0);
    L_tmp = L_shr(L_tmp, sub(10, exp_gcode0));
    *gain_cod = extract_h(L_tmp);

    return index;
}
