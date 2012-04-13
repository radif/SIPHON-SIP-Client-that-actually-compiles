/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : e_homing.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/

#include "e_homing.h"
const char e_homing_id[] = "@(#)$Id $" e_homing_h;


/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/

#include "typedef.h"
#include "cnst.h"


/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/


/*
********************************************************************************
*
*     Function        : encoder_homing_frame_test
*     In              : input_frame[]  one frame of speech samples
*     Out             : none
*     Calls           : none
*     Tables          : none
*     Compile Defines : none
*     Return          : 0  input frame does not match the encoder homing frame pattern
*                       1  input frame matches the encoder homing frame pattern
*     Information     : Checks if all samples of the input frame matches the encoder
*                       homing frame pattern, which is 0x0008 for all samples.
*
********************************************************************************
*/

Word16 encoder_homing_frame_test (Word16 input_frame[])
{
    Word16 i, j;

    /* check 160 input samples for matching EHF_MASK: defined in e_homing.h */
    for (i = 0; i < L_FRAME; i++)
    {
        j = input_frame[i] ^ EHF_MASK;

        if (j)
            break;
    }

    return !j;
}
