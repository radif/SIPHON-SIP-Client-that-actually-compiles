/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : az_lsp.h
*
********************************************************************************
*/
#ifndef az_lsp_h
#define az_lsp_h "$Id $"
 
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
/*
**************************************************************************
*
*  Function    : Az_lsp
*  Purpose     : Compute the LSPs from the LP coefficients
*  Description : - The sum and difference filters are computed
*                  and divided by 1+z^{-1} and 1-z^{-1}, respectively.
*
*                  f1[i] = a[i] + a[11-i] - f1[i-1] ;   i=1,...,5
*                  f2[i] = a[i] - a[11-i] + f2[i-1] ;   i=1,...,5
*
*                - The roots of F1(z) and F2(z) are found using
*                  Chebyshev polynomial evaluation. The polynomials
*                  are evaluated at 60 points regularly spaced in the
*                  frequency domain. The sign change interval is
*                  subdivided 4 times to better track the root. The
*                  LSPs are found in the cosine domain [1,-1].
*
*                - If less than 10 roots are found, the LSPs from 
*                  the past frame are used.
*  Returns     : void
*
**************************************************************************
*/
void Az_lsp (
    Word16 a[],        /* (i)  : predictor coefficients (MP1)              */
    Word16 lsp[],      /* (o)  : line spectral pairs (M)                   */
    Word16 old_lsp[]   /* (i)  : old lsp[] (in case not found 10 roots) (M)*/
);
 
#endif
