#!/bin/sh

TEST='00_515 02_515 04_515 '
TEST+='00_795 02_795 04_795 '
TEST+='00_122 02_122 04_122 '

for i in $TEST
do
time ../decoder T$i.COD T$i.dec

if (cmp T$i.dec T$i.OUT); then
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
