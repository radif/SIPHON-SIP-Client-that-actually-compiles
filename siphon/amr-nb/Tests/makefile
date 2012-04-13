#****************************************************************
#
#      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
#                                R99   Version 3.3.0                
#                                REL-4 Version 4.1.0                
#
#****************************************************************
#
#      File             : makefile
#      Purpose          : gcc makefile for AMR SPC fixed point library
#                       : and standalone encoder/decoder program
#
#                             make [MODE=DEBUG] [VAD=VAD#] [target [target...]]
#
#                         Important targets are:
#                             default           (same as not specifying a
#                                                target at all)
#                                               remove all objects and libs;
#                                               build libraries; then build
#                                               encoder & decoder programs
#                             depend            make new dependency list
#                             clean             Remove all object/executable/
#                                               verification output files
#                             clean_depend      Clean dependency list
#                             clean_all         clean & clean_depend & rm *.a
#
#
#                         Specifying MODE=DEBUG compiles in debug mode
#                         (libaries compiled in DEBUG mode will be linked)
#
#                         Specifying VAD=VAD1 compiles VAD option 1
#                         Specifying VAD=VAD2 compiles VAD option 2
#
#                         The makefile uses the GNU C compiler (gcc); change
#                         the line CC=gcc below if another compiler is desired
#                         (CFLAGSxxx probably must be changed then as well)
#                         
#
# $Id $
#
#****************************************************************

SRCDIR = ../Sources
OBJDIR = ../Build
EXEDIR = ../Bin

#CC = gcc
CC = /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.0 \
	-isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS2.2.sdk
MAKEFILENAME = makefile

# Use MODE=DEBUG for debuggable library (default target builds both)
#
# default mode = NORM ==> no debug
#
MODE=NORM

# Use VAD=VAD1 for VAD option 1, or VAD=VAD2 for VAD option 2
#
# default mode = VAD1
#
VAD=VAD1

#
# compiler flags (for normal, DEBUG compilation)
#
CFLAGS_NORM  = -arch armv6 -mno-thumb -Os 
CFLAGS_DEBUG = -arch armv6 -mno-thumb -O0 -gdwarf-2 -DDEBUG 

CFLAGS = -Wall -pedantic-errors -I. -I../Headers $(CFLAGS_$(MODE)) -D$(VAD)
CFLAGSDEPEND = -MM $(CFLAGS)                    # for make depend


TMP=$(MODE:NORM=)
TMP2=$(TMP:DEBUG=_debug)
#
# construct SPC library name:
#   spc.a        in normal or wmops mode
#   spc_debug.a  in debug mode (MODE=DEBUG)
#
SPCLIB=spc$(TMP2:WMOPS=).a

#
# construct FIP operation library name:
#   fipop.a        in normal mode
#   fipop_debug.a  in debug mode (MODE=DEBUG)
#
FIPOPLIB=fipop$(TMP2:WMOPS=_wmops).a


#
# source/object files
#
SPC_OBJS_TMP=  agc.o autocorr.o az_lsp.o bits2prm.o \
       cl_ltp.o convolve.o c1035pf.o d_plsf.o d_plsf_5.o \
       d_gain_c.o d_gain_p.o dec_lag6.o d1035pf.o cor_h.o \
       enc_lag3.o enc_lag6.o g_code.o g_pitch.o int_lpc.o \
       inter_36.o inv_sqrt.o \
       lag_wind.o levinson.o lsp_az.o lsp_lsf.o ol_ltp.o \
       pitch_fr.o pitch_ol.o pow2.o pre_big.o pre_proc.o pred_lt.o preemph.o \
       prm2bits.o \
       pstfilt.o q_gain_c.o q_gain_p.o q_plsf.o q_plsf_5.o lsfwt.o reorder.o \
       residu.o lsp.o lpc.o ec_gains.o spreproc.o syn_filt.o \
       weight_a.o qua_gain.o gc_pred.o q_plsf_3.o post_pro.o \
       dec_lag3.o dec_gain.o d_plsf_3.o d4_17pf.o c4_17pf.o d3_14pf.o \
       c3_14pf.o \
       d2_11pf.o c2_11pf.o d2_9pf.o c2_9pf.o cbsearch.o spstproc.o gain_q.o \
       cod_amr.o dec_amr.o sp_enc.o sp_dec.o ph_disp.o \
       g_adapt.o calc_en.o qgain795.o qgain475.o sqrt_l.o set_sign.o s10_8pf.o \
       bgnscd.o gmed_n.o \
       ex_ctrl.o c_g_aver.o lsp_avg.o int_lsf.o c8_31pf.o d8_31pf.o \
       p_ol_wgh.o ton_stab.o vad1.o dtx_enc.o dtx_dec.o a_refl.o \
       b_cn_cod.o calc_cor.o hp_max.o vadname.o \
       vad2.o r_fft.o lflg_upd.o \
       e_homing.o d_homing.o

SPC_OBJS = $(foreach file, $(SPC_OBJS_TMP), $(OBJDIR)/$(file))

ENCODER_SRCS=coder.c 
DECODER_SRCS=decoder.c
FIPOP_SRCS=basicop2.c oper_32b.c log2.c \
           strfunc.c n_proc.c sid_sync.c
 
ENCODER_OBJS=$(ENCODER_SRCS:.c=.o) 
DECODER_OBJS=$(DECODER_SRCS:.c=.o)
FIPOP_OBJS_TMP=$(FIPOP_SRCS:.c=.o)
FIPOP_OBJS = $(foreach file, $(FIPOP_OBJS_TMP), $(OBJDIR)/$(file))

ALL_SRCS=$(ENCODER_SRCS) $(DECODER_SRCS) $(FIPOP_SRCS) $(SPC_OBJS:.o=.c)

#
# default target: build standalone speech encoder and decoder
#
default: clean_all spclib fipoplib encoder decoder


encoder: $(ENCODER_OBJS) $(SPCLIB) $(FIPOPLIB)
	$(CC) -o $(EXEDIR)/encoder $(CFLAGS) $(ENCODER_OBJS) $(SPCLIB) $(FIPOPLIB) $(LDFLAGS)

decoder: $(DECODER_OBJS) $(SPCLIB) $(FIPOPLIB)
	$(CC) -o $(EXEDIR)/decoder $(CFLAGS) $(DECODER_OBJS) $(SPCLIB) $(FIPOPLIB) $(LDFLAGS)

#
# how to compile a .c file into a .o
#
#.SUFFIXES: .c .h .o
#.c.o:
#	$(CC) -c $(CFLAGS) $(SRCDIR)/$<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

#
# build normal and DEBUG version of SPC library from scratch
#
spclib_allmodes:
	rm -f spc.a spc_debug.a
	$(MAKE) -f $(MAKEFILENAME) $(MFLAGS) $(MAKEDEFS) MODE=      clean spclib
	$(MAKE) -f $(MAKEFILENAME) $(MFLAGS) $(MAKEDEFS) MODE=DEBUG clean spclib

#
# build the speech coder library
#
spclib: $(SPC_OBJS)
	$(AR) rc $(SPCLIB) $(SPC_OBJS)
	ranlib $(SPCLIB)
#	arm-apple-darwin9-ranlib -static -arch_only armv6 -o $(SPCLIB) $(SPC_OBJS)


#
# build normal, DEBUG, and WMOPS version of FIP operation library from scratch
#
fipoplib_allmodes:
	rm -f fipop.a fipop_debug.a fipop_wmops.a
	$(MAKE) -f $(MAKEFILENAME) $(MFLAGS) $(MAKEDEFS)            clean fipoplib
	$(MAKE) -f $(MAKEFILENAME) $(MFLAGS) $(MAKEDEFS) MODE=DEBUG clean fipoplib
	$(MAKE) -f $(MAKEFILENAME) $(MFLAGS) $(MAKEDEFS) MODE=WMOPS clean fipoplib

#
# build the FIP operation library
#
fipoplib:	$(FIPOP_OBJS)
	$(AR) rc $(FIPOPLIB) $(FIPOP_OBJS)
	ranlib $(FIPOPLIB)


#
# make / clean dependency list
#
depend:
	$(MAKE) -f $(MAKEFILENAME) $(MFLAGS) $(MAKEDEFS) clean_depend
	$(CC) $(CFLAGSDEPEND) $(ALL_SRCS) >> $(MAKEFILENAME)

clean_depend:
	chmod u+w $(MAKEFILENAME)
	(awk 'BEGIN{f=1}{if (f) print $0}/^\# DO NOT DELETE THIS LINE -- make depend depends on it./{f=0}'\
	    < $(MAKEFILENAME) > .depend && \
	mv .depend $(MAKEFILENAME)) || exit 1;

#
# remove object/executable files
#
clean:
	rm -f $(OBJDIR)/*.o *.o core

clean_all: clean
	rm -f *.a $(EXEDIR)/encoder $(EXEDIR)/decoder
