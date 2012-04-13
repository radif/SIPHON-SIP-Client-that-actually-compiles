/**
 *  AMR codec for iPhone and iPod Touch
 *  Copyright (C) 2009 Samuel <samuelv0304@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*******************************************************************************
 Portions of this file are derived from the following 3GPP standard:

    3GPP TS 26.073
    ANSI-C code for the Adaptive Multi-Rate (AMR) speech codec
    Available from http://www.3gpp.org

 (C) 2004, 3GPP Organizational Partners (ARIB, ATIS, CCSA, ETSI, TTA, TTC)
 Permission to distribute, modify and use this file under the standard license
 terms listed above has been obtained from the copyright holder.
*******************************************************************************/
/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : gc_pred.h
*      Purpose          : codebook gain MA prediction
*
********************************************************************************
*/

#ifndef gc_pred_h
#define gc_pred_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"

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
typedef struct {
   Word16 past_qua_en[4];         /* normal MA predictor memory,         Q10 */
                                  /* (contains 20*log10(qua_err))            */
   Word16 past_qua_en_MR122[4];   /* MA predictor memory for MR122 mode, Q10 */
                                  /* (contains log2(qua_err))                */
} gc_predState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int gc_pred_init (gc_predState *st);
/* initialize one instance of codebook gain MA predictor
   Stores pointer to state struct in *st. This pointer has to
   be passed to gc_pred in each call.
   returns 0 on success
 */
 
int gc_pred_reset (gc_predState *st);
/* reset of codebook gain MA predictor state (i.e. set state memory to zero)
   returns 0 on success
 */

void
gc_pred_copy(
    gc_predState *st_src,  /* i : State struct                           */
    gc_predState *st_dest  /* o : State struct                           */
);

/*************************************************************************
 *
 * FUNCTION:  gc_pred()
 *
 * PURPOSE: MA prediction of the innovation energy
 *          (in dB/(20*log10(2))) with mean  removed).
 *
 *************************************************************************/
void gc_pred(
    gc_predState *st,   /* i/o: State struct                           */
    enum Mode mode,     /* i  : AMR mode                               */
    Word16 *code,       /* i  : innovative codebook vector (L_SUBFR)   */
                        /*      MR122: Q12, other modes: Q13           */
    Word16 *exp_gcode0, /* o  : exponent of predicted gain factor, Q0  */
    Word16 *frac_gcode0,/* o  : fraction of predicted gain factor  Q15 */
    Word16 *exp_en,     /* o  : exponent of innovation energy,     Q0  */
                        /*      (only calculated for MR795)            */
    Word16 *frac_en     /* o  : fraction of innovation energy,     Q15 */
                        /*      (only calculated for MR795)            */
);

/*************************************************************************
 *
 * FUNCTION:  gc_pred_update()
 *
 * PURPOSE: update MA predictor with last quantized energy
 *
 *************************************************************************/
void gc_pred_update(
    gc_predState *st,      /* i/o: State struct                     */
    Word16 qua_ener_MR122, /* i  : quantized energy for update, Q10 */
                           /*      (log2(qua_err))                  */
    Word16 qua_ener        /* i  : quantized energy for update, Q10 */
                           /*      (20*log10(qua_err))              */
);

/*************************************************************************
 *
 * FUNCTION:  gc_pred_average_limited()
 *
 * PURPOSE: get average of MA predictor state values (with a lower limit)
 *          [used in error concealment]
 *
 *************************************************************************/
void gc_pred_average_limited(
    gc_predState *st,       /* i: State struct                    */
    Word16 *ener_avg_MR122, /* o: averaged quantized energy,  Q10 */
                            /*    (log2(qua_err))                 */
    Word16 *ener_avg        /* o: averaged quantized energy,  Q10 */
                            /*    (20*log10(qua_err))             */
);

#endif
