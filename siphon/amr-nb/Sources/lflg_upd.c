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
*      File             : lflg_upd.c
*      Purpose          : LTP_flag update for AMR VAD option 2
*
*****************************************************************************
*/

const char lflg_upd_id[] = "@(#)$Id $";


#include "typedef.h"
#include "cnst.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"

#include "vad2.h"
#include "mode.h"

/***************************************************************************
 *
 *   FUNCTION NAME: LTP_flag_update
 *
 *   PURPOSE:
 *     Set LTP_flag if the LTP gain > LTP_THRESHOLD, where the value of
 *     LTP_THRESHOLD depends on the LTP analysis window length.
 *
 *   INPUTS:
 *
 *     mode
 *                     AMR mode
 *     vadState->L_R0
 *                     LTP energy
 *     vadState->L_Rmax
 *                     LTP maximum autocorrelation
 *   OUTPUTS:
 *
 *     vadState->LTP_flag
 *                     Set if LTP gain > LTP_THRESHOLD
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *************************************************************************/

void LTP_flag_update (vadState2 * st, Word16 mode)
{
	Word16 thresh;
	Word16 hi1;
	Word16 lo1;
	Word32 Ltmp;


	if ((mode == MR475) || (mode == MR515))
	{
		thresh = (Word16)(32768.0*0.55);
	}
	else if (mode == MR102)
	{
		thresh = (Word16)(32768.0*0.60);
	}
	else
	{
		thresh = (Word16)(32768.0*0.65);
	}

	L_Extract (st->L_R0, &hi1, &lo1);
	Ltmp = Mpy_32_16(hi1, lo1, thresh);
	if (st->L_Rmax > Ltmp)
	{
		st->LTP_flag = TRUE;
	}
	else
	{
		st->LTP_flag = FALSE;
	}
}
