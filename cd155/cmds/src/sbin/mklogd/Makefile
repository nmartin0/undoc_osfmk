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
# Revision 1.2  1995/02/14  23:57:37  kat
#  Reviewer:
#  Risk: Low
#  Benefit or PTS #: 12451
#  Testing:
#  Module(s):
#
# Revision 1.1  1995/02/06  17:37:47  stans
# Initial revision
#
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
# $EndLog$
#
# use usr/sbin/ufs_fsck/ version of admagic.c
#
VPATH                   = ${MAKETOP}usr/sbin/ufs_fsck

SEC_PROGRAMS		= mklogd
PROGRAMS		= mklogd

SEC_ILIST		= mklogd
ILIST			= mklogd
IDIR			= /sbin/

#
CFLAGS			= -O ${SEC_DEFS} -DOSF1_ADFS


OFILES			= mklogd.o admagic.o

LIBS                    = ${SEC_LIBS} -lutil -lmach

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
