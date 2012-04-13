/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : c2_9pf.h
*      Purpose          : Searches a 9 bit algebraic codebook containing 
*                         2 pulses in a frame of 40 samples.
*
*****************************************************************************
*/
#ifndef c2_9pf_h
#define c2_9pf_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/

/*
*****************************************************************************
*                         DECLARATION OF PROTOTYPES
*****************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  code_2i40_9bits()
 *
 *  PURPOSE:  Searches a 9 bit algebraic codebook containing 2 pulses
 *            in a frame of 40 samples.
 *
 *  DESCRIPTION:
 *    The code length is 40, containing 2 nonzero pulses: i0...i1.
 *    All pulses can have two possible amplitudes: +1 or -1.
 *    Pulse i0 can have 8 possible positions, pulse i1 can have
 *    8 positions. Also coded is which track pair should be used,
 *    i.e. first or second pair. Where each pair contains 2 tracks.
 *
 *     First subframe:
 *     first   i0 :  0, 5, 10, 15, 20, 25, 30, 35.
 *             i1 :  2, 7, 12, 17, 22, 27, 32, 37.
 *     second  i0 :  1, 6, 11, 16, 21, 26, 31, 36.
 *             i1 :  3, 8, 13, 18, 23, 28, 33, 38.
 *
 *     Second subframe:
 *     first   i0 :  0, 5, 10, 15, 20, 25, 30, 35.
 *             i1 :  3, 8, 13, 18, 23, 28, 33, 38.
 *     second  i0 :  2, 7, 12, 17, 22, 27, 32, 37.
 *             i1 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *     Third subframe:
 *     first   i0 :  0, 5, 10, 15, 20, 25, 30, 35.
 *             i1 :  2, 7, 12, 17, 22, 27, 32, 37.
 *     second  i0 :  1, 6, 11, 16, 21, 26, 31, 36.
 *             i1 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *     Fourth subframe:
 *     first   i0 :  0, 5, 10, 15, 20, 25, 30, 35.
 *             i1 :  3, 8, 13, 18, 23, 28, 33, 38.
 *     second  i0 :  1, 6, 11, 16, 21, 26, 31, 36.
 *             i1 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *************************************************************************/

Word16 code_2i40_9bits(
    Word16 subNr,       /* i : subframe number                               */
    Word16 x[],         /* i : target vector                                 */
    Word16 h[],         /* i : impulse response of weighted synthesis filter */
                        /*     h[-L_subfr..-1] must be set to zero.          */
    Word16 T0,          /* i : Pitch lag                                     */
    Word16 pitch_sharp, /* i : Last quantized pitch gain                     */
    Word16 code[],      /* o : Innovative codebook                           */
    Word16 y[],         /* o : filtered fixed codebook excitation            */
    Word16 * sign       /* o : Signs of 2 pulses                             */
);

#endif
