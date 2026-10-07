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

#
# Make file for lint, lint1, lint2, and lint libraries
#

VPATH			= ../common

SUBDIRS			= pass2
PROGRAMS		= lint1
SCRIPTS			= lint
MSGHDRS			= ctools_msg.h
CATFILES		= ctools.cat
DATAFILES		= ${ANSI_LIST} ${LLIB_LIST} Mkf.native

ILIST			= lint1 lint ${DATAFILES}
IDIR			= /usr/ccs/lib/lint/
BIN_IDIR		= /usr/ccs/bin/
LIB_IDIR		= /usr/ccs/lib/
lint1_IDIR		= ${LIB_IDIR}
lint_IDIR		= ${BIN_IDIR}
CFLAGS	= \
	-DNEWCALL	\
	-DHOSTIEEE	\
	-DSDBDEBUG	\
	-DTWOPASS	\
	-DSINGLE_PRECISION	\
	-D_COMPILER	\
	-DLINT

INCFLAGS		= -I../common
HFILES			= ${MSGHDRS}
OFILES = \
	cgram.o	\
	code.o	\
	comm1.o	\
	local.o	\
	messages.o	\
	optim.o	\
	pftn.o	\
	scan.o	\
	tftn.o	\
	trees.o	\
	xdefs.o

ANSI_LIST = llib-lansi.c
LLIB_LIST =	\
	llib-lc.c	\
	llib-port.c	\
	llib-lm.c	\
	llib-lcrses.c

NLINT1	= lint1

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/scripts.mk
include ${MAKEFILEPATH}/datafiles.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
