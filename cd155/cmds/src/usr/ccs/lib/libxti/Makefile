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

VPATH			= ../libtli

LIBRARIES		= libxti.a

EXPLIB_TARGETS		= export_libxti.a

ILIST			= libxti.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

#CFLAGS			= -DXTI -DXTIDBG -DXTIBDBG
CFLAGS			= -DXTI
OFILES= iostate.o taccept.o talloc.o tbind.o tblock.o tclose.o \
	tconnect.o terror.o tfixup.o tfree.o tgetinfo.o tgetst.o \
	tlisten.o tlook.o tnblock.o topen.o toptmgmt.o trcv.o \
	trcvcon.o trcvdis.o trcvrel.o trudata.o truderr.o tsnd.o \
	tsnddis.o tsndrel.o tsudata.o tsync.o tunbind.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
