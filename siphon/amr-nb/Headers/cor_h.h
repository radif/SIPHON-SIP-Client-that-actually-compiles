/*
*****************************************************************************
*                                                                     
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*                                                                     
*****************************************************************************
*                                                                     
*      File             : cor_h.h                                     
*      Purpose          : correlation functions for codebook search   
*                                                                     
*****************************************************************************
*/                                                                    
/*                                                                    
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID          
*****************************************************************************
*/
#ifndef cor_h_h
#define cor_h_h "@(#)$Id $"
/*
*****************************************************************************
*                         INCLUDE FILES                                      
*****************************************************************************
*/                                                                           
#include "typedef.h"                                                         
#include "cnst.h"                                                            
                                                                             
/*                                                                           
*****************************************************************************
*                         DECLARATION OF PROTOTYPES                          
*****************************************************************************
*/
void cor_h_x (
    Word16 h[],     /* (i) : impulse response of weighted synthesis filter */
    Word16 x[],     /* (i) : target                                        */
    Word16 dn[],    /* (o) : correlation between target and h[]            */
    Word16 sf       /* (i) : scaling factor: 2 for 12.2, 1 for 7.4         */
);

void cor_h_x2 (
    Word16 h[],     /* (i) : impulse response of weighted synthesis filter */
    Word16 x[],     /* (i) : target                                        */
    Word16 dn[],    /* (o) : correlation between target and h[]            */
    Word16 sf,      /* (i) : scaling factor: 2 for 12.2, 1 for 7.4         */
    Word16 nb_track,/* (i) : the number of ACB tracks                      */
    Word16 step     /* (i) : step size from one pulse position to the next
                             in one track                                  */
);

void cor_h (
    Word16 h[],     /* (i) : impulse response of weighted synthesis filter */
    Word16 sign[],      /* (i) : sign of d[n]                              */
    Word16 rr[][L_CODE] /* (o) : matrix of autocorrelation                 */
);

#endif
  
