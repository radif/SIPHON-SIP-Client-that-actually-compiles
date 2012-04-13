/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*      File             : e_homing.h
*      Purpose          : Definition of encoder homing frame pattern and
*                         declaration of encoder homing function prototype.
*
********************************************************************************
*/

#ifndef e_homing_h
#define e_homing_h "$Id $"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/

#include "typedef.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

Word16 encoder_homing_frame_test (Word16 input_frame[]);

#endif
