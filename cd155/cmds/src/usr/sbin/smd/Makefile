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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/Makefile,v 1.10 1994/12/19 19:46:23 sdh Exp $
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.10  1994/12/19  19:46:23  sdh
# Merged R1.2 fixes into R1.3.
#
#  Reviewer: none
#  Risk: Low
#  Benefit or PTS #: 11511
#  Testing:
# 	EATS: controlc, rmcall, rmcmd, nqs
# 	manual tests
#  Module(s):
# 	cmds_libs/src/usr/sbin/smd/allocator.c
#       	 cmds_libs/src/usr/sbin/smd/app_db.c
#       	 cmds_libs/src/usr/sbin/smd/app_service.c
#       	 cmds_libs/src/usr/sbin/smd/init.c
#       	 cmds_libs/src/usr/sbin/smd/main.c
#       	 cmds_libs/src/usr/sbin/smd/response.c
#       	 cmds_libs/src/usr/sbin/smd/service_db.c
#       	 cmds_libs/src/usr/sbin/smd/socket_iface.c
#       	 cmds_libs/src/usr/sbin/smd/user_db.c
# 	 cmds_libs/src/usr/sbin/smd/Makefile
#
# Revision 1.9  1994/11/19  03:16:39  mtm
# Copyright additions/changes
#
# Revision 1.8  1993/12/20  19:56:03  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.7  1993/11/18  20:27:21  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get nx and mcmsg headrs out of export tree
#  Testing: built on Suns and 486
#  Module(s): Makefile  allocator.c defs.h
#
# Revision 1.6  1993/06/29  15:50:17  shala
# Removed mach3 dependencies.
#
# Revision 1.5  1993/05/26  17:40:29  carbajal
# Moved allocUser.c from here to libnx
#
# Revision 1.4  1992/11/17  19:49:28  rkl
# Change the LIBS format.
#
# Revision 1.3  1992/11/11  02:23:18  shala
# Removed path to norma12.
#
# Revision 1.2  1992/10/29  17:23:35  rkl
# Added LINTFLAGS to enable lint_smd target.
#
# Revision 1.1  1992/10/05  23:30:28  rkl
# Initial revision
#
#
#

PROGRAMS	= smd
IDIR            = /usr/sbin/
ILIST		= smd

smd_OFILES	= main.o init.o	allocator.o socket_iface.o user_db.o \
	  	  service_db.o app_db.o	app_service.o part_service.o \
	  	  response.o debug.o debug2.o

LIBS		= -lnx \
		  -lmach -lnetname \
		  -lc -lmach_sa

HEADERS	= smd.h globals.h defs.h
PROG	= smd
DEFS	= -DINET_SOCKET -DDEBUG -DDUMPS #-DUNIX_SIMULATION
OPTS	=
CFLAGS	= $(DEFS) $(OPTS)
LINTFLAGS = -hbcxnz ${CFLAGS}


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
