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
# Revision 1.8  1994/11/19  01:18:38  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/10/18  16:10:17  davidl
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
# Revision 1.6  1994/06/13  23:38:26  jkearns
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.5  1994/02/02  23:14:00  mwan
# Fixed PTS #7837
#
#  Reviewer: jkearns
#  Risk: M
#  Benefit or PTS #: 7837
#  Testing:
#  Module(s):  All the MACS modules are replaced in this one time checkin.
#
# Revision 1.4  1994/01/13  17:35:05  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.3  1993/12/20  18:45:30  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.2  1993/10/27  17:45:15  shala
# Added ILIST.
#
# Revision 1.1  1993/10/20  17:20:47  jkearns
# Initial revision
#
# Revision 1.0  1993/10/19  10:40:00 jk
# Initial revision
#
#

PROGRAMS		= acctrep

ILIST			= acctrep

acctrep_IDIR		= /usr/bin/
acctrep_IOWNER		= root
acctrep_IMODE		= 4755

IPATH			= ../../include
INCFLAGS                = -I$(IPATH) -I$(IPATH)/macs
CFLAGS			= $(DBG) -DIPSC ${${TARGET_MACHINE}CFLAGS} 
LIBS			= -lmac -lrin

acctrep_OFILES		= acctrep.o


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
