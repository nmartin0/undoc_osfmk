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

VPATH			= ${TARGET_MACHINE}:methods

PROGRAMS 		= cfgmgr sysconfig sysconfigdb 
DATAFILES		= sysconfigtab
MSGHDRS			= cfgmgr_msg.h 
CATFILES		= cfgmgr.cat 

ILIST			= cfgmgr sysconfig sysconfigdb sysconfigtab
IDIR			= /sbin/
sysconfigtab_IDIR	= /etc/

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I${TARGET_MACHINE}
CFLAGS			= -MD ${${TARGET_MACHINE}_CFLAGS}
HFILES			= $(MSGHDRS)
cfgmgr_OFILES 		= cfgmgr.o cm_startup_mode.o cm_request_mode.o \
			  cm_db.o cm_expr.o cm_kls.o cm_method.o \
			  cm_mknods.o cm_msg.o \
			  ${${TARGET_MACHINE}_OFILES}
sysconfig_OFILES	= sysconfig.o cm_msg.o
sysconfigdb_OFILES	= sysconfigdb.o cm_msg.o
cfgmgr_LIBS		= -lAF
sysconfigdb_LIBS	= -lAF

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${TARGET_MACHINE}/machdep.mk

#include ${MAKEFILEPATH}/depend.mk

#-include depend.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
