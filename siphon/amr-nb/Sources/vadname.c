/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : vadname.c
*      Purpose          : check VAD option
*
*****************************************************************************
*/
 
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "vadname.h"
const char vadname_id[] = "@(#)$Id $" vadname_h;
 
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
 
/*************************************************************************
*
*  Function:   get_vadname
*  Purpose:    return pointer to string with name of VAD option
*              ("VAD1" or "VAD2")
*
**************************************************************************
*/
const char *get_vadname(void)
{
    static const char name[] = 
#if defined(VAD1)
        "VAD1 (ENS)";
#elif defined(VAD2)
        "VAD2 (Motorola)";
#else
        "unknown";
#endif
    
    return name;
}
