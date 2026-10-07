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
# HISTORY
# $Log: Makefile,v $
# Revision 1.3  1994/11/19  02:19:44  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:41:00  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/07/19  23:52:58  stans
#    Created 'libenv.a' as the interface to the Mach 3.0 environment server.
#
# $EndLog$

VPATH			= ${MAKETOP}usr/include/servers

LIBRARIES		= libenv.a

EXPLIB_TARGETS		= export_libenv.a

ILIST			= libenv.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

INCFLAGS                = -I.

MIGFLAGS		= ${DEFINES} ${INCFLAGS} -nostdinc

MIG_DEFS		= env_mgr.defs

OFILES			= env_mgrServer.o env_mgrUser.o

GARBAGE			= env_mgr.h

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
