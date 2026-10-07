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
# Revision 1.7  1994/12/20  19:04:05  davidl
# Added a CRC check and version ID to the macd.data file to detect
# corruption.  This involved changing many functions that read or write
# the macd.data file.
#
# MACS now checks the CRC and version of the macd.data file when it is
# opened.  If the macd.data file is invalid, MACS tries macd.data.new,
# macd.data.old, and macd.data.bak before giving up.  MACS also ensures
# that an existing macd.data is valid before renaming it to
# macd.data.old.
#
# A new command, "dbconvert", reads a database in the old format and adds
# the CRC and version ID.
#
# Note: Cindy wrote most of this code, but I've rewritten a lot of it.
#
#  Reviewer: sdh
#  Risk: Medium
#  Benefit or PTS #:
#  Testing: Developer tests, EATs (EATs failed, but only in expected ways)
#  Module(s): 	cmds_libs/src/usr/include/macs/db.h
# 		cmds_libs/src/usr/include/macs/macpd.h
# 		cmds_libs/src/usr/ccs/lib/libmacs/Makefile
# 		cmds_libs/src/usr/ccs/lib/libmacs/db.c
# 		cmds_libs/src/usr/ccs/lib/libmacs/dbcrc.c	(new file)
# 		cmds_libs/src/usr/lib/macs/macpd.c
# 		cmds_libs/src/usr/lib/macs/ttywrite.c
# 		cmds_libs/src/usr/sbin/dbconvert/Makefile	(new file)
# 		cmds_libs/src/usr/sbin/dbconvert/dbconvert.c	(new file)
# 		cmds_libs/src/usr/sbin/dbcreate/dbcreate.c
#
# Revision 1.6  1994/11/19  02:25:11  mtm
# Copyright additions/changes
#
# Revision 1.5  1994/02/02  23:15:12  mwan
# Fixed PTS #7837
#
#  Reviewer: jkearns
#  Risk: M
#  Benefit or PTS #: 7837
#  Testing:
#  Module(s):  All the MACS modules are replaced in this one time checkin.
#
# Revision 1.4  1994/01/13  17:29:34  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: make a link in the obj tree so command builds can find
# 	this library without using a  hardcoded relative path
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.3  1993/12/20  19:41:45  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1993/11/18  20:17:46  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get macs headers out of export tree
#  Testing: built on Suns and 486
#  Module(s): Makefile
#
# Revision 1.1  1993/10/20  00:04:02  jkearns
# Initial revision
#
# Revision 1.1  1993 10/11/93 jk
# Initial revision
#
#
VPATH			= ${TARGET_MACHINE}
OTHERS			= libmac.a librin.a
INCFLAGS                = -I../../../include -I../../../include/macs
CFLAGS			= $(DBG) -DIPSC -DFILELOCK ${${TARGET_MACHINE}CFLAGS}
OFILES			= ${LIBMACOBJS} ${LIBRINOBJS}
GARBAGE			= *.S *.as tags

LIBMACOBJS =	balance.o runok.o mac_send.o cpuctl.o close.o open.o \
		db.o dbcrc.o actab.o

LIBRINOBJS =	agdb.o checkinfo.o create.o delacct.o \
                deluser.o dump.o list.o misc.o modify.o \
                readconf.o mac_adm.o macerr.o mactalk.o transfer.o \
                update.o get.o syntax.o exist.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c

# i860 COFF doesn't require lorder ...
#COFF_LORDER=`lorder *.o | tsort`
COFF_LORDER=`ls *.o`
A_OUT_LORDER=${OFILES}
MACHO_LORDER=${OFILES}

libmac.a: libmac.a(${LIBMACOBJS})
	${AR} ${DEF_ARFLAGS} $@ $?
	${RM} -rf tmp
	mkdir tmp
	cd tmp && { \
		${AR} x ../libmac.a; ${RM} -f __.SYMDEF __________ELELX; \
		${AR} cr libmac.a ${${OBJECT_FORMAT}_LORDER}; }
	${MV} -f tmp/libmac.a .
	${RM} -rf tmp
	${RANLIB} $@
	${RM} -f $? ../libmac.a
	ln -s libmacs/libmac.a ../libmac.a

librin.a: librin.a(${LIBRINOBJS})
	${AR} ${DEF_ARFLAGS} $@ $?
	${RM} -rf tmp
	mkdir tmp
	cd tmp && { \
		${AR} x ../librin.a; ${RM} -f __.SYMDEF __________ELELX; \
		${AR} cr librin.a ${${OBJECT_FORMAT}_LORDER}; }
	${MV} -f tmp/librin.a .
	${RM} -rf tmp
	${RANLIB} $@
	${RM} -f $? ../librin.a
	ln -s libmacs/librin.a ../librin.a

include ${MAKEFILEPATH}/depend.mk
-include Makedep
