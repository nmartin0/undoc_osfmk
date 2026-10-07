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

LIBLD			= ${MAKETOP}/usr/ccs/lib/libld
VPATH			= ${LIBLD}:${LIBLD}/${TARGET_MACHINE}

SHARED_LIBRARIES	= libld.so
LDFLAGS			= -S -x
EXPORTS			= -export libld:
LIBS			= -lc

EXPSHLIB_TARGETS	= export_libld.so

ILIST			= libld.so
IDIR			= /usr/shlib/
IMODE			= 644

OFILES			= ARfuncs.o libld.o mach_o_hdr.o symutil.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
