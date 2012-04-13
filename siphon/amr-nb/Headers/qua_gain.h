/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : qua_gain.h
*      Purpose          : Quantization of pitch and codebook gains.
*
********************************************************************************
*/
#ifndef qua_gain_h
#define qua_gain_h "$Id $"
 
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
);
#endif
