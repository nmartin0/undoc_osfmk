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

VPATH			= ../libc

LIBRARIES		= libresolv.a

EXPLIB_TARGETS		= export_libresolv.a

ILIST			= libresolv.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

CFLAGS			= -DDEBUG
OFILES			= herror.o res_comp.o res_debug.o res_init.o \
			  res_mkquery.o res_query.o res_send.o \
			  gethostnmad.o sethostent.o mk_query.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
