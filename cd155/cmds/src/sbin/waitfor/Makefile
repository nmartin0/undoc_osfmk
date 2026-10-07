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
# Revision 1.4  1994/11/21  16:48:23  mtm
# Copyright additions/changes
#
# Revision 1.3  1993/12/20  18:45:12  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1993/07/22  15:16:02  shala
# Fix the install path to get install under sbin directory.
#
# Revision 1.1  1993/05/21  01:04:52  stans
# Initial revision
#
# Revision 1.2  1992/10/21  13:17:06  devrcs
# 	Created for OSF/1 MK.
# 	[1992/09/23  13:23:52  barbou]
#
# $EndLog$

PROGRAMS		= waitfor

ILIST			= waitfor

IDIR			= /sbin/

LIBS			= -lservice -lmach -lcs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
