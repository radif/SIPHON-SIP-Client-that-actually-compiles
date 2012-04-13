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
*      File             : c_g_aver.h
*      Purpose          : Background noise source charateristic detector (SCD)
*
********************************************************************************
*/
#ifndef c_g_aver_h
#define c_g_aver_h "$Id $"
 
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
#define L_CBGAINHIST 7

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct{
   /* history vector of past synthesis speech energy */
   Word16 cbGainHistory[L_CBGAINHIST];
   
   /* state flags */
   Word16 hangVar;       /* counter; */
   Word16 hangCount;     /* counter; */

} Cb_gain_averageState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Cb_gain_average_init
*  Purpose     : Allocates initializes state memory
*  Description : Stores pointer to filter status struct in *st. This
*                pointer has to be passed to Cb_gain_average in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/
Word16 Cb_gain_average_init (Cb_gain_averageState *st);

/*
**************************************************************************
*
*  Function    : Cb_gain_average_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
Word16 Cb_gain_average_reset (Cb_gain_averageState *st);

/*
**************************************************************************
*
*  Function    : Cb_gain_average
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = bgn, 1 = no bgn
*
**************************************************************************
*/
Word16 Cb_gain_average (
   Cb_gain_averageState *st, /* i/o : State variables for CB gain avergeing   */
   enum Mode mode,           /* i   : AMR mode                                */
   Word16 gain_code,         /* i   : CB gain                              Q1 */
   Word16 lsp[],             /* i   : The LSP for the current frame       Q15 */
   Word16 lspAver[],         /* i   : The average of LSP for 8 frames     Q15 */
   Word16 bfi,               /* i   : bad frame indication flag               */
   Word16 prev_bf,           /* i   : previous bad frame indication flag      */
   Word16 pdfi,              /* i   : potential degraded bad frame ind flag   */
   Word16 prev_pdf,          /* i   : prev pot. degraded bad frame ind flag   */
   Word16 inBackgroundNoise, /* i   : background noise decision               */
   Word16 voicedHangover     /* i   : # of frames after last voiced frame     */
);
 
#endif
