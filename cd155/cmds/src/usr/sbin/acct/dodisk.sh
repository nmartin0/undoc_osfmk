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
# @(#) $RCSfile: dodisk.sh,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:03:15 $
# 
# 	 COMPONENT_NAME: (CMDACCT) Command Accounting
# 
# FUNCTIONS: none
# 
# ORIGINS: 3, 9, 27
# 
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
# 
# 	 US Government Users Restricted Rights - Use, duplication or
# 	 disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
# 
# 	dodisk.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 10:55:51
#
# 	'perform disk accounting'
# 

_adm=${ACCTDIR:-/var/adm}
_nite=${_adm}/acct/nite
PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH
set -- `getopt o $*`
if [ $? -ne 0 ]
then
	if ( dspmsg acct.cat 9  "Usage: dodisk [ -o ] [ File ... ]\n" >&2 )
	then :
	else echo "Usage: dodisk [ -o ] [ File ... ]" >&2
	fi
	exit 1
fi
for i in $*; do
	case $i in
	-o)	SLOW=1; shift;;
	--)	shift; break;;
	esac
done

cd ${_adm}
date

if [ "$SLOW" = "" ]
then
	if [ $# -lt 1 ]
	then
		args=`awk '/^\/dev\// { print $1 } ' /etc/fstab | sed 's/rz/rrz/'`
	else
		args="$*"
	fi
	diskusg $args > dtmp
else
	if [ $# -lt 1 ]
	then
		args="/"
	else
		args="$*"
	fi
	for i in $args; do
		if [ ! -d $i ]
		then
			echo "dodisk: $i \c" >&2
			if ( dspmsg acct.cat 10 "is not a directory -- ignored\n" >&2 )
			then :
			else echo "is not a directory -- ignored" >&2
			fi
		else
			dir="$i $dir"
		fi
	done
	if [ "$dir" = "" ]
	then
		if ( dspmsg acct.cat 11 "dodisk: No data\n" >&2 )
		then :
		else echo "dodisk: No data" >&2
		fi
		cp /dev/null dtmp
	else
		find $dir -xdev -print | acctdusg > dtmp
	fi
fi

date
sort +0n +1 -o dtmp dtmp
acctdisk <dtmp >disktmp
chmod 644 disktmp
chown adm disktmp
rm -f dtmp
mv disktmp ${_nite}/dacct
