#!/bin/sh
# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1991,1990 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie the
# rights to redistribute these changes.
#
#
# HISTORY
# $Log: mkobj.sh,v $
# Revision 1.5  1994/11/18  21:07:34  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/06/30  23:06:54  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.3  1993/04/27  21:01:12  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  19:08:52  dleslie
# First R1_0 release
#
# Revision 2.2  91/02/05  16:54:48  mrt
# 	Checked in for first time
# 
# 
#
#	File: mkobj.sh
#	Author: Robert Baron - derived from mktree.sh
#	Date: 1990
#
#	Shell script to create directories and symbolic links
#	needed before the Mach 3.0 kernel sources can be built.
#
#	usage: This script assumes that your sources are in the
#	a directory mk/src/latest/kernel. cd to mk and then type
#		mkobj [<machine> [<name>]]
#	where machine defaults to i386 and name default to $machine. The
#	correct values for the other machine types are:
#		vax
#		pmax	mips
# 		sun3
#	If you are building for more than one machine type you
#	should run mkobj for each platform.
#

if [ $# -eq 0 ]; then
	machine=i386
else
	machine=$1
fi
if [ $# -eq 2 ]; then
	name=$2
else
	name=$machine
fi
echo machine = $machine and name = $name

Quiet=0

OBJS=obj

if [ ! -d "$OBJS" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS" ]"; fi
	rm -rf "$OBJS"
	mkdir "$OBJS"
fi

if [ ! -d "$OBJS"/${machine}_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/${machine}_mach ]"; fi
	rm -rf "$OBJS"/${machine}_mach
	mkdir "$OBJS"/${machine}_mach
fi
if [ ! -d "$OBJS"/${machine}_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/${machine}_mach/latest ]"; fi
	rm -rf "$OBJS"/${machine}_mach/latest
	mkdir "$OBJS"/${machine}_mach/latest
fi
if [ ! -d "$OBJS"/${machine}_mach/latest/kernel ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/${machine}_mach/latest/kernel ]"; fi
	rm -rf "$OBJS"/${machine}_mach/latest/kernel
	mkdir "$OBJS"/${machine}_mach/latest/kernel
fi
if [ ! -d "$OBJS"/${machine}_mach/latest/user ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/${machine}_mach/latest/user ]"; fi
	rm -rf "$OBJS"/${machine}_mach/latest/user
	mkdir "$OBJS"/${machine}_mach/latest/user
fi
if [ ! -d "$OBJS"/${machine}_mach/latest/user/libmach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/${machine}_mach/latest/user/libmach ]"; fi
	rm -rf "$OBJS"/${machine}_mach/latest/user/libmach
	mkdir "$OBJS"/${machine}_mach/latest/user/libmach
fi
if [ ! -d "$OBJS"/${machine}_mach/latest/user/threads ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/${machine}_mach/latest/user/threads ]"; fi
	rm -rf "$OBJS"/${machine}_mach/latest/user/threads
	mkdir "$OBJS"/${machine}_mach/latest/user/threads
fi

RELS=release
if [ ! -d "$RELS" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS" ]"; fi
	rm -rf "$RELS"
	mkdir "$RELS"
fi

if [ ! -d "$RELS"/${machine}_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/${machine}_mach ]"; fi
	rm -rf "$RELS"/${machine}_mach
	mkdir "$RELS"/${machine}_mach
fi
if [ ! -d "$RELS"/${machine}_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/${machine}_mach/latest ]"; fi
	rm -rf "$RELS"/${machine}_mach/latest
	mkdir "$RELS"/${machine}_mach/latest
fi
if [ ! -d "$RELS"/${machine}_mach/latest/lib ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/${machine}_mach/latest/lib ]"; fi
	rm -rf "$RELS"/${machine}_mach/latest/lib
	mkdir "$RELS"/${machine}_mach/latest/lib
fi
if [ ! -d "$RELS"/${machine}_mach/latest/bin ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/${machine}_mach/latest/bin ]"; fi
	rm -rf "$RELS"/${machine}_mach/latest/bin
	mkdir "$RELS"/${machine}_mach/latest/bin
fi
if [ ! -d "$RELS"/${machine}_mach/latest/include ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/${machine}_mach/latest/include ]"; fi
	rm -rf "$RELS"/${machine}_mach/latest/include
	mkdir "$RELS"/${machine}_mach/latest/include
fi

cd src/latest/kernel
if [ ! "$Quiet" ]; then echo "[ creating kernel/machine ]"; fi
rm -rf machine
ln -s @sys machine
if [ $# -eq 0 ]; then ln -s ${name} @sys; fi
ln -s ${name} ${machine}_mach

cd mach
if [ ! "$Quiet" ]; then echo "[ creating kernel/mach/machine ]"; fi
rm -rf machine
ln -s @sys machine
if [ $# -eq 0 ]; then ln -s ${name} @sys; fi
ln -s ${name} ${machine}_mach

cd ../../user/libmach
if [ ! "$Quiet" ]; then echo "[ creating user/libmach/machine ]"; fi
rm -rf machine
ln -s @sys machine
if [ $# -eq 0 ]; then ln -s ${name} @sys; fi
ln -s ${name} ${machine}_mach

cd ../threads
if [ ! "$Quiet" ]; then echo "[ creating user/threads/machine ]"; fi
rm -rf machine
ln -s @sys machine
if [ $# -eq 0 ]; then ln -s ${name} @sys; fi
ln -s ${name} ${machine}_mach

cd ../..

exit 0
