/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : hp_max.h
*      Purpose          : Find the maximum correlation of scal_sig[] in a given
*                         delay range.
*
********************************************************************************
*/
#ifndef hp_max_h
#define hp_max_h "$Id $"
 
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
/*************************************************************************
 *
 *  FUNCTION:  hp_max
 *
 *  PURPOSE: Find the maximum high-pass filtered correlation of
 *           signal scal_sig[] in a given delay range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           corr[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs the maximum high-pass filtered correlation
 *      after normalization.
 *
 *************************************************************************/
Word16 hp_max (   
    Word32 corr[],      /* i   : correlation vector.                      */
    Word16 scal_sig[],  /* i   : scaled signal.                           */
    Word16 L_frame,     /* i   : length of frame to compute pitch         */
    Word16 lag_max,     /* i   : maximum lag                              */
    Word16 lag_min,     /* i   : minimum lag                              */
    Word16 *cor_hp_max  /* o   : max high-pass filtered norm. correlation */
    );
#endif
