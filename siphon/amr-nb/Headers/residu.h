/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : residu.h
*      Purpose          : Computes the LP residual.
*      Description      : The LP residual is computed by filtering the input
*                       : speech through the LP inverse filter A(z).
*
*
********************************************************************************
*/
#ifndef residu_h
#define residu_h "$Id $"
 
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
 
void Residu (
    Word16 a[],        /* (i)  : prediction coefficients                    */
    Word16 x[],        /* (i)  : speech signal                              */
    Word16 y[],        /* (o)  : residual signal                            */
    Word16 lg          /* (i)  : size of filtering                          */
);
 
#endif
