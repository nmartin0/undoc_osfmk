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
#	@(#)indxbib.sh	4.1	(Berkeley)	83/05/08
#
#	indxbib sh script
#
if test $1
	then /usr/lib/refer/mkey $* | /usr/lib/refer/inv _$1
	mv _$1.ia $1.ia
	mv _$1.ib $1.ib
	mv _$1.ic $1.ic
else
	echo 'Usage:  indxbib database [ ... ]
	first argument is the basename for indexes
	indexes will be called database.{ia,ib,ic}'
fi
