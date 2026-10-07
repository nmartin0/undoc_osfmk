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
# @(#) $RCSfile: monacct.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:18 $
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
# 	monacct.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 10:56:39
#
# 	"this procedure should be run periodically ( by month or fiscal )"
# 

_adm=${ACCTDIR:-/var/adm}
_sum=${_adm}/acct/sum
_fiscal=${_adm}/acct/fiscal
PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH


#if test $# -ne 1; then
#	echo "usage: monacct fiscal-number"
#	exit 1
#fi

_period=${1-`date +%m`}

cd ${_adm}

#	"move summary tacct file to fiscal directory"
if [ -f ${_sum}/tacct ]
then
	mv ${_sum}/tacct ${_fiscal}/tacct${_period}
fi

#	"delete the daily tacct files"
rm -f ${_sum}/tacct????

#	"restart summary tacct file"
nulladm ${_sum}/tacct

#	"move summary cms file to fiscal directory
if [ -f ${_sum}/cms ]
then
	mv ${_sum}/cms ${_fiscal}/cms${_period}
fi

#	"restart summary cms file"
nulladm ${_sum}/cms

#	"remove old prdaily reports"
rm -f ${_sum}/rprt*

#	"produce monthly reports"
prtacct ${_fiscal}/tacct${_period} > ${_fiscal}/fiscrpt${_period}

if _hperiod="`dspmsg acct.cat 66 'TOTAL COMMAND SUMMARY FOR FISCAL'`"
then :
else
	_hperiod="TOTAL COMMAND SUMMARY FOR FISCAL"
fi

if _hlogin="`dspmsg acct.cat 67 'LAST LOGIN'`"
then :
else
	_hlogin="LAST LOGIN"
fi

acctcms -a -s ${_fiscal}/cms${_period} |  \
pr -h "${_hperiod} ${_period}" >> ${_fiscal}/fiscrpt${_period}
pr -h "${_hlogin}" -3 ${_sum}/loginlog >> ${_fiscal}/fiscrpt${_period}

#	"add commands here to do any charging of fees, etc"
exit 0
