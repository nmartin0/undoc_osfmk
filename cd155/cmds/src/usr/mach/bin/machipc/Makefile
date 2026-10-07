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
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie the
# rights to redistribute these changes.
# 
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.4  1994/11/19  03:01:46  mtm
# Copyright additions/changes
#
# Revision 1.3  1993/12/20  19:49:27  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1993/06/29  13:52:08  shala
# Corrected the ILIST to install machipc.
#
# Revision 1.1  1993/05/19  22:07:30  stans
# Initial revision
#
# Revision 1.2  1992/10/21  13:15:42  devrcs
# 	new for V4.1 release
# 	[1992/10/01  17:05:03  david]
#
# Revision 2.4  92/03/05  22:47:54  rpd
# 	Changed to use Makefile-man.
# 	[92/02/29            rpd]
# 
# Revision 2.3  92/02/16  15:43:02  rpd
# 	Changed libcs to libcmucs.
# 
# Revision 2.2  92/01/22  23:10:05  rpd
# 	Created.
# 	[92/01/22            rpd]
# 
# $EndLog$
# 

PROGRAMS		= machipc

ILIST			= machipc
IDIR			= /usr/mach/bin/

USE_STATIC_LIBRARIES	=

machipc_OFILES			= machipc.o

LIBS			= -lnetname -lmach -lcs

CFLAGS			= -D_BSD

# Also provide rules for palloc, icmp, and tcpip,
# but don't build/install them by default.

palloc : palloc.o

icmp : icmp.o

tcpip : tcpip.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
