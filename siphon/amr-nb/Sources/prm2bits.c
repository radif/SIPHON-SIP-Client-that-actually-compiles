/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : prm2bits.c
*      Purpose          : Converts the encoder parameter vector into a
*                       : vector of serial bits.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "prm2bits.h"

const char prm2bits_id[] = "@(#)$Id $" prm2bits_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "mode.h"

#include <stdlib.h>
#include <stdio.h>

 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "bitno.tab"
#define MASK      0x0001
 
/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
 *
 *  FUNCTION:  Int2bin
 *
 *  PURPOSE:  convert integer to binary and write the bits to the array
 *            bitstream[]. The most significant bits are written first.
 *
 *************************************************************************/

static void Int2bin (
    Word16 value,       /* input : value to be converted to binary      */
    Word16 no_of_bits,  /* input : number of bits associated with value */
    Word16 *bitstream   /* output: address where bits are written       */
)
{
    Word16 *pt_bitstream, i, bit;

    pt_bitstream = &bitstream[no_of_bits];

    for (i = 0; i < no_of_bits; i++)
    {
        bit = value & MASK;

        if (bit == 0)
        {
            *--pt_bitstream = BIT_0;
        }
        else
        {
            *--pt_bitstream = BIT_1;
        }
        value = shr (value, 1);
    }
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:    Prm2bits
 *
 *  PURPOSE:     converts the encoder parameter vector into a vector of serial
 *               bits.
 *
 *  DESCRIPTION: depending on the mode, different numbers of parameters
 *               (with differing numbers of bits) are processed. Details
 *               are found in bitno.tab
 *
 *************************************************************************/
void Prm2bits (
    enum Mode mode,    /* i : AMR mode                                      */
    Word16 prm[],      /* i : analysis parameters (size <= MAX_PRM_SIZE)    */
    Word16 bits[]      /* o : serial bits         (size <= MAX_SERIAL_SIZE) */
)
{
   Word16 i;

              /* account for pointer init (bitno[mode])    */

   for (i = 0; i < prmno[mode]; i++)
   {
       Int2bin (prm[i], bitno[mode][i], bits);
       bits += bitno[mode][i];
       add(0,0);       /* account for above pointer update  */
   }      

   return;
}
