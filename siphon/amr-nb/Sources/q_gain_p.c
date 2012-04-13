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
*      File             : q_gain_p.c
*      Purpose          : Scalar quantization of the pitch gain
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "q_gain_p.h"
const char q_gain_p_id[] = "@(#)$Id $" q_gain_p_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "gains.tab"
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
Word16 q_gain_pitch (   /* Return index of quantization                      */
    enum Mode mode,     /* i  : AMR mode                                     */
    Word16 gp_limit,    /* i  : pitch gain limit                             */
    Word16 *gain,       /* i/o: Pitch gain (unquant/quant),              Q14 */
    Word16 gain_cand[], /* o  : pitch gain candidates (3),   MR795 only, Q14 */ 
    Word16 gain_cind[]  /* o  : pitch gain cand. indices (3),MR795 only, Q0  */ 
)
{
    Word16 i, index, err, err_min;

    err_min = abs_s (sub (*gain, qua_gain_pitch[0]));
    index = 0;

    for (i = 1; i < NB_QUA_PITCH; i++)
    {
      if (qua_gain_pitch[i] <= gp_limit)
        {
            err = abs_s (sub (*gain, qua_gain_pitch[i]));

            if (err < err_min)
            {
                err_min = err;
                index = i;
            }
        }
    }

    if (mode == MR795)
    {
        /* in MR795 mode, compute three gain_pit candidates around the index
         * found in the quantization loop: the index found and the two direct
         * neighbours, except for the extreme cases (i=0 or i=NB_QUA_PITCH-1),
         * where the direct neighbour and the neighbour to that is used.
         */
        Word16 ii;


        if (index == 0)
        {
            ii = index;
        }
        else
        {
          if (index == NB_QUA_PITCH-1 || qua_gain_pitch[index+1] > gp_limit)
          {
            ii = index - 2;
          }
          else
          {
            ii = index - 1;
          }
        }

        /* store candidate indices and values */
        for (i = 0; i < 3; i++)
        {
            gain_cind[i] = ii;
            gain_cand[i] = qua_gain_pitch[ii];
            ++ii;
        }
        
        *gain = qua_gain_pitch[index];
    }
    else
    {
        /* in MR122 mode, just return the index and gain pitch found.
         * If bitexactness is required, mask away the two LSBs (because
         * in the original EFR, gain_pit was scaled Q12)
         */
      if (mode == MR122)
       {
          /* clear 2 LSBits */
          *gain = qua_gain_pitch[index] & 0xFFFC;
       }
       else
       {
          *gain = qua_gain_pitch[index];
       }
    }
    return index;
}
