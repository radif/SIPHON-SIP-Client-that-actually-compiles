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
*      File             : sp_dec.h
*      Purpose          : Decoding and post filtering of one speech frame.
*
*****************************************************************************
*/
#ifndef sp_dec_h
#define sp_dec_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "dec_amr.h"
#include "pstfilt.h"
#include "post_pro.h"
#include "mode.h"

/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/
typedef struct{
  Decoder_amrState decoder_amrState;
  Post_FilterState  post_state;
  Post_ProcessState postHP_state;
  enum Mode prev_mode;

  int complexityCounter;   /* Only for complexity computation            */
} Speech_Decode_FrameState;

/*
*****************************************************************************
*                         DECLARATION OF PROTOTYPES
*****************************************************************************
*/
int Speech_Decode_Frame_memSize();
int Speech_Decode_Frame_init (Speech_Decode_FrameState *st,
                              char *id);
/* initialize one instance of the speech decoder
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Speech_Decode_Frame in each call.
   returns 0 on success
 */
 
int Speech_Decode_Frame_reset (Speech_Decode_FrameState *st);
/* reset speech decoder (i.e. set state memory to zero)
   returns 0 on success
 */
 
int Speech_Decode_Frame (
    Speech_Decode_FrameState *st, /* io: post filter states                */
    enum Mode mode,               /* i : AMR mode                          */
    Word16 *serial,               /* i : serial bit stream                 */
    enum RXFrameType frame_type,  /* i : Frame type                        */
    Word16 *synth                 /* o : synthesis speech (postfiltered    */
                                  /*     output)                           */
);
/*    return 0 on success
 */

#ifdef MMS_IO

enum RXFrameType UnpackBits (
    Word8  q,              /* i : Q-bit (i.e. BFI)        */
	Word16 ft,             /* i : frame type (i.e. mode)  */
    UWord8 packed_bits[],  /* i : sorted & packed bits    */
	enum Mode *mode,       /* o : mode information        */
    Word16 bits[]          /* o : serial bits             */
);
#endif
 
#endif
