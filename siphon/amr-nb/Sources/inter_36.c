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
*      File             : inter_36.c
*      Purpose          : Interpolating the normalized correlation
*                       : with 1/3 or 1/6 resolution.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "inter_36.h"
const char inter_36_id[] = "@(#)$Id $" inter_36_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define UP_SAMP_MAX  6

#include "inter_36.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  Interpol_3or6()
 *
 *  PURPOSE:  Interpolating the normalized correlation with 1/3 or 1/6
 *            resolution.
 *
 *************************************************************************/
Word16 Interpol_3or6 (  /* o : interpolated value                        */
    Word16 *x,          /* i : input vector                              */
    Word16 frac,        /* i : fraction  (-2..2 for 3*, -3..3 for 6*)    */
    Word16 flag3        /* i : if set, upsampling rate = 3 (6 otherwise) */
)
{
    Word16 i, k;
    Word16 *x1, *x2;
    const Word16 *c1, *c2;
    Word32 s;

    if (flag3 != 0)
    {
      frac <<= 1;  /* inter_3[k] = inter_6[2*k] -> k' = 2*k */
    }
    

    if (frac < 0)
    {
        frac += UP_SAMP_MAX;
        x--;
    }
    
    x1 = &x[0];
    x2 = &x[1];
    c1 = &inter_6[frac];
    c2 = &inter_6[UP_SAMP_MAX - frac];

    s = 0x00004000;
    for (i = 0, k = 0; i < L_INTER_SRCH; i++, k += UP_SAMP_MAX)
    {
      /*s += (Word32)x1[-i] * c1[k];
      s += (Word32)x2[i] * c2[k];*/
      s += (Word32)*(x1--) * c1[k];
      s += (Word32)*(x2++) * c2[k];
    }

    return s >> 15;
}
