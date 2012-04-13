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
*      File             : lpc.c
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lpc.h"
const char lpc_id[] = "@(#)$Id $" lpc_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "autocorr.h"
#include "lag_wind.h"
#include "levinson.h"
#include "cnst.h"
#include "mode.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "window.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   lpc_init
*
**************************************************************************
*/
int lpc_init (lpcState *state)
{
  if (state == (lpcState *) NULL){
      fprintf(stderr, "lpc_init: invalid parameter\n");
      return -1;
  }

  /* Init sub states */
  if (Levinson_init(&state->levinsonSt)) {
     lpc_reset(state);
     return -1;
  }


  lpc_reset(state);

  return 0;
}
 
/*************************************************************************
*
*  Function:   lpc_reset
*
**************************************************************************
*/
int lpc_reset (lpcState *state)
{
  
  if (state == (lpcState *) NULL){
      fprintf(stderr, "lpc_reset: invalid parameter\n");
      return -1;
  }
  
  Levinson_reset(&state->levinsonSt);

  return 0;
}

int lpc(
    lpcState *st,     /* i/o: State struct                */
    enum Mode mode,   /* i  : coder mode                  */
    Word16 x[],       /* i  : Input signal           Q15  */
    Word16 x_12k2[],  /* i  : Input signal (EFR)     Q15  */
    Word16 a[]        /* o  : predictor coefficients Q12  */
)
{
   Word16 rc[4];                  /* First 4 reflection coefficients Q15 */
   Word16 rLow[MP1], rHigh[MP1];  /* Autocorrelations low and hi      */
                                  /* No fixed Q value but normalized  */
                                  /* so that overflow is avoided      */

   if (mode == MR122)
   {
       /* Autocorrelations */
       Autocorr(x_12k2, M, rHigh, rLow, window_160_80);
       /* Lag windowing    */
       Lag_window(M, rHigh, rLow);
       /* Levinson Durbin  */
       Levinson(&st->levinsonSt, rHigh, rLow, &a[MP1], rc);

       /* Autocorrelations */
       Autocorr(x_12k2, M, rHigh, rLow, window_232_8);
       /* Lag windowing    */
       Lag_window(M, rHigh, rLow);
       /* Levinson Durbin  */
       Levinson(&st->levinsonSt, rHigh, rLow, &a[MP1 * 3], rc);
   }
   else
   {
       /* Autocorrelations */
       Autocorr(x, M, rHigh, rLow, window_200_40);
       /* Lag windowing    */
       Lag_window(M, rHigh, rLow);
       /* Levinson Durbin  */
       Levinson(&st->levinsonSt, rHigh, rLow, &a[MP1 * 3], rc);
   }

   return 0;
}
