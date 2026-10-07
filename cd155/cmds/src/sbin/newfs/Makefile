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

VPATH			= ${MAKETOP}/usr/sbin/newfs

SEC_PROGRAMS		= newfs
PROGRAMS		= newfs

SEC_ILIST		= newfs
ILIST			= newfs
IDIR			= /sbin/
ILINKS			= ${IDIR}mfs

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I${MAKETOP}/usr/sbin/newfs
CFLAGS			= -D_BSD -DMACH_FASTLINK -DMFS ${SEC_DEFS}
OFILES			= newfs.o mkfs.o ${SEC_DEFS?mkfs_sec.o:}
LIBS                    = ${SEC_LIBS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
