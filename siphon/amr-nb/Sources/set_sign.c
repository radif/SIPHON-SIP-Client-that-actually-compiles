/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : set_sign.c
*      Purpose          : Builds sign vector according to "dn[]" and "cn[]".
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "set_sign.h"
const char set_sign_id[] = "@(#)$Id $" set_sign_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "inv_sqrt.h"
#include "cnst.h" 
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
 *
 *  FUNCTION  set_sign()
 *
 *  PURPOSE: Builds sign[] vector according to "dn[]" and "cn[]".
 *           Also finds the position of maximum of correlation in each track
 *           and the starting position for each pulse.
 *
 *************************************************************************/
void set_sign(Word16 dn[],   /* i/o : correlation between target and h[]    */
              Word16 sign[], /* o   : sign of dn[]                          */
              Word16 dn2[],  /* o   : maximum of correlation in each track. */
              Word16 n       /* i   : # of maximum correlations in dn2[]    */
)
{
   Word16 i, j, k;
   Word16 val, min;
   Word16 pos = 0; /* initialization only needed to keep gcc silent */
   
   /* set sign according to dn[] */
   
   for (i = 0; i < L_CODE; i++) {
      val = dn[i];
      

      if (val >= 0) {
         sign[i] = 32767;
      } else {
         sign[i] = -32767;
         val = negate(val);
      }
      dn[i] = val;     /* modify dn[] according to the fixed sign */
      dn2[i] = val;
   }
   
   /* keep 8-n maximum positions/8 of each track and store it in dn2[] */
   
   for (i = 0; i < NB_TRACK; i++)
   {
      for (k = 0; k < (8-n); k++)
      {
         min = 0x7fff;
         for (j = i; j < L_CODE; j += STEP)
         {

            if (dn2[j] >= 0)
            {
               val = sub(dn2[j], min);

               if (val < 0)
               {
                  min = dn2[j];
                  pos = j;
               }
            }
         }
         dn2[pos] = -1;
      }
   }
   
   return;
}

/*************************************************************************
 *
 *  FUNCTION  set_sign12k2()
 *
 *  PURPOSE: Builds sign[] vector according to "dn[]" and "cn[]", and modifies
 *           dn[] to include the sign information (dn[i]=sign[i]*dn[i]).
 *           Also finds the position of maximum of correlation in each track
 *           and the starting position for each pulse.
 *
 *************************************************************************/
void set_sign12k2 (
    Word16 dn[],      /* i/o : correlation between target and h[]         */
    Word16 cn[],      /* i   : residual after long term prediction        */
    Word16 sign[],    /* o   : sign of d[n]                               */
    Word16 pos_max[], /* o   : position of maximum correlation            */
    Word16 nb_track,  /* i   : number of tracks tracks                    */        
    Word16 ipos[],    /* o   : starting position for each pulse           */
    Word16 step       /* i   : the step size in the tracks                */        
)
{
    Word16 i, j;
    Word16 val, cor, k_cn, k_dn, max, max_of_all;
    Word16 pos = 0; /* initialization only needed to keep gcc silent */
    Word16 en[L_CODE];                  /* correlation vector */
    Word32 s;
 
    /* calculate energy for normalization of cn[] and dn[] */
 
    s = 256;
    for (i = 0; i < L_CODE; i++)
    {
        s = L_mac (s, cn[i], cn[i]);
    }
    s = Inv_sqrt (s);
    k_cn = extract_h (L_shl (s, 5));
    
    s = 256;
    for (i = 0; i < L_CODE; i++)
    {
        s = L_mac (s, dn[i], dn[i]);
    }
    s = Inv_sqrt (s);
    k_dn = extract_h (L_shl (s, 5));
    
    for (i = 0; i < L_CODE; i++)
    {
        val = dn[i];
        cor = round (L_shl (L_mac (L_mult (k_cn, cn[i]), k_dn, val), 10));
 

        if (cor >= 0)
        {
            sign[i] = 32767;                      /* sign = +1 */
        }
        else
        {
            sign[i] = -32767;                     /* sign = -1 */
            cor = negate (cor);
            val = negate (val);
        }
        /* modify dn[] according to the fixed sign */        
        dn[i] = val;
        en[i] = cor;
    }
    
    max_of_all = -1;
    for (i = 0; i < nb_track; i++)
    {
        max = -1;
        
        for (j = i; j < L_CODE; j += step)
        {
            cor = en[j];
            val = sub (cor, max);

            if (val > 0)
            {
                max = cor;
                pos = j;
            }
        }
        /* store maximum correlation position */
        pos_max[i] = pos;
        val = sub (max, max_of_all);

        if (val > 0)
        {
            max_of_all = max;
            /* starting position for i0 */            
            ipos[0] = i;
        }
    }
    
    /*----------------------------------------------------------------*
     *     Set starting position of each pulse.                       *
     *----------------------------------------------------------------*/
    
    pos = ipos[0];
    ipos[nb_track] = pos;
    
    for (i = 1; i < nb_track; i++)
    {
        pos = add (pos, 1);

        if (sub (pos, nb_track) >= 0)
        {
           pos = 0;
        }
        ipos[i] = pos;
        ipos[add(i, nb_track)] = pos;
    }
}
