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
*      File             : preemph.c
*      Purpose          : Preemphasis filtering
*      Description      : Filtering through 1 - g z^-1 
*
********************************************************************************
*/


/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "preemph.h"
const char preemph_id[] = "@(#)$Id $" preemph_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   Post_Filter_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int preemphasis_init (preemphasisState *state)
{
  if (state == (preemphasisState *) NULL){
      fprintf(stderr, "preemphasis_init: invalid parameter\n");
      return -1;
  }

  preemphasis_reset(state);

  return 0;
}

/*************************************************************************
*
*  Function:   preemphasis_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int preemphasis_reset (preemphasisState *state)
{
  if (state == (preemphasisState *) NULL){
      fprintf(stderr, "preemphasis_reset: invalid parameter\n");
      return -1;
  }
  
  state->mem_pre = 0;
 
  return 0;
}
 
/*
**************************************************************************
*  Function:  preemphasis
*  Purpose:   Filtering through 1 - g z^-1 
*
**************************************************************************
*/
void preemphasis (
    preemphasisState *st, /* (i/o)  : preemphasis filter state    */
    Word16 *signal, /* (i/o)   : input signal overwritten by the output */
    Word16 g,       /* (i)     : preemphasis coefficient                */
    Word16 L        /* (i)     : size of filtering                      */
)
{
    Word16 temp, i;

    temp = signal[L-1];

    for (i = L - 1; i > 0; --i)
      signal[i] -= ((Word32)g * (Word32)signal[i-1]) >> 15;

    signal[0] -= ((Word32)g * (Word32)st->mem_pre) >> 15;

    st->mem_pre = temp;
}
