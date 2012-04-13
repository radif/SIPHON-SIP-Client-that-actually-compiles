/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : ex_ctrl.h
*      Purpose          : Excitation Control module in background noise
*
********************************************************************************
*/
#ifndef ex_ctrl_h
#define ex_ctrl_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define L_ENERGYHIST 60


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
*  Function    : Ex_ctrl
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = bgn, 1 = no bgn
*
**************************************************************************
*/
Word16 Ex_ctrl (Word16 excitation[],   /*i/o: Current subframe excitation   */
                Word16 excEnergy,      /* i : Exc. Energy, sqrt(totEx*totEx)*/
                Word16 exEnergyHist[], /* i : History of subframe energies  */
                Word16 voicedHangover, /* i : # of fr. after last voiced fr.*/
                Word16 prevBFI,        /* i : Set i previous BFI            */
                Word16 carefulFlag     /* i : Restrict dymamic in scaling   */
);
 
#endif


