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

PROGRAMS		= vm_stat 
MSGHDRS			= vmstat_msg.h
CATFILES		= vmstat.cat

ILIST			= vm_stat
IDIR			= /usr/bin/
IOWNER			= ${ROOT_OWNER}
IMODE			= ${SETUID_MODE}

USE_STATIC_LIBRARIES	=

HFILES			= ${MSGHDRS}
OFILES 			= vm_stat.o
LIBS            	= -lmachid -lnetname -lmach -lcs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
