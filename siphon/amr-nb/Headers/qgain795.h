/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : qgain795.h
*      Purpose          : (pre-) quantization of pitch gain for MR795
*
********************************************************************************
*/
#ifndef qgain795_h
#define qgain795_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "g_adapt.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

/*************************************************************************
 *
 * FUNCTION:  MR795_gain_quant
 *
 * PURPOSE:   pitch and codebook quantization for MR795
 *
 *************************************************************************/
void
MR795_gain_quant(
    GainAdaptState *adapt_st, /* i/o: gain adapter state structure       */
    Word16 res[],             /* i  : LP residual,                  Q0   */
    Word16 exc[],             /* i  : LTP excitation (unfiltered),  Q0   */
    Word16 code[],            /* i  : CB innovation (unfiltered),   Q13  */
    Word16 frac_coeff[],      /* i  : coefficients (5),             Q15  */
    Word16 exp_coeff[],       /* i  : energy coefficients (5),      Q0   */
                              /*      coefficients from calc_filt_ener() */
    Word16 exp_code_en,       /* i  : innovation energy (exponent), Q0   */
    Word16 frac_code_en,      /* i  : innovation energy (fraction), Q15  */
    Word16 exp_gcode0,        /* i  : predicted CB gain (exponent), Q0   */
    Word16 frac_gcode0,       /* i  : predicted CB gain (fraction), Q15  */
    Word16 L_subfr,           /* i  : Subframe length                    */
    Word16 cod_gain_frac,     /* i  : opt. codebook gain (fraction),Q15  */
    Word16 cod_gain_exp,      /* i  : opt. codebook gain (exponent), Q0  */
    Word16 gp_limit,          /* i  : pitch gain limit                   */
    Word16 *gain_pit,         /* i/o: Pitch gain (unquant/quant),   Q14  */
    Word16 *gain_cod,         /* o  : Code gain,                    Q1   */
    Word16 *qua_ener_MR122,   /* o  : quantized energy error,       Q10  */
                              /*      (for MR122 MA predictor update)    */
    Word16 *qua_ener,         /* o  : quantized energy error,       Q10  */
                              /*      (for other MA predictor update)    */
    Word16 **anap             /* o  : Index of quantization              */
                              /*      (first gain pitch, then code pitch)*/
);

#endif
