/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : b_cn_cod.c
*      Purpose          : Contains function for comfort noise generation.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "b_cn_cod.h"
const char b_cn_cod_id[] = "@(#)$Id $" b_cn_cod_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"
#include "cnst.h"

#include <stdio.h>
#include <stdlib.h>

#include "window.tab"
/*
********************************************************************************
*                         LOCAL CONSTANTS
********************************************************************************
*/
#define  NB_PULSE 10 /* number of random pulses in DTX operation   */

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *   FUNCTION NAME: pseudonoise
 *
 *************************************************************************/
Word16 pseudonoise (
    Word32 *shift_reg, /* i/o : Old CN generator shift register state */
    Word16 no_bits     /* i   : Number of bits                        */
)
{
   Word16 noise_bits, Sn, i;
   
   noise_bits = 0;
   for (i = 0; i < no_bits; i++)
   {
      /* State n == 31 */
      if ((*shift_reg & 0x00000001L) != 0)
      {
         Sn = 1;
      }
      else
      {
         Sn = 0;
      }
      
      /* State n == 3 */
      if ((*shift_reg & 0x10000000L) != 0)
      {
         Sn = Sn ^ 1;
      }
      else
      {
         Sn = Sn ^ 0;
      }
      
      noise_bits = shl (noise_bits, 1);
      noise_bits = noise_bits | (extract_l (*shift_reg) & 1);

      
      *shift_reg = L_shr (*shift_reg, 1);         

      if (Sn & 1)
      {
         *shift_reg = *shift_reg | 0x40000000L;
      }
   }
   return noise_bits;
}

/***************************************************************************
*
*  Function    : build_CN_code
*
***************************************************************************/ 
void build_CN_code (
    Word32 *seed,         /* i/o : Old CN generator shift register state */
    Word16 cod[]          /* o   : Generated CN fixed codebook vector    */
)
{
   Word16 i, j, k;
   
   for (i = 0; i < L_SUBFR; i++)
   {
      cod[i] = 0;
   }
   
   for (k = 0; k < NB_PULSE; k++)
   {
      i = pseudonoise (seed, 2);      /* generate pulse position */
      i = shr (extract_l (L_mult (i, 10)), 1);
      i = add (i, k);
      
      j = pseudonoise (seed, 1);      /* generate sign           */


      if (j > 0)
      {
         cod[i] = 4096;
      }
      else
      {
         cod[i] = -4096;
      }
   }
   
   return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: build_CN_param
 *
 *************************************************************************/
void build_CN_param (
    Word16 *seed,             /* i/o : Old CN generator shift register state */
    const Word16 n_param,           /* i  : number of params */  
    const Word16 param_size_table[],/* i : size of params */   
    Word16 parm[]             /* o : CN Generated params */
    )
{
   Word16 i;
   const Word16 *p;

   *seed = extract_l(L_add(L_shr(L_mult(*seed, 31821), 1), 13849L));

   p = &window_200_40[*seed & 0x7F];
   for(i=0; i< n_param;i++){

     parm[i] = *p++ & ~(0xFFFF<<param_size_table[i]);  
   }
}
