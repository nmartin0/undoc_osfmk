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
# copy the Mach3 mk libmach_sa (has norma routines) to our export lib dir.

EXPLIB_TARGETS	= export_libmach3_sa.a

USE_STATIC_LIBRARIES =

ILIST		= libmach3_sa.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

SRC		= ${MAKETOP}../../src/mk/release/i860_mach/lib/libmach_sa.a

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

${ILIST}:
	cp -p ${SRC} ${ILIST}

clean::
	rm -f ${ILIST}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
