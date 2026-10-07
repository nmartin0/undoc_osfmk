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

VPATH			= ${MAKETOP}/usr/bin/ls

PROGRAMS		= ls
MSGHDRS			= ls_msg.h
CATFILES		= ls.cat

ILIST			= ls
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

# -DOSF1_ADFS must be present for -DPFS support in CFLAGS
CFLAGS                  = -DOSF1_ADFS -DPFS
LIBS                    = -lesize

INCFLAGS		= -I${MAKETOP}/usr/bin/ls
HFILES			= ${MSGHDRS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
