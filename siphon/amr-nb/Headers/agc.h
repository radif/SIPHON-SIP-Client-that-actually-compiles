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
*      File             : agc.h
*      Purpose          : Scales the postfilter output on a subframe basis
*                       : by automatic control of the subframe gain.
*
*****************************************************************************
*/
#ifndef agc_h
#define agc_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
 
/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/
typedef struct {
    Word16 past_gain;
} agcState;

/*
*****************************************************************************
*                         DECLARATION OF PROTOTYPES
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : agc_init
*  Purpose     : Allocates memory for agc state and initializes
*                state memory
*  Description : Stores pointer to agc status struct in *st. This pointer
*                has to be passed to agc in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/
int agc_init(agcState *st);

/*
**************************************************************************
*
*  Function    : agc_reset
*  Purpose     : Reset of agc (i.e. set state memory to 1.0)
*  Returns     : 0 on success
*
**************************************************************************
*/
int agc_reset (agcState *st);

/*
**************************************************************************
*
*  Function    : agc
*  Purpose     : Scales the postfilter output on a subframe basis
*  Description : sig_out[n] = sig_out[n] * gain[n];
*                where gain[n] is the gain at the nth sample given by
*                gain[n] = agc_fac * gain[n-1] + (1 - agc_fac) g_in/g_out
*                g_in/g_out is the square root of the ratio of energy at 
*                the input and output of the postfilter.
*
**************************************************************************
*/
int agc (
    agcState *st,      /* i/o : agc state                         */
    Word16 *sig_in,    /* i   : postfilter input signal, (l_trm)  */
    Word16 *sig_out,   /* i/o : postfilter output signal, (l_trm) */
    Word16 agc_fac,    /* i   : AGC factor                        */
    Word16 l_trm       /* i   : subframe size                     */
);

/*
**************************************************************************
*
*  Function:  agc2
*  Purpose:   Scales the excitation on a subframe basis
*
**************************************************************************
*/
void agc2 (
    Word16 *sig_in,    /* i   : postfilter input signal   */
    Word16 *sig_out,   /* i/o : postfilter output signal  */
    Word16 l_trm       /* i   : subframe size             */
);

#endif
