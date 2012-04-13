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
*      File             : d_plsf.h
*      Purpose          : common part (init, exit, reset) of LSF decoder
*                         module (rest in d_plsf_3.c and d_plsf_5.c)
*
********************************************************************************
*/
#ifndef d_plsf_h
#define d_plsf_h "$Id $"
 
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
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*
*--------------------------------------------------*
* Constants (defined in cnst.h)                    *
*--------------------------------------------------*
* M            : LPC Order                         *
*--------------------------------------------------*
*/
 
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 past_r_q[M];   /* Past quantized prediction error, Q15 */
  Word16 past_lsf_q[M]; /* Past dequantized lsfs,           Q15 */
} D_plsfState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
 
/*
**************************************************************************
*
*  Function    : D_plsf_init
*  Purpose     : Allocates and initializes state memory
*  Description : Stores pointer to state struct in *st. This
*                pointer has to be passed to D_plsf in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/
int D_plsf_init (D_plsfState *st);

/*
**************************************************************************
*
*  Function    : D_plsf_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
int D_plsf_reset (D_plsfState *st);

/*
**************************************************************************
*
*  Function    : D_plsf_5
*  Purpose     : Decodes the 2 sets of LSP parameters in a frame 
*                using the received quantization indices.
*  Description : The two sets of LSFs are quantized using split by 
*                5 matrix quantization (split-MQ) with 1st order MA
*                prediction.
*                See "q_plsf_5.c" for more details about the
*                quantization procedure
*  Returns     : 0
*
**************************************************************************
*/
int D_plsf_5 (
    D_plsfState *st,  /* i/o: State variables                            */
    Word16 bfi,       /* i  : bad frame indicator (set to 1 if a bad    
                              frame is received)                         */
    Word16 *indice,   /* i  : quantization indices of 5 submatrices, Q0  */
    Word16 *lsp1_q,   /* o  : quantized 1st LSP vector (M)           Q15 */
    Word16 *lsp2_q    /* o  : quantized 2nd LSP vector (M)           Q15 */
);

/*************************************************************************
 *
 *  FUNCTION:   D_plsf_3()
 *
 *  PURPOSE: Decodes the LSP parameters using the received quantization
 *           indices.1st order MA prediction and split by 3 matrix
 *           quantization (split-MQ)
 *
 *************************************************************************/

void D_plsf_3(
    D_plsfState *st,  /* i/o: State struct                               */
    enum Mode mode,   /* i  : coder mode                                 */
    Word16 bfi,       /* i  : bad frame indicator (set to 1 if a         */
                      /*      bad frame is received)                     */
    Word16 * indice,  /* i  : quantization indices of 3 submatrices, Q0  */
    Word16 * lsp1_q   /* o  : quantized 1st LSP vector,              Q15 */
);

/*************************************************************************
 *
 *  FUNCTION:   Init_D_plsf_3()
 *
 *  PURPOSE: Set the past_r_q[M] vector to one of the eight
 *           past_rq_init vectors.
 *
 *************************************************************************/
void Init_D_plsf_3(D_plsfState *st,  /* i/o: State struct                */
		   Word16 index      /* i  : past_rq_init[] index [0, 7] */
);

#endif
