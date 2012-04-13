#!/bin/sh
#
# Unix shell script to check correct installation of AMR
# speech encoder and decoder
#
# $Id $

if test "$1" = "-vad2"; then
    vad=2;
    shift;
elif test "$1" = "-vad1"; then
    vad=1;
    shift;
else
    vad=1;
fi
echo toto $1 $vad;
if test "$1" = "unix"; then
    BASEin=spch_unx;
    if ([ $vad == 1 ]) then
        BASEout=$BASEin;
    else
        BASEout=spch_un2;
    fi
elif test "$1" = "dos"; then
    BASEin=spch_dos;
    if ([ $vad == 1 ]) then
        BASEout=$BASEin;
    else
        BASEout=spch_do2;
    fi
else
    echo "Use:    $0 [-vad2] dos"
    echo "  or    $0 [-vad2] unix"
    exit -1;
fi
    
time ./encoder -dtx -modefile=allmodes.txt $BASEin.inp tmp.cod
echo ""
#cmp tmp.cod $BASEout.cod

#if ([ $status == 0 ]) then
if (cmp tmp.cod $BASEout.cod); then
    echo "##################################################"
    echo "# AMR encoder executable installation successful #"
    echo "##################################################"
else
    echo "#########################################################"
    echo "# \!\!\! ERROR in AMR encoder installation verification \!\!\!#"
    echo "#########################################################"
    exit -1;
fi

time ./decoder $BASEout.cod tmp.out
echo ""
#cmp tmp.out $BASEout.out
#if ($status == 0) then
if (cmp tmp.out $BASEout.out); then
    echo "##################################################"
    echo "# AMR decoder executable installation successful #"
    echo "##################################################"
else
    echo "#########################################################"
    echo "# \!\!\! ERROR in AMR decoder installation verification \!\!\!#"
    echo "#########################################################"
    exit -1;
fi
