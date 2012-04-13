/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : dec_lag6.h
*      Purpose          : Decoding of fractional pitch lag with 1/6 resolution.
*
********************************************************************************
*/
#ifndef dec_lag6_h
#define dec_lag6_h "$Id $"
 
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
 
void Dec_lag6 (
    Word16 index,      /* input : received pitch index           */
    Word16 pit_min,    /* input : minimum pitch lag              */
    Word16 pit_max,    /* input : maximum pitch lag              */
    Word16 i_subfr,    /* input : subframe flag                  */
    Word16 *T0,        /* in/out: integer part of pitch lag      */
    Word16 *T0_frac    /* output: fractional part of pitch lag   */
);

#endif
