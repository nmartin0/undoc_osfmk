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
# Revision 1.14  1994/11/19  02:52:28  mtm
# Copyright additions/changes
#
# Revision 1.13  1994/10/18  16:08:50  davidl
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
# Revision 1.12  1994/01/13  17:30:12  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.11  1993/12/20  19:46:59  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.10  1993/06/29  15:36:08  shala
# Removed mach3 dependencies.
#
# Revision 1.9  1992/11/20  16:35:10  rkl
# Removed qmgr.hlp file and moved it to usr/bin/qmgr so that the nqsmgrhelp.h
# file can be generated when qmgr is built.
#
# Revision 1.8  1992/11/13  18:57:13  shala
# Added -lmach3.
#
# Revision 1.7  1992/11/11  01:31:52  shala
# Fixed install path.
#
# Revision 1.6  1992/10/21  20:32:26  rkl
# Added $(NQS_LIBS) to nqsdaemon_OFILES list.
#
# Revision 1.5  1992/10/17  00:13:10  rkl
# Make libnqs.a a dependency of the build programs.
#
# Revision 1.4  1992/10/16  19:01:05  rkl
# Added setup subdirectory.
#
# Revision 1.3  1992/10/12  04:09:13  rkl
# Added individual IMODE enties for each installable program.
#
# Revision 1.2  1992/10/09  22:24:15  mwan
# T6 freeze
#
# Revision 1.1  1992/09/24  18:58:13  rkl
# Initial revision
#
#

SUBDIRS			= setup

PROGRAMS		= lpserver \
			  netclient \
			  pipeclient \
			  shlexefai \
			  nqsdaemon \
			  logdaemon \
			  netdaemon \
			  netserver

ILIST			= lpserver \
			  netclient \
			  pipeclient \
			  shlexefai \
			  nqsdaemon \
			  logdaemon \
			  netdaemon \
			  netserver

IDIR			= /usr/lib/nqs/
IOWNER			= root

lpserver_IMODE		=  774
netclient_IMODE		=  774
pipeclient_IMODE	=  774
shlexefai_IMODE		= 6754
nqsdaemon_IMODE		=  774
logdaemon_IMODE		=  774
netdaemon_IMODE		=  774
netserver_IMODE		=  774

INCFLAGS		= -I../../include -I../../include/nqs 
CFLAGS			= ${NQSFLAGS} ${${TARGET_MACHINE}CFLAGS}
LIBS			= -lnqs -lnx -lmach

nqsdaemon_OFILES	= nqs_a2s.o nqs_aboque.o nqs_abort.o nqs_boot.o \
			  nqs_bsc.o nqs_complt.o nqs_delreq.o nqs_delrfs.o \
			  nqs_deque.o nqs_disreq.o nqs_dsc.o nqs_enfile.o \
			  nqs_enf.o nqs_failed.o nqs_family.o nqs_fndnnq.o \
			  nqs_fndreq.o nqs_ldconf.o nqs_mai.o nqs_main.o \
			  nqs_modreq.o nqs_mov.o nqs_nsq.o nqs_pip.o nqs_psc.o \
			  nqs_quereq.o nqs_rbuild.o nqs_reqcom.o nqs_reqexi.o \
			  nqs_reqser.o nqs_spawn.o nqs_tid.o nqs_udb.o \
			  nqs_upc.o nqs_upd.o nqs_upf.o nqs_upm.o nqs_upp.o \
			  nqs_upq.o nqs_ups.o nqs_upv.o nqs_vtimer.o \
			  nqs_wakdes.o nqs_wakreq.o \
			  macs_sched.o macs_rootp.o macs_blk.o macs_job.o \
			  macs_lib.o smd_msg.o res_msg.o 

lpserver_OFILES		= lpserver.o 
netclient_OFILES	= netclient.o 
pipeclient_OFILES	= pipeclient.o 
shlexefai_OFILES	= shlexefai.o 
logdaemon_OFILES	= logdaemon.o 
netdaemon_OFILES	= netdaemon.o 
netserver_OFILES	= netserver.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/subdirs.mk
include ../../ccs/lib/libnqs/nqs.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
