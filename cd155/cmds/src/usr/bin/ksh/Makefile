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

SUBDIRS			= shlib
PROGRAMS		= ksh
MSGHDRS			= ksh_msg.h
CATFILES		= ksh.cat

ILIST			= ${PROGRAMS}
IDIR			= /usr/bin/

INCFLAGS		= -I. -Iinclude
CFLAGS                  = -DKSHELL -DKSH_88D
jobs.o_CFLAGS		= ${CFLAGS} -D_BSD
HFILES			= ${MSGHDRS}
OFILES			= args.o arith.o blok.o builtin.o cmd.o \
			  ctype.o defs.o echo.o edit.o emacs.o \
			  error.o expand.o fault.o history.o io.o \
			  jobs.o macro.o main.o msg.o name.o \
			  print.o service.o stak.o string.o test.o \
			  vfork.o vi.o word.o xec.o dir.o
LIBFLAGS		= -Lshlib
LIBS			= -lksh

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
