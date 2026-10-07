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

VPATH			= ${TARGET_MACHINE}

SUBDIRS			= include
OBJECTS			= mo_ldr.o relocate.o standard.o

EXPLOADER_SUBDIRS	= include
EXPLOADER_TARGETS	= ${OBJECTS}

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I../include -Iinclude -I${TARGET_MACHINE}/include
CFLAGS			= ${${TARGET_MACHINE}CFLAGS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
