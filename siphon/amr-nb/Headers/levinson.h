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
*      File             : levinson.h
*      Purpose          : Levinson-Durbin algorithm in double precision.
*                       : To compute the LP filter parameters from the
*                       : speech autocorrelations.
*
********************************************************************************
*/
#ifndef levinson_h
#define levinson_h "$Id $"
 
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
 
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 old_A[M + 1];     /* Last A(z) for case of unstable filter */
} LevinsonState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int Levinson_init (LevinsonState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Levinson in each call.
   returns 0 on success
 */
 
int Levinson_reset (LevinsonState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

int Levinson (
    LevinsonState *st,
    Word16 Rh[],       /* i : Rh[m+1] Vector of autocorrelations (msb) */
    Word16 Rl[],       /* i : Rl[m+1] Vector of autocorrelations (lsb) */
    Word16 A[],        /* o : A[m]    LPC coefficients  (m = 10)       */
    Word16 rc[]        /* o : rc[4]   First 4 reflection coefficients  */
);
 
#endif
