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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.4
#

SEC_SUBDIRS		= 

SUBDIRS			= ${SEC_SUBDIRS} \
			  mach_init machid snames envmgr \
			  ${${TARGET_MACHINE}_SUBDIRS}

DATAFILES		=

EXPLOADER_SUBDIRS	= 

ILIST			= ${DATAFILES}
IDIR			= /mach_servers/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
-include ${TARGET_MACHINE}/machdep.mk
