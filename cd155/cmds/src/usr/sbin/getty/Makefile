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

SEC_PROGRAMS		= getty
PROGRAMS		= getty
MSGHDRS                 = getty_msg.h
CATFILES                = getty.cat

SEC_ILIST		= getty
ILIST			= getty
IDIR			= /usr/sbin/

CFLAGS			= ${SEC_DEFS} -O1
HFILES			= ${MSGHDRS}
OFILES			= getty.o ${SEC_DEFS?getty_sec.o:}
LIBS			= ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
