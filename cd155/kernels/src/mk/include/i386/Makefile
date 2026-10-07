# 
# $Copyright
# Copyright 1991, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# 
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
# any improvements or extensions that they make and grant Carnegie Mellon 
# the rights to redistribute these changes.
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.4  1994/11/18  20:24:20  mtm
# Copyright additions/changes
#
# Revision 1.3  1993/06/30  22:15:38  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.2  1993/04/27  20:13:10  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  18:12:43  dleslie
# First R1_0 release
#
# Revision 2.2.2.1  92/03/28  10:03:09  jeffreyh
# 	Picked up from MK71
# 	[92/03/20            jeffreyh]
# 
# Revision 2.2  92/03/05  23:09:48  rpd
# 	Created.
# 	[92/02/28            rpd]
# 

include ${MAKETOP}Makefile-common
include ${MAKETOP}Makefile-mkdirs

# First, we get the standard machine-dependent definitions:

Machine = i386
include ../Makefile-machine

# This architecture adds some files of its own.

KERNEL_1 = ${KERNEL_1} i386/asm.h i386/eflags.h i386/exec.h i386at/disk.h

KERNEL_2 = ${KERNEL_2} mach/i386/mach_i386.defs mach/i386/mach_i386_types.h \
	mach/i386/fp_reg.h


OBJECT_2 = ${OBJECT_2} mach/i386/mach_i386.h

IDIRS = ${IDIRS} i386 i386at

# We have files in the object directory.

MKODIRS = ${MKODIRS} mach mach/i386

.INIT :: mkodirs

mach/i386/mach_i386.h : ${MAKETOP}kernel/mach/i386/mach_i386.defs
	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${MAKETOP}kernel/mach/i386/mach_i386.defs

# Finally, we include the standard rules.

include ../Makefile-include
-include Makedep
