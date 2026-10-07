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
#              INTEL CORPORATION PROPRIETARY INFORMATION
#
#  This software is supplied under the terms of a license
#  agreement or nondisclosure agreement with Intel Corporation
#  and may not be copied or disclosed except in accordance
#  with the terms of that agreement.
#
#
#      Copyright 1992  Intel Corporation.
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.8  1994/11/19  01:35:31  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/10/18  16:07:48  davidl
# Changed file permissions to more reasonable values
#
#  Reviewer: doyle
#  Risk: Medium
#  Benefit or PTS #: 10606
#  Testing: Developer testing, EATs
#  Module(s): cmds_libs/src/usr/bin/nmapmgr/Makefile
#             cmds_libs/src/usr/bin/qcmplx/Makefile
#             cmds_libs/src/usr/bin/qdel/Makefile
#             cmds_libs/src/usr/bin/qdev/Makefile
#             cmds_libs/src/usr/bin/qlimit/Makefile
#             cmds_libs/src/usr/bin/qmgr/Makefile
#             cmds_libs/src/usr/bin/qpr/Makefile
#             cmds_libs/src/usr/bin/qstart/Makefile
#             cmds_libs/src/usr/bin/qstat/Makefile
#             cmds_libs/src/usr/bin/qsub/Makefile
#             cmds_libs/src/usr/lib/nqs/Makefile
#             cmds_libs/src/usr/lib/nqs/setup/Makefile
#             cmds_libs/src/usr/lib/nqs/setup/nqs_dirs.mk
#
# Revision 1.6  1994/01/13  17:34:04  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.5  1993/12/20  19:01:33  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.4  1992/11/20  16:28:50  rkl
# Changed the makefile to deal with automatic nqsmgrhelp.h file creation.
# The qmgr.hlp file was moved from usr/lib/nqs to this directory and the
# makefile changed to use the native compiler on nqsmgrhelp.c.
#
# Revision 1.3  1992/10/17  00:40:08  rkl
# Made libnqs.a a dependency of the program.
#
# Revision 1.2  1992/10/12  04:16:00  rkl
# IMODE set to the correct value.
#
# Revision 1.1  1992/09/24  19:28:28  rkl
# Initial revision
#
#

PROGRAMS		= qmgr
DATAFILES		= qmgr.hlp
ILIST			= qmgr qmgr.hlp
CATFILES		= qmgr.cat

qmgr_IDIR		= /usr/bin/
qmgr_IOWNER		= root
qmgr_IMODE		= 4755

qmgr.hlp_IDIR		= /usr/lib/nqs/
qmgr.hlp_IOWNER		= root
qmgr.hlp_IMODE		=  774

IPATH			= ../../include
INCFLAGS		= -I$(IPATH) -I$(IPATH)/nqs 
CFLAGS			= ${NQSFLAGS} ${${TARGET_MACHINE}CFLAGS}
LIBS			= -lnqs
SRC_DIR			= ../../../../../src/usr/bin/qmgr

MGRDEP			= $(IPATH)/nqs/nqsmgr.h $(IPATH)/nqs/transactcc.h \
			  $(IPATH)/nqs/nqsxvars.h $(IPATH)/nqs/nqsxdirs.h \
			  $(IPATH)/nqs/nqsdirs.h $(IPATH)/nqs/nqspacket.h \
			  $(IPATH)/nqs/nqsvars.h

HFILES			= $(MGRDEP) $(NQSDEP)

qmgr_OFILES		= mgr_adsset.o mgr_cmd.o mgr_main.o mgr_movset.o \
			  mgr_mssgs.o mgr_packet.o mgr_scan.o mgr_show.o 

#
#  The following three targets are needed to compile the nqsmgrhelp.h
#  generation tool and run it on the platform that is producing the
#  qmgr binary.  It seems a little brute force.  Is there a better way?
#
NATIVE_CC = gcc

mgr_main.o:	nqsmgrhelp.h $(HFILES)

nqsmgrhelp.h:	nqsmgrhelp qmgr.hlp
	./nqsmgrhelp

nqsmgrhelp:
	$(NATIVE_CC) -I/usr/include -o $@ $(SRC_DIR)/nqsmgrhelp.c


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ../../ccs/lib/libnqs/nqs.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
