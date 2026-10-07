#!/bin/sh -
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
# @(#)$RCSfile: calendar.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:19:58 $
#
# @(#)calendar.sh	1.7  com/cmd/misc/calendar,3.1,9021 4/27/90 16:19:35
#
# COMPONENT_NAME: (CMDFILES) commands that manipulate files
#
# FUNCTIONS: calendar
#
# ORIGINS: 3, 26, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
#

#	calendar.sh - calendar command, uses calprog (in /usr/lbin)

#  NAME: calendar [-]
#                                                                     
#  FUNCTION: Writes reminder messages to stdout.
# 	OPTIONS:
#	-	Calls calendar for everyone having calendar in their home 
#		directory and sends any reminders by mail.

PATH=/bin:/usr/bin:/usr/lbin:/usr/lib
_tmp=/tmp/cal$$
_mailmsg=/tmp/calmail2$$
_subj=/tmp/calmail1$$
trap "rm -f ${_tmp} ${_mailmsg} ${_subj}" 0
trap exit 1 2 13 15
calprog > ${_tmp}
case $# in
0)	if [ -s calendar ]; then
		egrep -f ${_tmp} calendar
	else
		dspmsg calendar.cat 1 "%s: %s/calendar not found\n" $0 \
			`pwd`
	fi;;
*)	cat /etc/passwd | \
		sed 's/\([^:]*\):.*:\(.*\):[^:]*$/_dir=\2 _user=\1/' | \
		while read _token; do
			eval ${_token}	# evaluates _dir= and _user=
			if [ -s ${_dir}/calendar ]; then
				egrep -f ${_tmp} ${_dir}/calendar \
				 2>/dev/null > ${_mailmsg}
				if [ -s ${_mailmsg} ]; then
					_date=`date '+%D'`
					dspmsg calendar.cat 2 \
					 "Subject: Calendar for %s\n" ${_date}\
					 > ${_subj}
					cat ${_subj} ${_mailmsg} | mail ${_user}
				fi
			fi
		done;;
esac
exit 0
