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

VPATH			= ${MAKETOP}/usr/bin/mv

PROGRAMS		= mv
MSGHDRS			= mv_msg.h
CATFILES		= mv.cat

ILIST			= mv
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

CFLAGS                  = -DPFS
LIBS                    =

INCFLAGS		= -I${MAKETOP}/usr/bin/mv
HFILES			= ${MSGHDRS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
