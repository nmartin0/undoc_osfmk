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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/pspart/Makefile,v 1.8 1994/12/19 16:27:05 johannes Exp $
#
#
# HISTORY:
# $Log: Makefile,v $
# Revision 1.8  1994/12/19  16:27:05  johannes
# Define the PARACORE option.
#
#  Reviewer: Scott Hahn
#  Risk: High (several components involved)
#  Benefit or PTS #: 11577
#  Testing: developer testing, special testing by Simon Tsang,
#           corefile EAT, sched EAT, rmcmd EAT, controlc EAT
#  Module(s): svr/server/paracore/dvp_pvpcore.c
#             svr/server/nx/nx.c, nx.defs
#             usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
#                                allocator.c, pspart_rpc.c
#             usr/include/nx/schedule.h
#             usr/include/allocsys.h
#             usr/bin/pspart/pspart.c, Makefile
#
# Revision 1.7  1994/11/19  01:34:52  mtm
# Copyright additions/changes
#
# Revision 1.6  1993/12/20  19:01:10  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.5  1993/11/18  20:15:20  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get mcmsg and nx headers and libs out of export tree
#  Testing: built on Suns and 486
#  Module(s): Makefile pspart.c
#
# Revision 1.4  1993/07/14  14:24:22  shala
# Removed mach3 dependencies.
#
# Revision 1.3  1993/05/26  17:39:36  carbajal
# Moved allocUser.c from here to libnx
#
# Revision 1.2  1992/10/20  22:46:25  rkl
# Turned DEBUG flag off.
#
# Revision 1.1  1992/10/13  17:58:33  carbajal
# Initial check in
#
#

PROGRAMS		= pspart
MSGHDRS			= pspart_msg.h
CATFILES		= pspart.cat

ILIST			= pspart
IDIR			= /usr/bin/

IOWNER                  = root
IMODE                   = 4755

HFILES			= 


CFLAGS                	= -DPARACORE #-DDEBUG

LIBS                    = -lnx -lmach -lnetname -lc -lc_r -lmach_sa

OFILES                  = pspart.o  

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
