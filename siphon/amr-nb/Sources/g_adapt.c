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
*      File             : g_adapt.c
*      Purpose          : gain adaptation for MR795 gain quantization
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "g_adapt.h"
const char g_adapt_id[] = "@(#)$Id $" g_adapt_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "gmed_n.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define LTP_GAIN_THR1 2721 /* 2721 Q13 = 0.3322 ~= 1.0 / (10*log10(2)) */
#define LTP_GAIN_THR2 5443 /* 5443 Q13 = 0.6644 ~= 2.0 / (10*log10(2)) */

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   gain_adapt_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int gain_adapt_init (GainAdaptState *st)
{
    if (st == (GainAdaptState *) NULL){
        fprintf(stderr, "gain_adapt_init: invalid parameter\n");
        return -1;
    }

    gain_adapt_reset(st);

    return 0;
}

/*************************************************************************
*
*  Function:   gain_adapt_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int gain_adapt_reset (GainAdaptState *st)
{
    Word16 i;

    if (st == (GainAdaptState *) NULL){
        fprintf(stderr, "gain_adapt_reset: invalid parameter\n");
        return -1;
    }

    st->onset = 0;
    st->prev_alpha = 0;
    st->prev_gc = 0;

    for (i = 0; i < LTPG_MEM_SIZE; i++)
    {
        st->ltpg_mem[i] = 0;
    }

    return 0;
}

/*************************************************************************
 *
 *  Function:   gain_adapt()
 *  Purpose:    calculate pitch/codebook gain adaptation factor alpha
 *              (and update the adaptor state)
 *
 **************************************************************************
 */
void gain_adapt(
    GainAdaptState *st,  /* i  : state struct                  */
    Word16 ltpg,         /* i  : ltp coding gain (log2()), Q13 */
    Word16 gain_cod,     /* i  : code gain,                Q1  */
    Word16 *alpha        /* o  : gain adaptation factor,   Q15 */
)
{
    Word16 adapt;      /* adaptdation status; 0, 1, or 2       */
    Word16 result;     /* alpha factor, Q13                    */
    Word16 filt;       /* median-filtered LTP coding gain, Q13 */
    Word16 tmp, i;
    
    /* basic adaptation */

    if (sub (ltpg, LTP_GAIN_THR1) <= 0)
    {
        adapt = 0;
    }
    else
    {

        if (sub (ltpg, LTP_GAIN_THR2) <= 0)
        {
            adapt = 1;
        }
        else
        {
            adapt = 2;
        }
    }

    /*
     * // onset indicator
     * if ((cbGain > onFact * cbGainMem[0]) && (cbGain > 100.0))
     *     onset = 8;
     * else
     *     if (onset)
     *         onset--;
     */
    /* tmp = cbGain / onFact; onFact = 2.0; 200 Q1 = 100.0 */
    tmp = shr_r (gain_cod, 1);

    if ((sub (tmp, st->prev_gc) > 0) && sub(gain_cod, 200) > 0)
    {
        st->onset = 8;
    }
    else
    {

        if (st->onset != 0)
        {
            st->onset = sub (st->onset, 1);
        }
    }

    /*
     *  // if onset, increase adaptor state
     *  if (onset && (gainAdapt < 2)) gainAdapt++;
     */
    if ((st->onset != 0) && (sub (adapt, 2) < 0))
    {
        adapt = add (adapt, 1);
    }

    st->ltpg_mem[0] = ltpg;
    filt = gmed_n (st->ltpg_mem, 5);    /* function result */


    if (adapt == 0)
    {

        if (sub (filt, 5443) > 0) /* 5443 Q13 = 0.66443... */
        {
            result = 0;
        }
        else
        {

            if (filt < 0)
            {
                result = 16384;   /* 16384 Q15 = 0.5 */
            }
            else
            {   /* result       =   0.5 - 0.75257499*filt     */
                /* result (Q15) = 16384 - 24660 * (filt << 2) */
                filt = shl (filt, 2); /* Q15 */
                result = sub (16384, mult (24660, filt));
            }
        }
    }
    else
    {
        result = 0;
    }
    /*
     *  if (prevAlpha == 0.0) result = 0.5 * (result + prevAlpha);
     */

    if (st->prev_alpha == 0)
    {
        result = shr (result, 1);
    }

    /* store the result */
    *alpha = result;
    
    /* update adapter state memory */
    st->prev_alpha = result;
    st->prev_gc = gain_cod;

    for (i = LTPG_MEM_SIZE-1; i > 0; i--)
    {
        st->ltpg_mem[i] = st->ltpg_mem[i-1];
    }
    /* mem[0] is just present for convenience in calling the gmed_n[5]
     * function above. The memory depth is really LTPG_MEM_SIZE-1.
     */
}
