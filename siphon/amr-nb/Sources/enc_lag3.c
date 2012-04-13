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
*      File             : enc_lag3.c
*      Purpose          : Encoding of fractional pitch lag with 1/3 resolution.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "enc_lag3.h"
const char enc_lag3_id[] = "@(#)$Id $" enc_lag3_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *   FUNCTION:  Enc_lag3
 *
 *   PURPOSE:  Encoding of fractional pitch lag with 1/3 resolution.
 *
 *   DESCRIPTION:
 *                    First and third subframes:
 *                    --------------------------
 *   The pitch range is divided as follows:
 *           19 1/3  to   84 2/3   resolution 1/3
 *           85      to   143      resolution 1
 *
 *   The period is encoded with 8 bits.
 *   For the range with fractions:
 *     index = (T-19)*3 + frac - 1;
 *                         where T=[19..85] and frac=[-1,0,1]
 *   and for the integer only range
 *     index = (T - 85) + 197;        where T=[86..143]
 *
 *                    Second and fourth subframes:
 *                    ----------------------------
 *   For the 2nd and 4th subframes a resolution of 1/3 is always used,
 *   and the search range is relative to the lag in previous subframe.
 *   If t0 is the lag in the previous subframe then
 *   t_min=t0-5   and  t_max=t0+4   and  the range is given by
 *        t_min - 2/3   to  t_max + 2/3
 *
 *   The period in the 2nd (and 4th) subframe is encoded with 5 bits:
 *     index = (T-(t_min-1))*3 + frac - 1;
 *                 where T=[t_min-1..t_max+1]
 *
 *************************************************************************/
Word16
Enc_lag3(                /* o  : Return index of encoding             */
    Word16 T0,           /* i  : Pitch delay                          */
    Word16 T0_frac,      /* i  : Fractional pitch delay               */
    Word16 T0_prev,      /* i  : Integer pitch delay of last subframe */
    Word16 T0_min,       /* i  : minimum of search range              */
    Word16 T0_max,       /* i  : maximum of search range              */
    Word16 delta_flag,   /* i  : Flag for 1st (or 3rd) subframe       */
    Word16 flag4         /* i  : Flag for encoding with 4 bits        */
    )
{
   Word16 index, i, tmp_ind, uplag;
   Word16 tmp_lag;
   

   if (delta_flag == 0)
   {  /* if 1st or 3rd subframe */

      /* encode pitch delay (with fraction) */
      

      if (T0 <= 85)
      {
         index = T0*3 - 58 + T0_frac;
      }
      else
      {
         index = T0 + 112;
      }
   }
   else
   {   /* if second or fourth subframe */

      if (flag4 == 0) {
         
         /* 'normal' encoding: either with 5 or 6 bit resolution */
         index = 3*(T0 - T0_min) + 2 + T0_frac;
      }
      else {
         /* encoding with 4 bit resolution */
         tmp_lag = T0_prev;

         if ( (tmp_lag - T0_min) > 5)
            tmp_lag = T0_min + 5;

         if ( (T0_max - tmp_lag) > 4)
            tmp_lag = T0_max - 4;
         
         uplag = 3*T0 + T0_frac;
         
         tmp_ind = 3*(tmp_lag - 2);
         

         if (tmp_ind >= uplag) {
            index = add (sub (T0, tmp_lag), 5);
         } 
         else {
            i = 3*(tmp_lag + 1);
            
            if (i > uplag) {
                index = uplag - tmp_ind + 3;
            }
            else {
               index = T0 - tmp_lag + 11;
            }
         }
         
      } /* end if (encoding with 4 bit resolution) */
   }   /* end if (second of fourth subframe) */
   
   return index;
}
