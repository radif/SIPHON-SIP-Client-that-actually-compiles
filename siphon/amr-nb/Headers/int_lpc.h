/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : int_lpc.h
*      Purpose          : Interpolates the LSPs and converts to LPC parameters 
*
********************************************************************************
*/
#ifndef int_lpc_h
#define int_lpc_h "$Id $"
 
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
*  Function    : Int_lpc_1and3
*  Purpose     : Interpolates the LSPs and converts to LPC parameters 
*                to get a different LP filter in each subframe.
*  Description : The 20 ms speech frame is divided into 4 subframes.
*                The LSPs are quantized and transmitted at the 2nd and 
*                4th subframes (twice per frame) and interpolated at the
*                1st and 3rd subframe.
*
*                      |------|------|------|------|
*                         sf1    sf2    sf3    sf4
*                   F0            Fm            F1
*      
*                 sf1:   1/2 Fm + 1/2 F0         sf3:   1/2 F1 + 1/2 Fm
*                 sf2:       Fm                  sf4:       F1
*  Returns     : void
*
**************************************************************************
*/
void Int_lpc_1and3 (
    Word16 lsp_old[],  /* i : LSP vector at the 4th subfr. of past frame (M) */
    Word16 lsp_mid[],  /* i : LSP vector at the 2nd subfr. of
                              present frame (M)                              */
    Word16 lsp_new[],  /* i : LSP vector at the 4th subfr. of
                              present frame (M)                              */
    Word16 Az[]        /* o : interpolated LP parameters in all subfr.
                              (AZ_SIZE)                                      */
);

/*
**************************************************************************
*
*  Function    : Int_lpc_1and3_2
*  Purpose     : Interpolation of the LPC parameters. Same as the Int_lpc
*                function but we do not recompute Az() for subframe 2 and
*                4 because it is already available.
*  Returns     : void
*
**************************************************************************
*/
void Int_lpc_1and3_2 (
    Word16 lsp_old[],  /* i : LSP vector at the 4th subfr. of past frame (M) */
    Word16 lsp_mid[],  /* i : LSP vector at the 2nd subframe of
                             present frame (M)                                  */
    Word16 lsp_new[],  /* i : LSP vector at the 4th subframe of
                             present frame (M)                                  */
    Word16 Az[]        /* o :interpolated LP parameters
                             in subframes 1 and 3 (AZ_SIZE)                     */
);

/*
**************************************************************************
*
*  Function    : Int_lpc_1to3
*  Purpose     : Interpolates the LSPs and converts to LPC parameters 
*                to get a different LP filter in each subframe.
*  Description : The 20 ms speech frame is divided into 4 subframes.
*                The LSPs are quantized and transmitted at the 4th 
*                subframes (once per frame) and interpolated at the
*                1st, 2nd and 3rd subframe.
*
*                      |------|------|------|------|
*                         sf1    sf2    sf3    sf4
*                   F0                          F1
*      
*                 sf1:   3/4 F0 + 1/4 F1         sf3:   1/4 F0 + 3/4 F1
*                 sf2:   1/2 F0 + 1/2 F1         sf4:       F1
*  Returns     : void
*
**************************************************************************
*/
void Int_lpc_1to3 (
    Word16 lsp_old[], /* i : LSP vector at the 4th SF of past frame (M)      */
    Word16 lsp_new[], /* i : LSP vector at the 4th SF of present frame (M)   */
    Word16 Az[]       /* o : interpolated LP parameters in all SFs (AZ_SIZE) */
);
 
/*
**************************************************************************
*
*  Function    : Int_lpc_1to3_2
*  Purpose     : Interpolation of the LPC parameters. Same as the Int_lpc
*                function but we do not recompute Az() for subframe 4
*                because it is already available.
*  Returns     : void
*
**************************************************************************
*/
void Int_lpc_1to3_2 (
    Word16 lsp_old[],  /* i : LSP vector at the 4th SF of past frame (M) */
    Word16 lsp_new[],  /* i : LSP vector at the 4th SF present frame (M) */
    Word16 Az[]        /* o :interpolated LP parameters in SFs 1, 2, 3 
                             (AZ_SIZE)                                   */
);
 
#endif
