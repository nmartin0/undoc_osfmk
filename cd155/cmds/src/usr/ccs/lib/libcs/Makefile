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
#
# Mach 3.0: CMU CS dept. libc routines.
#

VPATH                   = ${TARGET_MACHINE}

LIBRARIES		= libcs.a

EXPLIB_TARGETS		= export_libcs.a

ILIST			= libcs.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

INCFLAGS                = -I${TARGET_MACHINE}

OFILES			= atoh.o fold.o getbool.o getname.o makepath.o openp.o \
			  prstab.o quit.o salloc.o searchp.o setpath.o \
			  skipto.o stablk.o stlmatch.o

GARBAGE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

# -include ${TARGET_MACHINE}/machdep.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
