#! /bin/sh
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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

# 
# @(#) $RCSfile: prtacct.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:21 $
# 
# 	 COMPONENT_NAME: (CMDACCT) Command Accounting
# 
# 	 FUNCTIONS: none
# 
# 	 ORIGINS: 3, 9, 27
# 
# 	 (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# 	 All Rights Reserved
# 	 Licensed Materials - Property of IBM
# 
# 	 US Government Users Restricted Rights - Use, duplication or
# 	 disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
# 
# 	prtacct.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 10:58:03
#

#	"print daily/summary total accounting (any file in tacct.h format)"
#       "prtacct [-f FIELDS ] [-v] file [heading]"
#
#       "FIELDS is in the form of a field specification for acctmerg (1)"

FIELDS="1-2,5-6,11-15"
PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH
VFLAG=
while [ -n "$1" ]
do
	case "$1" in
	-f)	FIELDS="$2"; shift; shift ;;
	-v)     VFLAG="$1";  shift ;;
	*)      break ;;
	esac
done
if [ ! -r "$1" ]
then
	if ( dspmsg acct.cat 22 "Usage: prtacct [-f Specification ] [-v] File ['Heading']\n" >&2 )
	then :
	else echo "Usage: prtacct [-f Specification ] [-v] File ['Heading']\n" >&2
	fi
	exit 1
fi
# Take all arguments after the file as part of the heading.
# Unfortunately, getopt does not quote its arguments so "heading words"
# appears as two arguments.
(acctmerg -t $VFLAG -h"$FIELDS" <"$1"; acctmerg -p"$FIELDS" $VFLAG <"$1") |\
pr -h "$2"
exit 0
