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
*      File             : agc.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "agc.h"
const char agc_id[] = "@(#)$Id $" agc_h;

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
#include "inv_sqrt.h"
 
/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
*****************************************************************************
*                         LOCAL PROGRAM CODE
*****************************************************************************
*/

static Word32 energy_old( /* o : return energy of signal     */
    Word16 in[],          /* i : input signal (length l_trm) */
    Word16 l_trm          /* i : signal length               */
)
{
    Word32 s;
    Word16 i, temp;

    temp = shr (in[0], 2);
    s = L_mult (temp, temp);
    
    for (i = 1; i < l_trm; i++)
    {
        temp = shr (in[i], 2);
        s = L_mac (s, temp, temp);
    }

    return s;
}

static Word32 energy_new( /* o : return energy of signal     */
    Word16 in[],          /* i : input signal (length l_trm) */
    Word16 l_trm          /* i : signal length               */
)
{
    Word32 s;
    Word16 i;
    /*Flag ov_save;*/

    /*ov_save = Overflow; */           /* save overflow flag in case energy_old */
                                   /* must be called                        */
    s = L_mult(in[0], in[0]);
    for (i = 1; i < l_trm; i++)
    {
        s = L_mac(s, in[i], in[i]);
    }
    
    /* check for overflow */

    if (L_sub (s, MAX_32) == 0L)
    {
        /*Overflow = ov_save;*/ /* restore overflow flag */
        s = energy_old (in, l_trm); /* function result */
    }
    else
    {
       s = L_shr(s, 4);
    }

    return s;
}
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : agc_init
*  Purpose     : Allocates memory for agc state and initializes
*                state memory
*
**************************************************************************
*/
int agc_init (agcState *state)
{
  if (state == (agcState *) NULL){
      fprintf(stderr, "agc_init: invalid parameter\n");
      return -1;
  }
  
  agc_reset(state);
  
  return 0;
}
 
/*
**************************************************************************
*
*  Function    : agc_reset
*  Purpose     : Reset of agc (i.e. set state memory to 1.0)
*
**************************************************************************
*/
int agc_reset (agcState *state)
{
  if (state == (agcState *) NULL){
      fprintf(stderr, "agc_reset: invalid parameter\n");
      return -1;
  }
  
  state->past_gain = 4096;   /* initial value of past_gain = 1.0  */
  
  return 0;
}

/*
**************************************************************************
*
*  Function    : agc
*  Purpose     : Scales the postfilter output on a subframe basis
*
**************************************************************************
*/
int agc (
    agcState *st,      /* i/o : agc state                        */
    Word16 *sig_in,    /* i   : postfilter input signal  (l_trm) */
    Word16 *sig_out,   /* i/o : postfilter output signal (l_trm) */
    Word16 agc_fac,    /* i   : AGC factor                       */
    Word16 l_trm       /* i   : subframe size                    */
)
{
    Word16 i, exp;
    Word16 gain_in, gain_out, g0, gain;
    Word32 s;
            
    /* calculate gain_out with exponent */
    s = energy_new(sig_out, l_trm);  /* function result */
        

    if (s == 0)
    {
        st->past_gain = 0;
        return 0;
    }
    exp = sub (norm_l (s), 1);
    gain_out = round (L_shl (s, exp));

    /* calculate gain_in with exponent */
    s = energy_new(sig_in, l_trm);    /* function result */
    

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        gain_in = round (L_shl (s, i));
        exp = sub (exp, i);

        /*---------------------------------------------------*
         *  g0 = (1-agc_fac) * sqrt(gain_in/gain_out);       *
         *---------------------------------------------------*/

        s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_shr (s, exp);     /* add exponent */

        s = Inv_sqrt (s);  /* function result */
        i = round (L_shl (s, 9));

        /* g0 = i * (1-agc_fac) */
        g0 = mult (i, sub (32767, agc_fac));
    }

    /* compute gain[n] = agc_fac * gain[n-1]
                        + (1-agc_fac) * sqrt(gain_in/gain_out) */
    /* sig_out[n] = gain[n] * sig_out[n]                        */

    gain = st->past_gain;

    for (i = 0; i < l_trm; i++)
    {
        gain = mult (gain, agc_fac);
        gain = add (gain, g0);
        sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], gain), 3));

    }

    st->past_gain = gain;

    return 0;
}

/*
**************************************************************************
*
*  Function    : agc2
*  Purpose     : Scales the excitation on a subframe basis
*
**************************************************************************
*/
void agc2 (
 Word16 *sig_in,        /* i   : postfilter input signal  */
 Word16 *sig_out,       /* i/o : postfilter output signal */
 Word16 l_trm           /* i   : subframe size            */
)
{
    Word16 i, exp;
    Word16 gain_in, gain_out, g0;
    Word32 s;
    
    /* calculate gain_out with exponent */
    s = energy_new(sig_out, l_trm);    /* function result */
        

    if (s == 0)
    {
        return;
    }
    exp = sub (norm_l (s), 1);
    gain_out = round (L_shl (s, exp));

    /* calculate gain_in with exponent */
    s = energy_new(sig_in, l_trm);    /* function result */
    

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        gain_in = round (L_shl (s, i));
        exp = sub (exp, i);

        /*---------------------------------------------------*
         *  g0 = sqrt(gain_in/gain_out);                     *
         *---------------------------------------------------*/

        s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_shr (s, exp);     /* add exponent */

        s = Inv_sqrt (s);  /* function result */
        g0 = round (L_shl (s, 9));
    }

    /* sig_out(n) = gain(n) sig_out(n) */

    for (i = 0; i < l_trm; i++)
    {
        sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], g0), 3));

    }

    return;
}
