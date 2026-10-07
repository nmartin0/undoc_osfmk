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
# @(#) $RCSfile: prctmp.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:19 $
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
# 	prctmp.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 10:57:27
#

#
# 	"print session record file (ctmp.h/ascii) with headings"
# 	"prctmp file [heading]"
# 
PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH
if HDR=`dspmsg acct.cat 14 "SESSIONS, SORTED BY ENDING TIME"`
then :
else HDR="SESSIONS, SORTED BY ENDING TIME"
fi
(
if dspmsg   acct.cat 12 "MAJ/MIN			CONNECT SECONDS	START TIME	SESSION START\n"
then dspmsg acct.cat 13 "DEVICE	UID	LOGIN	PRIME	NPRIME	(NUMERIC)	DATE	TIME\n"
else echo "MAJ/MIN			CONNECT SECONDS	START TIME	SESSION START"
echo "DEVICE	UID	LOGIN	PRIME	NPRIME	(NUMERIC)	DATE	TIME"
fi
cat $*) | pr -h "$HDR"
