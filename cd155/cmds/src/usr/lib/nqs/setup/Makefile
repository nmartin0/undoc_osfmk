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
# Revision 1.9  1994/11/19  02:53:41  mtm
# Copyright additions/changes
#
# Revision 1.8  1994/10/18  16:09:00  davidl
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
# Revision 1.7  1994/06/14  00:04:01  jkearns
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.6  1994/05/11  00:55:47  jkearns
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.5  1994/01/13  17:30:31  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.4  1993/12/20  19:46:56  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.3  1992/11/11  01:41:36  shala
# Fixed install path.
#
# Revision 1.2  1992/10/21  20:27:16  rkl
# nqsmktrans_OFILES was missing the S in OFILES.
#
# Revision 1.1  1992/10/17  00:05:23  rkl
# NQS setup help strings.
#
#

PROGRAMS		= nqsmkdirs nqsmktrans

DATAFILES		= nqs_dirs.mk sched_conf.help

SCRIPTS			= nqs_setup nqs_params sanity_check setup_nodes

ILIST			= ${PROGRAMS} ${DATAFILES} ${SCRIPTS}

IDIR			= /usr/lib/nqs/setup/
IOWNER			= root

nqsmkdirs_IMODE		=  754
nqsmktrans_IMODE	=  754
nqs_dirs.mk_IMODE	=  444
nqs_setup_IMODE		=  754
nqs_params_IMODE	=  754
sanity_check_IMODE	=  754
setup_nodes_IMODE	=  754
sched_conf.help_IMODE   =  754

INCFLAGS		= -I../../../include -I../../../include/nqs 
CFLAGS			= -DNQS_OWNER=\"$(NQS_OWNER)\" \
			  -DNQS_GROUP=\"$(NQS_GROUP)\" \
			  ${NQSFLAGS} \
			  ${${TARGET_MACHINE}CFLAGS}

LIBS		= -lnqs

nqsmkdirs_OFILES	= nqsmkdirs.o  
nqsmktrans_OFILES	= nqsmktrans.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/scripts.mk
include ../../../ccs/lib/libnqs/nqs.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
