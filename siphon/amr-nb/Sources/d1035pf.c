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
*      File             : d1035pf.c
*      Purpose          : Builds the innovative codevector
*
********************************************************************************
*/
 
 
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d1035pf.h"
const char d1035pf_id[] = "@(#)$Id $" d1035pf_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "set_zero.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define NB_PULSE  10            /* number of pulses  */

#include "gray.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:   dec_10i40_35bits()
 *
 *  PURPOSE:  Builds the innovative codevector from the received
 *            index of algebraic codebook.
 *
 *   See  c1035pf.c  for more details about the algebraic codebook structure.
 *
 *************************************************************************/
void dec_10i40_35bits (
    Word16 index[],    /* (i)     : index of 10 pulses (sign+position)       */
    Word16 cod[]       /* (o)     : algebraic (fixed) codebook excitation    */
)
{
    Word16 i, j, pos1, pos2, sign, tmp;

    Set_zero(cod, L_CODE);

    /* decode the positions and signs of pulses and build the codeword */

    for (j = 0; j < NB_TRACK; j++)
    {
        /* compute index i */

        tmp = index[j];
        i = tmp & 7;
        i = dgray[i];

        i *= 5;
        pos1 = i + j; /* position of pulse "j" */

        i = (tmp >> 3) & 1;

        /*sign = (i ? -4096: 4096);*/
        sign = 4096 - i*8192;

        cod[pos1] = sign;

        /* compute index i */
        i = index[j+5] & 7;
        i = dgray[i];
        i *= 5;

        pos2 = i + j; /* position of pulse "j+5" */

        if (pos2 < pos1)
        {
            sign = - sign;
        }
        cod[pos2] += sign;
    }
}
