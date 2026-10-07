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

VPATH		= ../../common

PROGRAMS	= lint2
MSGHDRS		= lint_msg.h ctools_msg.h
CATFILES	= lint.cat ctools.cat

ILIST		= lint2
IDIR		= /usr/ccs/lib/

HFILES		= ${MSGHDRS}
INCFLAGS	= -I../../common
CFLAGS	= 		\
	-DHOSTIEEE	\
	-DLINTP2
OFILES =	\
	hasher.o	\
	lint2.o		\
	main.o		\
	output.o	\
	reader.o	\
	storage.o	\
	tftn.o		\
	xdefs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
