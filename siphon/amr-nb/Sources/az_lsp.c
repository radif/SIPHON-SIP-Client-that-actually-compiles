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
*      File             : az_lsp.c
*      Purpose          : Compute the LSPs from the LP coefficients
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "az_lsp.h"
const char az_lsp_id[] = "@(#)$Id $" az_lsp_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "copy.h"
#include "cnst.h"
 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "grid.tab"
#define NC   M/2                  /* M = LPC order, NC = M/2 */

/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Chebps
*  Purpose     : Evaluates the Chebyshev polynomial series
*  Description : - The polynomial order is   n = m/2 = 5
*                - The polynomial F(z) (F1(z) or F2(z)) is given by
*                   F(w) = 2 exp(-j5w) C(x)
*                  where
*                   C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2
*                  and T_m(x) = cos(mw) is the mth order Chebyshev
*                  polynomial ( x=cos(w) )
*  Returns     : C(x) for the input x.
*
**************************************************************************
*/
static Word16 Chebps (Word16 x,
                      Word16 f[], /* (n) */
                      Word16 n)
{
    Word16 i, cheb;
    Word16 b1_h, b1_l;
    Word32 t0;
    Word32 L_temp;

   /* Note: All computation are done in Q24. */

    L_temp = 0x01000000;

    /* 2*x in Q24 + f[1] in Q24 */
    t0 = ((Word32)x << 10) + ((Word32)f[1] << 14);

    /* b1 = 2*x + f[1]     */
    b1_h = (Word16)(t0 >> 16);
    b1_l = (Word16)((t0 >> 1) - (b1_h << 15));

    for (i = 2; i<n; i++)
    {
      /* t0 = 2.0*x*b1              */
      t0  = ((Word32) b1_h * x) + (((Word32) b1_l * x) >> 15);
      t0 <<= 2;
      /* t0 = 2.0*x*b1 - b2         */
      t0 -= L_temp;
      /* t0 = 2.0*x*b1 - b2 + f[i]; */
      t0 += ((Word32)f[i] << 14);

      /* b2 = b1; */
      L_temp = ((Word32) b1_h << 16) + ((Word32) b1_l << 1);

      /* b0 = 2.0*x*b1 - b2 + f[i]; */
      b1_h = (Word16)(t0 >> 16);
      b1_l = (Word16)((t0 >> 1) - (b1_h << 15));
    }

    /* t0 = x*b1;              */
    t0  = ((Word32) b1_h * x) + (((Word32) b1_l * x) >> 15);
    t0 <<= 1;
    /* t0 = x*b1 - b2          */
    t0 -= L_temp;
    /* t0 = x*b1 - b2 + f[i]/2 */
    t0 += ((Word32)f[i] << 13);

    /* Q24 to Q30 with saturation */
    /* Result in Q14              */
    if ((UWord32)(t0 - 0xfe000000L) < 0x01ffffffL -  0xfe000000L)
      cheb = (Word16)(t0 >> 10);
    else
      cheb = t0 > (Word32) 0x01ffffffL ? MAX_16 : MIN_16;

    return(cheb);
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Az_lsp
*  Purpose     : Compute the LSPs from  the LP coefficients
*
**************************************************************************
*/
void Az_lsp (
    Word16 a[],         /* (i)  : predictor coefficients (MP1)               */
    Word16 lsp[],       /* (o)  : line spectral pairs (M)                    */
    Word16 old_lsp[]    /* (i)  : old lsp[] (in case not found 10 roots) (M) */
)
{
    Word16 i, j, nf, ip;
    Word16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
    Word16 x, y, sign, exp;
    Word16 *coef;
    Word16 f1[M / 2 + 1], f2[M / 2 + 1];
    Word32 L_temp1, L_temp2;

    /*-------------------------------------------------------------*
     *  find the sum and diff. pol. F1(z) and F2(z)                *
     *    F1(z) <--- F1(z)/(1+z**-1) & F2(z) <--- F2(z)/(1-z**-1)  *
     *                                                             *
     * f1[0] = 1.0;                                                *
     * f2[0] = 1.0;                                                *
     *                                                             *
     * for (i = 0; i< NC; i++)                                     *
     * {                                                           *
     *   f1[i+1] = a[i+1] + a[M-i] - f1[i] ;                       *
     *   f2[i+1] = a[i+1] - a[M-i] + f2[i] ;                       *
     * }                                                           *
     *-------------------------------------------------------------*/

    f1[0] = 1024;                   /* f1[0] = 1.0 */
    f2[0] = 1024;                   /* f2[0] = 1.0 */

    for (i = 0; i < NC; i++)
    {
        L_temp1 = (Word32)a[i+1];
        L_temp2 = (Word32)a[M-i];

        /* x = (a[i+1] + a[M-i]) >> 2        */
        x = ((L_temp1 + L_temp2) >> 2);
        /* x = (a[i+1] - a[M-i]) >> 2        */
        y = ((L_temp1 - L_temp2) >> 2);

        /* f1[i+1] = a[i+1] + a[M-i] - f1[i] */
        f1[i+1] = (Word32)x - (Word32)f1[i];
        /* f2[i+1] = a[i+1] - a[M-i] + f2[i] */
        f2[i+1] = (Word32)y + (Word32)f2[i];
    }

    /*-------------------------------------------------------------*
     * find the LSPs using the Chebychev pol. evaluation           *
     *-------------------------------------------------------------*/

    nf = 0;                         /* number of found frequencies */
    ip = 0;                         /* indicator for f1 or f2      */

    coef = f1;

    xlow = grid[0];
    ylow = Chebps (xlow, coef, NC);

    j = 0;

    while ( (nf < M) && (j < grid_points) )
    {
        j++;
        xhigh = xlow;
        yhigh = ylow;
        xlow = grid[j];
        ylow = Chebps (xlow, coef, NC);

        if (((Word32)ylow*yhigh) <= (Word32) 0L)
        {
            /* divide 4 times the interval */
            for (i = 0; i < 4; i++)
            {
                /* xmid = (xlow + xhigh)/2 */
                xmid = (xlow >> 1) + (xhigh >> 1);
                ymid = Chebps (xmid, coef, NC);

                if ( ((Word32)ylow*ymid) <= (Word32)0L)
                {
                    yhigh = ymid;
                    xhigh = xmid;
                }
                else
                {
                    ylow = ymid;
                    xlow = xmid;
                }
            }

            /*-------------------------------------------------------------*
             * Linear interpolation                                        *
             *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
             *-------------------------------------------------------------*/
            x   = xhigh - xlow;
            y   = yhigh - ylow;

            if (y == 0)
            {
                xint = xlow;
            }
            else
            {
                sign= y;
                y   = abs_s(y);
                exp = norm_s(y);
                y <<= exp;
                y   = div_s( (Word16)16383, y);
                /* y= (xhigh-xlow)/(yhigh-ylow) in Q11 */
                y = ((Word32)x * (Word32)y) >> (19 - exp);

                if(sign < 0) y = -y;

                /* xint = xlow - ylow*y */
                xint = xlow - (Word16)(((Word32) ylow * y) >> 10);
            }

            lsp[nf] = xint;
            xlow = xint;
            nf++;

            if (ip == 0)
            {
                ip = 1;
                coef = f2;
            }
            else
            {
                ip = 0;
                coef = f1;
            }
            ylow = Chebps (xlow, coef, NC);
        }
    }

    /* Check if M roots found */
    if (nf < M)
      Copy(old_lsp, lsp, M);
}

