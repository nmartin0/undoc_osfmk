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
#  Run the test to read the datasets using multiple devices
#
#  Syntax: test7.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test7.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  MULTIPLE DEVICES READ TEST *****" | tee -a test7.SLr0
echo "" | tee -a test7.SLr0
echo "Use TEST01-TEST04 written by \"test8\" on \"/cfs/ctape0\" & LOAD the 1st tape," |
  tee -a test7.SLr0
echo "    TEST11-TEST14 written by \"test8\" on \"/cfs/ctape1\" & LOAD the 1st tape," |
  tee -a test7.SLr0
echo "then press return to continue ..." | tee -a test7.SLr0

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
cat scratch | tee -a test7.SLr0

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
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test7.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test7.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test7.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test7.SRr0
fi

echo "" | tee -a test7.SLr0
echo "*****  MULTIPLE DEVICES READ TEST COMPLETE *****" | tee -a test7.SLr0
