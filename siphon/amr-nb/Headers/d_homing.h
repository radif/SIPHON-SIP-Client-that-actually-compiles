/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*      File             : d_homing.h
*      Purpose          : Declarations of decoder homing function prototypes.
*
********************************************************************************
*/

#ifndef d_homing_h
#define d_homing_h "$Id $"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/

#include "typedef.h"
#include "mode.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

Word16 decoder_homing_frame_test (Word16 input_frame[], enum Mode mode);
Word16 decoder_homing_frame_test_first (Word16 input_frame[], enum Mode mode);

#endif
