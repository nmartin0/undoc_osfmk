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

PROGRAMS		= eqn

ILIST			= eqn
IDIR			= /usr/bin/

YFLAGS			= -d
HFILES			= e.h e.def
OFILES			= e.o diacrit.o eqnbox.o font.o fromto.o funny.o \
			  glob.o integral.o io.o lex.o lookup.o mark.o \
			  matrix.o move.o over.o paren.o pile.o shift.o \
			  size.o sqrt.o text.o
GARBAGE			= e.c e.def y.tab.c y.tab.h

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

e.def: e.c
	-if [ -f y.tab.h ]; then ${MV} -f y.tab.h e.def; fi
include ${MAKEFILEPATH}/depend.mk
-include Makedep
