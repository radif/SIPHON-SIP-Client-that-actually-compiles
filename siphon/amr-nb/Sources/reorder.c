/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : reorder.c
*      Purpose          : To make sure that the LSFs are properly ordered
*                       : and to keep a certain minimum distance between
*                       : adjacent LSFs. 
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "reorder.h"
const char reorder_id[] = "@(#)$Id $" reorder_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  Reorder_lsf()
 *
 *  PURPOSE: To make sure that the LSFs are properly ordered and to keep a
 *           certain minimum distance between adjacent LSFs.
 *
 *           The LSFs are in the frequency range 0-0.5 and represented in Q15
 *
 *************************************************************************/
void Reorder_lsf (
    Word16 *lsf,        /* (i/o)     : vector of LSFs   (range: 0<=val<=0.5) */
    Word16 min_dist,    /* (i)       : minimum required distance             */
    Word16 n            /* (i)       : LPC order                             */
)
{
    Word16 i;
    Word16 lsf_min;

    lsf_min = min_dist;
    for (i = 0; i < n; i++)
    {
        if (sub (lsf[i], lsf_min) < 0)
        {
            lsf[i] = lsf_min;
        }
        lsf_min = add (lsf[i], min_dist);
    }
}
