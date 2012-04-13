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
*      File             : q_plsf.h
*      Purpose          : common include file for LSF vector/matrix quanti-
*                         sation modules (q_plsf_3.c/q_plsf_5.c)
*
********************************************************************************
*/
#ifndef q_plsf_h
#define q_plsf_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "mode.h"
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
    Word16 past_rq[M];    /* Past quantized prediction error, Q15 */

} Q_plsfState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int Q_plsf_init (Q_plsfState *st);
/* initialize one instance of the state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Q_plsf_5 / Q_plsf_3 in each call.
   returns 0 on success
 */
 
int Q_plsf_reset (Q_plsfState *st);
/* reset of state (i.e. set state memory to zero)
   returns 0 on success
 */

void Q_plsf_3(
    Q_plsfState *st,    /* i/o: state struct                             */
    enum Mode mode,     /* i  : coder mode                               */
    Word16 *lsp1,       /* i  : 1st LSP vector                      Q15  */
    Word16 *lsp1_q,     /* o  : quantized 1st LSP vector            Q15  */
    Word16 *indice,     /* o  : quantization indices of 3 vectors   Q0   */
    Word16 *pred_init_i /* o  : init index for MA prediction in DTX mode */
);

void Q_plsf_5 (
    Q_plsfState *st,
    Word16 *lsp1,      /* i  : 1st LSP vector,                     Q15 */
    Word16 *lsp2,      /* i  : 2nd LSP vector,                     Q15 */   
    Word16 *lsp1_q,    /* o  : quantized 1st LSP vector,           Q15 */
    Word16 *lsp2_q,    /* o  : quantized 2nd LSP vector,           Q15 */
    Word16 *indice     /* o  : quantization indices of 5 matrices, Q0  */
);
 
#endif
