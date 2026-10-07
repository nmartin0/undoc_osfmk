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
# OSF/1 Release 1.0.3

VPATH			= ${SVROBJ}/include/kern

DATAFILES		= assert.h event.h kalloc.h lock.h \
			  macro_help.h mfs.h parallel.h queue.h \
			  sched_prim.h threadcall.h xpr.h zalloc.h

INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/kern/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
