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
*      File             : lag_wind.c
*      Purpose          : Lag windowing of autocorrelations.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lag_wind.h"
const char lag_wind_id[] = "@(#)$Id $" lag_wind_h;
 
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
#include "lag_wind.tab"     /* Table for Lag_Window() */
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  Lag_window()
 *
 *  PURPOSE:  Lag windowing of autocorrelations.
 *
 *  DESCRIPTION:
 *         r[i] = r[i]*lag_wind[i],   i=1,...,10
 *
 *     r[i] and lag_wind[i] are in special double precision format.
 *     See "oper_32b.c" for the format.
 *
 *************************************************************************/
void Lag_window (
    Word16 m,           /* (i)     : LPC order                        */
    Word16 r_h[],       /* (i/o)   : Autocorrelations  (msb)          */
    Word16 r_l[]        /* (i/o)   : Autocorrelations  (lsb)          */
)
{
    Word16 i;
    Word32 x;

    for (i = 1; i <= m; i++)
    {
        x  = Mpy_32(r_h[i], r_l[i], lag_h[i-1], lag_l[i-1]);
        /*L_Extract(x, &r_h[i], &r_l[i]);*/
        r_h[i] = (Word16) (x >> 16);
        r_l[i] = (Word16)((x >> 1) - (r_h[i] << 15));
    }
}
