/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : s10_8pf.h
*      Purpose          : Searches a 35/31 bit algebraic codebook containing 
*                       : 10/8 pulses in a frame of 40 samples.
*
********************************************************************************
*/
#ifndef s10_8pf_h
#define s10_8pf_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
 
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

void search_10and8i40 (
    Word16 nbPulse,      /* i : nbpulses to find                       */
    Word16 step,         /* i :  stepsize                              */
    Word16 nbTracks,     /* i :  nbTracks                              */
    Word16 dn[],         /* i : correlation between target and h[]     */
    Word16 rr[][L_CODE], /* i : matrix of autocorrelation              */
    Word16 ipos[],       /* i : starting position for each pulse       */
    Word16 pos_max[],    /* i : position of maximum of dn[]            */
    Word16 codvec[]      /* o : algebraic codebook vector              */
);

#endif
