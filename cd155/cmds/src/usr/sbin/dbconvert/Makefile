# $Copyright
# Copyright 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.1.8.1  1995/06/11  18:43:00  kat
# Updated copyright for R1.3 PSCP
#
# Revision 1.1  1994/12/20  19:04:40  davidl
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
#


PROGRAMS		= dbconvert

ILIST			= dbconvert

dbconvert_IDIR		= /usr/sbin/
dbconvert_IOWNER	= root
dbconvert_IMODE		= 750

CFLAGS			= $(DBG) -DIPSC ${${TARGET_MACHINE}CFLAGS} 
INCFLAGS                = -I../../include -I../../include/macs
LIBS			= -lmac -lrin

dbconvert_OFILES	= dbconvert.o


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
