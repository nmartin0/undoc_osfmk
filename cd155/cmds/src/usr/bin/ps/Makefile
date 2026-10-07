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
# @OSF_COPYRIGHT@
#
#
# HISTORY
# Makefile,v
# Revision 1.2  1992/05/11  17:45:00  stans
# Grenoble ps changes with mk libmach.a library.
#
# Revision 1.7  90/10/31  14:53:28  devrcs
# 	Changed to be staticly bound because of libmach problems.
# 	[90/10/14  08:37:17  gm]
# 
# Revision 1.6  90/10/07  16:50:59  devrcs
# 	Added EndLog Marker.
# 	[90/09/28  15:02:16  gm]
# 
# Revision 1.5  90/09/23  16:26:12  devrcs
# 	Added security and shared library passes.
# 	[90/09/16  19:07:54  gm]
# 
# Revision 1.4  90/08/09  14:03:06  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:25:08  gm]
# 
# 	Change install permissions to setuid root.
# 	[90/08/03  11:45:27  brezak]
# 
# 	Use SEC_DEFS variable to trigger security builds.
# 	[90/08/03  10:07:44  seiden]
# 
# $EndLog$

VPATH			= ${TARGET_MACHINE}

SEC_PROGRAMS		= ps
PROGRAMS		= ps
MSGHDRS 		= ps_msg.h
CATFILES		= ps.cat

SEC_ILIST		= ps
ILIST			= ps
IDIR			= /usr/bin/
IOWNER			= ${ROOT_OWNER}
IMODE			= ${SETUID_MODE}

INCFLAGS                = 
USE_STATIC_LIBRARIES	=

CCTYPE			= writable_strings
HFILES			= ${MSGHDRS}
CFLAGS			= -DOSF1_SERVER -DOSF1_ADFS ${SEC_DEFS}
#CFLAGS			= ${SEC_DEFS}
OFILES 			= ps.o devname.o attime.o ${SEC_DEFS?ps_secure.o:} \
			  ${${TARGET_MACHINE}_OFILES}
LIBS			= -lmach ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
