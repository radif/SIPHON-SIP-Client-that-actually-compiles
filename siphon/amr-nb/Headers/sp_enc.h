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
*      File             : sp_enc.h
*      Purpose          : Encoding of one speech frame.
*
*****************************************************************************
*/
#ifndef sp_enc_h
#define sp_enc_h "$Id $"
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "pre_proc.h"
#include "mode.h"
#include "cod_amr.h"

/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/
typedef struct{
    Pre_ProcessState pre_state;
    cod_amrState     cod_amr_state;
    Flag dtx;
} Speech_Encode_FrameState;

/*
*****************************************************************************
*                         ENCLARATION OF PROTOTYPES
*****************************************************************************
*/
int Speech_Encode_Frame_memSize();

int Speech_Encode_Frame_init (Speech_Encode_FrameState *st,
                              Flag dtx);
/* initialize one instance of the speech encoder
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Speech_Encode_Frame in each call.
   returns 0 on success
 */
 
int Speech_Encode_Frame_reset (Speech_Encode_FrameState *st);
/* reset speech encoder (i.e. set state memory to zero)
   returns 0 on success
 */
 
int Speech_Encode_Frame_First (
    Speech_Encode_FrameState *st, /* i/o : post filter states     */
    Word16 *new_speech);          /* i   : speech input           */

int Speech_Encode_Frame (
    Speech_Encode_FrameState *st, /* i/o : encoder states         */
    enum Mode mode,               /* i   : speech coder mode      */
    Word16 *new_speech,           /* i   : input speech           */
    Word16 *serial,               /* o   : serial bit stream      */
    enum Mode *usedMode           /* o   : used speech coder mode */
);
/*    return 0 on success
 */

#ifdef MMS_IO

Word16 PackBits(
    enum Mode used_mode,       /* i : actual AMR mode             */
    enum Mode mode,            /* i : requested AMR (speech) mode */
    enum TXFrameType fr_type,  /* i : frame type                  */
    Word16 bits[],             /* i : serial bits                 */
    UWord8 packed_bits[]       /* o : sorted&packed bits          */
);

#endif
 
#endif
