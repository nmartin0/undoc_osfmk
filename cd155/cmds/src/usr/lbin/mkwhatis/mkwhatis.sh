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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

#
# @(#)$RCSfile: mkwhatis.sh,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:47:14 $
#
# COMPONENT_NAME: (CMDMAN) commands that allow users to read online 
# documentation
#
# FUNCTIONS: 
#
# ORIGINS: 26, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1989
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
# mkwhatis.sh	1.5  com/cmd/man,3.1,9021 4/12/90 18:09:13
#

trap "rm -f /tmp/whatisx.$$ /tmp/whatis$$; exit 1" 1 2 13 15
MANDIR=${1-/usr/share/man}
rm -f /tmp/whatisx.$$ /tmp/whatis$$
if test ! -d $MANDIR ; then exit 0 ; fi
cd $MANDIR
top=`pwd`

for i in man1 man2 man3 man4 man5 man6 man7 man8 mann manl
do
	if [ -d $i ] ; then
		cd $i
	 	if test "`echo [A-W]*.*`" != "[A-W]*.*" ; then
        		/usr/lbin/getNAME [A-W]*.*
		fi
	 	if test "`echo [X-Z]*.*`" != "[X-Z]*.*" ; then
        		/usr/lbin/getNAME [X-Z]*.*
		fi
	 	if test "`echo [a-m]*.*`" != "[a-m]*.*" ; then
        		/usr/lbin/getNAME [a-m]*.*
		fi
	 	if test "`echo [n-z]*.*`" != "[n-z]*.*" ; then
        		/usr/lbin/getNAME [n-z]*.*
		fi
	 	if test "`echo [0-9]*.*`" != "[0-9]*.*" ; then
        		/usr/lbin/getNAME [0-9]*.*
		fi
		cd $top
	fi
done >/tmp/whatisx.$$

sed  </tmp/whatisx.$$ >/tmp/whatis$$ \
	-e 's/\\-/-/'		\
	-e 's/ VAX-11//'	\
	-e 's/\.[iI]X.*//g'	\
	-e 's/\.PP *//g'	\
	-e 's/\.B *//g'		\
	-e 's/\.rS *//g'	\
	-e 's/\.gL *".*"//g'	\
	-e 's/\.br *//g'	\
	-e 's/\\&//g'		\
	-e 's/"" *//g'		\
	-e 's/\.UC [0-9]//g' \
	-e 's/"\([0-9]*\)"/\1/g' \
	-e 's/"LOCAL"/LOCAL/g'	\
	-e 's/\\f[PRIB0123]//g'	\
	-e 's/\\s[-+0-9]*//g'	\
        -e 's/\\\*L//g' \
        -e 's/\\\*O//g' \
	-e 's/.TH *[^ ][^ ]* "\([^ 	]*\)".*	\([^-]*\)/\2(\1)	/' \
	-e 's/.TH *[^ ][^ ]* \([^ 	]*\).*	\([^-]*\)/\2(\1)	/' \
	-e 's/	 /	/g'
/usr/bin/expand -24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100 \
	/tmp/whatis$$ > /tmp/whatis.$$.s
 sort /tmp/whatis.$$.s | /usr/bin/unexpand -a > whatis
chmod 664 whatis >/dev/null 2>&1
rm -f /tmp/whatisx.$$ /tmp/whatis$$ /tmp/whatis.$$.s
exit 0
