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
#  Run the IPSC extended integer to IBM zoned value conversion performance test
#
#  Syntax: extzperf.x dim arch
#

# Check arguments
if [ $# -lt 2 ]
then
        echo "Usage: extzperf.x dim arch"
        echo ""
        echo "T90_RESULT: F (usage error)" >> NOT_RUN
        exit 2
else
	if test -n "$CUBENAME"
	then
		echo CUBENAME = $CUBENAME
	fi

	TYPE="-t d$1$2"
	if test -z "$1$2"
	then
	        TYPE=""
	fi
	echo TYPE = $TYPE

	dim=`echo $1 | awk '{ print substr($1,1,1) }'`
	echo dim = $dim

	numnodes=`echo "2 ^ $dim" | bc`
	echo numnodes = $numnodes

        arch="$2"
        # Check architecture
 	if [ -n "$arch" ]
        then
                 
                case "$arch" in
                        cx)	echo "Warning: Test can only be run on RX node"
                                ;;
                        rx)      
                                ;;
                        sx)     echo "Warning: Test can only be run on RX node" 
                                ;;
                        vx)     echo "Warning: Test can only be run on RX node" 
                                ;;
                        *)       
                                echo "Unknown arch: $arch"
                                ;;
                esac             
        else     
                echo "No arch specified."
        fi       
fi       
echo ""
echo "*****  IPSC EXTENED INT TO IBM ZONED PERFORMANCE TEST  *****"
echo ""
         
d1=`date '+%j'`
h1=`date '+%H'`
m1=`date '+%M'`
s1=`date '+%S'`

getcube $SRM $TYPE $CUBENAME  > scratch 2>&1
load node.rx  
waitcube
relcube
cat scratch

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
 	echo "T90_RESULT: F (found FAILED) `hostname` $execTime"
elif [ $PAS -ne $numnodes ] 
then 
	echo "T90_RESULT: F (wrong pass count) `hostname` $execTime"
else
 	echo "T90_RESULT: P `hostname` $execTime"
fi

echo ""
echo "*****  IPSC EXTENED INT TO IBM ZONED PERFORMANCE TEST COMPLETE  *****"
echo ""
