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
# Revision 1.10  1994/11/19  03:09:05  mtm
# Copyright additions/changes
#
# Revision 1.9  1994/10/18  16:11:12  davidl
# Changed file permissions to more reasonable values
#
#  Reviewer: doyle
#  Risk: Medium
#  Benefit or PTS #: 9308
#  Testing: Developer testing, EATs
#  Module(s): cmds_libs/src/usr/bin/acctrep/Makefile
#             cmds_libs/src/usr/bin/arep/Makefile
#             cmds_libs/src/usr/bin/jrec/Makefile
#             cmds_libs/src/usr/bin/macalloc/Makefile
#             cmds_libs/src/usr/bin/maclist/Makefile
#             cmds_libs/src/usr/sbin/dbcreate/Makefile
#             cmds_libs/src/usr/sbin/macadmin/Makefile
#             cmds_libs/src/usr/sbin/macupdate/Makefile
#             cmds_libs/src/usr/sbin/si/Makefile
#             cmds_libs/src/usr/lib/macs/Makefile
#             cmds_libs/src/usr/lib/macs/setup/Makefile
#             cmds_libs/src/usr/lib/macs/setup/macs_params.sh
#             cmds_libs/src/usr/lib/macs/setup/macs_dirs.mk
#
# Revision 1.8  1994/04/20  18:56:53  jkearns
# Changed permissions from 6111 to 744.
#
#  Reviewer:Jerry Kearns
#  Risk:Low
#  Benefit or PTS #: 8863
#  Testing:
#  Module(s): Makefile
#
# Revision 1.7  1994/01/13  17:36:56  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.6  1993/12/20  19:53:27  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.5  1993/10/27  20:48:22  shala
# Get  mac_ui.c from "usr/bin/macalloc". Makefile is updated to have the
# correct flag to build "macadmin" and find the source from "usr/bin/macalloc".
#
# Revision 1.4  1993/10/27  18:23:55  shala
# Used correct name (changed macalloc to macadmin).
#
# Revision 1.3  1993/10/27  17:33:24  shala
# Added ILIST.
#
# Revision 1.2  1993/10/20  21:43:30  jkearns
# Add DEFS to CFLAGS.
#
# Revision 1.1  1993/10/20  21:32:53  jkearns
# Initial revision
#
# Revision 1.0  1993/10/19  10:40:00 jk
# Initial revision
#
#

VPATH                   = ${MAKETOP}/usr/bin/macalloc

PROGRAMS		= macadmin

ILIST			= macadmin

macadmin_IDIR		= /usr/sbin/
macadmin_IOWNER		= root
macadmin_IMODE		= 750

IPATH			= ../../include
INCFLAGS                = -I$(IPATH) -I$(IPATH)/macs
DEFS			= -DMACADMIN
CFLAGS			= $(DEFS) $(DBG) ${${TARGET_MACHINE}CFLAGS} 
LIBS			= -lrin -lmac -lnx -lmach

macadmin_OFILES		= mac_ui.o 


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
