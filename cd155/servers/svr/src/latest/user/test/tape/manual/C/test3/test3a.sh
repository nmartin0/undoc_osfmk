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
#  Run the test to read multi-volumes and multi-datasets written by "test4";
#  skip reading the middle dataset of the three in the volumns
#
#  Syntax: test3a.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test3a.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  MULTI-VOLUME/MULTI-DS READ TEST (SKIP MID-DS) *****" | tee -a test3a.SLr0
echo "" | tee -a test3a.SLr0
echo "please PREPARE 4 tapes written by TEST4 and LOAD 1st tape on STK 4980, " | tee -a test3a.SLr0
echo "then press return to continue ..." | tee -a test3a.SLr0
read response
         

d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`


TYPE="-t 1"
NUM=1

getcube $SRM $TYPE $CUBENAME  > scratch.a 2>&1
load 0 node_a.rx  
waitcube
relcube
cat scratch.a | tee -a test3a.SLr0

# Execute time
d=`expr \( \`date '+%j'\` - $d1 \) \* 86400`
h=`expr \( \`date '+%H'\` - $h1 \) \* 3600`
m=`expr \( \`date '+%M'\` - $m1 \) \* 60`
execTime=`expr $d + $h + $m + \( \`date '+%S'\` - $s1 \)`


#
#  Check for test failure
#

if grep 'PASSED' scratch.a > /dev/null
then
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test3a.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test3a.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test3a.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test3a.SRr0
fi

echo "" | tee -a test3a.SLr0
echo "*****  MULTI-VOLUME/MULTI-DS READ TEST COMPLETE (SKIP MID-DS) *****" | tee -a test3a.SLr0
