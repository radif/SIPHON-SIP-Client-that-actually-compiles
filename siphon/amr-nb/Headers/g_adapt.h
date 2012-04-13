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
*      File             : g_adapt.h
*      Purpose          : gain adaptation for MR795 gain quantization
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#ifndef g_adapt_h
#define g_adapt_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"

#define LTPG_MEM_SIZE 5 /* number of stored past LTP coding gains + 1 */

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
    Word16 onset;                   /* onset state,                   Q0  */
    Word16 prev_alpha;              /* previous adaptor output,       Q15 */
    Word16 prev_gc;                 /* previous code gain,            Q1  */

    Word16 ltpg_mem[LTPG_MEM_SIZE]; /* LTP coding gain history,       Q13 */
                                    /* (ltpg_mem[0] not used for history) */
} GainAdaptState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int gain_adapt_init (GainAdaptState *st);

/* initialize one instance of the gain adaptor
   Stores pointer to state struct in *st. This pointer has to
   be passed to gain_adapt and gain_adapt_update in each call.
   returns 0 on success
 */
 
int gain_adapt_reset (GainAdaptState *st);
/* reset of gain adaptor state (i.e. set state memory to zero)
   returns 0 on success
 */

/*************************************************************************
 *
 *  Function:   gain_adapt()
 *  Purpose:    calculate pitch/codebook gain adaptation factor alpha
 *              (and update the adaptor state)
 *
 **************************************************************************
 */
void gain_adapt(
    GainAdaptState *st,  /* i  : state struct                  */
    Word16 ltpg,         /* i  : ltp coding gain (log2()), Q   */
    Word16 gain_cod,     /* i  : code gain,                Q13 */
    Word16 *alpha        /* o  : gain adaptation factor,   Q15 */
);


#endif
