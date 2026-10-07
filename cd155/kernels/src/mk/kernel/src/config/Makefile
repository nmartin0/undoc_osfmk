# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
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
# Revision 1.8  1994/11/18  21:01:27  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/06/22  22:54:24  dleslie
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
# Revision 1.6  1993/10/11  16:43:43  dleslie
# Use HOST_CC to set I860_CC instead of hardcoded gcc.
# Add LPATH to the link line for config, so libcs.a can be found on Suns.
#
# Revision 1.5  1993/07/22  02:26:01  andyp
# Workaround a potential dependency problem with crt0.o.
#
# Revision 1.4  1993/06/30  22:58:51  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.3  1993/04/27  20:52:55  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  18:58:38  dleslie
# First R1_0 release
#
# Revision 2.5.2.1  92/03/28  10:16:22  jeffreyh
# 	Chaned CC_I860 to gcc so that kernel can be cross built for now.
# 	[92/03/09            jeffreyh]
# 
# 	Picked up new makefile from TRUNK, use libcs.a instead of
# 	libcmucs.a for now.
# 	[92/03/09            jeffreyh]
# 
# Revision 2.9  92/03/05  22:46:07  rpd
# 	Changed to use double-colon rules for top-level targets.
# 	[92/02/28            rpd]
# 
# Revision 2.8  92/02/20  12:05:02  elf
# 	Changed LIBCS and DEPCS to LIBCMUCS and DEPCMUCS.
# 	[92/02/20            elf]
# 
# Revision 2.7  92/01/24  18:15:42  rpd
# 	Removed mkswapconf.o.
# 	[92/01/24            rpd]
# 
# Revision 2.6  92/01/23  15:21:02  rpd
# 	Created.
# 	[92/01/23            rpd]
# 

include ${MAKETOP}Makefile-common
I860_CC = ${HOST_CC?${HOST_CC}:gcc}
CPATH=${HCPATH}
CFLAGS= `cpath`
I860_LD = ld 
all :: config

install :: ${INSTALLDIR}/bin/config

release :: ${TRELEASEDIR}/bin/config

clean ::
	${RM} ${INSTALLDIR}/bin/config

# installation rules

${INSTALLDIR}/bin/config : config
	${RM} $@
	${CP} $? $@
	${STRIP} $@
	${CHMOD_BIN} $@

# release rules

${TRELEASEDIR}/bin/config : ${FRELEASEDIR}/bin/config
	${RM} $@
	${CP} $? $@

# build rules

DEFS = -DCMU

OBJS = y.tab.o main.o lex.yy.o mkioconf.o mkmakefile.o \
	mkglue.o mkheaders.o mkswapconf.o

# config uses fopenp from libcmucs.a

LIBS = ${LIBL} ${LIBCS}
DEPS = ${DEPL} ${DEPCS} ${DEPCRT0}

config : ${OBJS}
	${RM} $@ $@.out
	LPATH="/usr/lib:/lib:${HLPATH}" ; export LPATH; \
		${DEPS/%/$@} ${CC} -o $@.out ${CFLAGS} ${OBJS} ${LIBS} && \
		${MV} $@.out $@

lex.yy.c : config.l
	lex config.l

y.tab.h y.tab.c : config.y
	yacc -d config.y

lex.yy.o main.o mkioconf.o mkmakefile.o mkglue.o mkheaders.o : y.tab.h

-include Makedep
