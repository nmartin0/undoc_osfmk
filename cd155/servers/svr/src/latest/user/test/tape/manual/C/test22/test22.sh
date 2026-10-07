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
#  Run the test to write 31 datasets with no data;
#
#  Syntax: test22.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test22.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  TEST22: WRITE 31 EMPTY DATASET TEST *****" | tee -a test22.SLr0
echo "" | tee -a test22.SLr0
echo "ASSUMING 1 tape has been loaded... " | tee -a test22.SLr0
echo "" | tee -a test22.SLr0
echo "starting nsh -s to run make scratch tape \"ms\" utilility" | tee -a test22.SLr0
getcube -t4
nsh -s << EOF
ms -f -vtest01 -neval /cfs/ctape0
exit
EOF
relcube 
echo "nsh -s done" | tee -a test22.SLr0
         
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
cat scratch | tee -a test22.SLr0

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
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test22.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test22.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test22.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test22.SRr0
fi

echo "" | tee -a test22.SLr0
echo "*****  TEST22: WRITE 31 EMPTY DATASET TEST COMPLETE *****" | tee -a test22.SLr0
