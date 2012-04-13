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
*      File             : d4_17pf.c
*      Purpose          : Algebraic codebook decoder
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d4_17pf.h"
const char d4_17pf_c_id[] = "@(#)$Id $" d4_17pf_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "set_zero.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define NB_PULSE 4

#include "gray.tab"
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  decod_ACELP()
 *
 *  PURPOSE:   Algebraic codebook decoder
 *
 *************************************************************************/

void decode_4i40_17bits(
    Word16 sign,   /* i : signs of 4 pulses.                       */
    Word16 index,  /* i : Positions of the 4 pulses.               */
    Word16 cod[]   /* o : algebraic (fixed) codebook excitation    */
)
{
    Word16 i, j;
    Word16 pos[NB_PULSE];

    /* Decode the positions */

    i = index & 7;
    i = dgray[i];

    pos[0] = i*5;

    index >>= 3;
    i = index & 7;
    i = dgray[i];

    pos[1] = i*5+1;

    index >>= 3;
    i = index & 7;
    i = dgray[i];

    pos[2] = i*5+2;

    index >>= 3;
    j = index & 1;
    index >>= 1;
    i = index & 7;
    i = dgray[i];

    pos[3] =i*5+3+j;

    /* decode the signs  and build the codeword */
    Set_zero(cod, L_SUBFR);

    for (j = 0; j < NB_PULSE; j++) {
        i = sign & 1;
        cod[pos[j]] = i*16383 - 8192;
        /*cod[pos[j]] = (i ? 8191 : -8192);*/

        sign >>= 1;
    }
}
