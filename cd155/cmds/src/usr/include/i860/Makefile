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

SVR_I860 = ${SVROBJ}/machine
MK_I860  = ${MKREL}/include/i860
#

VPATH = ${SVR_I860}:${MK_I860}


DATAFILES		= asm.h cpu.h endian.h machlimits.h vmparam.h pcb.h \
			  machtime.h machparam.h pmap.h trap.h psl.h reg.h \
			  inline_lock.h disk.h cthreads.h exec.h

INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/i860/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
