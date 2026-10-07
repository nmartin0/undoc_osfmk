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
# OSF/1 Release 1.0.1

VPATH			= ${TARGET_MACHINE}

LIBRARIES		= libsys5.a

EXPLIB_TARGETS		= export_libsys5.a

IDIR			= /usr/ccs/lib/
ILIST			= libsys5.a
IMODE			= 644

INCFLAGS		= -I${TARGET_MACHINE}
CFLAGS			= ${${TARGET_MACHINE}CFLAGS}
STDOBJS			= setpgrp.o mount.o umount.o mknod.o rmdir.o mktemp.o \
			  ptrace.o 
OFILES			= ${STDOBJS} \
			  ${${TARGET_MACHINE}SOBJS} \
			  ${${TARGET_MACHINE}COBJS} 

-include ${TARGET_MACHINE}/machdep.mk

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
