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

LIBRARIES		= libutils.a

EXPLOADER_TARGETS	= ${LIBRARIES}

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I../include -I../ldr_include
open_hash.o_INCFLAGS	= -I${MAKETOP}usr/include/loader ${INCFLAGS}
hash_string.o_INCFLAGS	= -I${MAKETOP}usr/include/loader ${INCFLAGS}
oh_nelem.o_INCFLAGS	= -I${MAKETOP}usr/include/loader ${INCFLAGS}
CFLAGS			= ${${TARGET_MACHINE}CFLAGS}
OFILES			= hash_string.o ldr_alloc.o open_hash.o oh_nelem.o \
			  chain_hash.o squeue.o dqueue.o ldr_sys_int.o \
			  ldr_windows.o ldr_auxv.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
