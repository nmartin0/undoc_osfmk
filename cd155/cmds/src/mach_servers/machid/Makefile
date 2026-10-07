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
# @OSF_FREE_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.4  1994/11/21  16:39:22  mtm
# Copyright additions/changes
#
# Revision 1.3  1993/12/20  18:39:55  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1993/05/19  15:15:14  stans
# Moved machid from usr/mach/bin to mach_servers/machid; now installed in
# mach_servers.
#
# Revision 1.1  1993/05/04  01:38:53  stans
# Initial revision
#
# Revision 1.3  1992/06/15  15:25:27  devrcs
# 	corrected copyright
# 	[1992/06/11  08:39:44  david]
#
# 	Update for NORMA_MK9 build.
# 	[1992/06/11  07:50:33  david]
#
# Revision 1.2  1992/05/12  14:32:42  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  05:35:01  condict]
# 
# $EndLog$


PROGRAMS		= machid

ILIST			= machid
IDIR			= /mach_servers/

USE_STATIC_LIBRARIES	=

INCFLAGS                = -I.
LIBS			= -lmachid -lnetname -lmach -lcs
OFILES			= machid.o machid_procs.o machid_debug_procs.o \
			  machid_notify_procs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
