/*****************************************************************************
 *  $Id $
 *
 *  This file contains operations in double precision.                       *
 *  These operations are not standard double precision operations.           *
 *  They are used where single precision is not enough but the full 32 bits  *
 *  precision is not necessary. For example, the function Div_32() has a     *
 *  24 bits precision which is enough for our purposes.                      *
 *                                                                           *
 *  The double precision numbers use a special representation:               *
 *                                                                           *
 *     L_32 = hi<<16 + lo<<1                                                 *
 *                                                                           *
 *  L_32 is a 32 bit integer.                                                *
 *  hi and lo are 16 bit signed integers.                                    *
 *  As the low part also contains the sign, this allows fast multiplication. *
 *                                                                           *
 *      0x8000 0000 <= L_32 <= 0x7fff fffe.                                  *
 *                                                                           *
 *  We will use DPF (Double Precision Format )in this file to specify        *
 *  this special format.                                                     *
 *****************************************************************************
*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"

#if 1
/*****************************************************************************
 * Function Mpy_32_16()                                                      *
 *                                                                           *
 *   Multiply a 16 bit integer by a 32 bit (DPF). The result is divided      *
 *   by 2**15                                                                *
 *                                                                           *
 *                                                                           *
 *   L_32 = (hi1*lo2)<<1 + ((lo1*lo2)>>15)<<1                                *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi          hi part of 32 bit number.                                    *
 *  lo          lo part of 32 bit number.                                    *
 *  n           16 bit number.                                               *
 *                                                                           *
 *****************************************************************************
*/

Word32 Mpy_32_16 (Word16 hi, Word16 lo, Word16 n)
{
#if 0
    Word32 L_32;

    L_32 = L_mult (hi, n);
    L_32 = L_mac (L_32, mult (lo, n), 1);
    /*L_32 += mult(lo, n) << 1;
    L_32 += L_mult(lo, n) >> 15;*/

    return (L_32);
#else
    register Word32 ra = hi;
    register Word32 rb = lo;
    register Word32 rc = n;
    Word32 out, out1;

    __asm__("smulbb %0, %2, %4 \n\t"
            "qadd %0, %0, %0   \n\t"
            "smulbb %1, %3, %4 \n\t"
            "mov %1, %1, ASR #15 \n\t"
            "qdadd %0, %0, %1   \n\t"
            : "=r"(out), "=r"(out1)
            : "r"(ra), "r"(rb), "r"(rc));

    return out;
#endif
}
#endif
/*****************************************************************************
 *                                                                           *
 *   Function Name : Div_32                                                  *
 *                                                                           *
 *   Purpose :                                                               *
 *             Fractional integer division of two 32 bit numbers.            *
 *             L_num / L_denom.                                              *
 *             L_num and L_denom must be positive and L_num < L_denom.       *
 *             L_denom = denom_hi<<16 + denom_lo<<1                          *
 *             denom_hi is a normalize number.                               *
 *                                                                           *
 *   Inputs :                                                                *
 *                                                                           *
 *    L_num                                                                  *
 *             32 bit long signed integer (Word32) whose value falls in the  *
 *             range : 0x0000 0000 < L_num < L_denom                         *
 *                                                                           *
 *    L_denom = denom_hi<<16 + denom_lo<<1      (DPF)                        *
 *                                                                           *
 *       denom_hi                                                            *
 *             16 bit positive normalized integer whose value falls in the   *
 *             range : 0x4000 < hi < 0x7fff                                  *
 *       denom_lo                                                            *
 *             16 bit positive integer whose value falls in the              *
 *             range : 0 < lo < 0x7fff                                       *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *    L_div                                                                  *
 *             32 bit long signed integer (Word32) whose value falls in the  *
 *             range : 0x0000 0000 <= L_div <= 0x7fff ffff.                  *
 *                                                                           *
 *  Algorithm:                                                               *
 *                                                                           *
 *  - find = 1/L_denom.                                                      *
 *      First approximation: approx = 1 / denom_hi                           *
 *      1/L_denom = approx * (2.0 - L_denom * approx )                       *
 *                                                                           *
 *  -  result = L_num * (1/L_denom)                                          *
 *****************************************************************************
*/

Word32 Div_32 (Word32 L_num, Word16 denom_hi, Word16 denom_lo)
{
    Word16 approx, hi, lo, n_hi, n_lo;
    Word32 L_32;

    /* First approximation: 1 / L_denom = 1/denom_hi */

    approx = div_s ((Word16) 0x3fff, denom_hi);

    /* 1/L_denom = approx * (2.0 - L_denom * approx) */

    L_32 = Mpy_32_16 (denom_hi, denom_lo, approx);

    L_32 = L_sub (MAX_32, L_32);

    hi = (Word16)(L_32 >> 16);
    lo = (L_32 >> 1) - (hi << 15);

    L_32 = Mpy_32_16 (hi, lo, approx);

    /* L_num * (1/L_denom) */
    hi = (Word16)(L_32 >> 16);
    lo = (L_32 >> 1) - (hi << 15);
    n_hi = (Word16)(L_num >> 16);
    n_lo = (L_num >> 1) - (n_hi << 15);
    L_32 = Mpy_32 (n_hi, n_lo, hi, lo);
    L_32 = L_shl (L_32, 2);

    return (L_32);
}
