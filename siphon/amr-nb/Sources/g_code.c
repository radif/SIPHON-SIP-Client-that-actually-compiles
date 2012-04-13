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
*      File             : g_code.c
*      Purpose          : Compute the innovative codebook gain.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "g_code.h"
const char g_code_id[] = "@(#)$Id $" g_code_h;
 
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
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:   G_code
 *
 *  PURPOSE:  Compute the innovative codebook gain.
 *
 *  DESCRIPTION:
 *      The innovative codebook gain is given by
 *
 *              g = <x[], y[]> / <y[], y[]>
 *
 *      where x[] is the target vector, y[] is the filtered innovative
 *      codevector, and <> denotes dot product.
 *
 *************************************************************************/
Word16 G_code (         /* out   : Gain of innovation code         */
    Word16 xn2[],       /* in    : target vector                   */
    Word16 y2[]         /* in    : filtered innovation vector      */
)
{
    Word16 i;
    Word16 xy, yy, exp_xy, exp_yy, gain;
    Word16 scal_y2;
    Word32 s, s1;

    s = 1L; /* Avoid case of all zeros */
    s1 = 0L;
    for (i = 0; i < L_SUBFR; i++)
    {
      /* Scale down Y[] by 2 to avoid overflow */
      /*scal_y2 = shr (y2[i], 1);*/
      scal_y2 = y2[i] >> 1;
      /* Compute scalar product <X[],Y[]> */
      /*s = L_mac (s, xn2[i], scal_y2);*/
      s += xn2[i] * scal_y2;
      /* Compute scalar product <Y[],Y[]> */
      /*s1 = L_mac (s1, scal_y2, scal_y2);*/
      s1 += scal_y2 * scal_y2;
    }
    s <<= 1;
    exp_xy = norm_l (s);
    xy = extract_h (L_shl (s, exp_xy));

    if (xy <= 0)
        return ((Word16) 0);

    s1 <<= 1;
    exp_yy = norm_l (s1);
    yy = extract_h (L_shl (s1, exp_yy));

    /* compute gain = xy/yy */

    xy = shr (xy, 1);                 /* Be sure xy < yy */
    gain = div_s (xy, yy);

    /* Denormalization of division */
    i = add (exp_xy, 5);              /* 15-1+9-18 = 5 */
    i = sub (i, exp_yy);

    gain = shl (shr (gain, i), 1);    /* Q0 -> Q1 */
    
    return (gain);
}
