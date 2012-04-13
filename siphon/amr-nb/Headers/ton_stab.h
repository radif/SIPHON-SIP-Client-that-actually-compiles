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
*      File             : ton_stab.h
*      Purpose          : Tone stabilization routines 
*
********************************************************************************
*/
#ifndef ton_stab_h
#define ton_stab_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/

/* state variable */
typedef struct {

   /* counters */
   Word16 count;
   
   /* gain history Q11 */
   Word16 gp[N_FRAME];
   
} tonStabState;

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int ton_stab_init (tonStabState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to ton_stab in each call.
   returns 0 on success
 */
 
int ton_stab_reset (tonStabState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

Word16 check_lsp(tonStabState *st, /* i/o : State struct            */
                 Word16 *lsp       /* i   : unquantized LSP's       */
);

Word16 check_gp_clipping(tonStabState *st, /* i/o : State struct            */
                         Word16 g_pitch    /* i   : pitch gain              */
);

void update_gp_clipping(tonStabState *st, /* i/o : State struct            */
                        Word16 g_pitch    /* i   : pitch gain              */
);
#endif
