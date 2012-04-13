/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : strfunc.c
*      Purpose          : string <-> mode and string <-> traffic channel
*                         conversion functions
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "strfunc.h"
const char strfunc_id[] = "@(#)$Id $" strfunc_h;
 
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <string.h>

#include "mode.h"
#include "frame.h"


/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
 * generic string <-> id lookup table
 */
typedef struct {
    char *name; /* name string */
    int   id;   /* integer id  */
} conv_table;

/*
 * mode name <-> mode id conversion table
 */
static const conv_table modetable[] = {
    {"MR475", MR475},
    {"MR515", MR515},
    {"MR59",  MR59},
    {"MR67",  MR67},
    {"MR74",  MR74},
    {"MR795", MR795},
    {"MR102", MR102},
    {"MR122", MR122},
    {"MRDTX", MRDTX},
    {NULL,    -1}
};

/*
 * frame type name <-> frame type id conversion table
 */
static const conv_table rxframetable[] = {
  {"RX_SPEECH_GOOD", RX_SPEECH_GOOD},
  {"RX_SPEECH_DEGRADED", RX_SPEECH_DEGRADED},
  {"RX_SPEECH_BAD", RX_SPEECH_BAD},
  {"RX_SID_FIRST", RX_SID_FIRST},
  {"RX_SID_UPDATE", RX_SID_UPDATE},
  {"RX_SID_BAD", RX_SID_BAD},
  {"RX_ONSET", RX_ONSET},
  {"RX_NO_DATA", RX_NO_DATA},
  {NULL, -1}
};
/*
 * frame type name <-> frame type id conversion table
 */
static const conv_table txframetable[] = {
  {"TX_SPEECH_GOOD", TX_SPEECH_GOOD},
  {"TX_SPEECH_DEGRADED", TX_SPEECH_DEGRADED},
  {"TX_SPEECH_BAD", TX_SPEECH_BAD},
  {"TX_SID_FIRST", TX_SID_FIRST},
  {"TX_SID_UPDATE", TX_SID_UPDATE},
  {"TX_SID_BAD", TX_SID_BAD},
  {"TX_ONSET", TX_ONSET},
  {"TX_NO_DATA", TX_NO_DATA},
  {NULL, -1}
};

/*
*****************************************************************************
*                         LOCAL PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   search_string
*  Purpose:    search table for string; return id if found, -1 otherwise
*
**************************************************************************
*/
static int search_string(const conv_table *p, const char *str)
{
    if (str == NULL)
        return -1;
    
    do
    {
        if (strcmp(p->name, str) == 0)
            return p->id;
        p++;
    } while (p->name != NULL);

    return -1;
}
    
/*************************************************************************
*
*  Function:   search_id
*  Purpose:    search table for id; return string if found, NULL otherwise
*
**************************************************************************
*/
static char *search_id(const conv_table *p, int id)
{
    if (id == -1)
        return NULL;
    
    do
    {
        if (p->id == id)
            return p->name;
        p++;
    } while (p->name != NULL);

    return NULL;
}
    
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   str2mode
*  Purpose:    convert AMR mode string into mode id; return 0 on success,
*              1 otherwise
*
**************************************************************************
*/
int str2mode(const char* str, enum Mode *mode)
{
    int m;

    if ((m = search_string(modetable, str)) == -1)
        return 1;

    *mode = (enum Mode) m;
    return 0;
}

/*************************************************************************
*
*  Function:   mode2str
*  Purpose:    convert AMR mode id into mode name string; return 0 on success,
*              1 otherwise
*
**************************************************************************
*/
int mode2str(enum Mode mode, char** str)
{
    return ((*str = search_id(modetable, (int) mode)) != NULL);
}

/*************************************************************************
*
*  Function:   frame2str
*
**************************************************************************
*/
int rxframe2str(enum RXFrameType ft, char** str)
{
    return ((*str = search_id(rxframetable, (int) ft)) != NULL);
}

/*************************************************************************
*
*  Function:   txframe2str
*
**************************************************************************
*/
int txframe2str(enum TXFrameType ft, char** str)
{
    return ((*str = search_id(txframetable, (int) ft)) != NULL);
}
