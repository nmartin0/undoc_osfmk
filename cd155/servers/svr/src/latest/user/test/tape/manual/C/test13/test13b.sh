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
#  Run the test to read datasets made with feov by "test14a";   
#
#  Syntax: test13b.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test13b.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  FEOV MADE DATASET READ TEST *****" | tee -a test13b.SLr0
echo "" | tee -a test13b.SLr0
echo "please PREPARE tape 1-9 written by TEST14A and LOAD 1st tape on STK 4980, " | tee -a test13b.SLr0
echo "later, manually feed tape 10-32 and type TEST10 to TEST32 1 by 1 on screen" | tee -a test13b.SLr0
echo "Now, after INSERT tape 1-9, press return to continue ..." | tee -a test13b.SLr0

read response
         

d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`


TYPE="-t 1"
NUM=1
(getcube $SRM $TYPE $CUBENAME; load 0 node_b.rx; waitcube; relcube) | tee -a scratch.b
cat test13b.SLr0 scratch.b > tempfile
mv tempfile test13b.SLr0


# Execute time
d=`expr \( \`date '+%j'\` - $d1 \) \* 86400`
h=`expr \( \`date '+%H'\` - $h1 \) \* 3600`
m=`expr \( \`date '+%M'\` - $m1 \) \* 60`
execTime=`expr $d + $h + $m + \( \`date '+%S'\` - $s1 \)`


#
#  Check for test failure
#

if grep 'PASSED' scratch.b > /dev/null
then
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test13b.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test13b.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test13b.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test13b.SRr0
fi

echo "" | tee -a test13b.SLr0
echo "*****  FEOV MADE DATASET READ TEST COMPLETE *****" | tee -a test13b.SLr0
