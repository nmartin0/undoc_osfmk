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
# Revision 1.3  1994/11/19  01:31:23  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  18:59:50  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/06/18  14:46:07  shala
# Files for new version of awk command.
#
# Revision 1.11  90/10/07  16:46:28  devrcs
# 	Changed name to nawk.
# 	[90/10/05  13:05:38  gm]
# 
# Revision 1.10  90/09/23  16:18:12  devrcs
# 	Sigh..  Still need changes to y.tab.h.
# 	[90/09/20  23:05:26  gm]
# 
# 	Fixed up y.tab.h tests.
# 	[90/09/20  18:34:54  gm]
# 
# 	Updated code to rename y.tab.h file.
# 	[90/09/20  15:45:40  gm]
# 
# Revision 1.9  90/09/13  12:11:05  devrcs
# 	Added -DMAKETAB to maketab CFLAGS.
# 	[90/09/07  07:44:08  gm]
# 
# 	Trying to work-around problems with hostcc....
# 	[90/09/04  08:14:49  lehotsky]
# 
# 	Cleanup, RCS header, NLS, MSG
# 	[90/08/29  09:55:17  knight]
# 
# Revision 1.8  90/08/09  13:51:38  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:23:09  gm]
# 
# $EndLog$

PROGRAMS		= nawk
MSGHDRS			= awk_msg.h
CATFILES		= awk.cat

ILIST			= nawk
IDIR			= /usr/bin/

YFLAGS			= -d
FILES			= ${I18N_FLAG}.lx.o b.o main.o tran.o lib.o\
			  run.o parse.o proctab.o
OFILES			= awk.g.o ${FILES}
LIBS			= -lm -lPW
GARBAGE			= proctab.c y.tab.h maketab.o maketab lex.yy.c

maketab.o_CCTYPE	= host
maketab.o_INCFLAGS	= -I.
maketab.o_CFLAGS	= -DMAKETAB
maketab_CCTYPE		= host
maketab_LIBS		=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${I18N_FLAG}.lx.o: awk.h awk.g.h

awk.g.o: awk.g.y awk.def awk_msg.h

awk.g.h: awk.g.o
	if [ -f y.tab.h ]; then ${MV} -f y.tab.h awk.g.h; fi

$(FILES): awk.h awk.def awk_msg.h awk.g.h

proctab.c: maketab
	${RM} -f $@
	./maketab > $@

maketab.o: awk.g.h

maketab: maketab.o
	${_CC_} ${_LDFLAGS_} -o $@.X maketab.o
	${MV} $@.X $@
include ${MAKEFILEPATH}/depend.mk
-include Makedep
