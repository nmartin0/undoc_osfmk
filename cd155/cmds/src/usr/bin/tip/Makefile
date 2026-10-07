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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2

SUBDIRS			= aculib
PROGRAMS		= tip
MSGHDRS			= tip_msg.h
CATFILES		= tip.cat
DATAFILES		= remote-file phones-file

ILIST			= tip ${DATAFILES}
IDIR			= /usr/bin/
IOWNER			= uucp
IGROUP			= uucp
tip_IMODE		= 4755
remote-file_IDIR	= /etc/
remote-file_IMODE	= 644
phones-file_IDIR	= /etc/
phones-file_IMODE	= 644


CFLAGS			= -D_BSD -DDEFBR=1200 -DDEFFS=BUFSIZ -DACULOG -DPRISTINE
remcap.o_CFLAGS		= -DBUFSIZE=1024 ${CFLAGS}
acutab.o_CFLAGS		= -DV831 -DVENTEL -DHAYES -DCOURIER ${CFLAGS}
HFILES			= tip_msg.h tip.h pathnames.h
OFILES			= acu.o cmds.o cmdtab.o cu.o hunt.o log.o \
			  partab.o remote.o tip.o tipout.o value.o \
			  vars.o acutab.o remcap.o uucplock.o
tip_LIBS		= -lbsd aculib/aculib.a
LINTFLAGS		= -hbacvx

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
