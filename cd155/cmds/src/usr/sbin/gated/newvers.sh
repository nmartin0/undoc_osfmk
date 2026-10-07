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
awk '	{	vers = $1 + 1; }\
END	{	printf "char Version[] = \"Version 1.%d ", vers > "vers.c";\
	printf "%d\n", version > "Version"; }' < "$1"

echo `date`'";' >> vers.c
