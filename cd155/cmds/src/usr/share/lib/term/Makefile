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

OBJECTS			= tablpr

ILIST			= tablpr.X
IDIR			= /usr/share/lib/term/
tablpr_ILINKS		= ${IDIR}tabtn300
tablpr_ILINKS		= ${IDIR}tabcrt
README_IMODE		= 644

GARBAGE			= ${OBJECTS:=.o} a.out

mktab.o_INCFLAGS	= -I../../../bin/troff

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${OBJECTS}: mktab.o $${@:=.o}
	${RM} -f $@
	${_CC_} ${_LDFLAGS_} -o ${@:=.X} mktab.o ${@:=.o}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
