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
#  Run the test to read 31 different datasets in random order with 
#	"tdsopen" position to the specific blocknum
#
#  Syntax: test1c.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test1c.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  TEST1C: READ 31 DATASETS USING BLOCKNUM (IN RANDOM) TEST *****" | tee -a test1c.SLr0
echo "" | tee -a test1c.SLr0
echo "ASSUMING data have been written thru \"test2\" or \"test6\"" | tee -a test1c.SLr0
echo "ASSUMING the tape has been loaded..." | tee -a test1c.SLr0
         
d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`


TYPE="-t 1"
NUM=1

getcube $SRM $TYPE $CUBENAME  > scratch.c 2>&1
load 0 node_c.rx  
waitcube
relcube
cat scratch.c | tee -a test1c.SLr0

# Execute time
d=`expr \( \`date '+%j'\` - $d1 \) \* 86400`
h=`expr \( \`date '+%H'\` - $h1 \) \* 3600`
m=`expr \( \`date '+%M'\` - $m1 \) \* 60`
execTime=`expr $d + $h + $m + \( \`date '+%S'\` - $s1 \)`


#
#  Check for test failure
#

if grep 'PASSED' scratch.c > /dev/null
then
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test1c.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test1c.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test1c.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test1c.SRr0
fi

echo "" | tee -a test1c.SLr0
echo "*****  TEST1C: READ 31 DATASETS USING BLOCKNUM (IN RANDOM) TEST COMPLETE *****" | tee -a test1c.SLr0
