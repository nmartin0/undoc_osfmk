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
# OSF/1 Release 1.0.2

VPATH			= ../libc

LIBRARIES		= libbsd.a

EXPLIB_TARGETS		= export_libbsd.a

ILIST			= libbsd.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

CFLAGS			= -D_BSD
OFILES			= ftime.o mktemp.o nice.o open.o \
			  rand.o regex.o siginter.o \
			  signal.o timezone.o util.o valloc.o \
			  vtimes.o wait.o siginter.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
