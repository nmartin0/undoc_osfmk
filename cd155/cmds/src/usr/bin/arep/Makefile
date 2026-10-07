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
# Revision 1.5  1994/11/19  01:18:44  mtm
# Copyright additions/changes
#
# Revision 1.4  1994/10/18  16:10:25  davidl
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
# Revision 1.3  1994/05/26  16:16:48  dleslie
# Put R1.2 changes in main trunk.
#
# Revision 1.1.2.2  1994/05/10  01:13:01  jkearns
# Changed MACS commands and directories' permissions and ownerships.
#
#  Reviewer: doyle
#  Risk: Low
#  Benefit or PTS #: 9308
#  Module(s): All MACS commands' Makefiles, macs_params.sh and macs_dirs.mk
#
# Revision 1.1.2.1  1993/12/17  09:54:27  jkearns
#  Reviewer: Jerry Kearns
#  Risk: Low
#  Benefit or PTS #: Fix MACS Makefile(s) to have ILIST line included.
#  Testing:
#  Module(s):
#
# Revision 1.1  1993/12/12  00:07:02  jkearns
# Initial revision
#
# Revision 1.0  1993/10/19  10:40:00 jk
# Initial revision
#
#

PROGRAMS		= arep

ILIST			= arep

IDIR		= /usr/bin/
IOWNER		= root
IMODE		= 4755

IPATH			= ../../include
INCFLAGS                = -I$(IPATH) -I$(IPATH)/macs
CFLAGS			= $(DBG) -DIPSC ${${TARGET_MACHINE}CFLAGS} 
MACS_LIBS		= ../../ccs/lib/libmacs/libmac.a \
			  ../../ccs/lib/libmacs/librin.a

arep_OFILES		= arep.o $(MACS_LIBS) 


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep

