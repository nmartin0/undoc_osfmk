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
# (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
# ALL RIGHTS RESERVED 
#
#
# OSF/1 Release 1.0.4

SEC_PROGRAMS		= cp
PROGRAMS		= cp
MSGHDRS			= cp_msg.h
CATFILES		= cp.cat

SEC_ILIST		= cp
ILIST			= cp
IDIR			= /usr/bin/

CFLAGS			= ${SEC_DEFS} -DPFS
LIBS			= ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${MSGHDRS:_msg.h=.o}: $${@:.o=_msg.h}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
