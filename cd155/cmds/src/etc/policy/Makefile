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

# @(#)Makefile	1.1 23:02:58 4/30/90 SecureWare

SUBDIRS_C2		= acl
SUBDIRS_B1		= ${SUBDIRS_C2} mac
SUBDIRS			= ${SUBDIRS_${SEC_LEVEL}}
SEC_SUBDIRS		= ${SUBDIRS_${SEC_LEVEL}}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
