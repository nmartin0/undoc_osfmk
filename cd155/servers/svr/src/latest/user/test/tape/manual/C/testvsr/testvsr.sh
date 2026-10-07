# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
:	#  The ':' assures that this will run under Bourne shell
#
#  Run the test to read the datasets written by "testvsw" with variable spanned
#  (VS) and variable blocked & spanned (VR) format
#
#  Syntax: testvsr.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: testvsr.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  VS/VR READ TEST *****" | tee -a testvsr.SLr0
echo "" | tee -a testvsr.SLr0
echo "please PREPARE 3 tapes written by TESTVSW and LOAD 1st tape on STK 4980, " | tee -a testvsr.SLr0
echo "then press return to continue ..." | tee -a testvsr.SLr0
read response
         

d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`


TYPE="-t 1"
NUM=1

getcube $SRM $TYPE $CUBENAME  > scratch 2>&1
load 0 node.rx  
waitcube
relcube
cat scratch | tee -a testvsr.SLr0

# Execute time
d=`expr \( \`date '+%j'\` - $d1 \) \* 86400`
h=`expr \( \`date '+%H'\` - $h1 \) \* 3600`
m=`expr \( \`date '+%M'\` - $m1 \) \* 60`
execTime=`expr $d + $h + $m + \( \`date '+%S'\` - $s1 \)`


#
#  Check for test failure
#

if grep 'PASSED' scratch > /dev/null
then
 echo "T90_RESULT: P `hostname` $execTime" | tee -a testvsr.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > testvsr.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a testvsr.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > testvsr.SRr0
fi

echo "" | tee -a testvsr.SLr0
echo "*****  VS/VR READ TEST COMPLETE *****" | tee -a testvsr.SLr0
