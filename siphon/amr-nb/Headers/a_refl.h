/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : a_refl.h
*      Purpose          : Convert from direct form coefficients to 
*                         reflection coefficients
*
********************************************************************************
*/
#ifndef a_refl_h
#define a_refl_h "$Id $"

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

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

/*************************************************************************
 *
 *   FUNCTION:  A_Refl()
 *
 *   PURPOSE: Convert from direct form coefficients to reflection coefficients
 *
 *   DESCRIPTION:
 *       Directform coeffs in Q12 are converted to 
 *       reflection coefficients Q15 
 *
 *************************************************************************/
void A_Refl(
   Word16 a[],	      /* i   : Directform coefficients */
   Word16 refl[]      /* o   : Reflection coefficients */
);

#endif
