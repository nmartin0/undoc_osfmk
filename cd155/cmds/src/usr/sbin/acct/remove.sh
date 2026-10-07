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
# @(#) $RCSfile: remove.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:27 $
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
# @(#)remove.sh	1.2  com/cmd/acct,3.1,8943 10/24/89 10:59:05
#

PATH=/usr/sbin/acct:/bin:/usr/bin:/sbin
export PATH

_adm=${ACCTDIR:-/var/adm}

rm -f ${_adm}/acct/sum/wtmp*
rm -f ${_adm}/acct/sum/pacct*
rm -f ${_adm}/acct/nite/lock*
