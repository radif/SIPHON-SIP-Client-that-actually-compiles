/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : int_lpc.c
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "int_lpc.h"
const char int_lpc_id[] = "@(#)$Id $" int_lpc_h;
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "cnst.h"
#include "lsp_az.h" 

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*
*--------------------------------------*
* Constants (defined in cnst.h)        *
*--------------------------------------*
*  M         : LPC order               *
*  MP1       : LPC order + 1           *
*--------------------------------------*
*/
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
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
)
{
    Word16 i;
    Word16 lsp[M];

    /*  lsp[i] = lsp_mid[i] * 0.5 + lsp_old[i] * 0.5 */

    for (i = 0; i < M; i++)
    {
        lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_old[i], 1));

    }

    Lsp_Az (lsp, Az);           /* Subframe 1 */
    Az += MP1;

    Lsp_Az (lsp_mid, Az);       /* Subframe 2 */
    Az += MP1;

    for (i = 0; i < M; i++)
    {
        lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_new[i], 1));

    }

    Lsp_Az (lsp, Az);           /* Subframe 3 */
    Az += MP1;

    Lsp_Az (lsp_new, Az);       /* Subframe 4 */

    return;
}

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
)
{
    Word16 i;
    Word16 lsp[M];

    /*  lsp[i] = lsp_mid[i] * 0.5 + lsp_old[i] * 0.5 */

    for (i = 0; i < M; i++)
    {
        lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_old[i], 1));

    }
    Lsp_Az (lsp, Az);           /* Subframe 1 */
    Az += MP1 * 2;

    for (i = 0; i < M; i++)
    {
        lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_new[i], 1));

    }
    Lsp_Az (lsp, Az);           /* Subframe 3 */

    return;
}
/*************************************************************************
 *
 *  FUNCTION:  Int_lpc_1to3()
 *
 *  PURPOSE:  Interpolates the LSPs and convert to LP parameters to get
 *            a different LP filter in each subframe.
 *
 *  DESCRIPTION:
 *     The 20 ms speech frame is divided into 4 subframes.
 *     The LSPs are quantized and transmitted at the 4th subframe
 *     (once per frame) and interpolated at the 1st, 2nd and 3rd subframe.
 *
 *          |------|------|------|------|
 *             sf1    sf2    sf3    sf4
 *       F0                          F1
 *
 *     sf1:   3/4 F0 + 1/4 F1         sf3:   1/4 F0 + 3/4 F1
 *     sf2:   1/2 F0 + 1/2 F1         sf4:       F1
 *
 *************************************************************************/
void Int_lpc_1to3(
    Word16 lsp_old[], /* input : LSP vector at the 4th SF of past frame    */
    Word16 lsp_new[], /* input : LSP vector at the 4th SF of present frame */
    Word16 Az[]       /* output: interpolated LP parameters in all SFs     */
)
{
    Word16 i;
    Word16 lsp[M];

    for (i = 0; i < M; i++) {
        lsp[i] = add(shr(lsp_new[i], 2), sub(lsp_old[i], shr(lsp_old[i], 2)));

    }

    Lsp_Az(lsp, Az);        /* Subframe 1 */
    Az += MP1;


    for (i = 0; i < M; i++) {
        lsp[i] = add(shr(lsp_old[i], 1), shr(lsp_new[i], 1));

    }

    Lsp_Az(lsp, Az);        /* Subframe 2 */
    Az += MP1;

    for (i = 0; i < M; i++) {
        lsp[i] = add(shr(lsp_old[i], 2), sub(lsp_new[i], shr(lsp_new[i], 2)));

    }

    Lsp_Az(lsp, Az);       /* Subframe 3 */
    Az += MP1;

    Lsp_Az(lsp_new, Az);        /* Subframe 4 */

    return;
}

/*************************************************************************
 * Function Int_lpc_1to3_2()
 * Interpolation of the LPC parameters.
 * Same as the previous function but we do not recompute Az() for
 * subframe 4 because it is already available.
 *************************************************************************/

void Int_lpc_1to3_2(
     Word16 lsp_old[],  /* input : LSP vector at the 4th SF of past frame    */
     Word16 lsp_new[],  /* input : LSP vector at the 4th SF of present frame */
     Word16 Az[]        /* output: interpolated LP parameters in SFs 1,2,3   */
)
{
    Word16 i;
    Word16 lsp[M];

    for (i = 0; i < M; i++) {
        lsp[i] = add(shr(lsp_new[i], 2), sub(lsp_old[i], shr(lsp_old[i], 2)));

    }

    Lsp_Az(lsp, Az);        /* Subframe 1 */
    Az += MP1;

    for (i = 0; i < M; i++) {
        lsp[i] = add(shr(lsp_old[i], 1), shr(lsp_new[i], 1));

    }

    Lsp_Az(lsp, Az);        /* Subframe 2 */
    Az += MP1;

    for (i = 0; i < M; i++) {
        lsp[i] = add(shr(lsp_old[i], 2), sub(lsp_new[i], shr(lsp_new[i], 2)));

    }

    Lsp_Az(lsp, Az);        /* Subframe 3 */

    return;
}
