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

SEC_PROGRAMS		= ping
PROGRAMS		= ping
MSGHDRS			= ping_msg.h
CATFILES		= ping.cat

SEC_ILIST		= ping
ILIST			= ping
IDIR			= /usr/sbin/
IOWNER			= ${ROOT_OWNER}
IMODE			= ${SETUID_MODE}

CFLAGS			= -D_BSD ${SEC_DEFS}
LIBS			= -lbsd ${SEC_LIBS}

HFILES			= ${MSGHDRS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
