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
*      File             : post_pro.c
*      Purpose          : Postprocessing of output speech.
*
*                         - 2nd order high pass filtering with cut
*                           off frequency at 60 Hz.
*                         - Multiplication of output by two.
*
********************************************************************************
*/
 
 
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "post_pro.h"
const char post_pro_id[] = "@(#)$Id $" post_pro_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"
 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/* filter coefficients (fc = 60 Hz) */
static const Word16 b[3] = {7699, -15398, 7699};
static const Word16 a[3] = {8192, 15836, -7667};

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   Post_Process_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int Post_Process_init (Post_ProcessState *state)
{
  if (state == (Post_ProcessState *) NULL)
  {
      fprintf(stderr, "Post_Process_init: invalid parameter\n");
      return -1;
  }

  Post_Process_reset(state);

  return 0;
}
 
/*************************************************************************
*
*  Function:   Post_Process_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int Post_Process_reset (Post_ProcessState *state)
{
  if (state == (Post_ProcessState *) NULL){
      fprintf(stderr, "Post_Process_reset: invalid parameter\n");
      return -1;
  }
  
  state->y2_hi = 0;
  state->y2_lo = 0;
  state->y1_hi = 0;
  state->y1_lo = 0;
  state->x1 = 0;
  state->x2 = 0;

  return 0;
}
 
/*************************************************************************
 *
 *  FUNCTION:  Post_Process()
 *
 *  PURPOSE: Postprocessing of input speech.
 *
 *  DESCRIPTION:
 *     - 2nd order high pass filtering with cut off frequency at 60 Hz.
 *     - Multiplication of output by two.
 *                                                                        
 * Algorithm:                                                             
 *                                                                        
 *  y[i] = b[0]*x[i]*2 + b[1]*x[i-1]*2 + b[2]*x[i-2]*2
 *                     + a[1]*y[i-1]   + a[2]*y[i-2];                     
 *                                                                        
 *                                                                        
 *************************************************************************/
void Post_Process (
    Post_ProcessState *st,  /* i/o : post process state                   */
    Word16 signal[],        /* i/o : signal                               */
    Word16 lg               /* i   : length of signal                     */
    )
{
    Word16 i;
    Word32 L_tmp;

    for (i = 0; i < lg; ++i)
    {
      L_tmp = b[0] * (signal[i] - 2*st->x1/*signal[i-1]*/ + st->x2/*signal[i-2]*/);
      L_tmp <<= 1;
      L_tmp += Mpy_32_16 (st->y1_hi, st->y1_lo, a[1]);
      L_tmp += Mpy_32_16 (st->y2_hi, st->y2_lo, a[2]);

      /*L_tmp  = L_shl(L_tmp, 2);*/
      L_tmp <<= 2;

      st->x2 = st->x1;
      st->x1 = signal[i];

       /* Multiplication by two of output speech with saturation. */
      /*signal[i] = round(L_shl(L_tmp, 1));*/
      signal[i] = round(L_tmp << 1);

      st->y2_hi = st->y1_hi;
      st->y2_lo = st->y1_lo;
      st->y1_hi = (Word16) (L_tmp >> 16);
      st->y1_lo = (Word16)((L_tmp >> 1) - (st->y1_hi << 15));
    }
}
