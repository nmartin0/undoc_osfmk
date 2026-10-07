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
# @(#) $RCSfile: chargefee.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:12 $
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
# 	chargefee.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 10:53:59
#

#
# 	      "chargefee login-name number"
# 	"emits tacct.h/ascii record to charge name $number"
# 

_adm=${ACCTDIR:-/var/adm}
cd ${_adm}
PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH

if test $# -lt 2; then
	if dspmsg acct.cat 65 "Usage: chargefee User Number\n" >&2
	then :
	else echo "Usage: chargefee User Number" >&2
	fi
	exit 1
fi

_entry="`printpw -u  | grep \^$1`"
if test -z "${_entry}"; then
	if dspmsg acct.cat 1 "can't find login name " >&2
	then :
	else echo "can't find login name " >&2
	fi
	echo "$1" >&2
	exit 1
fi

amt="`expr "$2".00 : '\(-\{0,1\}[0-9]*\.[0-9][0-9]*\)\.\{0,1\}[0-9]*$'`"
if test -z "$amt" ; then
	if dspmsg acct.cat 2 "charge invalid: " >&2
	then :
	else echo "charge invalid: " >&2
	fi
	echo "$2" >&2
	exit 1
fi

if test ! -r fee; then
	nulladm fee
fi

_userid=`echo "${_entry}" | cut -d: -f2`  # get the UID
echo  "${_userid} $1 0 0 0 0 0 0 0 0 0 0 0 0 $2 0 0 0" >>fee
exit 0
