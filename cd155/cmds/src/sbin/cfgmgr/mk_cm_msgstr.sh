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

# @(#)$RCSfile: mk_cm_msgstr.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:32 $

IN=cfgmgr.msg
OUT=cm_msgstr.h

echo '
/*
 * @O''SF_COPYRIGHT@
 */

/*
 * HISTORY
 * $L''og$
 * 
 * $E''ndLog$
 */

/*
 * @(#)$R''CSfile$ $R''evision$ (OSF) $D''ate$
 */
'								> $OUT

echo "
char * cmgr_msgstr[] = {
	\"No error\", "						>> $OUT

grep  '^[A-Z]' $IN | awk -F\" '{printf("\t\"%s\",\n",$2)}'  	>> $OUT

echo "	NULL
}; "								>> $OUT

echo Created cm_msgstr.h
