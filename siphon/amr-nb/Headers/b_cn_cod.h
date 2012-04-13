/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : b_cn_cod.h
*      Purpose          : Contains function for comfort noise generation.
*
********************************************************************************
*/
#ifndef b_cn_cod_h
#define b_cn_cod_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

/*************************************************************************
 *
 *   FUNCTION NAME: pseudonoise
 *
 *   PURPOSE: Generate a random integer value to use in comfort noise
 *            generation. The algorithm uses polynomial x^31 + x^3 + 1
 *            (length of PN sequence is 2^31 - 1).
 *
 *   INPUTS:      *shift_reg    Old CN generator shift register state
 *
 *
 *   OUTPUTS:     *shift_reg    Updated CN generator shift register state
 *
 *   RETURN VALUE: Generated random integer value
 *
 *************************************************************************/

Word16 pseudonoise (
    Word32 *shift_reg, /* i/o : Old CN generator shift register state */
    Word16 no_bits     /* i   : Number of bits                        */
);

/*************************************************************************
 *
 *   FUNCTION NAME: build_CN_code
 *
 *   PURPOSE: Compute the comfort noise fixed codebook excitation. The
 *            gains of the pulses are always +/-1.
 *
 *   INPUTS:      *seed         Old CN generator shift register state
 *
 *   OUTPUTS:     cod[0..39]    Generated comfort noise fixed codebook vector
 *                *seed         Updated CN generator shift register state
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/
void build_CN_code (
    Word32 *seed,         /* i/o : Old CN generator shift register state */
    Word16 cod[]          /* o   : Generated CN fixed codebook vector    */
);

/*************************************************************************
 *
 *   FUNCTION NAME: build_CN_param
 *
 *   PURPOSE: Randomize the speech parameters. So that they 
 *            do not produce tonal artifacts if used by ECU.
 *
 *   INPUTS:      *seed            Old CN generator shift register state
 *                n_param,         Number of parameters to randomize
 *                parm_size_table, 
 *                  
 *
 *   OUTPUTS:      parm    CN Generated Parameters
 *                *seed    Updated CN generator shift register state
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/
void build_CN_param (
    Word16 *seed,              /* i/o : Old CN generator shift register state */
    const Word16 n_param,            /* i : number of params     */  
    const Word16 param_size_table[], /* i : size of params       */   
    Word16 parm[]              /* o   : CN Generated Params*/
);

#endif
