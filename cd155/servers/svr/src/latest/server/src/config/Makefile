# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.6  1994/11/18  20:38:59  mtm
# Copyright additions/changes
#
# Revision 1.5  1994/06/22  22:57:07  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: 9090 -- eliminate dependence on CPATH and LPATH
# 	referring to local system paths on the build machine for
# 	Suns
#  Testing: built server, emulator on Sun and Mach X86
#  Module(s):
#         server/src/config/Makefile
#         server/conf/make.template
#         emulator/conf/make.template
#         Makeconf
#
# Revision 1.4  1993/07/14  18:19:01  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  20:13:25  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:14:47  nandy
# ad103+tnc merged with Intel code.
#
# Revision 1.2  1992/11/30  22:36:26  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:34:14  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  00:32:41  cfj
# Bump major revision number.
#
# Revision 2.3  1991/11/13  13:17:44  rabii
#
# Revision 2.4  1993/04/15  22:47:55  condict
# 	Remove absolute pathname to allow building in a Mach 2.5 environment
# 	that has been ported to a different OS, e.g. OSF/1.
#
# Revision 2.3  91/11/13  13:17:44  rabii
# 	Added i860 support
# 
# Revision 2.2  91/08/31  13:58:42  rabii
# 	Initial V2.0 Checkin
# 
# Revision 3.0  91/01/17  12:07:11  condict
# Unchanged copy from Mach 3.0 BSD UNIX server
# 
# Revision 2.3  90/05/21  13:54:55  dbg
# 		Use -lcs for CS library.
# 		Fix install.
# 		[90/01/22            rvb]
# 
# Revision 2.2  89/11/29  14:15:51  af
# 	fopenp() is a CMU thing only found in libcs.a
# 	[89/10/28            af]
# 
# Revision 2.1  89/08/03  18:10:42  rwd
# Created.
# 
# Revision 2.7  89/02/25  19:17:50  gm0w
# 	Changes for cleanup.
# 
# Revision 2.6  89/02/07  22:56:09  mwyoung
# Code cleanup cataclysm.
# 
# Revision 2.5  89/01/23  22:21:19  af
# 	Added def of CMU for those who do not have the light yet.
# 	[89/01/16  17:42:42  af]
# 
# Revision 2.4  88/09/01  13:53:35  mwyoung
# 	Use /usr/cs/etc/install until it's placed correctly.
# 	[88/08/30            mwyoung]
# 
# Revision 2.3  88/07/29  03:20:45  rpd
# Cleaned up some oddities; no real functional changes.
# 
# 24-Mar-88  Mike Accetta (mja) at Carnegie-Mellon University
#	Added DESTDIR, DDIR and SRCS macros and parameterized
#	installation rules so that they can optionally be used to build
#	and install a new version if the sources are newer than the
#	currently installed binary.
#	[ V5.1(XF22) ]
#
# 09-Nov-87  Mike Accetta (mja) at Carnegie-Mellon University
#	Add ${CFILES} to "depend" rule.
#	[ V5.1(XF21) ]
#
# 08-Jan-87  Robert Beck (beck) at Sequent Computer Systems, Inc.
#	Mod config.x rule to take $P (for parallel make).
#
# 09-Dec-86  Mike Accetta (mja) at Carnegie-Mellon University
#	Install in bin/config with the other kernel maintenance
#	programs.
#
# 31-Oct-86  Avadis Tevanian (avie) at Carnegie-Mellon University
#	Build config.x instead of config.  Make install move config.x to
#	config (../../conf/config is a symbolic link to here).
#
# 19-Jul-86  Mike Accetta (mja) at Carnegie-Mellon University
#	Added -q to install and moved destination from /etc to conf.
#

#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.3 (Berkeley) 2/18/86
#
#

CFILES_SRC =	main.c mkioconf.c mkmakefile.c \
		mkglue.c mkheaders.c mkswapconf.c
CFILES=	y.tab.c lex.yy.c ${CFILES_SRC}
SRCS=${CFILES_SRC} config.h config.y config.l

OBJS=	y.tab.o main.o lex.yy.o mkioconf.o mkmakefile.o \
	mkglue.o mkheaders.o mkswapconf.o

DDIR=../../bin

CFLAGS=-O -DCMU `cpath`
LDFLAGS=-ll -lcs

config.x:$P ${OBJS}
	LPATH=${HLPATH}; export LPATH; ${CC} ${OBJS} -o config.x ${LDFLAGS}

lex.yy.c: config.l
	lex config.l

y.tab.h y.tab.c: config.y
	yacc -d config.y

clean:
	rm -f ${OBJS} lex.yy.c y.tab.c y.tab.h config.x

${DDIR}/config: ${SRCS}
	@$(MAKE) DDIR=${DDIR} install

install: config.x
#####	install -q -c -s config.x ${DDIR}/config
	cp config.x ${DDIR}/config && strip ${DDIR}/config

lint: ${CFILES}
	lint ${CFILES}


depend: ${CFILES}
	grep '^#include' ${CFILES} | grep -v '<' | \
	sed -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
	    -e 's/\.c/.o/' \
	    -e 's,../[a-zA-Z]*/,,' | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep
	echo '$$r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep

# DO NOT DELETE THIS LINE -- make depend uses it

y.tab.o: config.h
lex.yy.o: y.tab.h config.h
main.o: y.tab.h config.h
mkioconf.o: y.tab.h config.h
mkmakefile.o: y.tab.h config.h
mkglue.o: config.h y.tab.h
mkheaders.o: config.h y.tab.h
mkswapconf.o: config.h
