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
*      File             : dtx_dec.h
*      Purpose          : Decode comfort noice when in DTX
*
*****************************************************************************
*/
#ifndef dtx_dec_h
#define dtx_dec_h "$Id $" 
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "dtx_enc.h"
#include "d_plsf.h"
#include "gc_pred.h"
#include "c_g_aver.h"
#include "frame.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
enum DTXStateType {SPEECH = 0, DTX, DTX_MUTE};

#define DTX_MAX_EMPTY_THRESH 50

/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/

typedef struct {
   Word16 since_last_sid;
   Word16 true_sid_period_inv;
   Word16 log_en;
   Word16 old_log_en;
   Word32 L_pn_seed_rx; 
   Word16 lsp[M];
   Word16 lsp_old[M]; 
   
   Word16 lsf_hist[M*DTX_HIST_SIZE];
   Word16 lsf_hist_ptr;
   Word16 lsf_hist_mean[M*DTX_HIST_SIZE]; 
   Word16 log_pg_mean;
   Word16 log_en_hist[DTX_HIST_SIZE];
   Word16 log_en_hist_ptr;

   Word16 log_en_adjust;

   Word16 dtxHangoverCount;
   Word16 decAnaElapsedCount;

   Word16 sid_frame;       
   Word16 valid_data;          
   Word16 dtxHangoverAdded;
 
   enum DTXStateType dtxGlobalState;     /* contains previous state */
                                         /* updated in main decoder */ 

   Word16 data_updated;      /* marker to know if CNI data is ever renewed */ 

} dtx_decState;

/*
*****************************************************************************
*                         DECLARATION OF PROTOTYPES
*****************************************************************************
*/
/*
**************************************************************************
*  Function    : dtx_dec_init
*  Purpose     : Allocates memory and initializes state variables
*  Description : Stores pointer to filter status struct in *st. This
*                pointer has to be passed to dtx_dec in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/
int dtx_dec_init (dtx_decState *st);

/*
**************************************************************************
*
*  Function    : dtx_dec_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
int dtx_dec_reset (dtx_decState *st);

/*
**************************************************************************
*
*  Function    : dtx_dec
*  Purpose     :
*  Description :
*                
**************************************************************************
*/
int dtx_dec(
   dtx_decState *st,                /* i/o : State struct                    */
   Word16 mem_syn[],                /* i/o : AMR decoder state               */
   D_plsfState* lsfState,           /* i/o : decoder lsf states              */
   gc_predState* predState,         /* i/o : prediction states               */
   Cb_gain_averageState* averState, /* i/o : CB gain average states          */
   enum DTXStateType new_state,     /* i   : new DTX state                   */    
   enum Mode mode,                  /* i   : AMR mode                        */
   Word16 parm[],                   /* i   : Vector of synthesis parameters  */
   Word16 synth[],                  /* o   : synthesised speech              */
   Word16 A_t[]                     /* o   : decoded LP filter in 4 subframes*/
   );

void dtx_dec_activity_update(dtx_decState *st,
                             Word16 lsf[],
                             Word16 frame[]);

/*
**************************************************************************
*
*  Function    : rx_dtx_handler 
*  Purpose     : reads the frame type and checks history  
*  Description : to decide what kind of DTX/CNI action to perform

**************************************************************************
*/
enum DTXStateType rx_dtx_handler(dtx_decState *st,           /* i/o : State struct */
                                 enum RXFrameType frame_type /* i   : Frame type   */
                                 );

#endif
