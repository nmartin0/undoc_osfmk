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

# sec_export.c comes from seccmd
VPATH			= ${MAKETOP}/seccmd

SEC_PROGRAMS		= tar
PROGRAMS		= tar
MSGHDRS			= tar_msg.h
CATFILES		= tar.cat

SEC_ILIST		= tar
ILIST			= tar
IDIR			= /usr/bin/

sec_export.o_INCFLAGS	= -I${MAKETOP}/seccmd	# for sec_export.h
CFLAGS			= ${SEC_DEFS} -DPFS
HFILES			= ${MSGHDRS}
OFILES			= tar.o ${SEC_DEFS?sec_export.o:}
LIBS			= ${SEC_LIBS} -lesize

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

sec_export.o:	sec_export.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
