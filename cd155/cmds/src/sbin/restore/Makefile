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

VPATH			= ${MAKETOP}/usr/sbin/restore

PROGRAMS		= restore
MSGHDRS			= restore_msg.h
CATFILES		= restore.cat

ILIST			= restore
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

CCTYPE			= writable_strings
INCFLAGS		= -I${MAKETOP}/usr/sbin/restore
dirs.o_CFLAGS		= -D_BSD
HFILES			= restore.h restore_msg.h
OFILES			= main.o interactive.o restore.o dirs.o symtab.o \
			  utilities.o chfl.o restoreextern.o tape.o
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
