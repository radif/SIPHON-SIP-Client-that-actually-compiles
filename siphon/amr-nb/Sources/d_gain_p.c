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
*      File             : d_gain_p.c
*
********************************************************************************
*/
 
 
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d_gain_p.h"
const char d_gain_p_id[] = "@(#)$Id $" d_gain_p_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "mode.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"
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

/*
**************************************************************************
*
*  Function    : d_gain_pitch
*  Purpose     : Decodes the pitch gain using the received index.
*                output is in Q14 
*
**************************************************************************
*/
Word16 d_gain_pitch (      /* return value: gain (Q14)                */
    enum Mode mode,        /* i   : AMR mode                          */   
    Word16 index           /* i   : index of quantization             */
)
{
    Word16 gain;


    if (mode == MR122)
    {
       /* clear 2 LSBits */
      gain = qua_gain_pitch[index] & 0xfffc;
    }
    else
    {
       gain = qua_gain_pitch[index];
    }
    
    return gain;
}
