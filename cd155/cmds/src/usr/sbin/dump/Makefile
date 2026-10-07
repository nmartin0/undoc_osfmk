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
# Revision 1.5  1994/11/19  03:05:31  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/12/20  19:51:50  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.3  1992/11/13  21:12:29  shala
# Make dump.cat file.
#
# Revision 1.2  1992/10/12  21:45:57  shala
# New version to understand maj, min and node numbers.
#
# Revision 1.11  90/10/07  22:07:31  devrcs
# 	Added EndLog Marker.
# 	[90/09/29  15:08:30  gm]
# 
# Revision 1.10  90/08/25  12:28:20  devrcs
# 	insert rcsid string;
# 	[90/08/09  03:49:30  nm]
# 
# Revision 1.9  90/08/09  14:46:03  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:41:55  gm]
# 
# $EndLog$

# @(#) $RCSfile: Makefile,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 03:05:31 $

PROGRAMS		= dump rdump
MSGHDRS			= dump_msg.h
CATFILES                = dump.cat
GARBAGE			= dumprmain.c dumproptr.c dumprtape.c

ILIST			= dump rdump
IDIR			= /usr/sbin/

dumprmain.o_CFLAGS	= -DREMOTE
dumproptr.o_CFLAGS	= -DREMOTE
dumprtape.o_CFLAGS	= -DREMOTE
dumprmt.o_CFLAGS	= -DREMOTE -DDUMP
CFLAGS			= -DOSF1_ADFS

HFILES			= ${MSGHDRS} dump.h

dump_OFILES		= \
			  dumpcheck.o		\
			  dumpextern.o		\
			  dumpitime.o		\
			  dumpmain.o		\
			  dumpoptr.o		\
			  dumptape.o		\
			  dumptraverse.o	\
			  unctime.o

rdump_OFILES		= \
			  dumpextern.o		\
			  dumpitime.o		\
			  dumprmain.o		\
			  dumprmt.o		\
			  dumproptr.o		\
			  dumprtape.o		\
			  dumptraverse.o	\
			  unctime.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

dumprmain.c: dumpmain.c
	${RM} -f dumprmain.c
	${CP} dumpmain.c dumprmain.c

dumproptr.c: dumpoptr.c
	${RM} -f dumproptr.c
	${CP} dumpoptr.c dumproptr.c

dumprtape.c: dumptape.c
	${RM} -f dumprtape.c
	${CP} dumptape.c dumprtape.c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
