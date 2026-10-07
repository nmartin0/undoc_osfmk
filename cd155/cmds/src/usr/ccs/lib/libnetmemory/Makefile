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
# Revision 1.3  1994/11/19  02:26:00  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:41:52  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/06/23  01:09:56  stans
# Initial revision
#
# Revision 1.2  1992/05/12  14:48:14  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  07:48:31  condict]
#
# $EndLog$

VPATH			= ${MAKETOP}usr/include/servers

LIBRARIES		= libnetmemory.a

EXPLIB_TARGETS		= export_libnetmemory.a

ILIST			= libnetmemory.a

IDIR			= /usr/ccs/lib/

IMODE			= 644

INCFLAGS                = -I.

MIGFLAGS		= ${DEFINES} ${INCFLAGS} -nostdinc

MIG_DEFS		= netmemory.defs

OFILES			= netmemoryServer.o netmemoryUser.o

GARBAGE			= netmemory.h netmemoryServer.c netmemoryUser.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
