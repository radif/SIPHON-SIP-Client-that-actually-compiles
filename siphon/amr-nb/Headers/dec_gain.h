/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : dec_gain.h
*      Purpose          : Decode the pitch and codebook gains
*
********************************************************************************
*/
#ifndef dec_gain_h
#define dec_gain_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "gc_pred.h"
#include "mode.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

/*************************************************************************
 *
 *   FUNCTION:  Dec_gain()
 *
 *   PURPOSE: Decode the pitch and codebook gains
 *
 ************************************************************************/
void Dec_gain(
    gc_predState *pred_state, /* i/o: MA predictor state           */
    enum Mode mode,           /* i  : AMR mode                     */
    Word16 index,             /* i  : index of quantization.       */
    Word16 code[],            /* i  : Innovative vector.           */
    Word16 evenSubfr,         /* i  : Flag for even subframes      */     
    Word16 * gain_pit,        /* o  : Pitch gain.                  */
    Word16 * gain_cod         /* o  : Code gain.                   */
);

#endif
