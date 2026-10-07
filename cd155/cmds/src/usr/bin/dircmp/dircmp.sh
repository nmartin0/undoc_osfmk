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

#
# @(#)$RCSfile: dircmp.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:52 $
#
# COMPONENT_NAME: (CMDFS) commands that deal with the file system
#
# FUNCTIONS: dircmp
#
# ORIGINS: 3, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# dircmp.sh	1.5  com/cmd/fs/progs,3.1,9013 1/23/90 19:53:35
#
PATH=/bin:/usr/bin
trap "rm -f /usr/tmp/dc$$*;exit" 1 2 3 15
# 
# messages from file
#
if USAGE=`dspmsg -s 1 dircmp.cat 1 "dircmp: usage: dircmp -ds directory directory"`
then :
else USAGE='dircmp: usage: dircmp -ds directory directory' >&2 ; fi
if NOTDIR=`dspmsg -s 1 dircmp.cat 2 "not a directory !"`
then :
else NOTDIR='not a directory !' ; fi
if BADOPTION=`dspmsg -s 1 dircmp.cat 3 "unknown option"`
then :
else BADOPTION='unknown option' ; fi
if ONLYAND=`dspmsg -s 1 dircmp.cat 4 "only and"`
then :
else ONLYAND='only and' ; fi
if ONLY=`dspmsg -s 1 dircmp.cat 5 "only"`
then :
else ONLY='only' ; fi
if DIRECTORY=`dspmsg -s 1 dircmp.cat 6 "directory"`
then :
else DIRECTORY='directory' ; fi
if SAME=`dspmsg -s 1 dircmp.cat 7 "same"`
then :
else SAME='same' ; fi
if DIFFERENT=`dspmsg -s 1 dircmp.cat 8 "different"`
then :
else DIFFERENT='different' ; fi
if DIFFOF=`dspmsg -s 1 dircmp.cat 9 "diff of"`
then :
else DIFFOF='diff of' ; fi
if IN=`dspmsg -s 1 dircmp.cat 10 "in"`
then :
else IN='in' ; fi
if AND=`dspmsg -s 1 dircmp.cat 11 "and"`
then :
else AND='and' ; fi
if SPECIAL=`dspmsg -s 1 dircmp.cat 12 "special"`
then :
else SPECIAL='special' ; fi
if COMPAROF=`dspmsg -s 1 dircmp.cat 13 "Comparison of"`
then :
else COMPAROF='Comparison of' ; fi

while [ $# -ge 2 ]
do
	case $1 in
	-d)	Dflag="yes"
		shift
		;;
	-s)	Sflag="yes"
		shift
		;;
	-ds)	Dflag="yes"
	   	Sflag="yes"
		shift
		;;
	-*)	echo $BADOPTION 1>&2
		echo $USAGE	1>&2
		exit 3
		;;
	 *)	D1=$1 D2=$2
		break
		;;
	esac
done
if [ $# -lt 2 ]
then echo $USAGE 1>&2
     exit 1
elif [ ! -d "$D1" ]
then echo $D1 $NOTDIR 1>&2
     exit 2
elif [ ! -d "$D2" ]
then echo $D2 $NOTDIR 1>&2
     exit 2
fi
D0=`pwd`
if cd $D1
then find . -print | sort > /usr/tmp/dc$$a
     if [ $? != 0 ]
     then exit 2
     fi
else exit 2
fi
cd $D0
if cd $D2
then find . -print | sort > /usr/tmp/dc$$b
     if [ $? != 0 ]
     then exit 2
     fi
else exit 2
fi
comm /usr/tmp/dc$$a /usr/tmp/dc$$b | sed -n \
	-e "/^		/w /usr/tmp/dc$$c" \
	-e "/^	[^	]/w /usr/tmp/dc$$d" \
	-e "/^[^	]/w /usr/tmp/dc$$e"
rm -f /usr/tmp/dc$$a /usr/tmp/dc$$b 
pr -h "$D1 $ONLYAND $D2 $ONLY" -m /usr/tmp/dc$$e /usr/tmp/dc$$d
rm -f /usr/tmp/dc$$e /usr/tmp/dc$$d
sed -e s/..// < /usr/tmp/dc$$c > /usr/tmp/dc$$f
rm -f /usr/tmp/dc$$c
cd $D0 
> /usr/tmp/dc$$g
while read a

do
	if [ -d $D1/"$a" ]
	then if [ "$Sflag" != "yes" ]
	     then echo "$DIRECTORY	$a"
	     fi
	elif [ -f $D1/"$a" ]
	then cmp -s $D1/"$a" $D2/"$a"
	     if [ $? = 0 ]
	     then if [ "$Sflag" != "yes" ]
		  then echo "$SAME     	$a"
		  fi
	     else echo "$DIFFERENT	$a"
		  if [ "$Dflag" = "yes" ]
		  then diff $D1/"$a" $D2/"$a" | pr -h "$DIFFOF $a $IN $D1 $AND $D2" >> /usr/tmp/dc$$g
		  fi
	     fi
	elif [ "$Sflag" != "yes" ]
	then echo "$SPECIAL  	$a"
	fi
done < /usr/tmp/dc$$f | pr -r -h "$COMPAROF $D1 $D2" 
if [ "$Dflag" = "yes" ]
then cat /usr/tmp/dc$$g
fi
rm -f /usr/tmp/dc$$*
