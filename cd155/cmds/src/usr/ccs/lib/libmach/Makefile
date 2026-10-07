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

# Copy the kernels (Mach 3.0) version of libmach.a to our export dir.
# Kernels libmach.a superceeds the cms/libs (Mach 2.5) version.

EXPLIB_TARGETS	= export_libmach.a

USE_STATIC_LIBRARIES =

ILIST		= libmach.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

SRC		= ${MKREL}/lib/libmach.a

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

${ILIST}:
	cp -p ${SRC} ${ILIST}
	ar860 d ${ILIST} sbrk.o
	${RM} -f ../libmach.a
	ln -s libmach/libmach.a ../libmach.a

clean::
	rm -f ${ILIST}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
