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

PROGRAMS		= routed
MSGHDRS			= routed_msg.h
CATFILES		= routed.cat

ILIST			= routed
IDIR			= /usr/sbin/

CFLAGS			= -D_BSD -D_SOCKADDR_LEN
HFILES			= ${MSGHDRS} defs.h af.h defs.h interface.h \
			  table.h trace.h
OFILES			= af.o if.o input.o main.o output.o startup.o\
			  tables.o timer.o trace.o inet.o
LIBS			= -lbsd -lutil

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
