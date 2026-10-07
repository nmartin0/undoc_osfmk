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
#
# Mach 3.0 migcom

VPATH			= src

PROGRAMS		= migcom

INCFLAGS		= -Isrc
ILIST			= migcom
IDIR			= /usr/ccs/lib/

error.o_CFLAGS		= ${NO_VFPRINTF?-DNO_VFPRINTF:}

parser.c_YFLAGS		= -d

OFILES			= mig.o parser.o lexxer.o error.o string.o type.o \
			  routine.o statement.o global.o header.o user.o \
			  server.o utils.o
LIBS			= -ll
GARBAGE			= parser.c parser.h

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

parser.h parser.c : parser.y
	${YACC} -d parser.y && \
	mv y.tab.c parser.c && \
	mv y.tab.h parser.h


lexxer.c: parser.h
	${LEX} lexxer.l && \
	mv lex.yy.c lexxer.c

include ${MAKEFILEPATH}/depend.mk
-include Makedep
