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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
# 
# 
# OSF/1 Release 1.0

# @(#)$RCSfile: spellin.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:40:07 $

# COMPONENT_NAME: (CMDTEXT) Text Formatting Services
# 
# FUNCTIONS:
# 
# ORIGINS: 3,10,13,27
# 
# (C) COPYRIGHT International Business Machines Corp. 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
# 
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
# 
# spellin.sh - front end to /usr/lbin/spell/spellinprg
# 
# This front end script provides for the overlapping functionality
# of the BSD and ATT versions (which are completely different).
# spellin.sh	1.5  com/bsd.d/spell.d,3.1,9011A 3/13/90 09:23:28
# 

PATH=:/usr/bin
export PATH

SPELL=/usr/lbin/spell
TMP=/tmp/spellin
TMPA=${TMP}A.$$
TMPB=${TMP}B.$$
TMPC=${TMP}C.$$

status=0

trap 'rm -f ${TMP}[ABC].$$ >/dev/null 2>&1; exit $status' 0
trap 'status=1;exit' 1 2 3 15

case A$1 in

A[0-9]*)	# spellin num < in > out

	$SPELL/spellinprg $1
	;;

A)		# spellin < in > out

# $SPELL/hashmake | sort -u +0n > $TMPC
	$SPELL/hashmake | sort -u > $TMPC
	$SPELL/spellinprg `wc -l < $TMPC` < $TMPC
	;;

A[!0-9]*)	# spellin list < in > out

	if [ -s $1 ] 
        then
           $SPELL/hashcheck < $1 > $TMPA &
# $SPELL/hashmake | sort -u +0n > $TMPB
	   $SPELL/hashmake | sort -u  > $TMPB
	   wait
	   sort -mu $TMPA $TMPB > $TMPC
	   $SPELL/spellinprg `wc -l < $TMPC` < $TMPC
        else
	   dspmsg spell.cat 8 "spellin:  List file %s does not exist.\n" $1 >&2
        fi
	;;

esac

status=$?
# trap 0 handles the exit code
