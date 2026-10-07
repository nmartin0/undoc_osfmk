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

#
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
# 

PROGRAMS	= make
MSGHDRS		= make_msg.h
CATFILES	= make.cat

ILIST		= make
IDIR		= /usr/ccs/bin/

OFILES		= main.o doname.o misc.o files.o dirs.o libs.o \
		  rules.o dosys.o gram.o dyndep.o \
		  ${ENVIRONMENT} ${REWINDDIR}
HFILES		= defs.h ${MSGHDRS}
LINTFLAGS	= -ax ${CFLAGS}
GARBAGE		= gram.c

libs.o_CFLAGS	= -D${ARCHIVE_FORMAT} ${CFLAGS}
main.o_CFLAGS	= -DMACHINE=\"${MACHINE}\"
rules.o_CFLAGS	= -Dtarget_MACHINE=\"TARGET_MACHINE=${TARGET_MACHINE}\" \
		  -Dtarget_machine=\"target_machine=${target_machine}\" \
		  -Dthis_CPUTYPE=\"CPUTYPE=${CPUTYPE}\" \
		  -Dthis_cputype=\"cputype=${cputype}\"

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
