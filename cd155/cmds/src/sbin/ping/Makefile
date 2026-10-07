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

VPATH			= ${MAKETOP}/usr/sbin/ping

SEC_PROGRAMS		= ping
PROGRAMS		= ping
MSGHDRS			= ping_msg.h
CATFILES		= ping.cat

SEC_ILIST		= ping
ILIST			= ping
IDIR			= /sbin/
IOWNER			= root
IMODE			= 4755

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I${MAKETOP}/usr/sbin/ping
CFLAGS			= -D_BSD ${SEC_DEFS}
HFILES			= ${MSGHDRS}
LIBS			= -lbsd ${SEC_LIBS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
