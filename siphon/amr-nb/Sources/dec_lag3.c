/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : dec_lag3.c
*      Purpose          : Decoding of fractional pitch lag with 1/3 resolution.
*                         Extract the integer and fraction parts of the pitch lag from
*                         the received adaptive codebook index.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "dec_lag3.h"
const char dec_lag3_id[] = "@(#)$Id $" dec_lag3_h;

 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *   FUNCTION:   Dec_lag3
 *
 *   PURPOSE:  Decoding of fractional pitch lag with 1/3 resolution.
 *             Extract the integer and fraction parts of the pitch lag from
 *             the received adaptive codebook index.
 *
 *    See "Enc_lag3.c" for more details about the encoding procedure.
 *
 *    The fractional lag in 1st and 3rd subframes is encoded with 8 bits
 *    while that in 2nd and 4th subframes is relatively encoded with 4, 5
 *    and 6 bits depending on the mode.
 *
 *************************************************************************/
void Dec_lag3(Word16 index,     /* i : received pitch index                 */
              Word16 t0_min,    /* i : minimum of search range              */
              Word16 t0_max,    /* i : maximum of search range              */
              Word16 i_subfr,   /* i : subframe flag                        */
              Word16 T0_prev,   /* i : integer pitch delay of last subframe
                                       used in 2nd and 4th subframes        */
              Word16 * T0,      /* o : integer part of pitch lag            */ 
              Word16 * T0_frac, /* o : fractional part of pitch lag         */
              Word16 flag4      /* i : flag for encoding with 4 bits        */
              )
{
    Word16 i;
    Word16 tmp_lag;
    
    if (i_subfr == 0) /* if 1st or 3rd subframe */
    {
	   if (index < 197)
	   {
	     *T0 = ((Word32)(index+2) * 10923) >> 15;
       *T0 += 19;

       i = *T0 + (*T0 << 1);
       *T0_frac = index - i + 58;
     }
	   else
	   {
	     *T0 = index - 112;
       *T0_frac = 0;
     }
    }
    else /* 2nd or 4th subframe */
    {
       if (flag4 == 0)
       {
          /* 'normal' decoding: either with 5 or 6 bit resolution */
          i = ((Word32)(index+2) * 10923) >> 15;
          i--;
          *T0 = t0_min + i;
          
          i += ( i << 1);
          *T0_frac = index - 2 -i;
       }
       else
       {
          /* decoding with 4 bit resolution */
          tmp_lag = T0_prev;

          if ( (tmp_lag - t0_min) > 5)
             tmp_lag = t0_min + 5;

          if ( (t0_max - tmp_lag) > 4)
             tmp_lag = t0_max - 4;
          

          if (index < 4)
          {
             *T0 = tmp_lag - 5 + index;
             *T0_frac = 0;
          }
          else
          {

             if (index < 12)
             {
                i = ((Word32)(index-5)*10923) >> 15;
                i--;
                *T0 = i + tmp_lag;
                
                i += (i << 1);
                *T0_frac = index - 9 - i;
             }
             else
             {
                *T0 = index - 11 + tmp_lag;
                *T0_frac = 0;
             }
          }
       } /* end if (decoding with 4 bit resolution) */
    }
}
