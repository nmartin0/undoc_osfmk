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

VPATH                   = ${MKREL}/include/mach_debug

SUBDIRS			= 
EXPINC_SUBDIRS		= ${SUBDIRS}

MIG_DEFS                = mach_debug.defs mach_debug_types.defs

DATAFILES               = hash_info.h ipc_info.h vm_info.h zone_info.h \
                          page_info.h mach_debug.h mach_debug_types.h \
                          ${MIG_DEFS}

OTHERS			= 
INCLUDES		= ${DATAFILES} ${OTHERS}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/mach_debug/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
