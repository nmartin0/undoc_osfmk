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

VPATH			= ${MAKETOP}/usr/sbin/fsdb

SEC_PROGRAMS		= fsdb
PROGRAMS		= fsdb

SEC_ILIST		= fsdb
ILIST			= fsdb
IDIR			= /sbin/

IMODE                   = 600

USE_STATIC_LIBRARIES	=

CFLAGS			= ${SEC_DEFS} -DOSF1_ADFS -DPFS
OFILES			= fsdb.o ${SEC_DEFS?fsdb_sec.o:}
LIBS			= ${SEC_LIBS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
