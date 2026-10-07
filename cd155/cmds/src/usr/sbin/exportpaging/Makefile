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
# $Log: Makefile,v $
# Revision 1.3  1994/11/19  03:05:52  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:52:01  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/09/03  23:35:46  stans
#    exportpaging...
#
# $EndLog$

# For production builds use this version of admagic.c; When debugging use the
# local version and -DTEST=1 for testing on a machine without multiple REAL
# MIO's.
#
VPATH                   = ${MAKETOP}usr/sbin/ufs_fsck

SEC_PROGRAMS		= exportpaging
PROGRAMS		= exportpaging

SEC_ILIST		= exportpaging
ILIST			= exportpaging
IDIR			= /usr/sbin/

# TEST=1 imples an override on BOOT_NODE_LIST, test when MIO nodes are not
# really present; check the code.
#
CFLAGS			= -O ${SEC_DEFS} -DOSF1_ADFS -DTEST=0

HFILES			=

OFILES			= exportpaging.o admagic.o

LIBS                    = ${SEC_LIBS} -lmach

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
