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
# Revision 1.10  1994/11/19  03:05:17  mtm
# Copyright additions/changes
#
# Revision 1.9  1994/10/18  16:11:03  davidl
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
# Revision 1.8  1994/06/13  23:48:46  jkearns
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.7  1994/02/02  23:17:21  mwan
# Fixed PTS #7837
#
#  Reviewer: jkearns
#  Risk: M
#  Benefit or PTS #: 7837
#  Testing:
#  Module(s):  All the MACS modules are replaced in this one time checkin.
#
# Revision 1.6  1994/01/13  17:36:41  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.5  1993/12/20  19:51:28  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.4  1993/11/18  20:24:53  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get macs headers and libs out of the export tree
#  Testing: built on Suns and 486
#  Module(s): Makefile dbcreate.c
#
# Revision 1.3  1993/10/27  23:03:02  shala
# Fixed typo.
#
# Revision 1.2  1993/10/27  17:40:57  shala
# Added ILIST.
#
# Revision 1.1  1993/10/20  00:16:40  jkearns
# Initial revision
#
# Revision 1.0  1993/10/19  10:40:00 jk
# Initial revision
#
#

PROGRAMS		= dbcreate

ILIST			= dbcreate

dbcreate_IDIR		= /usr/sbin/
dbcreate_IOWNER		= root
dbcreate_IMODE		= 750

CFLAGS			= $(DBG) -DIPSC ${${TARGET_MACHINE}CFLAGS} 
INCFLAGS                = -I../../include -I../../include/macs
LIBS			= -lmac -lrin

dbcreate_OFILES		= dbcreate.o


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
