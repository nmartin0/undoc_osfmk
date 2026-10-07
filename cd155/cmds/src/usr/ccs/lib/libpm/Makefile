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
# (c) Copyright 1992, INTEL , CORP.
# ALL RIGHTS RESERVED
#
#
# INTEL  1.0
# $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libpm/Makefile,v 1.4 1994/11/19 02:33:35 mtm Exp $
#

VPATH			= 

OTHERS			= 
OBJECTS			= libpm.o
libpm.o_NOSTRIP		=

EXPLIB_TARGETS		= 
EXPCRT0_TARGETS         = export_libpm.o

ILIST			= libpm.o
IDIR			= /usr/ccs/lib/
IMODE			= 644

INCFLAGS		=
CFLAGS			=
OFILES			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
