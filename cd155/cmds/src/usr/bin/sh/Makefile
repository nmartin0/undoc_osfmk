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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.1

PROGRAMS		= sh
MSGHDRS 		= sh_msg.h
CATFILES 		= sh.cat

ILIST			= sh
IDIR			= /usr/bin/

sh_ILINKS		= ${IDIR}Rsh

expand.o_OPT_LEVEL	=
CFLAGS			= -D_SHRLIB
OFILES 			= main.o args.o blok.o cmd.o ctype.o defs.o \
			  echo.o error.o expand.o fault.o func.o \
			  hash.o hashserv.o io.o macro.o \
			  msg.o name.o nls.o print.o pwd.o \
			  service.o setbrk.o stak.o string.o \
			  test.o word.o xec.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

sh:	sh_msg.h
defs.h: brkincr.h shctype.h mac.h mode.h name.h stak.h sh_msg.h
expand.o macro.o service.o: shctype.h
args.o blok.o cmd.o ctype.o dosetpath.o echo.o error.o expand.o test.o: defs.h
fault.o func.o io.o macro.o main.o msg.o name.o print.o service.o: defs.h
hash.o hashserv.o nls.o pwd.o setbrk.o stak.o string.o word.o xec.o: defs.h
io.o main.o: dup.h
hashserv.o xec.o: hash.h
pwd.o: mac.h
service.o: mode.h
defs.o: name.h
defs.o main.o stak.o: stak.h
cmd.o macro.o main.o msg.o name.o word.o xec.o: sym.h
fault.o main.o: timeout.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
