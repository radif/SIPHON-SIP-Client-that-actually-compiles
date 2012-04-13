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
*      File             : g_pitch.c
*      Purpose          : Compute the pitch (adaptive codebook) gain.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "g_pitch.h"
const char g_pitch_id[] = "@(#)$Id $" g_pitch_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "mode.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  G_pitch
 *
 *  PURPOSE:  Compute the pitch (adaptive codebook) gain.
 *            Result in Q14 (NOTE: 12.2 bit exact using Q12) 
 *
 *  DESCRIPTION:
 *      The adaptive codebook gain is given by
 *
 *              g = <x[], y[]> / <y[], y[]>
 *
 *      where x[] is the target vector, y[] is the filtered adaptive
 *      codevector, and <> denotes dot product.
 *      The gain is limited to the range [0,1.2] (=0..19661 Q14)
 *
 *************************************************************************/
Word16 G_pitch     (    /* o : Gain of pitch lag saturated to 1.2       */
    enum Mode mode,     /* i : AMR mode                                 */
    Word16 xn[],        /* i : Pitch target.                            */
    Word16 y1[],        /* i : Filtered adaptive codebook.              */
    Word16 g_coeff[],   /* i : Correlations need for gain quantization  */
    Word16 L_subfr      /* i : Length of subframe.                      */
)
{
    Word16 i;
    Word16 xy, yy, exp_xy, exp_yy, gain;
    Word32 s, s1, L_temp;

    Word16 scaled_y1;

    /* Compute scalar product <y1[],y1[]> */

    /* Q12 scaling / MR122 */
    s = 1L;                          /* Avoid case of all zeros */
    for (i = 0; i < L_subfr; i++)
    {
        s += y1[i] * y1[i] << 1;
        if (s < 0)
          break;
    }

    if (i == L_subfr) /* Test for overflow */
    {
        exp_yy = norm_l (s);
        yy = round (L_shl (s, exp_yy));
    }
    else
    {
      s = 0;
      for(i=0; i<L_subfr; i++)
      {
        /* divide "y1[]" by 4 to avoid overflow */
        scaled_y1 = y1[i] >> 2;
        s += scaled_y1 * scaled_y1;
      }
      s <<= 1;
      s++; /* Avoid case of all zeros */

      exp_yy = norm_l (s);
      yy = round (L_shl (s, exp_yy));
      exp_yy -= 4;
    }
        
    /* Compute scalar product <xn[],y1[]> */
    s = 1L;                          /* Avoid case of all zeros */
    for (i = 0; i < L_subfr; i++)
    {
      L_temp = xn[i] * y1[i];
      if (L_temp == 0x40000000)
        break;
      s1 = s;
      s = (L_temp << 1) + s1;

      if (((s1 ^ L_temp) > 0) && ((s ^ s1) < 0))
        break;
    }

    if (i == L_subfr) /* Test for overflow */
    {
        exp_xy = norm_l (s);
        xy = round (L_shl (s, exp_xy));
    }
    else
    {
      s = 0L;
      for (i = 0; i < L_subfr; i++)
        s += xn[i] * (y1[i] >> 2);

      s <<=1;
      s += 1;  /* Avoid case of all zeros */

      exp_xy = norm_l (s);
      xy = round (L_shl (s, exp_xy));
      exp_xy -= 2;
    }

    g_coeff[0] = yy;
    g_coeff[1] = 15 - exp_yy; /*sub (15, exp_yy);*/
    g_coeff[2] = xy;
    g_coeff[3] = 15 - exp_xy; /*sub (15, exp_xy);*/
    
    /* If (xy < 4) gain = 0 */
    if (xy < 4)
        return ((Word16) 0);

    /* compute gain = xy/yy */
    xy >>= 1;                 /* Be sure xy < yy */
    gain = div_s (xy, yy);

    i = exp_xy - exp_yy;      /* Denormalization of division */
    gain = shr (gain, i);
    
    /* if(gain >1.2) gain = 1.2 */
    if (gain > 19661)
      gain = 19661;

    if (mode == MR122)
    {
       /* clear 2 LSBits */
       gain = gain & 0xfffC;
    }
    
    return (gain);
}
