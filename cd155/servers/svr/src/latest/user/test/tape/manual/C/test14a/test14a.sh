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
#  Run the test to write multi-volumes with "feov" (force end of volumn) call
#
#  Syntax: test14a.sh 
#

# Check arguments
if [ $# -gt 0 ]
then
        echo "Usage: test14a.sh "
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
fi       
echo ""
echo "*****  MULTI-VOLUME FEOV WRITE TEST *****" | tee -a test14a.SLr0
echo "" | tee -a test14a.SLr0
echo "This test requires 12 tapes!!!" | tee -a test14a.SLr0
echo "please PREPARE tape 1-8 on the STK 4980 cabinet and LOAD the first tape, " | tee -a test14a.SLr0
echo "then press return to continue ..." | tee -a test14a.SLr0
read response
echo "starting nsh -s to run make scratch tape \"ms\" utilility & unload tapes" | tee -a test14a.SLr0
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
ms -f -vtest05 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest06 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest07 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest08 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
exit
EOF
echo " " | tee -a test14a.SLr0
echo "please PREPARE tape 9-12 on the STK 4980 cabinet and LOAD the first tape, " | tee -a test14a.SLr0
echo "then press return to continue ..." | tee -a test14a.SLr0
read response
echo "continue nsh -s to run make scratch tape \"ms\" utilility & unload tapes" | tee -a test14a.SLr0
nsh -s << EOF
ms -f -vtest09 -neval /cfs/ctape0
/usr/eval/t90/Manual/lib/unld < /usr/eval/t90/Manual/lib/.mks1
echo "sleeping to wait for tape fully unloaded from tape drive ..."
/usr/eval/t90/Manual/lib/sleep 20
ms -f -vtest10 -neval /cfs/ctape0
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
exit
EOF
relcube 
echo "nsh -s done" | tee -a test14a.SLr0
         
echo " "
echo "please REINSERT the tape 1-9 & LOAD the 1st tape," |  tee -a test14a.SLr0
echo "later, manually feed tape 10-12 and type TEST10 to TEST12 1 by 1 on screen" | tee -a test14a.SLr0

echo "Now, after REINSERT tape 1-9, press return to continue ..." | tee -a test14a.SLr0
read response


d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`


TYPE="-t 1"
NUM=1

(getcube $SRM $TYPE $CUBENAME; load 0 node.rx; waitcube; relcube) | tee -a scratch
cat test14a.SLr0 scratch > tempfile
mv tempfile test14a.SLr0

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
 echo "T90_RESULT: P `hostname` $execTime" | tee -a test14a.SLr0
 echo "T90_RESULT: P `hostname` $execTime" > test14a.SRr0
else
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" | tee -a test14a.SLr0
 echo "T90_RESULT: F (found FAILED) `hostname` $execTime" > test14a.SRr0
fi

echo "" | tee -a test14a.SLr0
echo "*****  MULTI-VOLUME FEOV WRITE TEST COMPLETE *****" | tee -a test14a.SLr0
