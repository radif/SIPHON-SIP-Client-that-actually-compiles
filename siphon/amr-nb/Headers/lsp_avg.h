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
*      File             : lsp_avg.h
*      Purpose:         : LSP averaging and history
*                         The LSPs are averaged over 8 frames
*
********************************************************************************
*/
#ifndef lsp_avg_h
#define lsp_avg_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

#define EXPCONST          5243               /* 0.16 in Q15 */
 
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 lsp_meanSave[M];          /* Averaged LSPs saved for efficiency  */
} lsp_avgState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

/*
**************************************************************************
*
*  Function    : lsp_avg_init
*  Purpose     : Allocates memory and initializes state variables
*
**************************************************************************
*/
int lsp_avg_init (
    lsp_avgState *state
);
 
/*
**************************************************************************
*
*  Function    : lsp_history_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int lsp_avg_reset (
    lsp_avgState *state
);

/*
**************************************************************************
*
*  Function    : lsp_avg
*  Purpose     : Calculate the LSP averages
*
**************************************************************************
*/
void lsp_avg (
    lsp_avgState *st,     /* i/o : State struct                 Q15 */
    Word16 *lsp           /* i   : LSP vector                   Q15 */
);
#endif
