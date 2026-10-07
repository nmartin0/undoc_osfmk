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
# Revision 1.3  1994/11/21  16:38:49  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  18:39:46  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/07/20  01:05:42  stans
# Mach 3.0 environment-manager
#
# $EndLog$

PROGRAMS		= envmgr testenvmgr

ILIST			= envmgr

IDIR			= /mach_servers/

MIGFLAGS		= ${DEFINES} ${INCFLAGS} -nostdinc

MIG_DEFS		= env_mgr.defs

CFLAGS			= -D_BSD

USE_STATIC_LIBRARIES	=

LIBDIRS			= ${LIBDIRS}

LIBS			= -lenv -lservice -lnetname -lmach -lcs

envmgr_OFILES		= envmgrmain.o envprocs.o

testenvmgr_OFILES       = testenvmgr.o mach_getenv.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
