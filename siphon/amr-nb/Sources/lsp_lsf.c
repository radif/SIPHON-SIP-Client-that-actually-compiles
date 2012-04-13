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
*      File             : lsp_lsf.c
*      Purpose          : Lsp_lsf:   Transformation lsp to lsf
*                       : Lsf_lsp:   Transformation lsf to lsp
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lsp_lsf.h"
const char lsp_lsf_id[] = "@(#)$Id $" lsp_lsf_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "lsp_lsf.tab"          /* Look-up table for transformations */
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *   FUNCTIONS:  Lsp_lsf and Lsf_lsp
 *
 *   PURPOSE:
 *      Lsp_lsf:   Transformation lsp to lsf
 *      Lsf_lsp:   Transformation lsf to lsp
 *
 *   DESCRIPTION:
 *         lsp[i] = cos(2*pi*lsf[i]) and lsf[i] = arccos(lsp[i])/(2*pi)
 *
 *   The transformation from lsp[i] to lsf[i] and lsf[i] to lsp[i] are
 *   approximated by a look-up table and interpolation.
 *
 *************************************************************************/
void Lsf_lsp (
    Word16 lsf[],       /* (i) : lsf[m] normalized (range: 0.0<=val<=0.5) */
    Word16 lsp[],       /* (o) : lsp[m] (range: -1<=val<1)                */
    Word16 m            /* (i) : LPC order                                */
)
{
    Word16 i, ind, offset;
    Word32 L_tmp;

    for (i = 0; i < m; i++)
    {
        ind = shr (lsf[i], 8);      /* ind    = b8-b15 of lsf[i] */
        offset = lsf[i] & 0x00ff;    /* offset = b0-b7  of lsf[i] */

        /* lsp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 256 */

        L_tmp = L_mult (sub (table[ind + 1], table[ind]), offset);
        lsp[i] = add (table[ind], extract_l (L_shr (L_tmp, 9)));

    }
    return;
}

void Lsp_lsf (
    Word16 lsp[],       /* (i)  : lsp[m] (range: -1<=val<1)                */
    Word16 lsf[],       /* (o)  : lsf[m] normalized (range: 0.0<=val<=0.5) */
    Word16 m            /* (i)  : LPC order                                */
)
{
    Word16 i, ind;
    Word32 L_tmp;

    ind = 63;                        /* begin at end of table -1 */

    for (i = m - 1; i >= 0; i--)
    {
        /* find value in table that is just greater than lsp[i] */

        while (sub (table[ind], lsp[i]) < 0)
        {
            ind--;

        }

        /* acos(lsp[i])= ind*256 + ( ( lsp[i]-table[ind] ) *
           slope[ind] )/4096 */

        L_tmp = L_mult (sub (lsp[i], table[ind]), slope[ind]);
        /*(lsp[i]-table[ind])*slope[ind])>>12*/
        lsf[i] = round (L_shl (L_tmp, 3));
        lsf[i] = add (lsf[i], shl (ind, 8));
    }
    return;
}
