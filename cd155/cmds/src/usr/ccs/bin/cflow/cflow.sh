#!/bin/sh
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
 
#
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2

# @(#)$RCSfile: cflow.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:55 $
#
# COMPONENT_NAME: (CMDPROG) Programming Utilities
#
# FUNCTIONS: 
#
# ORIGINS: 00 03 10 27 32
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# cflow.sh	1.12  com/cmd/prog/cflow/cflow.sh, 9113320, bos320 2/20/91

o=
INVFLG=
DFLAG=
IFLAG=
MFLAG=
DIR=/usr/ccs/lib
TMP=/usr/tmp/cf.$$
TMPG=$TMP.g
trap "rm -f $TMP.?; kill $$" 1 2 3

base=`basename $0`
case $base in
	cflow | cflow.sh)	CC=/lib/cpp;;
	*cflow*)		CC=`echo $base | sed s/cflow/cpp/p`;;
	*) 			CC=/lib/cpp;;
esac

echo "" >$TMP.g
if [ "$1" = "" ]
then
	dspmsg cflow.cat 2 "Usage: cflow [-dNumber] [-i_] [-ip] [-ix] [-MA] [-r]\n\
\t[-NdNumber] [-NlNumber] [-NnNumber] [-NtNumber] File [File ...]\n"
	exit 0
fi

while [ "$1" != "" ]
do
	case "$1" in
	-r)
		INVFLG=1
		;;
	-d*)
		DFLAG=$1
		;;
	-i*)
		IFLAG="$IFLAG $1"
		;;
	-MA)
		MFLAG="$MFLAG $1"
		;;
	-f)
		cat $2 </dev/null >>$TMPG
		shift
		;;
	-g)
		TMPG=$2
		if [ "$TMPG" = "" ]
		then
			TMPG=$TMP.g
		fi
		shift
		;;
	-N*)
		MFLAG="$MFLAG $1"
		;;
	-[IDU])
		o="$o $1$2"
		shift
		;;
	-[IDU]*)
		o="$o $1"
		;;
	*.y)
		yacc $1
		sed -e "/^# line/d" y.tab.c > $1.c
		$CC -E $o $1.c > $TMP.i
		$DIR/cflow1 $MFLAG -L$TMP.j $TMP.i 
		if [ $? -eq 2 ]		# Unrecoverable failure in cflow1
		then
			 rm $TMP.? y.tab.c $1.c
			 exit 1;
		fi
		$DIR/lpfx $IFLAG $TMP.j >> $TMPG
		rm y.tab.c $1.c
		;;
	*.l)
		lex $1
		sed -e "/^# line/d" lex.yy.c > $1.c
		$CC -E $o $1.c > $TMP.i
		$DIR/cflow1 $MFLAG -L$TMP.j $TMP.i 
		if [ $? -eq 2 ]		# Unrecoverable failure in cflow1
		then
			rm $TMP.? lex.yy.c $1.c
			exit 1;
		fi
		$DIR/lpfx $IFLAG $TMP.j >> $TMPG
		rm lex.yy.c $1.c
		;;
	*.c)
		$CC -E $o $1 > $TMP.i
		$DIR/cflow1 $MFLAG -L$TMP.j $TMP.i 
		if [ $? -eq 2 ]		# Unrecoverable failure in cflow1
		then
			rm $TMP.?
			exit 1;
		fi
		$DIR/lpfx $IFLAG $TMP.j >> $TMPG
		;;
	*.i)
		$DIR/cflow1 $MFLAG -L$TMP.j $1 
		if [ $? -eq 2 ]		# Unrecoverable failure in cflow1
		then
			exit 1;
		fi
		$DIR/lpfx $IFLAG $TMP.j >> $TMPG
		;;
	*.s)
		a=`basename $1 .s`
		as -o $TMP.o $1
		nm -he $TMP.o | sort -t'|' -n +1 -2 | $DIR/nmf $a ${a}.s >>$TMPG
		;;
	*.o)
		a=`basename $1 .o`
		nm -he $1 | sort -t'|' -n +1 -2 | $DIR/nmf $a ${a}.o >>$TMPG
		;;
	*)
		dspmsg cflow.cat 1 "%s -- cflow can't process - file skipped\n" $1
		;;
	esac
	shift
done
if [ "$INVFLG" != "" ]
then
	grep "=" $TMPG >$TMP.q
	grep ":" $TMPG | $DIR/flip >>$TMP.q
	sort <$TMP.q >$TMPG
	rm $TMP.q
fi
$DIR/dag $DFLAG < $TMPG
rm -f $TMP.?
