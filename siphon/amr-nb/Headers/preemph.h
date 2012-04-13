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
*      File             : preemph.h
*      Purpose          : Preemphasis filtering
*      Description      : Filtering through 1 - g z^-1 
*
********************************************************************************
*/
#ifndef preemph_h
#define preemph_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
 
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 mem_pre;          /* filter state */
} preemphasisState;
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int preemphasis_init (preemphasisState *st);
/* initialize one instance of the preemphasis filter
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to preemphasis in each call.
   returns 0 on success
 */

int preemphasis_reset (preemphasisState *st);
/* reset of preemphasis filter (i.e. set state memory to zero)
   returns 0 on success
 */

void preemphasis (
    preemphasisState *st, /* (i/o): preemphasis filter state                  */
    Word16 *signal,    /* (i/o): input signal overwritten by the output     */
    Word16 g,          /* (i)  : preemphasis coefficient                    */
    Word16 L           /* (i)  : size of filtering                          */
);
 
#endif
