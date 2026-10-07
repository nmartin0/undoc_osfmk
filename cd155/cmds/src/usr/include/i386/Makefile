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

VPATH	= ${MAKETOP}/kernel/i386:${MAKETOP}/kernel/i386/${TARGET_MACHINE}

DATAFILES		= asm.h clock.h cpu.h debug.h disk.h endian.h fpreg.h \
			  ipl.h machlimits.h machtime.h machparam.h pcb.h \
			  pic.h pit.h pmap.h psl.h reg.h seg.h thread.h\
			  trap.h vm_tuning.h vmparam.h xpr.h addrconf.h
INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/i386/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
