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
# Revision 1.5  1994/11/21  16:39:30  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/12/20  18:39:59  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.3  1993/05/19  15:17:36  stans
# whitespace additions
#
# Revision 1.2  1993/05/17  17:03:33  stans
# 'snames' is a real Mach 3.0 server; install in '/mach_servers'.
#
# Revision 1.1  1993/05/04  02:01:17  stans
# Initial revision
#
# Revision 1.2  1992/05/12  14:34:42  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  05:51:04  condict]
#
# $EndLog$

PROGRAMS		= snames

ILIST			= snames
IDIR			= /mach_servers/

USE_STATIC_LIBRARIES	=

LIBDIRS			= ${LIBDIRS}

LIBS			= -lservice -lnetname -lmach -lcs

OFILES			= snames.o nprocs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
