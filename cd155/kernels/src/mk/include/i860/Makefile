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
# Revision 1.6  1994/11/18  20:24:25  mtm
# Copyright additions/changes
#
# Revision 1.5  1994/07/12  19:04:58  andyp
# Merge of the NORMA2 branch back to the mainline.
#
# Revision 1.4.4.1  1994/03/11  22:57:16  andyp
# Added varargs.gcc.h.
#
# Revision 1.4  1993/11/18  17:30:09  dleslie
#  Reviewer: shala
#  Risk: low
#  Benefit or PTS #: copy headers to release tree so cmds/libs build can
# 	find them
#  Testing: built on 486 and Sun
#  Module(s): Makefile
#
# Revision 1.3  1993/06/30  22:15:41  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.2  1993/04/27  20:13:14  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  18:12:46  dleslie
# First R1_0 release
#
# Revision 2.2.2.1  92/03/28  10:03:22  jeffreyh
# 	Picked up from MK71
# 	Added i860/vm_list.h
# 	[92/03/10            jeffreyh]
# 
# Revision 2.2  92/03/05  23:09:57  rpd
# 	Created.
# 	[92/02/28            rpd]
# 

include ${MAKETOP}Makefile-common
include ${MAKETOP}Makefile-mkdirs

# First, we get the standard machine-dependent definitions.

Machine = i860
include ../Makefile-machine

# This architecture adds some files of its own.

KERNEL_1 = ${KERNEL_1} i860/psl.h i860/reg.h i860/varargs.h \
	i860/varargs.pgi.h i860/varargs.gcc.h i860/exec.h i860/va_list.h \
	i860/pmap.h i860/trap.h

IDIRS = ${IDIRS} i860

# Finally, we include the standard rules.

include ../Makefile-include
-include Makedep
