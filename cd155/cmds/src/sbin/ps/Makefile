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

VPATH			= ${MAKETOP}usr/bin/ps:${MAKETOP}usr/bin/ps/${TARGET_MACHINE}

SEC_PROGRAMS		= ps
PROGRAMS		= ps
MSGHDRS 		= ps_msg.h
CATFILES		= ps.cat
INCFLAGS                = 

SEC_ILIST		= ps
ILIST			= ps
IDIR			= /sbin/
IOWNER			= ${ROOT_OWNER}
IMODE			= ${SETUID_MODE}

USE_STATIC_LIBRARIES	=

CCTYPE			= writable_strings
INCFLAGS		= -I${MAKETOP}/usr/bin/ps
CFLAGS                  = -DOSF1_SERVER -DOSF1_ADFS ${SEC_DEFS}
HFILES			= ${MSGHDRS}
OFILES 			= ps.o devname.o attime.o ${SEC_DEFS?ps_secure.o:} \
			  ${${TARGET_MACHINE}_OFILES}
LIBS			= -lmach ${SEC_LIBS}
LDFLAGS			= ${LDSTRIP}
GLINE			=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
