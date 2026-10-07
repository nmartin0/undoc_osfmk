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

PROGRAMS		= more
MSGHDRS			= more_msg.h
CATFILES		= more.cat
DATAFILES		= more.help

ILIST			= ${PROGRAMS} ${DATAFILES}
IDIR			= /usr/bin/
LIB_IDIR		= /usr/share/lib/
more_ILINKS		= ${IDIR}page
more.help_IDIR		= ${LIB_IDIR}

command.o_CFLAGS	= -DHELPFILE="\"${LIB_IDIR}more.help\""
HFILES			= globals.h more_msg.h
OFILES			= main.o command.o file.o globals.o print.o \
			  screen.o term.o
more_LIBS		= -lcurses

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
