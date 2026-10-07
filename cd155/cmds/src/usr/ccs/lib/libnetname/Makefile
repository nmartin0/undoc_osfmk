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
# Revision 1.8  1994/11/19  02:26:02  mtm
# Copyright additions/changes
#
# Revision 1.7  1993/12/20  19:41:57  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.6  1993/05/04  00:42:41  stans
# Convert to Mach 3.0 version
#
# Revision 1.2  1992/05/12  14:48:25  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  07:50:14  condict]
#
# $EndLog$
#
# Mach 3.0 version

VPATH			= ${MAKETOP}usr/include/servers

LIBRARIES		= libnetname.a

EXPLIB_TARGETS		= export_libnetname.a

ILIST			= libnetname.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

MIGFLAGS		= ${DEFINES} ${INCFLAGS} -nostdinc
MIG_DEFS		= netname.defs
OFILES			= netnameServer.o netnameUser.o
GARBAGE			= netname.h netnameServer.c netnameUser.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
