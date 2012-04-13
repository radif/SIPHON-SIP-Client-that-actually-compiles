/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : coder.c
*      Purpose          : Speech encoder main program.
*
*****************************************************************************
*
*    Usage : coder speech_file  bitstream_file
*
*    Format for speech_file:
*      Speech is read from a binary file of 16 bits data.
*
*    Format for bitstream_file:
*        1 word (2-byte) for the TX frame type
*          (see frame.h for possible values)
*      244 words (2-byte) containing 244 bits.
*          Bit 0 = 0x0000 and Bit 1 = 0x0001
*        1 word (2-byte) for the mode indication
*          (see mode.h for possible values)
*        4 words for future use, currently written as zero
*
*****************************************************************************
*/
 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "typedef.h"
#include "cnst.h"
#include "n_proc.h"
#include "mode.h"
#include "frame.h"
#include "strfunc.h"
#include "sp_enc.h"
#include "pre_proc.h"
#include "sid_sync.h"
#include "vadname.h"
#include "e_homing.h"

#ifdef MMS_IO
#define AMR_MAGIC_NUMBER "#!AMR\n"
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 2)
#endif

const char coder_id[] = "@(#)$Id $";

/* frame size in serial bitstream file (frame type + serial stream + flags) */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)

/*
*****************************************************************************
*                         LOCAL PROGRAM CODE
*****************************************************************************
*/
/*
 * read_mode  read next mode from mode file
 *
 * return 0 on success, EOF on end of file, 1 on other error
 */
int read_mode(FILE *file_modes, enum Mode *mode)
{
    char buf[10];
    
    if (fscanf(file_modes, "%9s\n", buf) != 1) {
        if (feof(file_modes))
            return EOF;

        fprintf(stderr, "\nerror reading mode control file: %s\n",
                strerror(errno));
        return 1;
    }

    if (str2mode(buf, mode) != 0 || *mode == MRDTX) {
        fprintf(stderr, "\ninvalid amr_mode found in mode control file: '%s'\n",
                buf);
        return 1;
    }

    return 0;
}


/*
*****************************************************************************
*                             MAIN PROGRAM 
*****************************************************************************
*/
int main (int argc, char *argv[])
{
  char *progname = argv[0];
  char *modeStr = NULL;
  char *usedModeStr = NULL;
  char *fileName = NULL;
  char *modefileName = NULL;
  char *serialFileName = NULL;
  
  FILE *file_speech = NULL;           /* File of speech data               */
  FILE *file_serial = NULL;           /* File of coded bits                */
  FILE *file_modes = NULL;            /* File with mode information        */
  
  Word16 new_speech[L_FRAME];         /* Pointer to new speech data        */
  Word16 serial[SERIAL_FRAMESIZE];    /* Output bitstream buffer           */

#ifdef MMS_IO
  UWord8 packed_bits[MAX_PACKED_SIZE];
  Word16 packed_size;
#endif

  Word32 frame;
  Word16 dtx = 0;                     /* enable encoder DTX                */
  
  /* changed eedodr */
  Word16 reset_flag;

  int i;
  enum Mode mode;
  enum Mode used_mode;
  enum TXFrameType tx_type;

  int useModeFile = 0;
  
  Speech_Encode_FrameState *speech_encoder_state = NULL;
  sid_syncState *sid_state = NULL;

  proc_head ("Encoder");
  fprintf(stderr, "Code compiled with VAD option: %s\n\n", get_vadname());

  /*----------------------------------------------------------------------*
   * Process command line options                                         *
   *----------------------------------------------------------------------*/
  while (argc > 1) {
      if (strcmp(argv[1], "-dtx") == 0) {
          dtx = 1;
      } else if (strncmp(argv[1], "-modefile=", 10) == 0) {
          useModeFile = 1;
          modefileName = argv[1]+10;
      } else
          break;
      argc--;
      argv++;
  }
  
  /*----------------------------------------------------------------------*
   * Check number of arguments                                            *
   *----------------------------------------------------------------------*/
  if (   (argc != 4 && !useModeFile)
      || (argc != 3 &&  useModeFile))
  {
    fprintf (stderr,
      " Usage:\n\n"
      "   %s [-dtx] amr_mode            speech_file  bitstream_file\n\n"
      " or \n\n"
      "   %s [-dtx] -modefile=mode_file speech_file  bitstream_file\n\n"
      " -dtx                enables DTX mode\n"
      " -modefile=mode_file reads AMR modes from text file (one line per frame)\n\n",
             progname, progname);
      exit (1);
  }

  /*----------------------------------------------------------------------*
   * Open mode file or convert mode string                                *
   *----------------------------------------------------------------------*/
  if (useModeFile) {
      fileName = argv[1];
      serialFileName = argv[2];
      
      /* Open mode control file */
      if (strcmp(modefileName, "-") == 0) {
          file_modes = stdin;
      }
      else if ((file_modes = fopen (modefileName, "rt")) == NULL)
      {
          fprintf (stderr, "Error opening mode control file  %s !!\n",
                   modefileName);
          exit (1);
      }
      fprintf (stderr, " Mode control file:      %s\n", modefileName);

  } else {
      
      modeStr = argv[1];
      fileName = argv[2];
      serialFileName = argv[3];
      
      /* check and convert mode string */
      if (str2mode(modeStr, &mode) != 0 && mode != MRDTX) {
          fprintf(stderr, "Invalid amr_mode specified: '%s'\n",
                  modeStr);
          exit(1);
      }
  }
  

  /*----------------------------------------------------------------------*
   * Open speech file and result file (output serial bit stream)          *
   *----------------------------------------------------------------------*/
  if (strcmp(fileName, "-") == 0) {
     file_speech = stdin;
  }
  else if ((file_speech = fopen (fileName, "rb")) == NULL)
  {
      fprintf (stderr, "Error opening input file  %s !!\n", fileName);
      exit (1);
  }
  fprintf (stderr, " Input speech file:      %s\n", fileName);

  if (strcmp(serialFileName, "-") == 0) {
     file_serial = stdout;
  }
  else if ((file_serial = fopen (serialFileName, "wb")) == NULL)
  {
      fprintf (stderr,"Error opening output bitstream file %s !!\n",serialFileName);
      exit (1);
  }
  fprintf (stderr, " Output bitstream file:  %s\n", serialFileName);


  /*-----------------------------------------------------------------------*
   * Initialisation of the coder.                                          *
   *-----------------------------------------------------------------------*/
  /* allocate memory */
  speech_encoder_state = (Speech_Encode_FrameState *) malloc(Speech_Encode_Frame_memSize());
  if (speech_encoder_state == NULL){
      fprintf(stderr, "Can not malloc state structure\n");
      exit(-1);
  }

  sid_state = (sid_syncState *) malloc(sid_sync_memSize());
  if (sid_state == NULL){
      fprintf(stderr, "Can not malloc state structure\n");
      exit(-1);
  }

  if (   Speech_Encode_Frame_init(speech_encoder_state, dtx)
      || sid_sync_init (sid_state))
      exit(-1);

#ifdef MMS_IO
  /* write magic number to indicate single channel AMR file storage format */
  fwrite(AMR_MAGIC_NUMBER, sizeof(UWord8), strlen(AMR_MAGIC_NUMBER), file_serial);
#endif

  /*-----------------------------------------------------------------------*
   * Process speech frame by frame                                         *
   *-----------------------------------------------------------------------*/
  frame = 0;
  while (fread (new_speech, sizeof (Word16), L_FRAME, file_speech) == L_FRAME)
  {
     /* read new mode string from file if required */
     if (useModeFile) {
         int res;
         if ((res = read_mode(file_modes, &mode)) == EOF) {
             fprintf(stderr, "\nend of mode control file reached");
             break;
         } else if (res == 1)
             exit(-1);
     }
      
     frame++;
     
     /* zero flags and parameter bits */
     for (i = 0; i < SERIAL_FRAMESIZE; i++)
         serial[i] = 0;

     /* check for homing frame */
     reset_flag = encoder_homing_frame_test(new_speech);
     
     /* encode speech */
     Speech_Encode_Frame(speech_encoder_state, mode,
                         new_speech, &serial[1], &used_mode); 

     /* print frame number and mode information */
     mode2str(mode, &modeStr);
     mode2str(used_mode, &usedModeStr);
     if ( (frame%50) == 0) {
        fprintf (stderr, "\rframe=%-8d mode=%-5s used_mode=%-5s",
                 frame, modeStr, usedModeStr);
     }

     /* include frame type and mode information in serial bitstream */
     sid_sync (sid_state, used_mode, &tx_type);

#ifndef MMS_IO
     serial[0] = tx_type;
     if (tx_type != TX_NO_DATA) {
       serial[1+MAX_SERIAL_SIZE] = mode;
     }
     else {
       serial[1+MAX_SERIAL_SIZE] = -1;
     }

     /* write bitstream to output file */
     if (fwrite (serial, sizeof (Word16), SERIAL_FRAMESIZE, file_serial)
         != SERIAL_FRAMESIZE) {
         fprintf(stderr, "\nerror writing output file: %s\n",
                 strerror(errno));
         exit(-1);
     }
#else

     packed_size = PackBits(used_mode, mode, tx_type, &serial[1], packed_bits);

     /* write file storage format bitstream to output file */
     if (fwrite (packed_bits, sizeof (UWord8), packed_size, file_serial)
         != packed_size) {
         fprintf(stderr, "\nerror writing output file: %s\n",
                 strerror(errno));
         exit(-1);
     }
#endif

     fflush(file_serial);

     /* perform homing if homing frame was detected at encoder input */
     if (reset_flag != 0)
     {
         Speech_Encode_Frame_reset(speech_encoder_state);
         sid_sync_reset(sid_state);
     }
  }
  fprintf (stderr, "\n%d frame(s) processed\n", frame);
  
  /*-----------------------------------------------------------------------*
   * Close down speech coder                                               *
   *-----------------------------------------------------------------------*/
  Speech_Encode_Frame_reset(speech_encoder_state);
  sid_sync_reset (sid_state);
  free(sid_state);
  free(speech_encoder_state);

  return (0);
}
