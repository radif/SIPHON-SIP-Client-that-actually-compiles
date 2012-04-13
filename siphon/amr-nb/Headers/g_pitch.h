/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : g_pitch.h
*      Purpose          : Compute the pitch (adaptive codebook) gain.
*
********************************************************************************
*/
#ifndef g_pitch_h
#define g_pitch_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
Word16 G_pitch     (    /* o : Gain of pitch lag saturated to 1.2       */
    enum Mode mode,     /* i : AMR mode                                 */
    Word16 xn[],        /* i : Pitch target.                            */
    Word16 y1[],        /* i : Filtered adaptive codebook.              */
    Word16 g_coeff[],   /* i : Correlations need for gain quantization.
                               (7.4 only). Pass NULL if not needed      */
    Word16 L_subfr      /* i : Length of subframe.                      */
);
 
#endif
