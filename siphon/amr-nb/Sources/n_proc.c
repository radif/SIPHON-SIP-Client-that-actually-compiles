#include <stdio.h>
#include <stdlib.h>
/* $Id $ */

void proc_head (char *mes)
{
    fprintf(stderr,"\n\
**************************************************************\n\
\n\
     European digital cellular telecommunications system\n\
           4750 ... 12200 bits/s speech codec for\n\
         Adaptive Multi-Rate speech traffic channels\n\
\n\
     Bit-Exact C Simulation Code - %s\n\
\n\
     R98:   Version 7.6.0  \n\
     R99:   Version 3.3.0  \n\
     REL-4: Version 4.1.0   December 12, 2001\n\
     REL-5: Version 5.1.0   March 26, 2003\n\
**************************************************************\n\n",
            mes);

}
