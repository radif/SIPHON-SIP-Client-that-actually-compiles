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
*      File             : lsfwt.c
*      Purpose          : Compute LSF weighting factors
*
********************************************************************************
*/
 
 
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lsfwt.h"
const char lsfwt_id[] = "@(#)$Id $" lsfwt_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "cnst.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/****************************************************
 *
 * FUNCTION  Lsf_wt                                                         *
 *                                                                          *
 ****************************************************
 * Compute LSF weighting factors                                            *
 *                                                                          *
 *  d[i] = lsf[i+1] - lsf[i-1]                                              *
 *                                                                          *
 *  The weighting factors are approximated by two line segment.             *
 *                                                                          *
 *  First segment passes by the following 2 points:                         *
 *                                                                          *
 *     d[i] = 0Hz     wf[i] = 3.347                                         *
 *     d[i] = 450Hz   wf[i] = 1.8                                           *
 *                                                                          *
 *  Second segment passes by the following 2 points:                        *
 *                                                                          *
 *     d[i] = 450Hz   wf[i] = 1.8                                           *
 *     d[i] = 1500Hz  wf[i] = 1.0                                           *
 *                                                                          *
 *  if( d[i] < 450Hz )                                                      *
 *    wf[i] = 3.347 - ( (3.347-1.8) / (450-0)) *  d[i]                      *
 *  else                                                                    *
 *    wf[i] = 1.8 - ( (1.8-1.0) / (1500-450)) *  (d[i] - 450)               *
 *                                                                          *
 *                                                                          *
 *  if( d[i] < 1843)                                                        *
 *    wf[i] = 3427 - (28160*d[i])>>15                                       *
 *  else                                                                    *
 *    wf[i] = 1843 - (6242*(d[i]-1843))>>15                                 *
 *                                                                          *
 *--------------------------------------------------------------------------*/

void Lsf_wt (
    Word16 *lsf,         /* input : LSF vector                  */
    Word16 *wf)          /* output: square of weighting factors */
{
#if 1
    Word16 temp;
    Word16 i;
    /* wf[0] = lsf[1] - 0  */
    wf[0] = lsf[1];
    for (i = 1; i < 9; i++)
    {
        wf[i] = sub (lsf[i + 1], lsf[i - 1]);
    }
    /* wf[9] = 0.5 - lsf[8] */    
    wf[9] = sub (16384, lsf[8]);

    for (i = 0; i < 10; i++)
    {
        temp = sub (wf[i], 1843);

        if (temp < 0)
        {
            wf[i] = sub (3427, mult (wf[i], 28160));
        }
        else
        {
            wf[i] = sub (1843, mult (temp, 6242));
        }

        wf[i] = shl (wf[i], 3);
    }
#else
    if (lsf[1] < 1843)
      wf[0] = sub (3427, mult (lsf[1], 28160));
    else
      wf[0] = sub (1843, mult (sub (lsf[1], 1843), 6242));
    wf[0] = shl (wf[0], 3);
    for (i = 1; i < 9; i++)
    {
      wf[i] = sub (lsf[i + 1], lsf[i - 1]);
      temp = sub (wf[i], 1843);

      if (temp < 0)
      {
          wf[i] = sub (3427, mult (wf[i], 28160));
      }
      else
      {
          wf[i] = sub (1843, mult (temp, 6242));
      }

      wf[i] = shl (wf[i], 3);
    }
    wf[9] = 16384 - lsf[8]; /*sub (16384, lsf[8]);*/
    if (wf[9] < 1843)
      wf[9] = sub (3427, mult (wf[9], 28160));
    else
      wf[9] = sub (1843, mult (sub (wf[9], 1843), 6242));
    wf[9] = shl (wf[9], 3);
    }
#endif
}
