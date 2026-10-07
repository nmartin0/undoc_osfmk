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

VPATH			= ${TARGET_MACHINE}

DATAFILES		= chain_hash.h dqueue.h ldr_auxv.h ldr_errno.h \
			  ldr_hash.h ldr_macro_help.h \
			  ldr_machdep.h ldr_malloc.h ldr_package.h \
			  ldr_region.h ldr_switch.h \
			  ldr_symbol.h ldr_symval_std.h ldr_sys_int.h \
			  ldr_types.h ldr_windows.h \
			  open_hash.h squeue.h

EXPLOADER_TARGETS	= ${DATAFILES}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
