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

PROGRAMS		= csh
MSGHDRS 		= csh_msg.h
CATFILES 		= csh.cat

ILIST			= csh
IDIR			= /usr/bin/

CFLAGS			= -DALLOC -D_BSD -DFILEC
HFILES			= csh_msg.h
OFILES 			= alloc.o dir.o dol.o doprnt.o err.o exec.o \
			  exp.o file.o func.o glob.o hist.o init.o \
			  lex.o misc.o parse.o print.o proc.o sem.o \
			  set.o sh.o time.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
