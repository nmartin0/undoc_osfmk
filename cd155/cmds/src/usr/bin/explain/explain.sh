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

# @(#)$RCSfile: explain.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:38 $

#
# COMPONENT_NAME: (CMDTEXT) Text Formatting Services
#
# FUNCTIONS:
#
# ORIGINS: 26,27,28
#
# (C) COPYRIGHT International Business Machines Corp. 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# explain.sh	1.6 3/3/90 14:39:16

#	explain.sh	4.5	(Berkeley)	83/05/27
#

D=/usr/share/lib/explain.d
while	dspmsg explain.cat 1 'phrase?'
	read x
do
	case $x in
	[a-z]*)	sed -n /"$x"'.*	/s/\(.*\)	\(.*\)/use "\2" for "\1"/p' $D
	esac
done
