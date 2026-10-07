# 
# $Copyright
# Copyright 1991, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
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
# Revision 1.9  1994/11/19  02:39:03  mtm
# Copyright additions/changes
#
# Revision 1.8  1994/11/18  21:02:04  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/06/22  22:54:19  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: remove dependence on setting CPATH and LPATH to
# 	local paths for Sun cross builds (PTS #9090)
#  Testing: built microkernel on Sun and Mach X86 platforms
#  Module(s):
#         Makeconf
#         Makefile
#         Makefile-subdirs
#         kernel/src/mig/Makefile
#         kernel/src/config/Makefile
#
# Revision 1.6  1993/10/11  16:41:30  dleslie
# Use HOST_CC (set in toplevel Makeconf) instead of hardcoded gcc
#
# Revision 1.5  1993/06/30  22:59:40  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.4  1993/05/18  21:13:38  dleslie
# Added /lib to start of LPATH so migcom always finds crt0.o in /lib
#
# Revision 1.3  1993/04/27  20:53:42  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  18:59:25  dleslie
# First R1_0 release
#
# Revision 2.8.2.1  92/03/28  10:16:25  jeffreyh
# 	Change I860_CC so that we can build a migcom that will run in the
# 	cross developent enviroment
# 	[92/03/09            jeffreyh]
# 
# Revision 2.10  92/03/05  22:46:17  rpd
# 	Changed to use double-colon rules for top-level targets.
# 	[92/02/28            rpd]
# 
# Revision 2.9  92/01/23  15:21:21  rpd
# 	Revised for new Makefile organization.
# 	[92/01/16            rpd]
# 

include ${MAKETOP}Makefile-common
I860_CC = ${HOST_CC?${HOST_CC}:gcc}
I860_LD = ld

ITARGETS = ${INSTALLDIR}/bin/mig ${INSTALLDIR}/lib/migcom
TTARGETS = ${TRELEASEDIR}/bin/mig ${TRELEASEDIR}/lib/migcom

all :: mig.sh migcom

install :: ${ITARGETS}

release :: ${TTARGETS}

clean ::
	${RM} ${ITARGETS}

# installation rules

${INSTALLDIR}/bin/mig : mig.sh
	${RM} $@
	${CP} $? $@
	${CHMOD_BIN} $@

${INSTALLDIR}/lib/migcom : migcom
	${RM} $@
	${CP} $? $@
	${STRIP} $@
	${CHMOD_BIN} $@

# release rules

${TRELEASEDIR}/bin/mig : ${FRELEASEDIR}/bin/mig
	${RM} $@
	${CP} $? $@

${TRELEASEDIR}/lib/migcom : ${FRELEASEDIR}/lib/migcom
	${RM} $@
	${CP} $? $@

# build rules

OBJS = mig.o parser.o lexxer.o error.o string.o type.o routine.o \
	statement.o global.o header.o user.o server.o utils.o

PMAX_CCOPTS = -Dvoid=int
SUN3_CCOPTS = -Dvoid=int
MAC2_CCOPTS = -traditional
CPATH=${HCPATH}
CCOPTS = ${${TARGET_MACHINE}_CCOPTS} `cpath`

LIBS = ${LIBL}
DEPS = ${DEPL} ${DEPCRT0}

migcom : ${OBJS}
	${RM} $@ $@.out
	LPATH=/lib:/usr/lib:${HLPATH}; export LPATH;  \
		${DEPS/%/$@} ${CC} -o $@.out ${CFLAGS} ${OBJS} ${LIBS} && \
		${MV} $@.out $@

lexxer.o : parser.h

parser.c parser.h : parser.y
	yacc -d parser.y && \
	mv y.tab.c parser.c && \
	mv y.tab.h parser.h

lexxer.c : lexxer.l
	lex lexxer.l && \
	mv lex.yy.c lexxer.c

-include Makedep
