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

VPATH			= ${MAKETOP}/usr/sbin/mount

SEC_PROGRAMS		= mount
PROGRAMS		= mount

SEC_ILIST		= mount
ILIST			= mount
IDIR			= /sbin/
INCFLAGS		= -I${MAKETOP}/usr/sbin/mount

USE_STATIC_LIBRARIES	=

CFLAGS			= ${SEC_DEFS} -DOSF1_ADFS -DPFS -DTNC
mount.o_CFLAGS		= -DNFS ${CFLAGS}
mount_sec.o_CFLAGS	= ${SEC_DEFS}
OFILES			= mount.o ${SEC_DEFS?mount_sec.o:}
HFILES			= pathnames.h
LIBS			= -lrpc ${SEC_LIBS} -lesize
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
