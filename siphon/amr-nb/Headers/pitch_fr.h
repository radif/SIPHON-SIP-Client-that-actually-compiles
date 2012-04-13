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
*      File             : pitch_fr.h
*      Purpose          : Find the pitch period with 1/3 or 1/6 subsample
*                       : resolution (closed loop).
*
********************************************************************************
*/
#ifndef pitch_fr_h
#define pitch_fr_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
   Word16 T0_prev_subframe;   /* integer pitch lag of previous sub-frame */
} Pitch_frState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int Pitch_fr_init (Pitch_frState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Pitch_fr in each call.
   returns 0 on success
 */
 
int Pitch_fr_reset (Pitch_frState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

Word16 Pitch_fr (        /* o   : pitch period (integer)                    */
    Pitch_frState *st,   /* i/o : State struct                              */
    enum Mode mode,      /* i   : codec mode                                */
    Word16 T_op[],       /* i   : open loop pitch lags                      */
    Word16 exc[],        /* i   : excitation buffer                         */
    Word16 xn[],         /* i   : target vector                             */
    Word16 h[],          /* i   : impulse response of synthesis and   
                                  weighting filters                         */
    Word16 L_subfr,      /* i   : Length of subframe                        */
    Word16 i_subfr,      /* i   : subframe offset                           */
    Word16 *pit_frac,    /* o   : pitch period (fractional)                 */
    Word16 *resu3,       /* o   : subsample resolution 1/3 (=1) or 1/6 (=0) */
    Word16 *ana_index    /* o   : index of encoding                         */
);
 
#endif
