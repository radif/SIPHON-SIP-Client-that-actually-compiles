#!/bin/sh

TEST='00 02 04';
SPEED='515 795 122';


for i in $TEST
do
for j in $SPEED
do
time ../encoder MR$j T$i.INP T${i}_${j}.enc

if (cmp T${i}_${j}.enc T${i}_${j}.COD); then
    echo "##################################################"
    echo "# AMR decoder executable installation successful #"
    echo "##################################################"
else
    echo "#########################################################"
    echo "# \!\!\! ERROR in AMR decoder installation verification \!\!\!#"
    echo "#########################################################"
    exit -1;
fi
done
done
