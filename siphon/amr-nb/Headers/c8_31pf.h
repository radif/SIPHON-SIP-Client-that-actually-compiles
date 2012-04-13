/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : c8_31pf.h
*      Purpose          : Searches a 31 bit algebraic codebook containing 
*                       : 8 pulses in a frame of 40 samples.
*
********************************************************************************
*/
#ifndef c8_31pf_h
#define c8_31pf_h "$Id $"
 
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
void code_8i40_31bits (
    Word16 x[],        /* i : target vector                                  */
    Word16 cn[],       /* i : residual after long term prediction            */
    Word16 h[],        /* i : impulse response of weighted synthesis
                              filter                                         */
    Word16 cod[],      /* o : algebraic (fixed) codebook excitation          */
    Word16 y[],        /* o : filtered fixed codebook excitation             */
    Word16 indx[]      /* o : 7 Word16, index of 8 pulses (signs+positions)  */
    );
 
#endif
