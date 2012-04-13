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
*      File             : sp_enc.c
*      Purpose          : Pre filtering and encoding of one speech frame.
*
*****************************************************************************
*/
 
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "set_zero.h"
#include "pre_proc.h"
#include "prm2bits.h"
#include "mode.h"
#include "cod_amr.h"

#ifdef MMS_IO
#include "frame.h"
#include "bitno.tab"
#endif

#include "sp_enc.h"
const char sp_enc_id[] = "@(#)$Id $" sp_enc_h;

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*---------------------------------------------------------------*
 *    Constants (defined in "cnst.h")                            *
 *---------------------------------------------------------------*
 * L_FRAME     :
 * M           :
 * PRM_SIZE    :
 * AZ_SIZE     :
 * SERIAL_SIZE :
 *---------------------------------------------------------------*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
int Speech_Encode_Frame_memSize()
{
  return sizeof(Speech_Encode_FrameState);
}

/*************************************************************************
*
*  Function:   Speech_Encode_Frame_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int Speech_Encode_Frame_init (Speech_Encode_FrameState *state,
                              Flag dtx)
{
  if (state == (Speech_Encode_FrameState *) NULL){
      fprintf(stderr, "Speech_Encode_Frame_init: invalid parameter\n");
      return -1;
  }

  /*s->pre_state = NULL;
  s->cod_amr_state = NULL;*/
  state->dtx = dtx;

  if (Pre_Process_init(&state->pre_state) ||
      cod_amr_init(&state->cod_amr_state, state->dtx)) {
      Speech_Encode_Frame_reset(state);
      return -1;
  }

  Speech_Encode_Frame_reset(state);
  
  return 0;
}
 
/*************************************************************************
*
*  Function:   Speech_Encode_Frame_reset
*  Purpose:    Resetses state memory
*
**************************************************************************
*/
int Speech_Encode_Frame_reset (Speech_Encode_FrameState *state)
{
  if (state == (Speech_Encode_FrameState *) NULL){
      fprintf(stderr, "Speech_Encode_Frame_reset: invalid parameter\n");
      return -1;
  }
  
  Pre_Process_reset(&state->pre_state);
  cod_amr_reset(&state->cod_amr_state);

  return 0;
}

int Speech_Encode_Frame_First (
    Speech_Encode_FrameState *st,  /* i/o : post filter states       */
    Word16 *new_speech)            /* i   : speech input             */
{
#if !defined(NO13BIT)
   Word16 i;
#endif

#if !defined(NO13BIT)
  /* Delete the 3 LSBs (13-bit input) */
  for (i = 0; i < L_NEXT; i++) 
  {
     new_speech[i] = new_speech[i] & 0xfff8;
  }
#endif

  /* filter + downscaling */
  Pre_Process (&st->pre_state, new_speech, L_NEXT);

  cod_amr_first(&st->cod_amr_state, new_speech);

  return 0;
}

int Speech_Encode_Frame (
    Speech_Encode_FrameState *st, /* i/o : post filter states          */
    enum Mode mode,               /* i   : speech coder mode           */
    Word16 *new_speech,           /* i   : speech input                */
    Word16 *serial,               /* o   : serial bit stream           */
    enum Mode *usedMode           /* o   : used speech coder mode */
    )
{
  Word16 prm[MAX_PRM_SIZE];   /* Analysis parameters.                  */
  Word16 syn[L_FRAME];        /* Buffer for synthesis speech           */
  Word16 i;

  /* initialize the serial output frame to zero */
  Set_zero(serial, MAX_SERIAL_SIZE);
  /*for (i = 0; i < MAX_SERIAL_SIZE; i++)
  {
    serial[i] = 0;
  }*/

#if !defined(NO13BIT)
  /* Delete the 3 LSBs (13-bit input) */
  for (i = 0; i < L_FRAME; i++)   
  {
     new_speech[i] = new_speech[i] & 0xfff8;
  }
#endif

  /* filter + downscaling */
  Pre_Process (&st->pre_state, new_speech, L_FRAME);
  
  /* Call the speech encoder */
  cod_amr(&st->cod_amr_state, mode, new_speech, prm, usedMode, syn);

  /* Parameters to serial bits */
  Prm2bits (*usedMode, prm, &serial[0]); 

  return 0;
}

#ifdef MMS_IO

/*************************************************************************
 *
 *  FUNCTION:    PackBits
 *
 *  PURPOSE:     Sorts speech bits according decreasing subjective importance
 *               and packs into octets according to AMR file storage format
 *               as specified in RFC 3267 (Sections 5.1 and 5.3).
 *
 *  DESCRIPTION: Depending on the mode, different numbers of bits are
 *               processed. Details can be found in specification mentioned
 *               above and in file "bitno.tab".
 *
 *************************************************************************/
Word16 PackBits(
    enum Mode used_mode,       /* i : actual AMR mode             */
    enum Mode mode,            /* i : requested AMR (speech) mode */
    enum TXFrameType fr_type,  /* i : frame type                  */
    Word16 bits[],             /* i : serial bits                 */
    UWord8 packed_bits[]       /* o : sorted&packed bits          */
)
{
   Word16 i;
   UWord8 temp;
   UWord8 *pack_ptr;

   temp = 0;
   pack_ptr = (UWord8*)packed_bits;

   /* file storage format can handle only speech frames, AMR SID frames and NO_DATA frames */
   /* -> force NO_DATA frame */
   if (used_mode < 0 || used_mode > 15 || (used_mode > 8 && used_mode < 15))
   {
	   used_mode = 15;
   }

   /* mark empty frames between SID updates as NO_DATA frames */
   if (used_mode == MRDTX && fr_type == TX_NO_DATA)
   {
	   used_mode = 15;
   }

   /* insert table of contents (ToC) byte at the beginning of the frame */
   *pack_ptr = toc_byte[used_mode];
   pack_ptr++;

   /* note that NO_DATA frames (used_mode==15) do not need further processing */
   if (used_mode == 15)
   {
	   return 1;
   }

   temp = 0;

   /* sort and pack speech bits */
   for (i = 1; i < unpacked_size[used_mode] + 1; i++)
   {
       if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
	   {
		   temp++;
	   }

	   if (i % 8)
	   {
		   temp <<= 1;
	   }
	   else
	   {
		   *pack_ptr = temp;
		   pack_ptr++;
		   temp = 0;
	   }
   }

   /* insert SID type indication and speech mode in case of SID frame */
   if (used_mode == MRDTX)
   {
	   if (fr_type == TX_SID_UPDATE)
	   {
		   temp++;
	   }
	   temp <<= 3;

	   temp += ((mode & 0x4) >> 2) | (mode & 0x2) | ((mode & 0x1) << 2);

	   temp <<= 1;
   }

   /* insert unused bits (zeros) at the tail of the last byte */
   temp <<= (unused_size[used_mode] - 1);
   *pack_ptr = temp;

   return packed_size[used_mode];
}

#endif

