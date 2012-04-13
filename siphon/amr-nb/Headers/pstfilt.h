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
*      File             : pstfilt.h
*      Purpose          : Performs adaptive postfiltering on the synthesis
*                       : speech
*
********************************************************************************
*/
#ifndef pstfilt_h
#define pstfilt_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"
#include "cnst.h"
#include "preemph.h"
#include "agc.h"
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct{
  Word16 res2[L_SUBFR];
  Word16 mem_syn_pst[M];
  preemphasisState preemph_state;
  agcState agc_state;
  Word16 synth_buf[M + L_FRAME];
} Post_FilterState;

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

int Post_Filter_init (Post_FilterState *st);
/* initialize one instance of the post filter
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Post_Filter in each call.
   returns 0 on success
 */
 
int Post_Filter_reset (Post_FilterState *st);
/* reset post filter (i.e. set state memory to zero)
   returns 0 on success
 */

void Post_Filter (
    Post_FilterState *st, /* i/o : post filter states                        */
    enum Mode mode,       /* i   : AMR mode                                  */
    Word16 *syn,          /* i/o : synthesis speech (postfiltered is output) */
    Word16 *Az_4          /* i   : interpolated LPC parameters in all subfr. */
);
/* filters the signal syn using the parameters in Az_4 to calculate filter
   coefficients.
   The filter must be set up using Post_Filter_init prior to the first call
   to Post_Filter. Post_FilterState is updated to mirror the current state
   of the filter
 
   return 0 on success
 */
 
#endif
