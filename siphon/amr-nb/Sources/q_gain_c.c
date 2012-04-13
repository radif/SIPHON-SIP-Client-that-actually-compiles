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
*      File             : q_gain_c.c
*      Purpose          : Scalar quantization of the innovative
*                       : codebook gain.
*
********************************************************************************
*/
 
 
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "q_gain_c.h"
const char q_gain_c_id[] = "@(#)$Id $" q_gain_c_h;
 
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
#include "log2.h"
#include "pow2.h"

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
 
/*--------------------------------------------------------------------------*
 * Function q_gain_code()                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                  *
 * Scalar quantization of the innovative codebook gain.                     *
 *                                                                          *
 *--------------------------------------------------------------------------*/
Word16 q_gain_code (        /* o  : quantization index,            Q0  */
    enum Mode mode,         /* i  : AMR mode                           */
    Word16 exp_gcode0,      /* i  : predicted CB gain (exponent),  Q0  */
    Word16 frac_gcode0,     /* i  : predicted CB gain (fraction),  Q15 */
    Word16 *gain,           /* i/o: quantized fixed codebook gain, Q1  */
    Word16 *qua_ener_MR122, /* o  : quantized energy error,        Q10 */
                            /*      (for MR122 MA predictor update)    */
    Word16 *qua_ener        /* o  : quantized energy error,        Q10 */
                            /*      (for other MA predictor update)    */
)
{
    const Word16 *p;
    Word16 i, index;
    Word16 gcode0, err, err_min;
    Word16 g_q0;


    g_q0 = 0;

    if (mode == MR122)
    {
      g_q0 = shr (*gain, 1); /* Q1 -> Q0 */
      gcode0 = extract_l (Pow2 (exp_gcode0, frac_gcode0));  /* predicted gain */
      gcode0 = shl (gcode0, 4);
      p = &qua_gain_code[0];
      err_min = abs_s (sub (g_q0, mult (gcode0, *p++)));
      p += 2;                                  /* skip quantized energy errors */
      index = 0;
      for (i = 1; i < NB_QUA_CODE; i++)
      {
        err = abs_s (sub (g_q0,  mult (gcode0, *p++)));

        p += 2;                              /* skip quantized energy error */

         if (err < err_min)
         {
            err_min = err;
            index = i;
         }
      }
      p = &qua_gain_code[add (add (index,index), index)];
      *gain = shl (mult (gcode0, *p++), 1);
    }
    else
    {
      /*-------------------------------------------------------------------*
       *  predicted codebook gain                                          *
       *  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
       *  gc0     = Pow2(int(d)+frac(d))                                   *
       *          = 2^exp + 2^frac                                         *
       *                                                                   *
       *-------------------------------------------------------------------*/
      gcode0 = extract_l (Pow2 (exp_gcode0, frac_gcode0));  /* predicted gain */
      gcode0 = shl (gcode0, 5);
      /*-------------------------------------------------------------------*
       *                   Search for best quantizer                        *
       *-------------------------------------------------------------------*/

      p = &qua_gain_code[0];
      err_min = abs_s (sub (*gain, mult (gcode0, *p++)));
      p += 2;                                  /* skip quantized energy errors */
      index = 0;
      for (i = 1; i < NB_QUA_CODE; i++)
      {
         err = abs_s (sub (*gain, mult (gcode0, *p++)));

         p += 2;                              /* skip quantized energy error */

         if (err < err_min)
         {
            err_min = err;
            index = i;
         }
      }
      p = &qua_gain_code[add (add (index,index), index)];
      *gain = mult (gcode0, *p++);
    }

    /* quantized error energies (for MA predictor update) */
    *qua_ener_MR122 = *p++;
    *qua_ener = *p;

    return index;
}
