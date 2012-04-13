/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : dec_lag3.h
*      Purpose          : Decoding of fractional pitch lag with 1/3 resolution.
*                         Extract the integer and fraction parts of the pitch lag from
*                         the received adaptive codebook index.
*
********************************************************************************
*/
#ifndef dec_lag3_h
#define dec_lag3_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

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
 *   FUNCTION:   Dec_lag3
 *
 *   PURPOSE:  Decoding of fractional pitch lag with 1/3 resolution.
 *             Extract the integer and fraction parts of the pitch lag from
 *             the received adaptive codebook index.
 *
 *    See "Enc_lag3.c" for more details about the encoding procedure.
 *
 *    The fractional lag in 1st and 3rd subframes is encoded with 8 bits
 *    while that in 2nd and 4th subframes is relatively encoded with 4, 5
 *    and 6 bits depending on the mode.
 *
 *************************************************************************/
void Dec_lag3(Word16 index,     /* i : received pitch index                 */
              Word16 T0_min,    /* i : minimum of search range              */
              Word16 T0_max,    /* i : maximum of search range              */
              Word16 i_subfr,   /* i : subframe flag                        */
              Word16 T0_prev,   /* i : integer pitch delay of last subframe
                                       used in 2nd and 4th subframes        */
              Word16 * T0,      /* o : integer part of pitch lag            */ 
              Word16 * T0_frac, /* o : fractional part of pitch lag         */
              Word16 flag4      /* i : flag for encoding with 4 bits        */
              );

#endif
