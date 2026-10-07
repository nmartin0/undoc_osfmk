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
#	@(#)roffbib.sh	4.1	(Berkeley)	83/05/08
#
#	roffbib sh script
#
flags=
abstr=
headr=BIBLIOGRAPHY
xroff=nroff
macro=-mbib

for i
do case $1 in
	-[onsrT]*|-[qeh])
		flags="$flags $1"
		shift ;;
	-x)
		abstr=-x
		shift ;;
	-m)
		shift
		macro="-i $1"
		shift ;;
	-V)
		xroff=vtroff
		shift ;;
	-Q)
		xroff="troff -Q"
		shift ;;
	-H)
		shift
		headr="$1"
		shift ;;
	-*)
		echo "roffbib: unknown flag: $1"
		shift
	esac
done
if test $1
then
	(echo .ds TL $headr; refer -a1 -B$abstr $*) | $xroff $flags $macro
else
	(echo .ds TL $headr; refer -a1 -B$abstr) | $xroff $flags $macro
fi
