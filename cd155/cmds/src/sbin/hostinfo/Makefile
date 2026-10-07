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

# reuse admagic.c code; local version is for debug ONLY.
VPATH                   = ${MAKETOP}usr/sbin/ufs_fsck

PROGRAMS		= hostinfo 
MSGHDRS			= hostinfo_msg.h
CATFILES		= hostinfo.cat

ILIST			= hostinfo
IDIR			= /sbin/
IOWNER			= root
IMODE			= 711

USE_STATIC_LIBRARIES	=

HFILES			= ${MSGHDRS}
OFILES 			= hostinfo.o admagic.o
LIBS            	= -lmach

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
