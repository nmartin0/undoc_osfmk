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
#  Run the test to write datasets using multiple devices
#
#  Syntax: test8.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test8.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       

echo ""
echo "*****  MULTIPLE DEVICES WRITE TEST *****" | tee -a test8.SLr0
echo "" | tee -a test8.SLr0
echo "please PREPARE 8 tapes on the STK 4980 cabinet and LOAD the first tape, " | tee -a test8.SLr0
echo "then press return to continue ..." | tee -a test8.SLr0
read response
echo "starting nsh -s to run make scratch tape \"ms\" utilility & unload tapes" | tee -a test8.SLr0
getcube -t4 
nsh -s << EOF
ms -f -vtest01 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest02 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest03 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest04 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest11 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest12 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest13 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest14 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
exit
EOF
relcube 
echo "nsh -s done" | tee -a test8.SLr0

echo ""
echo "please use TEST01-TEST04 on device \"/cfs/ctape0\" & LOAD the 1st tape," |  tee -a test8.SLr0
echo "           TEST11-TEST14 on device \"/cfs/ctape1\" & LOAD the 1st tape," |  tee -a test8.SLr0
echo "then press return to continue ..." | tee -a test8.SLr0
read response


d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`


TYPE="-t 1"
NUM=1

(getcube $SRM $TYPE $CUBENAME; load 0 node.rx; waitcube; relcube) | tee -a scratch 2>&1
cat test8.SLr0 scratch > tempfile
mv tempfile test8.SLr0

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
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test8.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test8.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test8.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test8.SRr0
fi

echo "" | tee -a test8.SLr0
echo "*****  MULTIPLE DEVICES WRITE TEST COMPLETE *****" | tee -a test8.SLr0
