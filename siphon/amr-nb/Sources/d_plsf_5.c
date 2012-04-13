/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : d_plsf_5.c
*
*****************************************************************************
*/
 
 
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "d_plsf.h"
const char d_plsf_5_id[] = "@(#)$Id $" d_plsf_h;
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "lsp_lsf.h"
#include "reorder.h"
#include "cnst.h"
#include "copy.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
#include "q_plsf_5.tab"         /* Codebooks of LSF prediction residual */


/* ALPHA    ->  0.95       */
/* ONE_ALPHA-> (1.0-ALPHA) */
#define ALPHA     31128
#define ONE_ALPHA 1639

/*
*--------------------------------------------------*
* Constants (defined in cnst.h)                    *
*--------------------------------------------------*
*  M                    : LPC order
*--------------------------------------------------*
*/
 
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : D_plsf_5
*  Purpose     : Decodes the 2 sets of LSP parameters in a frame 
*                using the received quantization indices.
*
**************************************************************************
*/
int D_plsf_5 (
    D_plsfState *st,    /* i/o: State variables                            */
    Word16 bfi,         /* i  : bad frame indicator (set to 1 if a bad    
                                frame is received)                         */  
    Word16 *indice,     /* i  : quantization indices of 5 submatrices, Q0  */
    Word16 *lsp1_q,     /* o  : quantized 1st LSP vector (M),          Q15 */
    Word16 *lsp2_q      /* o  : quantized 2nd LSP vector (M),          Q15 */
)
{
    Word16 i;
    const Word16 *p_dico;
    Word16 temp, sign;
    Word16 lsf1_r[M], lsf2_r[M];
    Word16 lsf1_q[M], lsf2_q[M];


    if (bfi != 0)                               /* if bad frame */
    {
        /* use the past LSFs slightly shifted towards their mean */

        for (i = 0; i < M; i++)
        {
            /* lsfi_q[i] = ALPHA*st->past_lsf_q[i] + ONE_ALPHA*mean_lsf[i]; */

            lsf1_q[i] = add (mult (st->past_lsf_q[i], ALPHA),
                             mult (mean_lsf[i], ONE_ALPHA));


            lsf2_q[i] = lsf1_q[i];
        }

        /* estimate past quantized residual to be used in next frame */

        for (i = 0; i < M; i++)
        {
            /* temp  = mean_lsf[i] +  st->past_r_q[i] * LSP_PRED_FAC_MR122; */

            temp = add (mean_lsf[i], mult (st->past_r_q[i],
                                           LSP_PRED_FAC_MR122));

            st->past_r_q[i] = sub (lsf2_q[i], temp);

        }
    }
    else
        /* if good LSFs received */
    {
        /* decode prediction residuals from 5 received indices */

        p_dico = &dico1_lsf[shl (indice[0], 2)];
        lsf1_r[0] = *p_dico++;
        lsf1_r[1] = *p_dico++;
        lsf2_r[0] = *p_dico++;
        lsf2_r[1] = *p_dico++;

        p_dico = &dico2_lsf[shl (indice[1], 2)];
        lsf1_r[2] = *p_dico++;
        lsf1_r[3] = *p_dico++;
        lsf2_r[2] = *p_dico++;
        lsf2_r[3] = *p_dico++;

        sign = indice[2] & 1;
        i = shr (indice[2], 1);
        p_dico = &dico3_lsf[shl (i, 2)];


        if (sign == 0)
        {
            lsf1_r[4] = *p_dico++;
            lsf1_r[5] = *p_dico++;
            lsf2_r[4] = *p_dico++;
            lsf2_r[5] = *p_dico++;
        }
        else
        {
            lsf1_r[4] = negate (*p_dico++);
            lsf1_r[5] = negate (*p_dico++);
            lsf2_r[4] = negate (*p_dico++);
            lsf2_r[5] = negate (*p_dico++);
        }

        p_dico = &dico4_lsf[shl (indice[3], 2)];
        lsf1_r[6] = *p_dico++;
        lsf1_r[7] = *p_dico++;
        lsf2_r[6] = *p_dico++;
        lsf2_r[7] = *p_dico++;

        p_dico = &dico5_lsf[shl (indice[4], 2)];
        lsf1_r[8] = *p_dico++;
        lsf1_r[9] = *p_dico++;
        lsf2_r[8] = *p_dico++;
        lsf2_r[9] = *p_dico++;

        /* Compute quantized LSFs and update the past quantized residual */
        for (i = 0; i < M; i++)
        {
            temp = add (mean_lsf[i], mult (st->past_r_q[i],
                                           LSP_PRED_FAC_MR122));
            lsf1_q[i] = add (lsf1_r[i], temp);

            lsf2_q[i] = add (lsf2_r[i], temp);

            st->past_r_q[i] = lsf2_r[i];
        }
    }

    /* verification that LSFs have minimum distance of LSF_GAP Hz */

    Reorder_lsf (lsf1_q, LSF_GAP, M);
    Reorder_lsf (lsf2_q, LSF_GAP, M);

    Copy (lsf2_q, st->past_lsf_q, M);

    /*  convert LSFs to the cosine domain */

    Lsf_lsp (lsf1_q, lsp1_q, M);
    Lsf_lsp (lsf2_q, lsp2_q, M);

    return 0;
}
