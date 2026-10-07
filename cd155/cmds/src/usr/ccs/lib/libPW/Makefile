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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

LIBRARIES		= libPW.a
MSGHDRS			= pw_msg.h
CATFILES		= pw.cat

EXPLIB_TARGETS		= export_libPW.a

ILIST			= libPW.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

OFILES = \
	abspath.o \
	any.o \
	anystr.o \
	bal.o \
	cat.o \
	clean.o \
	curdir.o \
	data.o \
	dname.o \
	fatal.o \
	fdfopen.o \
	giveup.o \
	imatch.o \
	lockit.o \
	logname.o \
	move.o \
	patoi.o \
	patol.o \
	regcmp.o \
	regex.o \
	repeat.o \
	repl.o \
	satoi.o \
	setsig.o \
	sname.o \
	strend.o \
	substr.o \
	trnslat.o \
	userdir.o \
	userexit.o \
	username.o \
	verify.o \
	xalloc.o \
	xcreat.o \
	xlink.o \
	xmsg.o \
	xopen.o \
	xpipe.o \
	xunlink.o \
	xwrite.o \
	zero.o \
	zeropad.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

fatal.o regex.o xcreat.o xlink.o xmsg.o xopen.o: pw_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
