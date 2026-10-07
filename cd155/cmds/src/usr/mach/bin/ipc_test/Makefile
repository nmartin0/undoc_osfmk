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
# @OSF_FREE_COPYRIGHT@
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
# $Log: Makefile,v $
# Revision 1.3  1994/11/19  03:01:29  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:49:23  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/05/19  22:09:31  stans
# Initial revision
#
# Revision 1.2  1992/10/21  13:13:36  devrcs
# 	new for V4.1 release
# 	[1992/10/01  17:05:53  david]
#
# Revision 2.7  92/03/05  22:47:45  rpd
# 	Changed to use Makefile-man.
# 	[92/02/29            rpd]
# 
# Revision 2.6  92/02/16  15:42:04  rpd
# 	Changed libcs to libcmucs.
# 
# Revision 2.5  92/01/22  22:51:30  rpd
# 	Revised for new Makefile organization.
# 	[92/01/17            rpd]
# 
# $EndLog$
# 


ILIST			= ipc_test
IDIR			= /usr/mach/bin/

USE_STATIC_LIBRARIES	=

PROGRAMS = ipc_test

OFILES = main.o mtest.o ntest.o ptest.o utest.o nstest.o stest.o utils.o

LIBS= -lmach -lcs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
