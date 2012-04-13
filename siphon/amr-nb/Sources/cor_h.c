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
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
******************************************************************************
*
*      File             : cor_h.c
*      Purpose          : correlation functions for codebook search
*
*****************************************************************************
*/
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "cor_h.h"
const char cor_h_id[] = "@(#)$Id $" cor_h_h;
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "inv_sqrt.h"
#include "cnst.h" 
 
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  cor_h_x()
 *
 *  PURPOSE:  Computes correlation between target signal "x[]" and
 *            impulse response"h[]".
 *
 *  DESCRIPTION:
 *    The correlation is given by:
 *       d[n] = sum_{i=n}^{L-1} x[i] h[i-n]      n=0,...,L-1
 *
 *    d[n] is normalized such that the sum of 5 maxima of d[n] corresponding
 *    to each position track does not saturate.
 *
 *************************************************************************/
void cor_h_x (
    Word16 h[],    /* (i): impulse response of weighted synthesis filter */
    Word16 x[],    /* (i): target                                        */
    Word16 dn[],   /* (o): correlation between target and h[]            */
    Word16 sf      /* (i): scaling factor: 2 for 12.2, 1 for others      */
)
{
    cor_h_x2(h, x, dn, sf, NB_TRACK, STEP);
}

/*************************************************************************
 *
 *  FUNCTION:  cor_h_x2()
 *
 *  PURPOSE:  Computes correlation between target signal "x[]" and
 *            impulse response"h[]".
 *
 *  DESCRIPTION:
 *            See cor_h_x, d[n] can be normalized regards to sum of the
 *            five MR122 maxima or the four MR102 maxima.
 *
 *************************************************************************/
void cor_h_x2 (
    Word16 h[],    /* (i): impulse response of weighted synthesis filter */
    Word16 x[],    /* (i): target                                        */
    Word16 dn[],   /* (o): correlation between target and h[]            */
    Word16 sf,     /* (i): scaling factor: 2 for 12.2, 1 for others      */
    Word16 nb_track,/* (i): the number of ACB tracks                     */
    Word16 step    /* (i): step size from one pulse position to the next
                           in one track                                  */
)
{
    Word16 i, j, k;
    Word32 s, y32[L_CODE], max, tot;

    /* first keep the result on 32 bits and find absolute maximum */

    tot = 5;

    for (k = 0; k < nb_track; k++)
    {
        max = 0;
        for (i = k; i < L_CODE; i += step)
        {
            s = 0;
            for (j = i; j < L_CODE; j++)
              s += x[j] * h[j - i];
            
            /*s <<= 1;
            y32[i] = s;*/
            y32[i] = s << 1;
            
            s = L_abs (s);

            if (s > max)
                max = s;
        }
        /*tot += max >> 1;*/
        tot += max;
    }
    
    j = norm_l (tot) - sf;
    
    for (i = 0; i < L_CODE; i++)
    {
        dn[i] = round (L_shl (y32[i], j));
    }
}

/*************************************************************************
 *
 *  FUNCTION:  cor_h()
 *
 *  PURPOSE:  Computes correlations of h[] needed for the codebook search;
 *            and includes the sign information into the correlations.
 *
 *  DESCRIPTION: The correlations are given by
 *         rr[i][j] = sum_{n=i}^{L-1} h[n-i] h[n-j];   i>=j; i,j=0,...,L-1
 *
 *  and the sign information is included by
 *         rr[i][j] = rr[i][j]*sign[i]*sign[j]
 *
 *************************************************************************/

void cor_h (
    Word16 h[],         /* (i) : impulse response of weighted synthesis
                                 filter                                  */
    Word16 sign[],      /* (i) : sign of d[n]                            */
    Word16 rr[][L_CODE] /* (o) : matrix of autocorrelation               */
)
{
    Word16 i, j, k, dec, h2[L_CODE], tmp, tmp1;
    Word32 s, s1;

    /* Scaling for maximum precision */
    s = 1;
    for (i = 0; i < L_CODE; i++)
      s += (Word32)h[i] * h[i];
    s <<= 1;

    if (s & MIN_32)
    {
        for (i = 0; i < L_CODE; i++)
        {
          h2[i] = h[i] >> 1;
        }
    }
    else
    {
        s = L_shr (s, 1);
        k = extract_h (L_shl (Inv_sqrt (s), 7));
        k = mult (k, 32440);                     /* k = 0.99*k */
        
        for (i = 0; i < L_CODE; i++)
        {
            h2[i] = round (L_shl (L_mult (h[i], k), 9));
        }
    }
    
    /* build matrix rr[] */
    s1 = (Word32)h2[0] * h2[0];
    rr[L_CODE - 1][L_CODE - 1] = (Word16)((s1 + 0x4000) >> 15);
    for (dec = 1; dec < L_CODE; dec++)
    {
        s = 0;
        j = L_CODE - 1;
        i = j - dec;

        s1 += (Word32)h2[dec] * h2[dec];
        rr[i][i] = (Word16)((s1 + 0x4000) >> 15);
        for (k = 0; k < (L_CODE - dec); k++, i--, j--)
        {
            s += (Word32)h2[k] * h2[k + dec];
            tmp = (Word16)((s + 0x4000L) >> 15);
            tmp1 = ((Word32)sign[i] * sign[j]) >> 15;
            rr[j][i] = ((Word32)tmp * tmp1) >> 15;
            rr[i][j] = rr[j][i];
        }
    }
}
