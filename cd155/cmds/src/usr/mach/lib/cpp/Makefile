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
# Copyright (c) 1991,1990 Carnegie Mellon University
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
# Revision 1.4  1994/11/19  03:02:40  mtm
# Copyright additions/changes
#
# Revision 1.3  1993/12/20  19:50:20  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1993/06/21  16:31:27  stans
#    Use Mach 2.6 'yacc' instead of OSF tools version; cpp works much better.
#
# Revision 1.1  1993/05/19  23:22:00  stans
# Initial revision
#
# Revision 1.1  1993/05/19  22:09:31  stans
# Initial revision
#
# Revision 1.2  1992/10/21  13:13:36  devrcs
# 	new for V4.1 release
# 	[1992/10/01  17:05:53  david]
#
# Revision 2.7  92/03/05  22:47:45  rpd
# 	Changed to use Makefile-man.
# 	[92/02/29            rpd]
# 
# Revision 2.6  92/02/16  15:42:04  rpd
# 	Changed libcs to libcmucs.
# 
# Revision 2.5  92/01/22  22:51:30  rpd
# 	Revised for new Makefile organization.
# 	[92/01/17            rpd]
# 
# $EndLog$
# 


ILIST			= cpp
IDIR			= /usr/mach/lib/

USE_STATIC_LIBRARIES	=

PROGRAMS		= cpp

OFILES			= cpp.o cpy.o #rodata.o

INCLFAGS		= -I.

CFLAGS			= -DOSF1 -Dunix=1 -DCMU=1 -DCMUCS=1 -DMACH=1 -DFLEXNAMES -MD

YYFIX			= ../../../../../../src/usr/mach/lib/cpp/.yyfix

LIBS			= -lcs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

# YYFIX will produce rodata.c (read-only data).
cpy.c: cpy.y
	${YACC} cpy.y
#	${YYFIX} yyexca yyact yypact yypgo yyr1 yyr2 yychk yydef
	mv y.tab.c cpy.c

cpy.o: cpy.c yylex.c

# rodata.c generation ala YYFIX was dropped as it's compilation caused
# major problems with 'icc'.
#rodata.o: cpy.c

#
# CMU cpp depends on CMU yacc, we depend on the build system being Mach/CMU
#
YACC=/usr/bin/yacc
include ${MAKEFILEPATH}/depend.mk
-include Makedep
