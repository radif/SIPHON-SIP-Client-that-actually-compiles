/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : qgain475.h
*      Purpose          : Quantization of pitch and codebook gains for MR475.
*
********************************************************************************
*/
#ifndef qgain475_h
#define qgain475_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "gc_pred.h"
#include "mode.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
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
    Word16 exp_gcode0,     /* i  : predicted CB gain (exponent),      Q0  */
    Word16 frac_gcode0,    /* i  : predicted CB gain (fraction),      Q15 */
    Word16 cod_gain_exp,   /* i  : optimum codebook gain (exponent),  Q0  */
    Word16 cod_gain_frac   /* i  : optimum codebook gain (fraction),  Q15 */
);

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
);
#endif
