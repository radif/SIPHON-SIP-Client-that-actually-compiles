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
*      File             : calc_cor.c
*      Purpose          : Calculate all correlations for prior the OL LTP 
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "calc_cor.h"
const char calc_cor_id[] = "@(#)$Id $" calc_cor_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION: comp_corr 
 *
 *  PURPOSE: Calculate all correlations of scal_sig[] in a given delay
 *           range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs the all correlations 
 *
 *************************************************************************/
void comp_corr ( 
    Word16 scal_sig[],  /* i   : scaled signal.                          */
    Word16 L_frame,     /* i   : length of frame to compute pitch        */
    Word16 lag_max,     /* i   : maximum lag                             */
    Word16 lag_min,     /* i   : minimum lag                             */
    Word32 corr[])      /* o   : correlation of selected lag             */
{
    Word16 i, j;
    Word16 *p, *p1;
    Word32 t0;
    
    for (i = lag_max; i >= lag_min; i--)
    {
       p = scal_sig;
       p1 = &scal_sig[-i];
       t0 = 0;
       
       for (j = 0; j < L_frame; j++)
       {
         t0 += *(p++) * *(p1++);
       }
       corr[-i] = t0 << 1;
    }
}
