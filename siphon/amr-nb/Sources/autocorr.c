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
*      File             : autocorr.c
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "autocorr.h"
const char autocorr_id[] = "@(#)$Id $" autocorr_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
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
/*
**************************************************************************
*
*  Function    : autocorr
*  Purpose     : Compute autocorrelations of signal with windowing
*
**************************************************************************
*/
Word16 Autocorr (
    Word16 x[],            /* (i)    : Input signal (L_WINDOW)            */
    Word16 m,              /* (i)    : LPC order                          */
    Word16 r_h[],          /* (o)    : Autocorrelations  (msb)            */
    Word16 r_l[],          /* (o)    : Autocorrelations  (lsb)            */
    const Word16 wind[]    /* (i)    : window for LPC analysis (L_WINDOW) */
)
{
    Word16 i, j, norm;
    Word16 y[L_WINDOW];
    Word32 sum;
    Word16 overfl_shft = 0;

    /* Windowing of signal */
    sum = 0;
    for(i=0; i<L_WINDOW; i++)
    {
      y[i] = (Word16)(((Word32)x[i] * (Word32)wind[i] + 0x4000) >> 15);
      sum += ((Word32)y[i] * (Word32)y[i]) << 1;
      if (sum < 0) /* overflow */
        break;
    }

    if (i != L_WINDOW) /* overflow */
    {
      for (; i<L_WINDOW; i++)
        y[i] = (Word16)(((Word32)x[i] * (Word32)wind[i] + 0x4000) >> 15);

      /* Compute r[0] and test for overflow */
      while (1)
      {
        overfl_shft += 4;

        /* If overflow divide y[] by 4 */
        sum = 0;
        for(i=0; i<L_WINDOW; i++)
        {
          y[i] >>= 2;
          sum += ((Word32)y[i] * (Word32)y[i]);
        }
        sum <<= 1;
        sum += 1; /* Avoid case of all zeros */
        if (sum > 0)
          break;
      }
    }
    else
      sum += 1; /* Avoid case of all zeros */

    /* Normalization of r[0] */
    norm = norm_l (sum);
    sum <<= norm;
    /* Put in DPF format (see oper_32b) */
    r_h[0] = (Word16)(sum >> 16);
    r_l[0] = (Word16)((sum >> 1) - ((Word32)r_h[0] << 15));

    /* r[1] to r[m] */
    for (i = 1; i <= m; i++)
    {
      sum = 0;
      for(j=0; j<L_WINDOW-i; j++)
        sum += (Word32)y[j] * (Word32)y[j+i];

      sum <<= norm + 1;
      r_h[i] = (Word16)(sum >> 16);
      r_l[i] = (Word16)((sum >> 1) - ((Word32)r_h[i] << 15));
    }

    norm -= overfl_shft;

    return norm;
}
