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

SEC_SUBDIRS		= bin share sbin
SUBDIRS			= include ccs ${NO_SHARED_LIBRARIES?:shlib} \
			  lib lbin share bin sbin mach

EXPINC_SUBDIRS		= include
EXPLIBC_SUBDIRS		= ccs
EXPCOMMON_SUBDIRS	= ccs
EXPSHLIB_SUBDIRS	= shlib
EXPLIB_SUBDIRS		= ccs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
