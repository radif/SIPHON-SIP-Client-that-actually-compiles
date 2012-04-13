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
*      File             : bgnscd.h
*      Purpose          : Background noise source charateristic detector (SCD)
*
********************************************************************************
*/
#ifndef bgnscd_h
#define bgnscd_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define L_ENERGYHIST 60
#define INV_L_FRAME 102


/* 2*(160*x)^2 / 65536  where x is FLP values 150,5 and 50 */
#define FRAMEENERGYLIMIT  17578         /* 150 */
#define LOWERNOISELIMIT      20         /*   5 */
#define UPPERNOISELIMIT    1953         /*  50 */

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct{
   /* history vector of past synthesis speech energy */
   Word16 frameEnergyHist[L_ENERGYHIST];
   
   /* state flags */
   Word16 bgHangover;       /* counter; number of frames after last speech frame */

} Bgn_scdState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Bgn_scd_init
*  Purpose     : Allocates initializes state memory
*  Description : Stores pointer to filter status struct in *st. This
*                pointer has to be passed to Bgn_scd in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/
Word16 Bgn_scd_init (Bgn_scdState *st);

/*
**************************************************************************
*
*  Function    : Bgn_scd_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
Word16 Bgn_scd_reset (Bgn_scdState *st);

/*
**************************************************************************
*
*  Function    : Bgn_scd
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = bgn, 1 = no bgn
*
**************************************************************************
*/
Word16 Bgn_scd (Bgn_scdState *st,      /* i : State variables for bgn SCD         */
                Word16 ltpGainHist[],  /* i : LTP gain history                    */
                Word16 speech[],       /* o : synthesis speech frame              */
                Word16 *voicedHangover /* o : # of frames after last voiced frame */
);
 
#endif
