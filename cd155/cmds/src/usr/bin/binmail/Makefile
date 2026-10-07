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

SEC_PROGRAMS		= binmail
PROGRAMS		= binmail
MSGHDRS			= binmail_msg.h
CATFILES		= binmail.cat

SEC_ILIST		= binmail
ILIST			= binmail
IDIR			= /usr/bin/
IMODE			= ${SETUID_MODE}
IOWNER			= ${ROOT_OWNER}
ILINKS			= ${IDIR}mail

CFLAGS			= ${SEC_DEFS}
HFILES			= ${MSGHDRS}
OFILES			= binmail.o ${SEC_DEFS?binmail_sec.o:}
LIBS			= ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
