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
# Revision 1.12  1994/11/19  01:36:06  mtm
# Copyright additions/changes
#
# Revision 1.11  1994/10/18  16:08:33  davidl
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
# Revision 1.10  1994/01/13  17:34:47  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.9  1993/12/20  19:01:43  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.8  1993/09/11  00:08:52  cfj
# Added ../../lib/nqs/res_msg.o module to the qsub objects. (sdsc)
#
# Revision 1.7  1993/07/14  14:24:41  shala
# Removed mach3 dependencies.
#
# Revision 1.6  1993/01/04  18:42:04  shala
# Added -lmach3 for linking.
#
# Revision 1.5  1993/01/04  17:28:40  shala
# Fixed path to libnx.a .
#
# Revision 1.4  1992/12/17  21:25:14  mwan
# T6 update 2 (include libnx)
#
# Revision 1.3  1992/10/17  00:46:39  rkl
# Made libnqs.a a dependency of the program.
#
# Revision 1.2  1992/10/09  22:30:32  mwan
# T6 freeze
#
# Revision 1.1  1992/09/24  19:29:42  rkl
# Initial revision
#
#
#

PROGRAMS		= qsub
CATFILES		= qsub.cat

ILIST			= qsub
IDIR			= /usr/bin/
IOWNER                  = root
IMODE                   = 4755

IPATH			= ../../include
INCFLAGS		= -I$(IPATH) -I$(IPATH)/nqs 
CFLAGS			= ${NQSFLAGS} ${${TARGET_MACHINE}CFLAGS}
LIBS			= -lnqs -lnx -lmach

HFILES			= $(IPATH)/nqs/nqsdirs.h $(IPATH)/nqs/informcc.h \
			  $(IPATH)/nqs/mkreqcc.h $(NQSDEP)

qsub_OFILES		= qsub.o ../../lib/nqs/res_msg.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ../../ccs/lib/libnqs/nqs.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
