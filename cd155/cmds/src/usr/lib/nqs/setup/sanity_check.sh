#!/bin/ksh
# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
DBG=${DBG-":"}

###########################
# Error Checking Functions
###########################

function Is_A_Part
{
$DBG
	#
	# Make sure partition named in $BASE_PART does exist as named
	#
if [ -n "`echo $BASE_PART | grep ".compute"`" ]; then
   Part_path="/etc/nx""`echo $BASE_PART | sed 's/\./\//g'`"
else
   Part_path="/etc/nx/compute""`echo $BASE_PART | sed 's/\./\//g'`"
fi
if [ ! -d "$Part_path" ]; then
	#
	# If status of check is not zero, ask if they still want to proceed.
	#
     echo "Partition doesn't exist as named, do you want to continue? [y/n]\c"
     read ans
     if [ -z "$ans" ]; then
	echo "Please confirm to continue with NQS setup: [y/n]\c"
        read ans
        if [ -z "$ans" ]; then
		echo
		echo "NQS setup aborting, exiting ..."
		echo
		exit
	fi
     fi
     ans=`echo $ans | cut -c1 | tr [A-Z] [a-z]`
     if [ "$ans" != n -a "$ans" != y ]; then
        echo
        echo "Please enter  'y'  or  'n'  for the response"
        echo 
        echo "Confirm whether you would like continue NQS setup: [y/n]\c"
        read ans
        ans=`echo $ans | cut -c1 | tr [A-Z] [a-z]`
        if [ "$ans" != n -a "$ans" != y ]; then
                echo 
                echo "Incorrect response, exiting ..."
                echo 
                exit 1
        fi
     fi
     case "$ans" in
	y)	echo "Continuing with NQS setup ..."
		return 0
		;;
	n)	echo
		echo "Aborting NQS setup, exiting ..."
		echo
		exit 1
		;;
     esac
else
     return 0
fi
}


function Parse_Node_Spec
{
$DBG

   if [ -n "$NODE_SPEC" ]; then
	H="`echo $NODE_SPEC | cut -dx -f1`"
	W="`echo $NODE_SPEC | cut -dx -f2 | cut -d: -f1`"
	A="`echo $NODE_SPEC | cut -d: -f2`"
	D="`echo $NODE_SPEC | sed 's/[0-9]//g'`"
	if [ -z "$H" ]; then
		echo "No heighth parameter given in node specification $NODE_SPEC."
		return 1
	fi
	if [ -z "$W" ]; then
		echo "No width parameter given in node specification $NODE_SPEC."
		return 1
	fi
	if [ -z "$A" ]; then
		echo "No anchor parameter given in node specification $NODE_SPEC."
		return 1
	fi
	if [ "$D" != "x:" -a "$D" != "X:" ]; then
		echo "Delimiters are incorrect in node specification $NODE_SPEC."
		return 1
	fi
	if [ -n "`echo $H | sed 's/[0-9]//g'`" ]; then
		echo "Invalid heighth in node specification: $NODE_SPEC."
		return 1
	fi
	if [ -n "`echo $W | sed 's/[0-9]//g'`" ]; then
		echo "Invalid width in node specification: $NODE_SPEC."
		return 1
	fi
	if [ -n "`echo $A | sed 's/[0-9]//g'`" ]; then
		echo "Invalid anchor in node specification: $NODE_SPEC."
		return 1
	fi
   else
	echo "No node specification given."
	return 1
   fi
   return 0
}

function Check_Node_List
{
$DBG
	#
	# Make sure all entries in $NODE_LIST are numbers
	#
   for node in $NODE_LIST; do
	if [ -n "`echo $node | sed 's/[0-9]//g'`" ]; then
		echo "Node list includes non-numeric input."
		return 1
	fi
    done
	#
	# Are node_sets really defined?
	#
   for node in $NODE_LIST; do
	if [ "$node" -lt 0 -o "$node" -gt "$S_SAVE" ]; then
		echo "Node list includes undefined node sets."
		return 1
	fi
   done
   return 0
}


function Check_Set_Lists
{
$DBG
	#
	# Set some control variables
	#
# Number of fields in node_group
   N_CNT=`echo ${Node_Group[$GRP_NUM]} | awk '{ print NF }'`

# Number of elements in lists
   p_CNT=`echo $pSET_LIST | awk '{ print NF }'`
   n_CNT=`echo $nSET_LIST | awk '{ print NF }'`

   LOOP=`expr $N_CNT + 1`
   pOK=1
   nOK=1
	#
	# Make sure node_sets in $pSET_LIST belong to $GRP_NUM
	#
   good=0
   for X in $pSET_LIST; do
	j=3
	while [ "$j" -lt "$LOOP" ]; do
	    Y=`echo ${Node_Group[$GRP_NUM]} | awk '{ print $'"$j"' }'`
	    if [ "$X" -eq "$Y" ]; then
		good=`expr $good + 1`
	    fi
	    j=`expr $j + 1`
	done
    done
    if [ "$good" -eq "$p_CNT" ]; then
	pOK=0
    fi
	#
	# Make sure node_sets in $nSET_LIST belong to $GRP_NUM
	#
   good=0
   for X in $nSET_LIST; do
	j=3
	while [ "$j" -lt "$LOOP" ]; do
	    Y=`echo ${Node_Group[$GRP_NUM]} | awk '{ print $'"$j"' }'`
	    if [ "$X" -eq "$Y" ]; then
		good=`expr $good + 1`
	    fi
	    j=`expr $j + 1`
	done
    done
    if [ "$good" -eq "$n_CNT" ]; then
	nOK=0
    fi

    if [ "$pOK" -ne 0 -o "$nOK" -ne 0 ]; then
	return 1
    else
	return 0
    fi
}

#######################
# Start of main script
#######################
$DBG
# Parse the node_set, node_group, prime_list and nprime_list entries
# into tokens and then call all the error checking above.

#############################
# Check Node Set definitions
#############################
i=0
while [ "$i" -le "$S_SAVE" ]; do
#if [ -n "echo ${Node_Set[$i]}" ]; then
	BASE_PART=`echo ${Node_Set[$i]} | awk '{ print $3 }'`
	NODE_SPEC=`echo ${Node_Set[$i]} | awk '{ print $4 }'`
	Is_A_Part
	if [ $? -ne 0 ]; then
		echo "Partition specification in Node_Set$i has an error."
		return 1
	fi
	Parse_Node_Spec
	if [ $? -ne 0 ]; then
		echo "Node specification in Node_Set$i has an error."
		return 1
	fi	
	i=`expr $i + 1`
#fi
done

###############################
# Check Node Group definitions
###############################
i=0
while [ "$i" -le "$G_SAVE" ]; do
	NODE_LIST="`echo ${Node_Group[$i]} | awk '{ for ( x = 3; x <= NF; x++) print $x }'`"
	Check_Node_List
	if [ $? -ne 0 ]; then
		echo "Node Set in Node_Group$i are not defined."
		return 1
	fi
	i=`expr $i + 1`
done

###############################################
# Check Prime List and Nprime List definitions
###############################################
i=0
while [ "$i" -le "$G_SAVE" ]; do
	pSET_LIST=`echo ${Prime_List[$i]} | awk '{ for ( x = 3; x <= NF; x++) print $x }'`
	nSET_LIST=`echo ${Nprime_List[$i]} | awk '{ for ( x = 3; x <= NF; x++) print $x }'`
	GRP_NUM=$i
	Check_Set_Lists
	if [ $? -ne 0 ]; then
		echo "Prime List or Non-prime List contains entries not part of Node Group$i"
		return 1
	fi
	i=`expr $i + 1`
done

####################################
# All is right in the world so exit 
####################################
return 0
