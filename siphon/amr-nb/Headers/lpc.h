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
*      File             : lpc.h
*      Purpose          : 2 LP analyses centered at 2nd and 4th subframe
*                         for mode 12.2. For all other modes a
*                         LP analysis centered at 4th subframe is 
*                         performed.
*
********************************************************************************
*/
#ifndef lpc_h
#define lpc_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "levinson.h"
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
   LevinsonState levinsonSt;
} lpcState;

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int lpc_init (lpcState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to lpc in each call.
   returns 0 on success
 */
 
int lpc_reset (lpcState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

int lpc(
    lpcState *st,     /* i/o: State struct                */
    enum Mode mode,   /* i  : coder mode                  */
    Word16 x[],       /* i  : Input signal           Q15  */
    Word16 x_12k2[],  /* i  : Input signal (EFR)     Q15  */
    Word16 a[]        /* o  : predictor coefficients Q12  */
);

#endif
