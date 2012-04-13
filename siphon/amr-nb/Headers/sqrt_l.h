/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : sqrt_l.h
*      Purpose          : Computes sqrt(L_x),  where  L_x is positive.
*                       : If L_x is negative or zero, the result is
*                       : 0 (3fff ffff).
*
********************************************************************************
*/
#ifndef sqrt_l_h
#define sqrt_l_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
 
Word32 sqrt_l_exp (/* o : output value,                        Q31 */
    Word32 L_x,    /* i : input value,                         Q31 */
    Word16 *exp    /* o : right shift to be applied to result, Q0  */
);
 
#endif
