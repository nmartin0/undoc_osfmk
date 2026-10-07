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

VPATH			= ${MAKETOP}/usr/sbin/umount

SEC_PROGRAMS		= umount
PROGRAMS		= umount

SEC_ILIST		= umount
ILIST			= umount
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

CFLAGS			= ${SEC_DEFS} -DPFS
umount.o_CFLAGS		= -DNFS ${CFLAGS}
umount_sec.o_CFLAGS	= ${SEC_DEFS}
OFILES			= umount.o ${SEC_DEFS?umount_sec.o:}
LIBS			= -lrpc ${SEC_LIBS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
