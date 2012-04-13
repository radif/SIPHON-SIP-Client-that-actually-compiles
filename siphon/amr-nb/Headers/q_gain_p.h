/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : q_gain_p.h
*      Purpose          : Scalar quantization of the pitch gain
*
********************************************************************************
*/
#ifndef q_gain_p_h
#define q_gain_p_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"

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
Word16 q_gain_pitch (   /* Return index of quantization                      */
    enum Mode mode,     /* i  : AMR mode                                     */
    Word16 gp_limit,    /* i  : pitch gain limit                             */
    Word16 *gain,       /* i/o: Pitch gain (unquant/quant),              Q14 */
    Word16 gain_cand[], /* o  : pitch gain candidates (3),   MR795 only, Q14 */ 
    Word16 gain_cind[]  /* o  : pitch gain cand. indices (3),MR795 only, Q0  */ 
);
 
#endif
