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

# Copy Mach3 mk libmach.a to our export dir. Required for "ps" command.

EXPLIB_TARGETS	= export_libcthreads.a

USE_STATIC_LIBRARIES =

ILIST		= libcthreads.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

SRC		= ${MKREL}/lib/libthreads.a

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

${ILIST}:
	cp -p ${SRC} ${ILIST}
	${RM} -f ../libcthreads.a
	ln -s libcthreads/libcthreads.a ../libcthreads.a

clean::
	rm -f ${ILIST}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
