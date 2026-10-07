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

PROGRAMS		= rdist
MSGHDRS			= rdist_msg.h
CATFILES		= rdist.cat

ILIST			= rdist
IDIR			= /usr/bin/
IOWNER			= root
IMODE			= 4711

CFLAGS			= -D_BSD
HFILES			= rdist_msg.h defs.h
OFILES			= docmd.o expand.o gram.o lookup.o main.o server.o
LIBS			= -lbsd
GARBAGE			= gram.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
