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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

# @(#) $RCSfile: Makefile,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:14:20 $
#

VPATH			= ../dump

PROGRAMS		= restore rrestore
MSGHDRS			= restore_msg.h
CATFILES		= restore.cat

ILIST			= restore rrestore
IDIR			= /usr/sbin/

CCTYPE			= writable_strings
dirs.o_CFLAGS		= -D_BSD
rtape.o_CFLAGS		= -DREMOTE
dumprmt.o_CFLAGS	= -DREMOTE -DRESTORE
dumprmt.o_INCFLAGS	= -I../dump
HFILES			= restore.h restore_msg.h
OBJS			= main.o interactive.o restore.o dirs.o symtab.o \
			  utilities.o chfl.o restoreextern.o
restore_OFILES		= ${OBJS} tape.o
rrestore_OFILES		= ${OBJS} rtape.o dumprmt.o
GARBAGE			= rtape.c dumprmt.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

rtape.c: tape.c
	${RM} ${_RMFLAGS_} rtape.c
	${CP} tape.c rtape.c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
