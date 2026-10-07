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
# Revision 1.6  1994/11/19  01:35:25  mtm
# Copyright additions/changes
#
# Revision 1.5  1994/10/18  16:07:26  davidl
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
# Revision 1.4  1994/01/13  17:33:33  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.3  1993/12/20  19:01:27  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1992/10/17  00:38:33  rkl
# Made libnqs.a a dependency of the program.
#
# Revision 1.1  1992/09/24  19:26:52  rkl
# Initial revision
#
#
#

PROGRAMS		= qdev
CATFILES		= qdev.cat

ILIST			= qdev
IDIR			= /usr/bin/
IOWNER                  = root
IMODE                   = 4755

IPATH			= ../../include
INCFLAGS		= -I$(IPATH) -I$(IPATH)/nqs 
CFLAGS			= ${NQSFLAGS} ${${TARGET_MACHINE}CFLAGS}
LIBS			= -lnqs

HFILES			= $(IPATH)/nqs/nqsdirs.h $(NQSDEP)

qdev_OFILES		= qdev.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ../../ccs/lib/libnqs/nqs.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
