/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : subframePostProc.c
*      Purpose          : Subframe post processing
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "spstproc.h"
const char spstproc_id[] = "@(#)$Id $" spstproc_h;

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
#include "cnst.h"

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
int subframePostProc(
    Word16 *speech,   /* i   : speech segment                        */
    enum Mode mode,   /* i   : coder mode                            */
    Word16 i_subfr,   /* i   : Subframe nr                           */
    Word16 gain_pit,  /* i   : Pitch gain                       Q14  */
    Word16 gain_code, /* i   : Decoded innovation gain               */
    Word16 *Aq,       /* i   : A(z) quantized for the 4 subframes    */
    Word16 synth[],   /* i   : Local snthesis                        */
    Word16 xn[],      /* i   : Target vector for pitch search        */
    Word16 code[],    /* i   : Fixed codebook exitation              */
    Word16 y1[],      /* i   : Filtered adaptive exitation           */
    Word16 y2[],      /* i   : Filtered fixed codebook excitation    */
    Word16 *mem_syn,  /* i/o : memory of synthesis filter            */
    Word16 *mem_err,  /* o   : pointer to error signal               */
    Word16 *mem_w0,   /* o   : memory of weighting filter            */
    Word16 *exc,      /* o   : long term prediction residual         */
    Word16 *sharp     /* o   : pitch sharpening value                */
)
{
   Word16 i, j, k;
   Word16 temp;
   Word32 L_temp;
   Word16 tempShift;
   Word16 kShift;
   Word16 pitch_fac;


   if (sub(mode, MR122) != 0)
   {
      tempShift = 1;
      kShift = 2;
      pitch_fac = gain_pit;
   }
   else
   {
      tempShift = 2;
      kShift = 4;
      pitch_fac = shr (gain_pit, 1);
   }
   
   /*------------------------------------------------------------*
    * - Update pitch sharpening "sharp" with quantized gain_pit  *
    *------------------------------------------------------------*/
   
   *sharp = gain_pit;

   if (sub(*sharp, SHARPMAX) > 0)
   {
      *sharp = SHARPMAX;
   }
   /*------------------------------------------------------*
    * - Find the total excitation                          *
    * - find synthesis speech corresponding to exc[]       *
    * - update filters memories for finding the target     *
    *   vector in the next subframe                        *
    *   (update error[-m..-1] and mem_w_err[])             *
    *------------------------------------------------------*/

   for (i = 0; i < L_SUBFR; i++) {
      /* exc[i] = gain_pit*exc[i] + gain_code*code[i]; */

      /*
       *                      12k2  others
       * ---------------------------------
       * exc                   Q0      Q0
       * gain_pit              Q14     Q14
       * pitch_fac             Q13     Q14
       *    product:           Q14     Q15
       *                               
       * code                  Q12     Q13
       * gain_code             Q1      Q1
       *    product            Q14     Q15
       *    sum                Q14     Q15
       *                               
       * tempShift             2       1
       *    sum<<tempShift     Q16     Q16
       * result -> exc         Q0      Q0
       */
      L_temp = L_mult (exc[i + i_subfr], pitch_fac);
      L_temp = L_mac (L_temp, code[i], gain_code);
      L_temp = L_shl (L_temp, tempShift);
      exc[i + i_subfr] = round (L_temp);
   }

   Syn_filt(Aq, &exc[i_subfr], &synth[i_subfr], L_SUBFR,
            mem_syn, 1);

   for (i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++) {
       mem_err[j] = sub(speech[i_subfr + i],
                        synth[i_subfr + i]);
      /*
       *                      12k2  others
       * ---------------------------------
       * y1                    Q0      Q0
       * gain_pit              Q14     Q14
       *    product            Q15     Q15
       *    shifted prod.      Q16     Q16
       * temp                  Q0      Q0
       *                                
       * y2                    Q10     Q12
       * gain_code             Q1      Q1
       *    product            Q12     Q14
       * kshift                 4       2
       *    shifted prod.      Q16     Q16
       * k                     Q0      Q0
       * mem_w0,xn,sum         Q0      Q0
       */
      temp = extract_h(L_shl(L_mult(y1[i], gain_pit), 1));
      k = extract_h(L_shl(L_mult(y2[i], gain_code), kShift));
      mem_w0[j] = sub(xn[i], add(temp, k));
   }

   return 0;
}
