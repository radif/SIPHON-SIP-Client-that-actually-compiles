/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : ex_ctrl.c
*      Purpose          : Excitation Control module in background noise
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "ex_ctrl.h"
const char ex_ctrl_id[] = "@(#)$Id $" ex_ctrl_h;

#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "cnst.h"
#include "copy.h"
#include "set_zero.h"
#include "gmed_n.h"
#include "sqrt_l.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*-----------------------------------------------------------------*
 *   Decoder constant parameters (defined in "cnst.h")             *
 *-----------------------------------------------------------------*
 *   L_FRAME       : Frame size.                                   *
 *   L_SUBFR       : Sub-frame size.                               *
 *-----------------------------------------------------------------*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*
**************************************************************************
*
*  Function    : Ex_ctrl
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = no bgn, 1 = bgn
*
**************************************************************************
*/
Word16 Ex_ctrl (Word16 excitation[],   /*i/o: Current subframe excitation   */
                Word16 excEnergy,      /* i : Exc. Energy, sqrt(totEx*totEx)*/
                Word16 exEnergyHist[], /* i : History of subframe energies  */
                Word16 voicedHangover, /* i : # of fr. after last voiced fr.*/
                Word16 prevBFI,        /* i : Set i previous BFI            */
                Word16 carefulFlag     /* i : Restrict dymamic in scaling   */
                )
{
   Word16 i, exp;
   Word16 testEnergy, scaleFactor, avgEnergy, prevEnergy;
   Word32 t0;

   /* get target level */
   avgEnergy = gmed_n(exEnergyHist, 9);

   prevEnergy = shr( add (exEnergyHist[7], exEnergyHist[8]) ,1);


   if ( sub (exEnergyHist[8], prevEnergy) < 0)
   {
      prevEnergy = exEnergyHist[8];
   }

   /* upscaling to avoid too rapid energy rises  for some cases */

   if ( sub (excEnergy, avgEnergy) < 0 && sub (excEnergy, 5) > 0)
   {
      testEnergy = shl(prevEnergy, 2);  /* testEnergy = 4*prevEnergy; */


      if ( sub (voicedHangover, 7) < 0 || prevBFI != 0 )
      {
         /* testEnergy = 3*prevEnergy */
         testEnergy = sub (testEnergy, prevEnergy);  
      }


      if ( sub (avgEnergy, testEnergy) > 0)
      {
         avgEnergy = testEnergy;
      }

      /* scaleFactor=avgEnergy/excEnergy in Q0 (const 29 below)*/
      exp = norm_s (excEnergy);
      excEnergy = shl (excEnergy, exp);
      excEnergy = div_s ((Word16) 16383, excEnergy);
      t0 = L_mult (avgEnergy, excEnergy);
      t0 = L_shr (t0, sub (20, exp));  /* const=30 for t0 in Q0, 20 for Q10 */
      if ( L_sub(t0, 32767) > 0 )
      {
         t0 = 32767;    /* saturate  */
      }
      scaleFactor = extract_l (t0); 

      /* test if scaleFactor > 3.0 */

      if ( carefulFlag != 0 && sub(scaleFactor, 3072) > 0 ) 
      {
         scaleFactor = 3072;
      }

      /* scale the excitation by scaleFactor */
      for (i = 0; i < L_SUBFR; i++)
      {
         t0 = L_mult (scaleFactor, excitation[i]);
         t0 = L_shr (t0, 11);
         excitation[i] = extract_l (t0);
      } 
   }

   return 0;
}
