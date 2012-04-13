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
*      File             : ph_disp.c
*      Purpose          : Perform adaptive phase dispersion of the excitation
*                         signal.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "ph_disp.h"
const char ph_disp_id[] = "@(#)$Id $" ph_disp_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

#include "ph_disp.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   ph_disp_init
*
**************************************************************************
*/
int ph_disp_init (ph_dispState *state)
{
  if (state == (ph_dispState *) NULL){
      fprintf(stderr, "ph_disp_init: invalid parameter\n");
      return -1;
  }

  ph_disp_reset(state);

  return 0;

}

/*************************************************************************
*
*  Function:   ph_disp_reset
*
**************************************************************************
*/
int ph_disp_reset (ph_dispState *state)
{
  Word16 i;

   if (state == (ph_dispState *) NULL){
      fprintf(stderr, "ph_disp_reset: invalid parameter\n");
      return -1;
   }
   for (i=0; i<PHDGAINMEMSIZE; i++)
   {
       state->gainMem[i] = 0;
   }
   state->prevState = 0;
   state->prevCbGain = 0;
   state->lockFull = 0;
   state->onset = 0;          /* assume no onset in start */ 

   return 0;
}

/*************************************************************************
*
*  Function:   ph_disp_lock
*
**************************************************************************
*/
void ph_disp_lock (ph_dispState *state)
{
  state->lockFull = 1;
  return;
}

/*************************************************************************
*
*  Function:   ph_disp_release
*
**************************************************************************
*/
void ph_disp_release (ph_dispState *state)
{
  state->lockFull = 0;
  return;
}


/*************************************************************************
*
*  Function:   ph_disp
*
*              Adaptive phase dispersion; forming of total excitation
*              (for synthesis part of decoder)
*
**************************************************************************
*/
void ph_disp (
      ph_dispState *state, /* i/o     : State struct                     */
      enum Mode mode,      /* i       : codec mode                       */
      Word16 x[],          /* i/o Q0  : in:  LTP excitation signal       */
                           /*           out: total excitation signal     */
      Word16 cbGain,       /* i   Q1  : Codebook gain                    */
      Word16 ltpGain,      /* i   Q14 : LTP gain                         */
      Word16 inno[],       /* i/o Q13 : Innovation vector (Q12 for 12.2) */
      Word16 pitch_fac,    /* i   Q14 : pitch factor used to scale the
                                        LTP excitation (Q13 for 12.2)    */
      Word16 tmp_shift     /* i   Q0  : shift factor applied to sum of   
                                        scaled LTP ex & innov. before
                                        rounding                         */
)
{
   Word16 i, i1;
   Word16 tmp1;
   Word32 L_temp;
   Word16 impNr;           /* indicator for amount of disp./filter used */

   Word16 inno_sav[L_SUBFR];
   Word16 ps_poss[L_SUBFR];
   Word16 j, nze, nPulse, ppos;
   const Word16 *ph_imp;   /* Pointer to phase dispersion filter */

   /* Update LTP gain memory */
   for (i = PHDGAINMEMSIZE-1; i > 0; i--)
   {
       state->gainMem[i] = state->gainMem[i-1];
   }
   state->gainMem[0] = ltpGain;
   
   /* basic adaption of phase dispersion */

   if (sub(ltpGain, PHDTHR2LTP) < 0) {    /* if (ltpGain < 0.9) */

       if (sub(ltpGain, PHDTHR1LTP) > 0)
       {  /* if (ltpGain > 0.6 */
          impNr = 1; /* medium dispersion */
       }
       else
       {
          impNr = 0; /* maximum dispersion */
       }
   }
   else
   {
      impNr = 2; /* no dispersion */
   }
   
   /* onset indicator */
   /* onset = (cbGain  > onFact * cbGainMem[0]) */

   tmp1 = round(L_shl(L_mult(state->prevCbGain, ONFACTPLUS1), 2));

   if (sub(cbGain, tmp1) > 0)
   {
       state->onset = ONLENGTH;
   }
   else
   {

       if (state->onset > 0)
       {
           state->onset = sub (state->onset, 1);
       }
   }
   
   /* if not onset, check ltpGain buffer and use max phase dispersion if
      half or more of the ltpGain-parameters say so */

   if (state->onset == 0)
   {
       /* Check LTP gain memory and set filter accordingly */
       i1 = 0;
       for (i = 0; i < PHDGAINMEMSIZE; i++)
       {

           if (sub(state->gainMem[i], PHDTHR1LTP) < 0)
           {
               i1 = add (i1, 1);
           }
       }

       if (sub(i1, 2) > 0)
       {
           impNr = 0;
       }
       
   }
   /* Restrict decrease in phase dispersion to one step if not onset */

   if ((sub(impNr, add(state->prevState, 1)) > 0) && (state->onset == 0))
   {
       impNr = sub (impNr, 1);
   }
   /* if onset, use one step less phase dispersion */

   if((sub(impNr, 2) < 0) && (state->onset > 0))
   {
       impNr = add (impNr, 1);
   }
   
   /* disable for very low levels */

   if(sub(cbGain, 10) < 0)
   {
       impNr = 2;
   }
   

   if(sub(state->lockFull, 1) == 0)
   {
       impNr = 0;
   }

   /* update static memory */
   state->prevState = impNr;
   state->prevCbGain = cbGain;
  
   /* do phase dispersion for all modes but 12.2 and 7.4;
      don't modify the innovation if impNr >=2 (= no phase disp) */

   if (sub(mode, MR122) != 0 && 
       sub(mode, MR102) != 0 &&
       sub(mode, MR74) != 0 &&
       sub(impNr, 2) < 0)
   {
       /* track pulse positions, save innovation,
          and initialize new innovation          */
       nze = 0;
       for (i = 0; i < L_SUBFR; i++)
       {

           if (inno[i] != 0)
           {
               ps_poss[nze] = i;
               nze = add (nze, 1);
           }
           inno_sav[i] = inno[i];
           inno[i] = 0;
       }
       /* Choose filter corresponding to codec mode and dispersion criterium */

       if (sub (mode, MR795) == 0)
       {

           if (impNr == 0)
           {
               ph_imp = ph_imp_low_MR795;
           }
           else
           {
               ph_imp = ph_imp_mid_MR795;
           }
       }
       else
       {

           if (impNr == 0)
           {
               ph_imp = ph_imp_low;
           }
           else
           {
               ph_imp = ph_imp_mid;
           }
       }
       
       /* Do phase dispersion of innovation */
       for (nPulse = 0; nPulse < nze; nPulse++)
       {
           ppos = ps_poss[nPulse];
           
           /* circular convolution with impulse response */
           j = 0;
           for (i = ppos; i < L_SUBFR; i++)
           {
               /* inno[i1] += inno_sav[ppos] * ph_imp[i1-ppos] */
               tmp1 = mult(inno_sav[ppos], ph_imp[j++]);
               inno[i] = add(inno[i], tmp1);
           }    
           
           for (i = 0; i < ppos; i++)
           {
               /* inno[i] += inno_sav[ppos] * ph_imp[L_SUBFR-ppos+i] */
               tmp1 = mult(inno_sav[ppos], ph_imp[j++]);
               inno[i] = add(inno[i], tmp1);
           }
       }
   }
       
   /* compute total excitation for synthesis part of decoder
      (using modified innovation if phase dispersion is active) */
   for (i = 0; i < L_SUBFR; i++)
   {
       /* x[i] = gain_pit*x[i] + cbGain*code[i]; */
       L_temp = L_mult (        x[i],    pitch_fac);
                                                /* 12.2: Q0 * Q13 */
                                                /*  7.4: Q0 * Q14 */
       L_temp = L_mac  (L_temp, inno[i], cbGain);
                                                /* 12.2: Q12 * Q1 */
                                                /*  7.4: Q13 * Q1 */
       L_temp = L_shl (L_temp, tmp_shift);                 /* Q16 */           
       x[i] = round (L_temp);
   }

   return;
}
