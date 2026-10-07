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

SEC_PROGRAMS		= wall
PROGRAMS		= wall
MSGHDRS			= wall_msg.h
CATFILES		= wall.cat

SEC_ILIST		= wall
ILIST			= wall
IDIR			= /usr/sbin/
IGROUP			= ${TTY_GROUP}
IMODE			= ${SETGID_MODE}

CCTYPE			= writable_strings
CFLAGS			= ${SEC_DEFS}
HFILES			= ${MSGHDRS}
LIBS			= ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
