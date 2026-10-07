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
# OSF/1 Release 1.0.1
#
# @(#)$RCSfile: diff3.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:46 $
#
# COMPONENT_NAME: (CMDFILES) commands that manipulate files
#
# FUNCTIONS: diff3
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
# diff3.sh	1.10  com/cmd/files/diff,3.1,9013 3/5/90 16:31:39
#                                                                   
#  diff3  [-ex3EX] file1 file2 file3 
#  compares three files
#  this shell script calls /usr/lbin/diff3prog to do the work.
e=
case $1 in
-*)
	e=$1
	shift;;
esac
if test $# -lt 3
then
	if [ -x /usr/bin/dspmsg ]; then
	dspmsg -s 1 diff3.cat 2 'usage: diff3 [-exEX3] file1 file2 file3\n' 1>&2
	else	msg='usage: diff3 [-exEX3] file1 file2 file3\n';
	eval echo $msg 1>&2; fi
	exit 1
fi
if test -f $1 -a -f $2 -a -f $3
then
	:
else
	if [ -x /usr/bin/dspmsg ]; then
	dspmsg -s 1 diff3.cat 1 'file not found\n' 1>&2
	else	msg='file not found\n';
	eval echo $msg 1>&2; fi
	exit 1
fi
trap "rm -f /tmp/d3[ab]$$; trap '' 0; exit" 1 2 13 15
diff $1 $3 >/tmp/d3a$$
diff $2 $3 >/tmp/d3b$$
/usr/lbin/diff3prog $e /tmp/d3[ab]$$ $1 $2 $3
if [ $? != 0 ]
then rm -f /tmp/d3[ab]$$
     exit 1
else rm -f /tmp/d3[ab]$$
fi
