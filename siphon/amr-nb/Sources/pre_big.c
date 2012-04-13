/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : pre_big.c
*      Purpose          : Big subframe (2 subframes) preprocessing
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "pre_big.h"
const char pre_big_id[] = "@(#)$Id $" pre_big_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "syn_filt.h"
#include "weight_a.h"
#include "residu.h"
#include "count.h"
#include "cnst.h"


/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
int pre_big(
    enum Mode mode,            /* i  : coder mode                             */
    const Word16 gamma1[],     /* i  : spectral exp. factor 1                 */
    const Word16 gamma1_12k2[],/* i  : spectral exp. factor 1 for EFR         */
    const Word16 gamma2[],     /* i  : spectral exp. factor 2                 */
    Word16 A_t[],              /* i  : A(z) unquantized, for 4 subframes, Q12 */
    Word16 frameOffset,        /* i  : Start position in speech vector,   Q0  */
    Word16 speech[],           /* i  : speech,                            Q0  */
    Word16 mem_w[],            /* i/o: synthesis filter memory state,     Q0  */
    Word16 wsp[]               /* o  : weighted speech                    Q0  */
)
{
   Word16 Ap1[MP1];            /* A(z) with spectral expansion         */
   Word16 Ap2[MP1];            /* A(z) with spectral expansion         */
   const Word16 *g1;           /* Pointer to correct gammma1 vector    */
   Word16 aOffset;
   Word16 i;
   

   if (sub (mode, MR795) <= 0 )
   {
       g1 = gamma1;
   }
   else
   {
       g1 = gamma1_12k2;
   }


   if (frameOffset > 0) {
      aOffset = 2*MP1;
   }
   else {
      aOffset = 0;
   }

   /* process two subframes (which form the "big" subframe) */
   for (i = 0; i < 2; i++)
   {
       Weight_Ai(&A_t[aOffset], g1, Ap1);
       Weight_Ai(&A_t[aOffset], gamma2, Ap2);
       Residu(Ap1, &speech[frameOffset], &wsp[frameOffset], L_SUBFR);
       
       Syn_filt(Ap2, &wsp[frameOffset], &wsp[frameOffset], L_SUBFR, mem_w, 1);
       aOffset = add (aOffset, MP1);
       frameOffset = add (frameOffset, L_SUBFR);
   }   

   return 0;
}
