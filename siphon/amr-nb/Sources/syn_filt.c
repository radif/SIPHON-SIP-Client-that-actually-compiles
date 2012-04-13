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
*      File             : syn_filt.c
*      Purpose          : Perform synthesis filtering through 1/A(z).
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "syn_filt.h"
const char syn_filt_id[] = "@(#)$Id $" syn_filt_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*
*--------------------------------------*
* Constants (defined in cnst.h         *
*--------------------------------------*
*  M         : LPC order               *
*--------------------------------------*
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
void Syn_filt (
    Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
    Word16 x[],     /* (i)     : input signal                             */
    Word16 y[],     /* (o)     : output signal                            */
    Word16 lg,      /* (i)     : size of filtering                        */
    Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
    Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
    Word16 i, j;
    Word32 s, t;
    Word16 tmp[80];   /* This is usually done by memory allocation (lg+M) */
    Word16 *yy;

    /* Copy mem[] to yy[] */

    yy = tmp;

    Copy(mem, yy, M);
    yy += M;

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
      s = x[i] * a[0];
      for (j = 1; j <= M; j++)
        s -= a[j] * yy[-j];

      t = s << 4;
      if (t >> 4 != s)
        *yy++ = s & MIN_32 ? MIN_16 : MAX_16;
      else
        *yy++ = (t + 0x8000) >> 16;
    }

    Copy(&tmp[M], y, lg);

    /* Update of memory if update==1 */

    if (update != 0)
      Copy(&y[lg-M], mem, M);
}

Flag Syn_filt_overflow (
    Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
    Word16 x[],     /* (i)     : input signal                             */
    Word16 y[],     /* (o)     : output signal                            */
    Word16 lg,      /* (i)     : size of filtering                        */
    Word16 mem[]   /* (i/o)   : memory associated with this filtering.   */
)
{
    Word16 i, j;
    Word32 s, t;
    Word16 tmp[80];   /* This is usually done by memory allocation (lg+M) */
    Word16 *yy;

    /* Copy mem[] to yy[] */

    yy = tmp;

    Copy(mem, yy, M);
    yy += M;

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
      s = x[i] * a[0];
      for (j = 1; j <= M; j++)
        s -= a[j] * yy[-j];

      t = s << 4;
      if (t >> 4 != s)
        return 1;
      else
        *yy++ = (t + 0x8000) >> 16;
    }

    Copy(&tmp[M], y, lg);

    return 0;
}
