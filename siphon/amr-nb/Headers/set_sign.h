/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : set_sign.h
*      Purpose          : Builds sign vector according to "dn[]" and "cn[]".
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#ifndef set_sign_h
#define set_sign_h "@(#)$Id $"
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
void set_sign(Word16 dn[],   /* i/o : correlation between target and h[]    */
              Word16 sign[], /* o   : sign of dn[]                          */
              Word16 dn2[],  /* o   : maximum of correlation in each track. */
              Word16 n       /* i   : # of maximum correlations in dn2[]    */
);

void set_sign12k2 (
    Word16 dn[],      /* i/o : correlation between target and h[]         */
    Word16 cn[],      /* i   : residual after long term prediction        */
    Word16 sign[],    /* o   : sign of d[n]                               */
    Word16 pos_max[], /* o   : position of maximum correlation            */
    Word16 nb_track,  /* i   : number of tracks tracks                    */        
    Word16 ipos[],    /* o   : starting position for each pulse           */
    Word16 step       /* i   : the step size in the tracks                */        
);

#endif
