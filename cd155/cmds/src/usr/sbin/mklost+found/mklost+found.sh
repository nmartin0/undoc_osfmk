#!/sbin/sh -
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
# @(#)mklost+found	1.2  com/cmd/fs/progs,3.1,8943 9/12/89 12:45:46
#
# COMPONENT_NAME: (CMDFS) commands that deal with the file system
#
# FUNCTIONS: mklost+found
#
# ORIGINS: 26, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
# (#)mklost+found.sh	5.1 (Berkeley) 5/28/85
#
mkdir lost+found
cd lost+found
dspmsg mklost+found.cat 1 "creating slots...\n"
for i in 1 2 3 4 5 6 7 8 9 0 a b c d e f
do
	cat </dev/null >${i}XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
done
dspmsg mklost+found.cat 2 "removing dummy files...\n"
for i in 1 2 3 4 5 6 7 8 9 0 a b c d e f
do
	rm ${i}X*
done
cd ..
dspmsg mklost+found.cat 3 "done\n"
ls -ld `pwd`/lost+found
