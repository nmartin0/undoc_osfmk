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
#  Run the multi-node tape alloc/dealloc test 
#
#  Syntax: test5.sh 
#

# Check arguments
if [ $# -lt 1 ]
then
        echo "Usage: test5.sh dim "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
else

        TYPE="-t d$1rx"
        echo TYPE = $TYPE

        dim=`echo $1 | awk '{ print substr($1,1,1) }'`
        echo dim = $dim

        numnodes=`echo "2 ^ $dim" | bc`
        echo numnodes = $numnodes


fi       
echo ""
echo "*****  MULTI-NODE TAPE ALLOC/DEALLOC TEST  *****" | tee -a test5.SLr0
echo "" | tee -a test5.SLr0
         
d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`



(getcube $TYPE; load node.rx; waitcube; relcube) | tee scratch
cat test5.SLr0 scratch > tempfile
mv tempfile test5.SLr0

# Execute time
d=`expr \( \`date '+%j'\` - $d1 \) \* 86400`
h=`expr \( \`date '+%H'\` - $h1 \) \* 3600`
m=`expr \( \`date '+%M'\` - $m1 \) \* 60`
execTime=`expr $d + $h + $m + \( \`date '+%S'\` - $s1 \)`


#
#  Check for test failure
#

PAS=`grep -c 'PASS' scratch`

if grep 'FAILED' scratch > /dev/null
then
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test5.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test5.SRr0
elif [ $PAS -ne $numnodes ]
then
 echo "T90_RESULT: F (wrong pass count) `hostname` $execTime" | tee -a test5.SLr0
 echo "T90_RESULT: F (wrong pass count) `hostname` $execTime" > test5.SRr0
else
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test5.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test5.SRr0
fi

echo "" | tee -a test5.SLr0
echo "*****  MULTI-NODE TAPE ALLOC/DEALLOC TEST COMPLETE  *****" | tee -a test5.SLr0
