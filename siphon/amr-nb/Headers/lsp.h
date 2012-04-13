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
*      File             : lsp.h
*      Purpose          : Conversion from A(z) to LSP. Quantization and
*                         interpolation of LSPs.
*
********************************************************************************
*/
#ifndef lsp_h
#define lsp_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "q_plsf.h"
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

   /* Past LSPs */
   Word16 lsp_old[M];
   Word16 lsp_old_q[M];

   /* Quantization state */
   Q_plsfState qSt;

} lspState;

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : lsp_init
*  Purpose     : Allocates memory and initializes state variables
*  Description : Stores pointer to filter status struct in *st. This
*                pointer has to be passed to lsp in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/ 
int lsp_init (lspState *st);

/*
**************************************************************************
*
*  Function    : lsp_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
int lsp_reset (lspState *st);

/*
**************************************************************************
*
*  Function    : lsp
*  Purpose     : Conversion from LP coefficients to LSPs.
*                Quantization of LSPs.
*  Description : Generates 2 sets of LSPs from 2 sets of
*                LP coefficients for mode 12.2. For the other
*                modes 1 set of LSPs is generated from 1 set of
*                LP coefficients. These LSPs are quantized with
*                Matrix/Vector quantization (depending on the mode)
*                and interpolated for the subframes not yet having
*                their own LSPs.
*                
**************************************************************************
*/
int lsp(lspState *st,        /* i/o : State struct                            */
        enum Mode req_mode,  /* i   : requested coder mode                    */
        enum Mode used_mode, /* i   : used coder mode                         */        
        Word16 az[],         /* i/o : interpolated LP parameters Q12          */
        Word16 azQ[],        /* o   : quantization interpol. LP parameters Q12*/
        Word16 lsp_new[],    /* o   : new lsp vector                          */ 
        Word16 **anap        /* o   : analysis parameters                     */
        );

#endif
