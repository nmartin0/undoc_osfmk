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
# @(#) $RCSfile: turnacct.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:34 $
#
# COMPONENT_NAME: (CMDACCT) Command Accounting
#
# FUNCTIONS: none
#
# ORIGINS: 3, 9, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# turnacct.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 11:00:59
#

#	"control process accounting (must be root)"
#	"turnacct on	makes sure it's on"
#	"turnacct off	turns it off"
#	"turnacct switch	switches pacct to pacct?, starts fresh one"
#	"/var/adm/pacct is always the current pacct file"
#

PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH
cd ${ACCTDIR:-/var/adm}
case "$1"  in
on)
	if test ! -r pacct
	then
		nulladm pacct
	fi
	accton pacct
	rc=$?
	;;
off)
	accton
	rc=$?
	;;
switch)
	if test -r pacct
	then
		_i=1
		while test -r pacct${_i}
		do
			_i="`expr ${_i} + 1`"
		done
		mv pacct pacct${_i}
	fi
	nulladm pacct
	accton
	accton pacct
	_rc=$?
	if test ${_rc} -ne 0; then
		echo "accton failed" >&2
		rm pacct
		mv pacct${_i} pacct
		exit ${_rc}
	fi
	;;
*)
	if ( dspmsg acct.cat 64 "Usage: turnacct on|off|switch\n" >&2 ); then :
	else echo "Usage: turnacct on|off|switch" >&2
	fi
	_rc=1
	;;
esac
exit ${_rc}
