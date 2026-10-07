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
#  Run the Negative tdsopen test 
#
#  Syntax: test0.sh 
#

DEV=/dev/io1/rmt6
PART=test0
PATH=/home/slk/3480/util

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test0.sh "
        echo ""
        echo "RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  NEGATIVE TDSOPEN TEST  *****" | tee -a test0.SLr0
echo "" | tee -a test0.SLr0
echo "ASSUMING 1 scratch tape has been loaded..."| tee -a test0.SLr0
echo "starting nsh -s to run make scratch tape "ms" utilility" | tee -a test0.SLr0
mkpart -sz 1 $PART
nsh -s << EOF
$PATH/ms -f -vtest01 -neval $DEV
exit
EOF

echo "nsh -s done" | tee -a test0.SLr0
         
d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`

./node.rx $DEV -pn $PART

rmpart $PART

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
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test0.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test0.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test0.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test0.SRr0
fi

echo "" | tee -a test0.SLr0
echo "*****  NEGATIVE TDSOPEN TEST COMPLETE  *****" | tee -a test0.SLr0
