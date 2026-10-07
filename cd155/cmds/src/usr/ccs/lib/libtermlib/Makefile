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

LIBRARIES		= libtermlib.a
MSGHDRS			= termc_msg.h
CATFILES		= termc.cat

EXPLIB_TARGETS		= export_libtermlib.a
EXPLINKS		= libtermcap.a

ILIST			= libtermlib.a
IDIR			= /usr/ccs/lib/
IMODE			= 644
ILINKS			= ${IDIR}libtermcap.a

OFILES			= termcap.o tgoto.o tputs.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/objects.mk

termcap.o: termc_msg.h

include ${MAKEFILEPATH}/depend.mk
-include Makedep

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c


libtermlib.a: libnx.a(${OFILES})
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${AR} ${DEF_ARFLAGS} $@ _cvs_id.o $?
	${RANLIB} $@
	${RM} -f _cvs_id.c _cvs_id.o $? ../$@ ../libtermcap.a
	ln -s libtermlib/libtermlib.a ../libtermlib.a
	ln -s libtermlib.a ../libtermcap.a
