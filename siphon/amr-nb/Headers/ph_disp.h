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
*      File             : ph_disp.h
*      Purpose          : Phase dispersion of excitation signal
*
********************************************************************************
*/

#ifndef ph_disp_h
#define ph_disp_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define PHDGAINMEMSIZE 5
#define PHDTHR1LTP     9830  /* 0.6 in Q14 */
#define PHDTHR2LTP     14746 /* 0.9 in Q14 */
#define ONFACTPLUS1    16384 /* 2.0 in Q13   */
#define ONLENGTH 2
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 gainMem[PHDGAINMEMSIZE];
  Word16 prevState;
  Word16 prevCbGain;
  Word16 lockFull;
  Word16 onset;
} ph_dispState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
/*************************************************************************
*
*  Function:   ph_disp_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int ph_disp_init (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp_reset
*  Purpose:    Initializes state memory
*
**************************************************************************
*/
int ph_disp_reset (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp_lock
*  Purpose:    mark phase dispersion as locked in state struct
*
**************************************************************************
*/
void ph_disp_lock (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp_release
*  Purpose:    mark phase dispersion as unlocked in state struct
*
**************************************************************************
*/
void ph_disp_release (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp
*  Purpose:    perform phase dispersion according to the specified codec
*              mode and computes total excitation for synthesis part
*              if decoder
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
      Word16 inno[],       /* i   Q13 : Innovation vector (Q12 for 12.2) */
      Word16 pitch_fac,    /* i   Q14 : pitch factor used to scale the
                                        LTP excitation (Q13 for 12.2)    */
      Word16 tmp_shift     /* i   Q0  : shift factor applied to sum of   
                                        scaled LTP ex & innov. before
                                        rounding                         */
);



#endif
