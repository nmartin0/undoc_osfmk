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
# Revision 1.3  1994/11/19  03:02:02  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:49:40  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/05/19  22:05:22  stans
# Initial revision
#
# Revision 1.2  1992/05/12  14:34:22  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  05:48:32  condict]
#
# $EndLog$

PROGRAMS		= pinfo

ILIST			= pinfo
IDIR			= /usr/mach/bin/

LIBS			= -lmachid -lnetname -lmach -lcs -lm

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
