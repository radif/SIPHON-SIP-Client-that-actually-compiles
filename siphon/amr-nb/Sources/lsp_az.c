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
*      File             : lsp_az.c
*      Purpose          : Converts from the line spectral pairs (LSP) to
*                       : LP coefficients, for a 10th order filter.
*      Description      : 
*                 - Find the coefficients of F1(z) and F2(z) (see Get_lsp_pol)
*                 - Multiply F1(z) by 1+z^{-1} and F2(z) by 1-z^{-1}
*                 - A(z) = ( F1(z) + F2(z) ) / 2
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lsp_az.h"
const char lsp_az_id[] = "@(#)$Id $" 
lsp_az_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/
static inline Word32 mull(Word32 a, Word16 b)
{
  register Word32 ra = a;
  register Word32 rb = b;
  Word32 lo, hi;

  __asm__("smull %0, %1, %2, %3     \n\t"
          "mov   %0, %0,     LSR #16 \n\t"
          "add   %1, %0, %1, LSL #16  \n\t"
          : "=&r"(lo), "=&r"(hi)
          : "r"(rb), "r"(ra));

  return hi;
}

/*************************************************************************
 *
 *  FUNCTION:  Get_lsp_pol
 *
 *  PURPOSE:  Find the polynomial F1(z) or F2(z) from the LSPs.
 *            If the LSP vector is passed at address 0  F1(z) is computed
 *            and if it is passed at address 1  F2(z) is computed.
 *
 *  DESCRIPTION:
 *       This is performed by expanding the product polynomials:
 *
 *           F1(z) =   product   ( 1 - 2 lsp[i] z^-1 + z^-2 )
 *                   i=0,2,4,6,8
 *           F2(z) =   product   ( 1 - 2 lsp[i] z^-1 + z^-2 )
 *                   i=1,3,5,7,9
 *
 *       where lsp[] is the LSP vector in the cosine domain.
 *
 *       The expansion is performed using the following recursion:
 *
 *            f[0] = 1
 *            b = -2.0 * lsp[0]
 *            f[1] = b
 *            for i=2 to 5 do
 *               b = -2.0 * lsp[2*i-2];
 *               f[i] = b*f[i-1] + 2.0*f[i-2];
 *               for j=i-1 down to 2 do
 *                   f[j] = f[j] + b*f[j-1] + f[j-2];
 *               f[1] = f[1] + b;
 *
 *************************************************************************/

static void Get_lsp_pol (Word16 *lsp, Word32 *f)
{
#if 0
    Word16 i, j, hi, lo;
    Word32 t0;
    
    /* f[0] = 1.0;             */
    *f = L_mult (4096, 2048);
    f++;
    *f = L_msu ((Word32) 0, *lsp, 512);    /* f[1] =  -2.0 * lsp[0];  */
    f++;
    lsp += 2;                              /* Advance lsp pointer     */

    for (i = 2; i <= 5; i++)
    {
        *f = f[-2];

        for (j = 1; j < i; j++, f--)
        {
            L_Extract (f[-1], &hi, &lo);
            t0 = Mpy_32_16 (hi, lo, *lsp); /* t0 = f[-1] * lsp    */
            t0 = L_shl (t0, 1);
            *f = L_add (*f, f[-2]);         /* *f += f[-2]      */
            *f = L_sub (*f, t0); /* *f -= t0            */
        }
        *f = L_msu (*f, *lsp, 512);         /* *f -= lsp<<9     */
        f += i;                            /* Advance f pointer   */
        lsp += 2;                          /* Advance lsp pointer */
    }
#else
    Word16 i,j;

     /* All computation in Q24 */
     *f = 0x01000000;           /* f[0] = 1.0;             in Q24  */
     f++;
     *f = -*lsp << 10;         /* f[1] =  -2.0 * lsp[0];  in Q24  */

     f++;
     lsp += 2;                            /* Advance lsp pointer             */

     for(i=2; i<=5; i++)
     {
       *f = f[-2];

       for(j=1; j<i; j++, f--)
         *f += f[-2] - (mull(f[-1], *lsp) << 2);

       *f -= *lsp << 10;                       /* *f -= lsp<<9        */
       f   += i;                               /* Advance f pointer   */
       lsp += 2;                               /* Advance lsp pointer */
     }
#endif
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:   Lsp_Az
 *
 *  PURPOSE:  Converts from the line spectral pairs (LSP) to
 *            LP coefficients, for a 10th order filter.
 *
 *  DESCRIPTION:
 *     - Find the coefficients of F1(z) and F2(z) (see Get_lsp_pol)
 *     - Multiply F1(z) by 1+z^{-1} and F2(z) by 1-z^{-1}
 *     - A(z) = ( F1(z) + F2(z) ) / 2
 *
 *************************************************************************/
void Lsp_Az (
    Word16 lsp[],        /* (i)  : line spectral frequencies            */
    Word16 a[]           /* (o)  : predictor coefficients (order = 10)  */
)
{
#if 0
    Word16 i, j;
    Word32 f1[6], f2[6];
    Word32 t0;

    Get_lsp_pol (&lsp[0], f1);
    Get_lsp_pol (&lsp[1], f2);

    for (i = 5; i > 0; i--)
    {
        f1[i] = L_add (f1[i], f1[i - 1]);     /* f1[i] += f1[i-1]; */
        f2[i] = L_sub (f2[i], f2[i - 1]);     /* f2[i] -= f2[i-1]; */
    }

    a[0] = 4096;
    for (i = 1, j = 10; i <= 5; i++, j--)
    {
        t0 = L_add (f1[i], f2[i]);           /* f1[i] + f2[i] */
        a[i] = extract_l (L_shr_r (t0, 13));
        t0 = L_sub (f1[i], f2[i]);           /* f1[i] - f2[i] */
        a[j] = extract_l (L_shr_r (t0, 13));
    }
#else
    Word16 i;
    Word32 f1[6], f2[6];
    Word32 ff1, ff2, fff1, fff2;

    Get_lsp_pol(&lsp[0],f1);
    Get_lsp_pol(&lsp[1],f2);

    a[0] = 4096;
    for (i = 1; i <= 5; i++)
    {
      ff1 = f1[i] + f1[i-1];
      ff2 = f2[i] - f2[i-1];

      fff1 = ff1 + ff2 + ((Word32) 1 << 12);
      fff2 = ff1 - ff2 + ((Word32) 1 << 12);

      a[i]    = (Word16)(fff1 >> 13);
      a[11-i] = (Word16)(fff2 >> 13);
    }
#endif
}

