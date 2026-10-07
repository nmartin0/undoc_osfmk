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

SEC_PROGRAMS		= ufs_fsck
PROGRAMS		= ufs_fsck

SEC_ILIST		= ufs_fsck
ILIST			= ufs_fsck
IDIR			= /usr/sbin/

##
# Need to add SMART_FSCK to CFLAGS when the kernel is ready
##
CFLAGS			= ${SEC_DEFS}
dir.o_CFLAGS		= -D_BSD ${CFLAGS}
pass2.o_CFLAGS		= -D_BSD ${CFLAGS}

HFILES			= fsck.h
##
# NB: should find a way to get ufs_subr.c and ufs_tables.c directly out of
#	the kernel source.
##
OFILES			= dir.o inode.o main.o pass1.o pass1b.o pass2.o \
			  pass3.o pass4.o pass5.o setup.o utilities.o \
			  preen.o ufs_subr.o ufs_tables.o
LIBS                    = ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
