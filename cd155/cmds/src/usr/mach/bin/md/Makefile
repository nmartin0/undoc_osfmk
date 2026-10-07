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
# Revision 1.3  1994/11/19  03:01:52  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:49:30  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/05/05  19:21:25  stans
# Initial revision
#
# Revision 1.1  1993/05/04  02:08:49  stans
# Initial revision
#
# $EndLog$

PROGRAMS		= md

ILIST			= md
IDIR			= /usr/mach/bin/

CFLAGS			= -D_BSD
LIBS			= -lcs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
