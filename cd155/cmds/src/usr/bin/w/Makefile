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

SEC_PROGRAMS		= w
PROGRAMS		= w
MSGHDRS			= w_msg.h
CATFILES		= w.cat

SEC_ILIST		= w
ILIST			= w
IDIR			= /usr/bin/
IOWNER			= root
IMODE			= 4711
ILINKS			= ${IDIR}uptime

USE_STATIC_LIBRARIES	=

INCFLAGS                = 
CFLAGS			= -DOSF1_SERVER -DOSF1_ADFS ${SEC_DEFS}
OFILES			= w.o ${SEC_DEFS?w_secure.o:}
LIBS			= ${SEC_LIBS} -lmach

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${MSGHDRS:_msg.h=.o}: $${@:.o=_msg.h}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
