#!/bin/sh
# 
# $Copyright
# Copyright 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
# 
# $Copyright
# Copyright 1994 Intel Corporation.
#        INTEL CONFIDENTIAL
# Permission to use, copy, modify, and distribute this software and 
# its online documentation for any purpose is hereby granted.
# Intel Coprporation, Inc makes no representations about 
# the suitability of this software for any purpose.
# It is provided "as is" without express or implied warranty.
#
# INTEL CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD 
# TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INTEL CORPORATION.
# BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# Copyright$
#
  

ECHO=/usr/bin/echo
ACTABLE=/usr/spool/macs/conf/actable
NQSTABLE=/usr/spool/macs/conf/nqstable
MACSCONF=/usr/spool/macs/conf/macs.conf
TMP1=/tmp/charge1
TMP2=/tmp/charge2
rm -f $TMP1 $TMP2
touch $TMP1 $TMP2

parameters="	N_BINS
		BINS
		ACT_GRP_LEVEL
		ACT_NAME_SIM
		ACT_GRP_LABEL
		N_BATCH_QUEUE
		BATCH_QUEUE
		N_DEDICATE_QUEUE
		DEDICATE_QUEUE
		NODES
		MAILER
		ADMIN
		OPERATOR
		SWITCHLOG
		MACDMODE
		ENFORCE
		DEF_QRATE
		UNDERUSE
		IDLERATE
		SYNC_INTERVAL"

actable_parms="	N_BINS
		BINS
		ACT_GRP_LEVEL
		ACT_NAME_SIM
		ACT_GRP_LABEL
		N_BATCH_QUEUE
		BATCH_QUEUE
		N_DEDICATE_QUEUE
		DEDICATE_QUEUE"

macsconf_parms="NODES
		MAILER
		ADMIN
		OPERATOR
		SWITCHLOG
		MACDMODE
		ENFORCE
		DEF_QRATE
		UNDERUSE
		IDLERATE
		SYNC_INTERVAL"


N_BINS="10"
BINS="1 2 4 8 16 32 64 128 256 512 "
ACT_GRP_LEVEL="3"
ACT_NAME_SIM="3 3 1"
ACT_GRP_LABEL="site account project"
N_BATCH_QUEUE="0"
BATCH_QUEUE=""
N_DEDICATE_QUEUE="0"
DEDICATE_QUEUE=""
NODES=""
MAILER="/bin/mail"
ADMIN="root"
OPERATOR="root"
SWITCHLOG="daily"
MACDMODE="acctonly"
ENFORCE=""
DEF_QRATE="1.0"
UNDERUSE="0.0"
IDLERATE="0.0"
SYNC_INTERVAL="15"

. /usr/lib/macs/setup/sched_conf.help
if [ -s $ACTABLE ]
then
	for i in $actable_parms
	do
		set `grep "^$i" $ACTABLE`
		case $1 in

			N_BINS)			N_BINS=$2 ;;
			BINS)			shift; BINS=$* ;;
			ACT_GRP_LEVEL)		ACT_GRP_LEVEL=$2 ;;
			ACT_NAME_SIM)		shift; ACT_NAME_SIM=$* ;;
			ACT_GRP_LABEL)		shift; ACT_GRP_LABEL=$* ;;
			N_BATCH_QUEUE)		N_BATCH_QUEUE=$2 ;;
			BATCH_QUEUE)		shift; BATCH_QUEUE=$* ;;
			N_DEDICATE_QUEUE)	N_DEDICATE_QUEUE=$2 ;;
			DEDICATE_QUEUE)		shift; DEDICATE_QUEUE=$* ;;
		esac
	done
fi
			
if [ -s $MACSCONF ]
then
	for i in $macsconf_parms
	do
		set `grep $i $MACSCONF`
		case $1 in

			NODES)			NODES=$2 ;;
			MAILER)			MAILER=$2 ;;
			ADMIN)			ADMIN=$2 ;;
			OPERATOR)		OPERATOR=$2 ;;
			SWITCHLOG)		SWITCHLOG=$2 ;;
			MACDMODE)		MACDMODE=$2 ;;
			ENFORCE)		ENFORCE=$2 ;;
			DEF_QRATE)		DEF_QRATE=$2 ;;
			UNDERUSE)		UNDERUSE=$2 ;;
			IDLERATE)		IDLERATE=$2 ;;
			SYNC_INTERVAL)		SYNC_INTERVAL=$2 ;;

		esac
	done
fi

list=$parameters
$ECHO "$prompt"
while true
do
	for i in $list
	do
		case $i in
		N_BINS)
			while true
			do
				$ECHO "N_BINS : [$N_BINS] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$N_BINS_help"
						continue ;;
					 "")	;;
					  *)	N_BINS=$ans ;;
				esac

				if [ "x$N_BINS" = "x" ]
				then
					$ECHO "A 'N_BINS' value must be specified."
					continue
				fi

				break
			done
			;;
		BINS)
			while true
			do
				$ECHO "BINS : [$BINS] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$BINS_help"
						continue ;;
					 "")	;;
					  *)	BINS=$ans ;;
				esac

				if [ "x$BINS" = "x" ]
				then
					$ECHO "A 'BINS' value must be specified."
					continue
				fi

				break
			done
			;;
		ACT_GRP_LEVEL)
			while true
			do
				$ECHO "ACT_GRP_LEVEL : [$ACT_GRP_LEVEL] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$ACT_GRP_LEVEL_help"
						continue ;;
					 "")	;;
					  *)	ACT_GRP_LEVEL=$ans ;;
				esac

				if [ "x$ACT_GRP_LEVEL" = "x" ]
				then
					$ECHO "A 'ACT_GRP_LEVEL' value must be specified."
					continue
				fi

				break
			done
			;;
		ACT_NAME_SIM)
			while true
			do
				$ECHO "ACT_NAME_SIM : [$ACT_NAME_SIM] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$ACT_NAME_SIM_help"
						continue ;;
					 "")	;;
					  *)	ACT_NAME_SIM=$ans ;;
				esac

				if [ "x$ACT_NAME_SIM" = "x" ]
				then
					$ECHO "A 'ACT_NAME_SIM' value must be specified."
					continue
				fi

				break
			done
			;;
		ACT_GRP_LABEL)
			while true
			do
				$ECHO "ACT_GRP_LABEL : [$ACT_GRP_LABEL] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$ACT_GRP_LABEL_help"
						continue ;;
					 "")	;;
					  *)	ACT_GRP_LABEL=$ans ;;
				esac

				if [ "x$ACT_GRP_LABEL" = "x" ]
				then
					$ECHO "A 'ACT_GRP_LABEL' value must be specified."
					continue
				fi

				break
			done
			;;
		N_BATCH_QUEUE)
			while true
			do
				$ECHO "N_BATCH_QUEUE : [$N_BATCH_QUEUE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$N_BATCH_QUEUE_help"
						continue ;;
					 "")	;;
					  *)	N_BATCH_QUEUE=$ans ;;
				esac

				if [ "x$N_BATCH_QUEUE" = "x" ]
				then
					$ECHO "A 'N_BATCH_QUEUE' value must be specified."
					continue
				fi

				break
			done
			;;
		BATCH_QUEUE)
                        if [ "x$N_BATCH_QUEUE" = "x0" ]
                        then
        $ECHO "'BATCH_QUEUE' parameter has no effect with 'N_BATCH_QUEUE' set to 0"
                                continue
                        fi

			while true
			do
				$ECHO "BATCH_QUEUE : [$BATCH_QUEUE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$BATCH_QUEUE_help"
						continue ;;
					 "")	;;
					  *)	BATCH_QUEUE=$ans ;;
				esac

				if [ "x$BATCH_QUEUE" = "x" ]
				then
					$ECHO "A 'BATCH_QUEUE' value must be specified."
					continue
				fi

				break
			done
			;;
		N_DEDICATE_QUEUE)
			while true
			do
				$ECHO "N_DEDICATE_QUEUE : [$N_DEDICATE_QUEUE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$N_DEDICATE_QUEUE_help"
						continue ;;
					 "")	;;
					  *)	N_DEDICATE_QUEUE=$ans ;;
				esac

				if [ "x$N_DEDICATE_QUEUE" = "x" ]
				then
					$ECHO "A 'N_DEDICATE_QUEUE' value must be specified."
					continue
				fi

				break
			done
			;;
		DEDICATE_QUEUE)
                        if [ "x$N_DEDICATE_QUEUE" = "x0" ]
                        then
        $ECHO "'DEDICATE_QUEUE' parameter has no effect with 'N_DEDICATE_QUEUE' set to 0"
                                continue
                        fi

			while true
			do
				$ECHO "DEDICATE_QUEUE : [$DEDICATE_QUEUE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$DEDICATE_QUEUE_help"
						continue ;;
					 "")	;;
					  *)	DEDICATE_QUEUE=$ans ;;
				esac

				if [ "x$DEDICATE_QUEUE" = "x" ]
				then
					$ECHO "A 'DEDICATE_QUEUE' value must be specified."
					continue
				fi

				break
			done
			;;
		NODES)
			while true
			do
				$ECHO "NODES : [$NODES] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$NODES_help"
						continue ;;
					 "")	;;
					  *)	NODES=$ans ;;
				esac

				if [ "x$NODES" = "x" ]
				then
					$ECHO "A 'NODES' value must be specified."
					continue
				fi

				break
			done
			;;
		MAILER)
			while true
			do
				$ECHO "MAILER : [$MAILER] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$MAILER_help"
						continue ;;
					 "")	;;
					  *)	MAILER=$ans ;;
				esac

				if [ "x$MAILER" = "x" ]
				then
					$ECHO "A 'MAILER' value must be specified."
					continue
				fi

				break
			done
			;;
		ADMIN)
			while true
			do
				$ECHO "ADMIN : [$ADMIN] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$ADMIN_help"
						continue ;;
					 "")	;;
					  *)	ADMIN=$ans ;;
				esac

				if [ "x$ADMIN" = "x" ]
				then
					$ECHO "A 'ADMIN' value must be specified."
					continue
				fi

				break
			done
			;;
		OPERATOR)
			while true
			do
				$ECHO "OPERATOR : [$OPERATOR] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$OPERATOR_help"
						continue ;;
					 "")	;;
					  *)	OPERATOR=$ans ;;
				esac

				if [ "x$OPERATOR" = "x" ]
				then
					$ECHO "A 'OPERATOR' value must be specified."
					continue
				fi

				break
			done
			;;
		SWITCHLOG)
			while true
			do
				$ECHO "SWITCHLOG : [$SWITCHLOG] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$SWITCHLOG_help"
						continue ;;
					 "")	;;
					  *)	SWITCHLOG=$ans ;;
				esac

				if [ "x$SWITCHLOG" = "x" ]
				then
					$ECHO "A 'SWITCHLOG' value must be specified."
					continue
				fi

				break
			done
			;;
		MACDMODE)
			while true
			do
				$ECHO "MACDMODE : [$MACDMODE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$MACDMODE_help"
						continue ;;
					 "")	;;
					  *)	MACDMODE=$ans ;;
				esac

				if [ "x$MACDMODE" = "x" ]
				then
					$ECHO "A 'MACDMODE' value must be specified."
					continue
				fi

				break
			done
			;;
		ENFORCE)
			while true
			do
				$ECHO "ENFORCE : [$ENFORCE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$ENFORCE_help"
						continue ;;
					 "")	;;
					  *)	ENFORCE=$ans ;;
				esac

				break
			done
			;;
		DEF_QRATE)
			while true
			do
				$ECHO "DEF_QRATE : [$DEF_QRATE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$DEF_QRATE_help"
						continue ;;
					 "")	;;
					  *)	DEF_QRATE=$ans ;;
				esac

				if [ "x$DEF_QRATE" = "x" ]
				then
					$ECHO "A 'DEF_QRATE' value must be specified."
					continue
				fi

				break
			done
			;;
		UNDERUSE)
			while true
			do
				$ECHO "UNDERUSE : [$UNDERUSE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$UNDERUSE_help"
						continue ;;
					 "")	;;
					  *)	UNDERUSE=$ans ;;
				esac

				if [ "x$UNDERUSE" = "x" ]
				then
					$ECHO "A 'UNDERUSE' value must be specified."
					continue
				fi

				break
			done
			;;
		IDLERATE)
			while true
			do
				$ECHO "IDLERATE : [$IDLERATE] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$IDLERATE_help"
						continue ;;
					 "")	;;
					  *)	IDLERATE=$ans ;;
				esac

				if [ "x$IDLERATE" = "x" ]
				then
					$ECHO "A 'IDLERATE' value must be specified."
					continue
				fi

				break
			done
			;;
		SYNC_INTERVAL)
			while true
			do
				$ECHO "SYNC_INTERVAL : [$SYNC_INTERVAL] \c"
				ans=`line`
				case $ans in
					"?")	$ECHO "$SYNC_INTERVAL_help"
						continue ;;
					 "")	;;
					  *)	SYNC_INTERVAL=$ans ;;
				esac

				if [ "x$SYNC_INTERVAL" = "x" ]
				then
					$ECHO "A 'SYNC_INTERVAL' value must be specified."
					continue
				fi

				break
			done
			;;

		*)
			$ECHO "$i is an invalid MACS scheduling parameter."
			;;
		esac
	done

	$ECHO
	$ECHO "These are the parameter values that have been set:"
	$ECHO "N_BINS			: $N_BINS"
	$ECHO "BINS			: $BINS"
	$ECHO "ACT_GRP_LEVEL		: $ACT_GRP_LEVEL"
	$ECHO "ACT_NAME_SIM		: $ACT_NAME_SIM"
	$ECHO "ACT_GRP_LABEL		: $ACT_GRP_LABEL"
	$ECHO "N_BATCH_QUEUE		: $N_BATCH_QUEUE"
	$ECHO "BATCH_QUEUE		: $BATCH_QUEUE"
	$ECHO "N_DEDICATE_QUEUE	: $N_DEDICATE_QUEUE"
	$ECHO "DEDICATE_QUEUE		: $DEDICATE_QUEUE"
	$ECHO "NODES			: $NODES"
	$ECHO "MAILER			: $MAILER"
	$ECHO "ADMIN			: $ADMIN"
	$ECHO "OPERATOR		: $OPERATOR"
	$ECHO "SWITCHLOG		: $SWITCHLOG"
	$ECHO "MACDMODE		: $MACDMODE"
	$ECHO "ENFORCE			: $ENFORCE"
	$ECHO "DEF_QRATE		: $DEF_QRATE"
	$ECHO "UNDERUSE		: $UNDERUSE"
	$ECHO "IDLERATE		: $IDLERATE"
	$ECHO "SYNC_INTERVAL		: $SYNC_INTERVAL"


	$ECHO "Do you want to change any of these now (y/n)? \c"
	ans=`line`
	if [ "x$ans" = "xn" ]
	then
		break
	fi

	$ECHO "Enter the names of the scheduling parameters you want to change:"
	list=`line`

done


if [ "$N_BATCH_QUEUE" -ne 0 ]; then
	for i in $BATCH_QUEUE
	do
		$ECHO "$i 1.0" >> $TMP1
	done

	list=$BATCH_QUEUE
	$ECHO "For each batch queue, please assign a charge factor (default 1.0)"
	$ECHO
	while true
	do
		for i in $list
		do
			charge=`grep "^$i " $TMP1 | awk '{print $2}'`
			$ECHO "$i : [$charge] \c"
			ans=`line`
			case $ans in
				"?")	$ECHO "$QUEUE_CHARGE_help"
					continue ;;
				"")   ;;
				*)   for j in $BATCH_QUEUE
					do
						if [ "$i" = "$j" ] ;then
							$ECHO "$i $ans" >> $TMP2
						else 
							$ECHO `grep "^$j " $TMP1` >> $TMP2
						fi
					done
			mv $TMP2 $TMP1
			esac
		done
	
		$ECHO
		$ECHO "These are the queue charges that have been set:"
		$ECHO
		cat $TMP1
	
		$ECHO "Do you want to change any of these now (y/n)? \c"
		ans=`line`
		if [ "x$ans" = "xn" ]
		then
			break
		fi

		$ECHO "Enter the names of the queues whose charges you would like to change:"
		list=`line`
	done	
fi

$ECHO "Commit these parameters to the configuration file (y/n)? \c"
ans=`line`
if [ "x$ans" = "xn" ]
then
	$ECHO "Abandoning MACS configuration parameter changes."
	rm -f $TMP1 $TMP2
	exit 1
fi

if [ -s $ACTABLE ]
then
	$ECHO "Renaming old configuration file to $ACTABLE.`date +%y.%m.%d`"
	mv $ACTABLE $ACTABLE.`date +%y.%m.%d`
	rm -f $ACTABLE
fi

$ECHO "# File created `date`"			>  $ACTABLE
$ECHO "#"					>> $ACTABLE
$ECHO "N_BINS\t$N_BINS"				>> $ACTABLE
$ECHO "BINS\t$BINS" 				>> $ACTABLE
$ECHO "ACT_GRP_LEVEL\t$ACT_GRP_LEVEL"		>> $ACTABLE
$ECHO "ACT_NAME_SIM\t$ACT_NAME_SIM" 		>> $ACTABLE
$ECHO "ACT_GRP_LABEL\t$ACT_GRP_LABEL"		>> $ACTABLE
$ECHO "N_BATCH_QUEUE\t$N_BATCH_QUEUE"		>> $ACTABLE
$ECHO "BATCH_QUEUE\t$BATCH_QUEUE"			>> $ACTABLE
$ECHO "N_DEDICATE_QUEUE\t$N_DEDICATE_QUEUE"	>> $ACTABLE
$ECHO "DEDICATE_QUEUE\t$DEDICATE_QUEUE"		>> $ACTABLE



if [ -s $MACSCONF ]
then
	$ECHO "Renaming old configuration file to $MACSCONF.`date +%y.%m.%d`"
	mv $MACSCONF $MACSCONF.`date +%y.%m.%d`
	rm -f $MACSCONF
fi

$ECHO "# File created `date`"		>  $MACSCONF
$ECHO "#"				>> $MACSCONF
$ECHO "NODES\t$NODES"	        	>> $MACSCONF
$ECHO "MAILER\t$MAILER"			>> $MACSCONF
$ECHO "ADMIN\t$ADMIN"			>> $MACSCONF
$ECHO "OPERATOR\t$OPERATOR"		>> $MACSCONF
$ECHO "SWITCHLOG\t$SWITCHLOG"		>> $MACSCONF
$ECHO "MACDMODE\t$MACDMODE"		>> $MACSCONF
$ECHO "ENFORCE\t$ENFORCE"		>> $MACSCONF
$ECHO "DEF_QRATE\t$DEF_QRATE"		>> $MACSCONF
$ECHO "UNDERUSE\t$UNDERUSE"		>> $MACSCONF
$ECHO "IDLERATE\t$IDLERATE"		>> $MACSCONF
$ECHO "SYNC_INTERVAL\t$SYNC_INTERVAL"	>> $MACSCONF

if [ "$N_BATCH_QUEUE" -ne 0 ]; then
	if [ -s $NQSTABLE ]
	then
		$ECHO "Renaming old configuration file to $NQSTABLE.`date +%y.%m.%d`"
		mv $NQSTABLE $NQSTABLE.`date +%y.%m.%d`
		rm -f $NQSTABLE
	fi


	$ECHO "$N_BATCH_QUEUE"		>> $NQSTABLE
	cat $TMP1			>> $NQSTABLE
	rm -f $TMP1 $TMP2
fi

chmod 644 $MACSCONF
chmod 644 $ACTABLE
chgrp system $MACSCONF
chgrp system $ACTABLE
if [ "$N_BATCH_QUEUE" -ne 0 ]; then
	chmod 644 $NQSTABLE
	chgrp system $NQSTABLE
fi
if [ ! -s "/usr/spool/macs/private/macd.data" ]
then
	/usr/sbin/dbcreate 1000 100 /usr/spool/macs/private/macd.data > /dev/null 2>&1
	if [ $? -ne 0 ]
	then
        	$ECHO "Dbcreate failed.  MACS database not created in /usr/spool/macs/private."
	else
		chmod 755 /usr/spool/macs/private/macd.data
	fi
fi

$ECHO "MACS configuration parameters have been updated"
exit 0


