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

VPATH			= ${MAKETOP}/usr/bin/rm

PROGRAMS		= rm
MSGHDRS			= rm_msg.h
CATFILES		= rm.cat

ILIST			= rm
IDIR			= /sbin/
ILINKS			= ${IDIR}rmdir

USE_STATIC_LIBRARIES	=

CFLAGS                  = -DPFS

INCFLAGS		= -I${MAKETOP}/usr/bin/rm
HFILES			= ${MSGHDRS}
LIBS			= -lPW
#LDFLAGS			= -%ld,-x -%ld,-S
LDFLAGS			= ${NO_LD_STRIP?${NO_LD_STRIP}:-%ld," -x" -%ld," -S"}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
