# $Copyright
# Copyright 1992, 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
  


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
# Revision 1.10.4.1  1995/06/11  18:43:19  kat
# Updated copyright for R1.3 PSCP
#
# Revision 1.10  1994/10/18  16:11:28  davidl
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
# Revision 1.9  1994/06/13  23:51:17  jkearns
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.8  1994/05/11  00:55:01  jkearns
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.7  1994/02/02  23:17:35  mwan
# Fixed PTS #7837
#
#  Reviewer: jkearns
#  Risk: M
#  Benefit or PTS #: 7837
#  Testing:
#  Module(s):  All the MACS modules are replaced in this one time checkin.
#
# Revision 1.6  1994/01/20  19:28:37  jkearns
#  Reviewer: Jerry Kearns
#  Risk:Low
#  Benefit or PTS #: Initial MACS Release
#  Testing:
#  Module(s):
#
# Initial MACS Release (1.1)
#
# Revision 1.0  1993/10/19  10:40:00 jk
# Initial revision
#
#

PROGRAMS		= si

si_IDIR		= /usr/sbin/
si_IOWNER		= root
si_IMODE		= 750

ILIST			= si

IPATH			= ../../include
INCFLAGS                = -I$(IPATH) -I$(IPATH)/macs
CFLAGS			= $(DBG) -DIPSC ${${TARGET_MACHINE}CFLAGS} 
MACS_LIBS		= ../../ccs/lib/libmacs/libmac.a \
			  ../../ccs/lib/libmacs/librin.a

si_OFILES		= si.o $(MACS_LIBS) 


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

