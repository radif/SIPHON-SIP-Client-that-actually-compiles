
/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : decoder.c
*      Purpose          : Speech decoder main program.
*
********************************************************************************
*
*         Usage : decoder  bitstream_file  synth_file
*
*
*         Format for bitstream_file:
*             1 word (2-byte) for the frame type
*               (see frame.h for possible values)
*               Normally, the TX frame type is expected.
*               RX frame type can be forced with "-rxframetype"
*           244 words (2-byte) containing 244 bits.
*               Bit 0 = 0x0000 and Bit 1 = 0x0001
*             1 word (2-byte) for the mode indication
*               (see mode.h for possible values)
*             4 words for future use, currently unused
*
*         Format for synth_file:
*           Synthesis is written to a binary file of 16 bits data.
*
********************************************************************************
*/

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "typedef.h"
#include "n_proc.h"
#include "cnst.h"
#include "mode.h"
#include "frame.h"
#include "strfunc.h"
#include "sp_dec.h"
#include "d_homing.h"

#ifdef MMS_IO
#define AMR_MAGIC_NUMBER "#!AMR\n"
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 2)
#endif

const char decoder_id[] = "@(#)$Id $";

/* frame size in serial bitstream file (frame type + serial stream + flags) */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)

/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/
static enum RXFrameType tx_to_rx (enum TXFrameType tx_type)
{
    switch (tx_type) {
      case TX_SPEECH_GOOD:      return RX_SPEECH_GOOD;
      case TX_SPEECH_DEGRADED:  return RX_SPEECH_DEGRADED;
      case TX_SPEECH_BAD:       return RX_SPEECH_BAD;
      case TX_SID_FIRST:        return RX_SID_FIRST;
      case TX_SID_UPDATE:       return RX_SID_UPDATE;
      case TX_SID_BAD:          return RX_SID_BAD;
      case TX_ONSET:            return RX_ONSET;
      case TX_NO_DATA:          return RX_NO_DATA;
      default:
        fprintf(stderr, "tx_to_rx: unknown TX frame type %d\n", tx_type);
        exit(1);
    }
}

/*
********************************************************************************
*                             MAIN PROGRAM 
********************************************************************************
*/

int main (int argc, char *argv[])
{
  Speech_Decode_FrameState *speech_decoder_state = NULL;
  
  Word16 serial[SERIAL_FRAMESIZE];   /* coded bits                    */
  Word16 synth[L_FRAME];             /* Synthesis                     */
  Word32 frame;

  char *progname = argv[0];
  char *fileName = NULL;
  char *serialFileName = NULL;
  
  FILE *file_syn, *file_serial;

  int rxframetypeMode = 0;           /* use RX frame type codes       */
  enum Mode mode = (enum Mode)0;
  enum RXFrameType rx_type = (enum RXFrameType)0;
  enum TXFrameType tx_type = (enum TXFrameType)0;
     
  Word16 reset_flag = 0;
  Word16 reset_flag_old = 1;
  Word16 i;
  
#ifdef MMS_IO
  UWord8 toc, q, ft;
  Word8 magic[8];
  UWord8 packed_bits[MAX_PACKED_SIZE];
  Word16 packed_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};
#endif

   proc_head ("Decoder");

#ifndef MMS_IO
  /*----------------------------------------------------------------------*
   * process command line options                                         *
   *----------------------------------------------------------------------*/
  while (argc > 1) {
      if (strcmp(argv[1], "-rxframetype") == 0)
          rxframetypeMode = 1;
      else break;

      argc--;
      argv++;
  }
#endif

  /*----------------------------------------------------------------------*
   * check number of arguments                                            *
   *----------------------------------------------------------------------*/
  if (argc != 3)
  {
    fprintf (stderr,
      " Usage:\n\n"
#ifndef MMS_IO
      "   %s  [-rxframetype] bitstream_file synth_file\n\n"
      " -rxframetype expects the RX frame type in bitstream_file (instead of TX)\n\n",
#else
      "   %s  bitstream_file synth_file\n\n",
#endif
             progname);
      exit (1);
  }
  
  serialFileName = argv[1];
  fileName = argv[2];

  /*----------------------------------------------------------------------*
   * Open serial bit stream and output speech file                        *
   *----------------------------------------------------------------------*/
  if (strcmp(serialFileName, "-") == 0) {
     file_serial = stdin;
  }
  else if ((file_serial = fopen (serialFileName, "rb")) == NULL)
  {
      fprintf (stderr, "Input file '%s' does not exist !!\n", serialFileName);
      exit (0);
  }
  fprintf (stderr, "Input bitstream file:   %s\n", serialFileName);

  if (strcmp(fileName, "-") == 0) {
     file_syn = stdout;
  }
  else if ((file_syn = fopen (fileName, "wb")) == NULL)
  {
      fprintf (stderr, "Cannot create output file '%s' !!\n", fileName);
      exit (0);
  }
  fprintf (stderr, "Synthesis speech file:  %s\n", fileName);

#ifdef MMS_IO
   /* read and verify magic number */
  fread(magic, sizeof(Word8), strlen(AMR_MAGIC_NUMBER), file_serial);
  if (strncmp((const char *)magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
  {
	   fprintf(stderr, "%s%s\n", "Invalid magic number: ", magic);
	   fclose(file_serial);
	   fclose(file_syn);
	   return 1;
   }
#endif

  /*-----------------------------------------------------------------------*
   * Initialization of decoder                                             *
   *-----------------------------------------------------------------------*/
  /* allocate memory */
  speech_decoder_state = (Speech_Decode_FrameState *) malloc(Speech_Decode_Frame_memSize());
  if (speech_decoder_state == NULL){
      fprintf(stderr, "Can not malloc state structure\n");
      exit(-1);
  }

  if (Speech_Decode_Frame_init(speech_decoder_state, "Decoder"))
      exit(-1);
    
  /*-----------------------------------------------------------------------*
   * process serial bitstream frame by frame                               *
   *-----------------------------------------------------------------------*/
  frame = 0;

#ifndef MMS_IO
  while (fread (serial, sizeof (Word16), SERIAL_FRAMESIZE, file_serial)
         == SERIAL_FRAMESIZE)
  {
     /* get frame type and mode information from frame */
     if (rxframetypeMode) {
         rx_type = (enum RXFrameType)serial[0];
     } else {
         tx_type = (enum TXFrameType)serial[0];
         rx_type = tx_to_rx (tx_type);
     }
     mode = (enum Mode) serial[1+MAX_SERIAL_SIZE];

#else

  while (fread (&toc, sizeof(UWord8), 1, file_serial) == 1)
  {
	  /* read rest of the frame based on ToC byte */
	  q  = (toc >> 2) & 0x01;
	  ft = (toc >> 3) & 0x0F;
	  fread (packed_bits, sizeof(UWord8), packed_size[ft], file_serial);

	  rx_type = UnpackBits(q, ft, packed_bits, &mode, &serial[1]);

#endif

     ++frame;
     if ( (frame%50) == 0) {
        fprintf (stderr, "\rframe=%d  ", frame);
     }

     if (rx_type == RX_NO_DATA) {
       mode = speech_decoder_state->prev_mode;
     }
     else {
       speech_decoder_state->prev_mode = mode;
     }

     /* if homed: check if this frame is another homing frame */
     if (reset_flag_old == 1)
     {
         /* only check until end of first subframe */
         reset_flag = decoder_homing_frame_test_first(&serial[1], mode);
     }
     /* produce encoder homing frame if homed & input=decoder homing frame */
     if ((reset_flag != 0) && (reset_flag_old != 0))
     {
         for (i = 0; i < L_FRAME; i++)
         {
             synth[i] = EHF_MASK;
         }
     }
     else
     {     
         /* decode frame */
         Speech_Decode_Frame(speech_decoder_state, mode, &serial[1],
                             rx_type, synth);
     }
     
     /* write synthesized speech to file */
     if (fwrite (synth, sizeof (Word16), L_FRAME, file_syn) != L_FRAME) {
         fprintf(stderr, "\nerror writing output file: %s\n",
                 strerror(errno));
     };
     fflush(file_syn);

     /* if not homed: check whether current frame is a homing frame */
     if (reset_flag_old == 0)
     {
         /* check whole frame */
         reset_flag = decoder_homing_frame_test(&serial[1], mode);
     }
     /* reset decoder if current frame is a homing frame */
     if (reset_flag != 0)
     {
         Speech_Decode_Frame_reset(speech_decoder_state);
     }
     reset_flag_old = reset_flag;

  }
  fprintf (stderr, "\n%d frame(s) processed\n", frame);
  
  /*-----------------------------------------------------------------------*
   * Close down speech decoder                                             *
   *-----------------------------------------------------------------------*/
  Speech_Decode_Frame_reset(speech_decoder_state);
  free(speech_decoder_state);
  
  return 0;
}
