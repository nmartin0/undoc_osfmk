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

###
#  This script leads the user thru the setup for node_sets, node_groups and 
#  the definition of Prime Time and/or Non-prime Time attributes of the
#  node_sets per node_group
###

# Define Variables

SET_NUM=0
GRP_NUM=0
BASE_PART=
NODE_SPEC=
S_SAVE=
G_SAVE=
NODE_LIST=
pSET_LIST=
nSET_LIST=


##################
# Local Functions
##################

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


function Make_Node_Set
{
$DBG
	#
	# Define node_set$SET_NUM as $BASE_PART $NODE_SPEC
	#
   Node_Set[$SET_NUM]="node_set$SET_NUM : $BASE_PART $NODE_SPEC"
   echo "${Node_Set[$SET_NUM]}"
	#
	# Save set number for later
	#
   S_SAVE="$SET_NUM"
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


function Make_Node_Group
{
$DBG
	#
	# Define "node_group$GRP_NUM" as "$NODE_LIST"
	#
   Node_Group[$GRP_NUM]="node_group$GRP_NUM : $NODE_LIST"
   echo "${Node_Group[$GRP_NUM]}"
	#
	# Save $GRP_NUM as $G_SAVE
	#
   G_SAVE=$GRP_NUM
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
	i=3
	while [ "$i" -lt "$LOOP" ]; do
	    Y=`echo ${Node_Group[$GRP_NUM]} | awk '{ print $'"$i"' }'`
	    if [ "$X" -eq "$Y" ]; then
		good=`expr $good + 1`
	    fi
	    i=`expr $i + 1`
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
	i=3
	while [ "$i" -lt "$LOOP" ]; do
	    Y=`echo ${Node_Group[$GRP_NUM]} | awk '{ print $'"$i"' }'`
	    if [ "$X" -eq "$Y" ]; then
		good=`expr $good + 1`
	    fi
	    i=`expr $i + 1`
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


function Set_Prime
{
$DBG
	#
	# Define prime_list$GRP_NUM as $SET_LIST
	#
    Prime_List[$GRP_NUM]="prime_list$GRP_NUM : $pSET_LIST"
    echo "${Prime_List[$GRP_NUM]}"
}


function Set_Non_Prime
{
$DBG
	#
	# Define nprime_list$GRP_NUM as $SET_LIST
	#
    Nprime_List[$GRP_NUM]="nprime_list$GRP_NUM : $nSET_LIST"
    echo "${Nprime_List[$GRP_NUM]}"
}



###++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#		Start of main script
###++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
$DBG
####################
#  Define node sets
####################
TRUE=0
	echo "We are defining the node sets for the Paragon system,"
	echo "rectangles of nodes with homogeneous attributes in a"
	echo "single partition."
	echo "To end node_set definitions enter 'Q'"
while [ "$TRUE" -ne 1 ]; do
	if [ -z "$BASE_PART" ]; then
		echo "Please enter the name of the base partition for the node_set"
		read BASE_PART
	else
		echo "Current base partition == $BASE_PART"
		echo "Do you wish to change base partitions? [y/n]\c"
		read ans
		if [ ! -z "$ans" ]; then
		     ans=`echo $ans | cut -c1 | tr [A-Z] [a-z]`
		     case "$ans" in
			y)	echo "Please enter the name of the base partition for the node_set"
				read BASE_PART
				;;
			q)	break
				;;
		     esac
		fi
	fi
	Is_A_Part
	echo "Please enter node specification for node_set$SET_NUM"
	read NODE_SPEC
	if [ "$NODE_SPEC" = "Q" -o "$NODE_SPEC" = "q" ]; then
		TRUE=1
	else
	     Parse_Node_Spec
	     if [ $? -ne 0 ]; then
		echo "Bad node spec for this node_set, please enter again."
	     else
		Make_Node_Set
		SET_NUM=`expr $SET_NUM + 1`
	     fi
	fi
done

######################
#  Define node groups
######################
TRUE=0
	echo "We are defining the node_groups as lists of node_sets."
	echo "To end node_group definitions enter 'Q'"
while [ $TRUE -ne 1 ]; do
	echo "Please enter the list of node_sets for node_group$GRP_NUM"
	read NODE_LIST
	if [ "$NODE_LIST" = "Q" -o "$NODE_LIST" = "q" ]; then
		TRUE=1
	else
	     Check_Node_List
	     if [ $? -ne 0 ]; then
		echo "Node list given was incorrect, please re-enter"
	     else
		Make_Node_Group
		GRP_NUM=`expr $GRP_NUM + 1`
	     fi
	fi
done
MAX_GROUP=$GRP_NUM

########################################
#  Setup Prime and Non-prime Time lists
########################################
TRUE=0
GRP_NUM=0
	echo "We are defining the Prime-time and Non-Prime-time attribute of"
	echo "the node_sets per node_group"
	echo "Enter 'Q' to terminate list entry"
while [ $TRUE -ne 1 ]; do
	echo "Please enter the list of node_sets to be used in Prime time"
	echo "for node_group$GRP_NUM."
	read pSET_LIST

	echo "Please enter the list of node_sets to be used in Non-Prime time"
	echo "for node_group$GRP_NUM."
	read nSET_LIST

	if [ "$pSET_LIST" = "Q" -o "$pSET_LIST" = "q" -o "$nSET_LIST" = "Q" -o "$nSET_LIST" = "q" ]; then
		TRUE=1
	else
	     T2=0
	     Check_Set_Lists
	     if [ $? -ne 0 ]; then
		echo "Node_set(s) don't belong to node_group, please re-enter."
		T2=1
	     else
		Set_Prime
		Set_Non_Prime
	     fi
	     if [ $T2 -eq 0 ]; then
		GRP_NUM=`expr $GRP_NUM + 1`
		if [ "$MAX_GROUP" -eq "$GRP_NUM" ]; then
			TRUE=1
		fi
	     fi
	fi
done

###################################################################
#  Output the node_sets, node_groups, prime_lists and nprime_lists
#  to sched_param
###################################################################
#i=0
#while [ $i -le $S_SAVE ]; do
#  echo ${Node_Set[$i]}
#  i=`expr $i + 1`
#done
#i=0
#while [ $i -le $G_SAVE ]; do
#  echo ${Node_Group[$i]}
#  i=`expr $i + 1`
#done
#i=0
#while [ $i -le $G_SAVE ]; do
#  echo ${Prime_List[$i]}
#  i=`expr $i + 1`
#done
#i=0
#while [ $i -le $G_SAVE ]; do
#  echo ${Nprime_List[$i]}
#  i=`expr $i + 1`
#done
#echo "Fin"

#export BASE_PART NODE_SPEC SET_NUM S_SAVE NODE_LIST GRP_NUM G_SAVE
#export pSET_LIST nSET_LIST Node_Set[] Node_Group[] Prime_List[] NPrime_List[]
