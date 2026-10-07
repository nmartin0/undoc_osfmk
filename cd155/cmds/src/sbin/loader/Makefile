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

SUBDIRS			= ${EXPLOADER_SUBDIRS} kload

EXPLOADER_SUBDIRS	= include ldr_include loader utils \
			  ${${TARGET_MACHINE}_SUBDIRS} \
			  build

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
