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

VPATH			= ${MAKETOP}/usr/bin/chown

SEC_PROGRAMS		= chown
PROGRAMS		= chown
MSGHDRS			= chown_msg.h
CATFILES		= chown.cat

SEC_ILIST		= chown
ILIST			= chown
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I${MAKETOP}/usr/bin/chown
CFLAGS			= ${SEC_DEFS} -DPFS
HFILES			= ${MSGHDRS}
LIBS			= ${SEC_LIBS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
