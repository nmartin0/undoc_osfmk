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

PROGRAMS		= write writesrv
MSGHDRS			= write_msg.h writesrv_msg.h
CATFILES		= write.cat writesrv.cat

ILIST			= ${PROGRAMS}
IDIR			= /usr/bin/
IGROUP			= ${TTY_GROUP}
IMODE			= 2711
writesrv_IDIR		= /usr/sbin/

HFILES			= write.h
writesrv_LIBS		= -lutil

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${MSGHDRS:_msg.h=.o}: $${@:.o=_msg.h}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
