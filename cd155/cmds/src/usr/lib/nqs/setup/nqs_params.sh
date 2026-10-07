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
 
  
export BASE_PART NODE_SPEC SET_NUM S_SAVE NODE_LIST GRP_NUM G_SAVE  
export pSET_LIST nSET_LIST Node_Set[] Node_Group[] Prime_List[] Nprime_List[]  
  
ECHO=/usr/bin/echo  
SCHED_PARAM=$NQSROOT/usr/spool/nqs/conf/sched_param  
SETNODES_RUN=NO   
  
parameters="time_zone  
	    prime_start  
	    prime_end  
	    open_np_name  
	    open_p_name  
	    cur_open_name  
	    rollin_quan  
	    age_factor  
	    timeshare  
	    block_pri  
	    timesh_pri  
	    tsched_pri  
	    chk_runlimit  
	    grace_time  
	    do_wall  
	    np_overrun  
	    macs_flag
	    use_login
	    nosched
	    prime_script
	    nprime_script
	    wallclock_limits"  
 
  
time_zone=$TZ  
prime_start="08 08 08 08 08 08 08"  
prime_end="18 18 18 18 18 18 18"  
cur_open_name=  
open_np_name=  
open_p_name=  
timeshare=0  
block_pri=-1  
timesh_pri=10  
tsched_pri=20  
chk_runlimit=0  
age_factor=1.0  
grace_time=10  
rollin_quan=600  
do_wall=0  
np_overrun=0  
macs_flag=0  
use_login=0  
nosched=0  
prime_script=
nprime_script=
wallclock_limits=0  
  
want_open_p="yes"  
  
  
. $NQSROOT/usr/lib/nqs/setup/sched_conf.help  
  
if [ -s $SCHED_PARAM ]  
then  
	for i in $parameters  
	do  
		curval=`grep $i $SCHED_PARAM`  
		if [ -n "$curval" ]
		then
			set $curval
			case $1 in  
				time_zone)	time_zone=$3 ;;  
				prime_start)	prime_start="$3 $4 $5 $6 $7 $8 $9" ;;  
				prime_end)	prime_end="$3 $4 $5 $6 $7 $8 $9" ;;  
				cur_open_name)	cur_open_name=$3 ;;  
				open_np_name)	open_np_name=$3 ;;  
				open_p_name)	open_p_name=$3 ;;  
				timeshare)	timeshare=$3 ;;  
				block_pri)	block_pri=$3 ;;  
				timesh_pri)	timesh_pri=$3 ;;  
				tsched_pri)	tsched_pri=$3 ;;  
				chk_runlimit)	chk_runlimit=$3 ;;  
				age_factor)	age_factor=$3 ;;  
				grace_time)	grace_time=$3 ;;  
				rollin_quan)	rollin_quan=$3 ;;  
				do_wall)	do_wall=$3 ;;  
				np_overrun)	np_overrun=$3 ;;  
				macs_flag)	macs_flag=$3 ;;  
				use_login)	use_login=$3 ;;  
				nosched)	nosched=$3 ;;  
				prime_script)	prime_script=$3 ;;  
				nprime_script)	nprime_script=$3 ;;  
				wallclock_limits) wallclock_limits=$3 ;;  
				node_set0)	node_set0=$3 ;;  
				node_group0)	node_group0=$3 ;;  
				prime_list0)	prime_list0=$3 ;;  
				nprime_list0)	nprime_list0=$3 ;;  
			esac  
		fi
	done  
fi  
			  
N=0  
list=$parameters  
$ECHO "$prompt"  
while true  
do  
	for i in $list  
	do  
		case $i in  
		time_zone)  
			while true  
			do  
				$ECHO "time_zone : [$time_zone] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$time_zone_help"  
						continue ;;  
					 "")	;;  
					  *)	time_zone=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		prime_start)  
			while true  
			do  
				$ECHO "                S  M  T  W  T  F  S"  
				$ECHO "prime_start : [$prime_start] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$prime_start_help"  
						continue ;;  
					 "")	;;  
					  *)	prime_start=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		prime_end)  
			while true  
			do  
				$ECHO "                S  M  T  W  T  F  S"  
				$ECHO "prime_end   : [$prime_end] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$prime_end_help"  
						continue ;;  
					 "")	;;  
					  *)	prime_end=$ans ;;  
				esac  
				break  
			done  
			;;  
  
  
		open_p_name)  
			if [ $want_open_p = "no" ]  
			then  
				continue  
			fi  
  
			while true  
			do  
				$ECHO "open_p_name : [$open_p_name] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$open_p_name_help"  
							continue ;;  
				     "none")	want_open_p="no"  
						open_p_name="" ;;  
					 "")	;;  
					  *)	open_p_name=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		open_np_name)  
			if [ $want_open_p = "no" ]  
			then  
				continue  
			fi  
  
			while true  
			do  
				$ECHO "open_np_name : [$open_np_name] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$open_np_name_help"  
						continue ;;  
				     "none")	want_open_p="no"  
						open_np_name=""  
						;;  
					 "")	;;  
					  *)	open_np_name=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		cur_open_name)  
			if [ $want_open_p = "no" ]  
			then  
				continue  
			fi  
  
			while true  
			do  
				$ECHO "cur_open_name : [$cur_open_name] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$cur_open_name_help"  
						continue ;;  
				     "none")	want_open_p="no"  
						cur_open_name="" ;;  
					 "")	;;  
					  *)	cur_open_name=$ans ;;  
				esac  
				break  
			done  
			;;  
  
  
		timeshare)  
			while true  
			do  
				$ECHO "timeshare : [$timeshare] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$timeshare_help"  
						continue ;;  
				  "0" | "1")	timeshare=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;  
  
		block_pri)  
			while true  
			do  
				$ECHO "block_pri : [$block_pri] \c"  
				ans=`line`  
				case $ans in  
					"?")  $ECHO "$block_pri_help"  
						continue ;;  
					"")	;;  
					*)  block_pri=$ans ;;  
				esac  
				break  
			done  
			;;  
  
  
  
		timesh_pri)  
			if [ "x$timeshare" = "x0" ]  
			then  
	$ECHO "'timesh_pri' parameter has no effect with 'timeshare' set to 0"  
				continue  
			fi  
  
			while true  
			do  
				$ECHO "timesh_pri : [$timesh_pri] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$timesh_pri_help"  
						continue ;;  
					 "")	;;  
					  *)	timesh_pri=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		tsched_pri)  
			while true  
			do  
				$ECHO "tsched_pri : [$tsched_pri] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$tsched_pri_help"  
						continue ;;  
					 "")	;;  
					  *)	tsched_pri=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		chk_runlimit)  
			while true  
			do  
				$ECHO "chk_runlimit : [$chk_runlimit] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$chk_runlimit_help"  
						continue ;;  
				  "0" | "1")	chk_runlimit=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;  
  
		age_factor)  
			while true  
			do  
				$ECHO "age_factor : [$age_factor] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$age_factor_help"  
						continue ;;  
					 "")	;;  
					  *)	age_factor=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		grace_time)  
			while true  
			do  
				$ECHO "grace_time : [$grace_time] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$grace_time_help"  
						continue ;;  
					 "")	;;  
					  *)	grace_time=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		rollin_quan)  
			while true  
			do  
				$ECHO "rollin_quan : [$rollin_quan] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$rollin_quan_help"  
						continue ;;  
					 "")	;;  
					  *)	rollin_quan=$ans ;;  
				esac  
				break  
			done  
			;;  
  
		do_wall)  
			while true  
			do  
				$ECHO "do_wall : [$do_wall] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$do_wall_help"  
						continue ;;  
				  "0" | "1")	do_wall=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;  
		np_overrun)  
			while true  
			do  
				$ECHO "np_overrun: [$np_overrun] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$np_overrun_help"  
						continue ;;  
				  "0" | "1")	np_overrun=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;  
		macs_flag)  
			while true  
			do  
				$ECHO "macs_flag: [$macs_flag] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$macs_flag_help"  
						continue ;;  
				  "0" | "1")	macs_flag=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;  
		use_login)  
			while true  
			do  
				$ECHO "use_login: [$use_login] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$use_login_help"  
						continue ;;  
				  "0" | "1")	use_login=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;  
		nosched)  
			while true  
			do  
				$ECHO "nosched: [$nosched] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$nosched_help"  
						continue ;;  
				  "0" | "1")	nosched=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;
		prime_script)  
			while true  
			do  
				$ECHO "prime_script: [$prime_script] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$prime_script_help"  
						continue ;;  
					 "")	;;  
					  *)	prime_script=$ans ;;  
				esac  
				break  
			done  
			;;  
		nprime_script)  
			while true  
			do  
				$ECHO "nprime_script: [$nprime_script] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$nprime_script_help"  
						continue ;;  
					 "")	;;  
					  *)	nprime_script=$ans ;;  
				esac  
				break  
			done  
			;;  
		wallclock_limits)  
			while true  
			do  
				$ECHO "wallclock_limits: [$wallclock_limits] \c"  
				ans=`line`  
				case $ans in  
					"?")	$ECHO "$wallclock_limits_help"  
						continue ;;  
				  "0" | "1")	wallclock_limits=$ans ;;  
					 "")	;;  
					  *)	$ECHO "Valid values are 0 and 1"  
						continue;;  
				esac  
				break  
			done  
			;;
  
		*)  
			  
			if [ `echo $i | egrep -c '(node_set|node_group|prime_list|nprime_list)'` -ne 0 ]; then  
				while true  
				do  
					number=`echo $i | sed 's/[A-Za-z_]//g'`  
					varname=`echo $i | sed 's/[0-9]//g'`  
  
					case $varname in  
						  "node_set")  
						  	while true  
							do  
								$ECHO "[${Node_Set[$number]}] \c"  
								ans=`line`  
								case $ans in  
									"?")	$ECHO "$node_set_help"  
										continue ;;  
									"")	;;  
									*)	Node_Set[$number]="node_set$number : $ans";;  
								esac  
								break  
							done  
							;;  
						  "node_group")  
						  	while true  
							do  
								$ECHO "[${Node_Group[$number]}] \c"  
								ans=`line`  
								case $ans in  
									"?")	$ECHO "$node_group_help"  
										continue ;;  
									"")	;;  
									*)	Node_Group[$number]="node_group$number : $ans";;  
								esac  
								break  
							done  
							;;  
						  "prime_list")  
						  	while true  
							do  
								$ECHO "[${Prime_List[$number]}] \c"  
								ans=`line`  
								case $ans in  
									"?")	$ECHO "$prime_list_help"  
										continue ;;  
									"")	;;  
									*)	Prime_List[$number]="prime_list$number : $ans";;  
								esac  
								break  
							done  
							;;  
						  "nprime_list")  
						  	while true  
							do  
								$ECHO "[${Nprime_List[$number]}] \c"  
								ans=`line`  
								case $ans in  
									"?")	$ECHO "$nprime_list_help"  
										continue ;;  
									"")	;;  
									*)	Nprime_List[$number]="nprime_list$number : $ans";;  
								esac  
								break  
							done  
							;;  
  
					esac  
					break  
				done  
			else  
				$ECHO "$i is an invalid NQS scheduling parameter."  
			fi  
			;;  
		esac  
	done  
	  
	if [ $SETNODES_RUN = "NO" ]; then  
		. $NQSROOT/usr/lib/nqs/setup/setup_nodes
		SETNODES_RUN=YES  
	fi					  
  
	$ECHO  
	$ECHO "These are the parameter values that have been set:"  
	$ECHO "time_zone     : $time_zone"  
	$ECHO "prime_start   : $prime_start"  
	$ECHO "prime_end     : $prime_end"  
	$ECHO "open_np_name  : $open_np_name"  
	$ECHO "open_p_name   : $open_p_name"  
	$ECHO "cur_open_name : $cur_open_name"  
	$ECHO "rollin_quan   : $rollin_quan"  
	$ECHO "age_factor    : $age_factor"  
	$ECHO "timeshare     : $timeshare"  
	$ECHO "block_pri     : $block_pri"  
	$ECHO "timesh_pri    : $timesh_pri"  
	$ECHO "tsched_pri    : $tsched_pri"  
	$ECHO "chk_runlimit  : $chk_runlimit"  
	$ECHO "grace_time    : $grace_time"  
	$ECHO "do_wall       : $do_wall"  
	$ECHO "np_overrun    : $np_overrun"  
	$ECHO "macs_flag     : $macs_flag"  
	$ECHO "use_login     : $use_login"  
	$ECHO "nosched       : $nosched"  
	$ECHO "prime_script  : $prime_script"  
	$ECHO "nprime_script : $nprime_script"  
	$ECHO "wallclock_limits : $wallclock_limits"  
	N=0 
	while [ "$N" -le $S_SAVE ]; do  
		$ECHO "${Node_Set[$N]}"  
		N=`expr $N + 1`  
	done  
  
	N=0 
	while [ "$N" -le $G_SAVE ]; do  
		$ECHO "${Node_Group[$N]}"  
		N=`expr $N + 1`  
	done  
  
	N=0 
	while [ "$N" -le $G_SAVE ]; do  
		$ECHO "${Prime_List[$N]}"  
		N=`expr $N + 1`  
	done  
  
	N=0 
	while [ "$N" -le $G_SAVE ]; do  
		$ECHO "${Nprime_List[$N]}"  
		N=`expr $N + 1`  
	done  
  
	$ECHO "Do you want to change any of these now (y/n)? \c"  
	ans=`line`  
        ans=`echo $ans | tr [A-Z] [a-z]` 
	if [ "x$ans" = "xn" ]  
	then  
		break  
	fi  
  
	$ECHO "Enter the names of the scheduling parameters you want to change:"  
	list=`line`  
	list=`echo $list | tr [A-Z] [a-z]`  
  
  
	want_open_p="yes"  
done  
  
echo "Checking parameter values." 
. $NQSROOT/usr/lib/nqs/setup/sanity_check
  
$ECHO "Commit these parameters to the configuration file (y/n)? \c"  
ans=`line`  
ans=`echo $ans | tr [A-Z] [a-z]` 
if [ "x$ans" = "xn" ]  
then  
	$ECHO "Abandoning NQS configuration parameter changes."  
	exit 1  
fi  
  
if [ -s $SCHED_PARAM ]  
then  
	$ECHO "Renaming old configuration file to $SCHED_PARAM.`date +%y.%m.%d`"  
	mv $SCHED_PARAM $SCHED_PARAM.`date +%y.%m.%d`  
	rm -f $SCHED_PARAM  
fi  
  
echo "# File created `date`"             > $SCHED_PARAM  
echo "#"                              >> $SCHED_PARAM  
echo "time_zone : $time_zone"         >> $SCHED_PARAM  
echo "prime_start : $prime_start"     >> $SCHED_PARAM  
echo "prime_end : $prime_end"         >> $SCHED_PARAM  
echo "cur_open_name : $cur_open_name" >> $SCHED_PARAM  
echo "open_np_name : $open_np_name"   >> $SCHED_PARAM  
echo "open_p_name : $open_p_name"     >> $SCHED_PARAM  
echo "timeshare : $timeshare"         >> $SCHED_PARAM  
echo "block_pri    : $block_pri"      >> $SCHED_PARAM  
echo "timesh_pri : $timesh_pri"       >> $SCHED_PARAM  
echo "tsched_pri : $tsched_pri"       >> $SCHED_PARAM  
echo "chk_runlimit : $chk_runlimit"   >> $SCHED_PARAM  
echo "age_factor : $age_factor"       >> $SCHED_PARAM  
echo "grace_time : $grace_time"       >> $SCHED_PARAM  
echo "rollin_quan : $rollin_quan"     >> $SCHED_PARAM  
echo "do_wall : $do_wall"             >> $SCHED_PARAM  
echo "np_overrun : $np_overrun"       >> $SCHED_PARAM  
echo "macs_flag : $macs_flag"         >> $SCHED_PARAM  
echo "use_login : $use_login"         >> $SCHED_PARAM  
echo "nosched : $nosched"             >> $SCHED_PARAM  
echo "prime_script : $prime_script"   >> $SCHED_PARAM  
echo "nprime_script : $nprime_script" >> $SCHED_PARAM  
echo "wallclock_limits : $wallclock_limits" >> $SCHED_PARAM  
  
  
	N=0 
	while [ "$N" -le $S_SAVE ]; do  
		$ECHO "${Node_Set[$N]}"   >>  $SCHED_PARAM  
		N=`expr $N + 1`  
	done  
	N=0 
	while [ "$N" -le $G_SAVE ]; do  
		$ECHO "${Node_Group[$N]}"  >> $SCHED_PARAM  
		N=`expr $N + 1`  
	done  
	N=0 
	while [ "$N" -le $G_SAVE ]; do  
		$ECHO "${Prime_List[$N]}"   >> $SCHED_PARAM  
		N=`expr $N + 1`  
	done  
	N=0 
	while [ "$N" -le $G_SAVE ]; do  
		$ECHO "${Nprime_List[$N]}"  >> $SCHED_PARAM  
		N=`expr $N + 1` 
	done  
  
  
$ECHO "NQS configuration parameters have been updated"  
exit 0  
