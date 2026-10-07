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
# Revision 1.3  1994/11/19  02:25:03  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:41:42  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/05/04  00:26:30  stans
# Initial revision
#
# Revision 1.3  1992/06/15  15:27:17  devrcs
# 	Changed targets for NORMA_MK9 ...
# 	[1992/06/11  08:21:42  david]
#
# Revision 1.2  1992/05/12  14:47:55  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  07:45:55  condict]
# 
# $EndLog$

VPATH			= ${MAKETOP}usr/include/servers

LIBRARIES		= libmachid.a

EXPLIB_TARGETS		= export_libmachid.a

ILIST			= libmachid.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

INCFLAGS                = -I.

MIGFLAGS		= ${DEFINES} ${INCFLAGS} -nostdinc

MIG_DEFS		= machid.defs machid_debug.defs \
			  machid_dpager.defs

OFILES			= machid_lib.o			\
			  machidServer.o machidUser.o \
			  machid_debugServer.o machid_debugUser.o

GARBAGE			= machid.h		machidServer.c machidUser.c \
			  machid_debug.h	machid_debugServer.c \
						machid_debugUser.c \
			  machid_dpager.h	machid_dpagerServer.c \
						machid_dpagerUser.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
