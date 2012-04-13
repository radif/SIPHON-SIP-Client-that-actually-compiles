/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : q_gain_c.h
*      Purpose          : Scalar quantization of the innovative
*                       : codebook gain.
*
********************************************************************************
*/
#ifndef q_gain_c_h
#define q_gain_c_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"
#include "gc_pred.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
 
 
/*--------------------------------------------------------------------------*
 * Function q_gain_code()                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                  *
 * Scalar quantization of the innovative codebook gain.                     *
 *                                                                          *
 * gc_pred() is used for MA prediction of the innovation energy             *
 *--------------------------------------------------------------------------*/
Word16 q_gain_code (        /* o  : quantization index,            Q0  */
    enum Mode mode,         /* i  : AMR mode                           */
    Word16 exp_gcode0,      /* i  : predicted CB gain (exponent),  Q0  */
    Word16 frac_gcode0,     /* i  : predicted CB gain (fraction),  Q15 */
    Word16 *gain,           /* i/o: quantized fixed codebook gain, Q1  */
    Word16 *qua_ener_MR122, /* o  : quantized energy error,        Q10 */
                            /*      (for MR122 MA predictor update)    */
    Word16 *qua_ener        /* o  : quantized energy error,        Q10 */
                            /*      (for other MA predictor update)    */
);
 
#endif
