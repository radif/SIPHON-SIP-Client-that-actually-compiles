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
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : ton_stab.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "ton_stab.h"
const char ton_stab_id[] = "@(#)$Id $" ton_stab_h;
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "set_zero.h"
#include "copy.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
 *
 *  Function:   ton_stab_init
 *  Purpose:    Allocates state memory and initializes state memory
 *
 **************************************************************************
 */
int ton_stab_init (tonStabState *state)
{
    if (state == (tonStabState *) NULL){
        fprintf(stderr, "ton_stab_init: invalid parameter\n");
        return -1;
    }

    ton_stab_reset(state);

    return 0;
}

/*************************************************************************
 *
 *  Function:   ton_stab_reset
 *  Purpose:    Initializes state memory to zero
 *
 **************************************************************************
 */
int ton_stab_reset (tonStabState *st)
{
    if (st == (tonStabState *) NULL){
        fprintf(stderr, "ton_stab_init: invalid parameter\n");
        return -1;
    }

    /* initialize tone stabilizer state */ 
    st->count = 0;
    Set_zero(st->gp, N_FRAME);    /* Init Gp_Clipping */
    
    return 0;
}

/***************************************************************************
 *                                                                          *
 *  Function:  check_lsp()                                                  *
 *  Purpose:   Check the LSP's to detect resonances                         *
 *                                                                          *
 ****************************************************************************
 */
Word16 check_lsp(tonStabState *st, /* i/o : State struct            */
                 Word16 *lsp       /* i   : unquantized LSP's       */
)
{
   Word16 i, dist, dist_min1, dist_min2, dist_th;
 
   /* Check for a resonance:                             */
   /* Find minimum distance between lsp[i] and lsp[i+1]  */
 
   dist_min1 = MAX_16;
   for (i = 3; i < M-2; i++)
   {
      dist = sub(lsp[i], lsp[i+1]);


      if (sub(dist, dist_min1) < 0)
      {
         dist_min1 = dist;
      }
   }

   dist_min2 = MAX_16;
   for (i = 1; i < 3; i++)
   {
      dist = sub(lsp[i], lsp[i+1]);


      if (sub(dist, dist_min2) < 0)
      {
         dist_min2 = dist;
      }
   }

   /*if (test (), sub(lsp[1], 32000) > 0)*/
   if ( lsp[1] > 32000 )
   {
      dist_th = 600;
   }
   /*else if (test (), sub(lsp[1], 30500) > 0)*/
   else if ( lsp[1] > 30500 )
   {
      dist_th = 800;
   }
   else
   {
      dist_th = 1100;
   }

   /* */
   if (sub(dist_min1, 1500) < 0 ||
       sub(dist_min2, dist_th) < 0)
   {
      st->count = add(st->count, 1);
   }
   else
   {
      st->count = 0;
   }
   
   /* Need 12 consecutive frames to set the flag */

   if (sub(st->count, 12) >= 0)
   {
      st->count = 12;
      return 1;
   }
   else
   {
      return 0;
   }
}

/***************************************************************************
 *
 *  Function:   Check_Gp_Clipping()                                          
 *  Purpose:    Verify that the sum of the last (N_FRAME+1) pitch  
 *              gains is under a certain threshold.              
 *                                                                         
 ***************************************************************************
 */ 
Word16 check_gp_clipping(tonStabState *st, /* i/o : State struct            */
                         Word16 g_pitch    /* i   : pitch gain              */
)
{
   Word16 i, sum;
   
   sum = shr(g_pitch, 3);          /* Division by 8 */
   for (i = 0; i < N_FRAME; i++)
   {
      sum = add(sum, st->gp[i]);
   }


   if (sub(sum, GP_CLIP) > 0)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

/***************************************************************************
 *
 *  Function:  Update_Gp_Clipping()                                          
 *  Purpose:   Update past pitch gain memory
 *                                                                         
 ***************************************************************************
 */
void update_gp_clipping(tonStabState *st, /* i/o : State struct            */
                        Word16 g_pitch    /* i   : pitch gain              */
)
{
   Copy(&st->gp[1], &st->gp[0], N_FRAME-1);
   st->gp[N_FRAME-1] = shr(g_pitch, 3);
}
