/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : inv_sqrt.h
*      Purpose          : Computes 1/sqrt(L_x),  where  L_x is positive.
*                       : If L_x is negative or zero, the result is
*                       : 1 (3fff ffff).
*      Description      : The function 1/sqrt(L_x) is approximated by a table 
*                       : and linear interpolation. The inverse square root is 
*                       : computed using the following steps:
*               1- Normalization of L_x.
*               2- If (30-exponent) is even then shift right once.
*               3- exponent = (30-exponent)/2  +1
*               4- i = bit25-b31 of L_x;  16<=i<=63  because of normalization.
*               5- a = bit10-b24
*               6- i -=16
*               7- L_y = table[i]<<16 - (table[i] - table[i+1]) * a * 2
*               8- L_y >>= exponent
*
********************************************************************************
*/
#ifndef inv_sqrt_h
#define inv_sqrt_h "$Id $"
 
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
 
Word32 Inv_sqrt (      /* (o) : output value   (range: 0<=val<1)            */
    Word32 L_x           /* (i) : input value    (range: 0<=val<=7fffffff)    */
);
 
#endif
