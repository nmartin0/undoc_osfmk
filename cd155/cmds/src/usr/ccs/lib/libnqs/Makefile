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
# (c) Copyright 1992, INTEL, Corp.
# ALL RIGHTS RESERVED
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.7  1994/11/19  02:26:14  mtm
# Copyright additions/changes
#
# Revision 1.6  1994/01/13  17:28:48  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: make link in the obj tree so command builds can find
# 	this library without using a hardcoded relative path
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.5  1993/12/20  19:42:00  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.4  1993/01/27  01:39:47  mwan
# T8  Bug fix 3997. qstat -t switch didn't work for NQS operators.
# Not tested with T7 yet !
# GJK
#
# Revision 1.3  1992/12/27  00:25:12  shala
# Added version.o .
#
# Revision 1.2  1992/10/17  00:22:17  rkl
# Removed ILIST, IDIR, and IMODE variables in hopes that it won't install in
# the release directory.
#
# Revision 1.1  1992/09/24  16:45:44  rkl
# Initial revision
#
#
VPATH			= ${TARGET_MACHINE}
OTHERS			= libnqs.a
INCFLAGS		= -I../../../include -I../../../include/nqs 
CFLAGS			= ${NQSFLAGS} ${${TARGET_MACHINE}CFLAGS}
OFILES			= ${NQSLIBOBJS}
GARBAGE			= *.S *.as tags

NMAPLIBOBJS =	nmapread.o nmapwrite.o nmapcommon.o

NQSLIBOBJS =	a6btoul.o allodb.o analyzercm.o analyzetcm.o \
		asciierrno.o blockdb.o bufstderr.o bufstdout.o \
		bytecopy.o bytezero.o closedb.o daepres.o \
		delreq.o destacct.o destdev.o destpath.o \
		destqueue.o errnototcm.o establish.o fetchgrdb.o \
		fetchpwdb.o filecopy.o fmtgidname.o fmtmidname.o \
		fmttime.o fmtuidname.o fopendata.o freedb.o \
		fsizedb.o gethdr.o getreq.o getrreq.o getsockch.o \
		getwdir.o grpnam.o inter.o interread.o \
		interwrite.o isdecstr.o ldparam.o localmid.o \
		listnet.o listq.o listr.o main_dsp.o \
		machacct.o machpath.o machspec.o macnam.o \
		mapuser.o mergertcm.o mkdefault.o mkreq.o \
		movreq.o namstderr.o namstdlog.o namstdout.o \
		nextdb.o nextseqno.o nqspriv.o nqssleep.o \
		opendata.o opendb.o openqacc.o openqord.o \
		pack6name.o parseserv.o pipeqdest.o pipeqdiag.o \
		pipeqreq.o quereqvlpq.o quolimok.o rcimsgs.o \
		rcmmsgs.o readdb.o readhdr.o readreq.o \
		reqspec.o rewritedb.o scancpulim.o \
		scanquolim.o scantime.o secgrfir.o seekdb.o \
		seekdbb.o serexit.o setmtime.o setpeertcm.o \
		shoalldev.o shoalllim.o shoallque.o shodbydesc.o \
		shodbyname.o sholbymach.o shoqbydesc.o shoqbyname.o \
		sizedb.o tcimsgs.o tcmident.o tcmmsgs.o telldb.o \
		transact.o usenam.o verifyhdr.o verifyreq.o \
		writehdr.o writereq.o $(NMAPLIBOBJS)

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk
include nqs.mk

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c

# i860 COFF doesn't require lorder ...
#COFF_LORDER=`lorder *.o | tsort`
COFF_LORDER=`ls *.o`
A_OUT_LORDER=${OFILES}
MACHO_LORDER=${OFILES}

libnqs.a: libnqs.a(${OFILES})
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${AR} ${DEF_ARFLAGS} $@ _cvs_id.o $?
	${RM} -rf tmp
	mkdir tmp
	cd tmp && { \
		${AR} x ../libnqs.a; ${RM} -f __.SYMDEF __________ELELX; \
		${AR} cr libnqs.a ${${OBJECT_FORMAT}_LORDER}; }
	${MV} -f tmp/libnqs.a .
	${RM} -rf tmp
	${RANLIB} $@
	${RM} -f $? _cvs_id.c _cvs_id.o ../libnqs.a
	ln -s libnqs/libnqs.a ../libnqs.a

include ${MAKEFILEPATH}/depend.mk
-include Makedep
