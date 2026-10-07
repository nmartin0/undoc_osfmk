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
# @OSF_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.8  1994/11/19  03:18:08  mtm
# Copyright additions/changes
#
# Revision 1.7  1993/12/20  19:56:39  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.6  1993/09/08  21:42:26  shala
# Debug instructions commented. Fixed bug #6506 by stans.
#
# Revision 1.5.6.1  1993/09/08  21:35:05  shala
# Debug instructions commented. Fixed bug #6506 by stans.
#
# Revision 1.5  1993/06/21  22:04:56  wunder
# Added switch -DPFS to turn on disk block preallocation checking.
#
# Revision 1.4  1993/05/17  22:20:49  stans
# Support OSF1AD boot magic interface with a new file admagic.c
#
# Revision 1.3  1992/11/03  00:04:05  shala
# Added -DOSF1_ADFS .
#
# Revision 1.2  1992/10/12  22:07:49  shala
# New version to support maj, min and node numbers.
#
# Revision 1.12.1.3  91/06/03  15:28:18  garyf
# 	undo bsubmit damage
# 	[91/06/03  15:27:23  garyf]
# 
# Revision 1.12.1.2  91/06/03  13:41:06  garyf
# 	add messaging
# 
# Revision 1.12  91/05/14  13:32:28  devrcs
# 	Changes for Reno make
# 	[91/04/22  05:13:20  damon]
# 
# Revision 1.11  90/10/07  22:50:37  devrcs
# 	Turned on optimization.  It's OK now.
# 	[90/10/02  10:08:42  gmf]
# 
# Revision 1.10  90/09/23  17:01:09  devrcs
# 	Added security and shared library passes.
# 	[90/09/16  19:08:50  gm]
# 
# Revision 1.9  90/08/25  12:36:34  devrcs
# 	move MACH_FASTLINK define from Makefile to fsck.h
# 	[90/08/15  09:58:57  gmf]
# 
# Revision 1.8  90/08/09  15:11:52  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:43:46  gm]
# 
# 	Use SEC_DEFS variable to trigger security builds.
# 	[90/08/03  10:08:39  seiden]
# 
# 	Must not optimize any files for now.  This really should be
# 	investigated.  It's a problem on both pmax and mmax.
# 	[90/07/27  16:44:24  gmf]
# 
# 	Added MACH_FASTLINK to compiler directives (fixes bug #269).
# 	Optimize all objects EXCEPT pass1.o (still have bug, at least on pmax).
# 	[90/07/27  13:03:56  gmf]
# 
# $EndLog$

SEC_PROGRAMS		= ufs_fsck
PROGRAMS		= ufs_fsck
MSGHDRS			= ufs_fsck_msg.h
CATFILES		= ufs_fsck.cat

SEC_ILIST		= ufs_fsck
ILIST			= ufs_fsck
IDIR			= /usr/sbin/

##
# Need to add SMART_FSCK to CFLAGS when the kernel is ready
# -DDEBUG=1 enables verbose debug
#
CFLAGS			= ${SEC_DEFS} -DOSF1_ADFS -DPFS -O
dir.o_CFLAGS		= -D_BSD ${CFLAGS}
pass2.o_CFLAGS		= -D_BSD ${CFLAGS}

HFILES			= ${MSGHDRS} fsck.h
##
# NB: should find a way to get ufs_subr.c and ufs_tables.c directly out of
#	the kernel source.
##
OFILES			= dir.o inode.o main.o pass1.o pass1b.o pass2.o \
			  pass3.o pass4.o pass5.o setup.o utilities.o \
			  preen.o ufs_subr.o ufs_tables.o admagic.o
LIBS                    = ${SEC_LIBS} -lnx -lmach
GLINE                   =

# .include <${RULES_MK}>
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
