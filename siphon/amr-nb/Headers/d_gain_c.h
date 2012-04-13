/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : d_gain_c.h
*      Purpose          : Decode the fixed codebook gain using the received index.
*
********************************************************************************
*/
#ifndef d_gain_c_h
#define d_gain_c_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"
#include "gc_pred.h"
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : d_gain_code
*  Purpose     : Decode the fixed codebook gain using the received index.
*  Description : The received index gives the gain correction factor
*                gamma. The quantized gain is given by   g_q = g0 * gamma
*                where g0 is the predicted gain. To find g0, 4th order
*                MA prediction is applied to the mean-removed innovation
*                energy in dB.
*  Returns     : void
*
**************************************************************************
*/
void d_gain_code (
    gc_predState *pred_state, /* i/o : MA predictor state               */
    enum Mode mode,           /* i   : AMR mode                         */
    Word16 index,             /* i   : received quantization index      */
    Word16 code[],            /* i   : innovation codevector            */
    Word16 *gain_code         /* o   : decoded innovation gain          */
);
 
#endif
