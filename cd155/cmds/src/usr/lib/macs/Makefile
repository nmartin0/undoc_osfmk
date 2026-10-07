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
# Revision 1.9  1995/01/20  18:49:56  davidl
# Added check on return value of printf() for message written to logfile
# every SYNC_INTERVAL (default 15 minutes).  If printf() fails: sends
# mail to root, writes a message to the console, and logs error to
# syslog.
#
# This is a compromise solution that detects the problem at most 15
# minutes after it first occurs.  Detecting the problem immediately would
# require examining every printf() in MACS (there are approximately 2000
# of these) and adding error-checking code where appropriate.
#
#  Reviewer: doyle
#  Risk: Low
#  Benefit or PTS #: 8753
#  Testing: Developer testing, EATs
#  Module(s): cmds_libs/src/usr/include/macs/macd.h
#             cmds_libs/src/usr/lib/macs/Makefile
#             cmds_libs/src/usr/lib/macs/log.c
#             cmds_libs/src/usr/lib/macs/ports.c
#
# Revision 1.8  1994/11/19  02:50:22  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/10/18  16:11:38  davidl
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
# Revision 1.6  1994/02/02  23:16:51  mwan
# Fixed PTS #7837
#
#  Reviewer: jkearns
#  Risk: M
#  Benefit or PTS #: 7837
#  Testing:
#  Module(s):  All the MACS modules are replaced in this one time checkin.
#
# Revision 1.5  1994/01/13  17:31:01  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.4  1993/12/20  19:46:26  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.3  1993/11/19  22:32:37  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: get macs and nx headers out of exports tree, not obj tree
#  Testing: built successfully
#  Module(s):
#     Makefile app.c app_misc.c db.c job.c job_misc.c log.c macd.c
#     macpd.c ports.c smd_req.c upd.c
#
# Revision 1.2  1993/11/18  20:22:55  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get nx headers out of the obj tree
#  Testing: built on Suns and 486
#  Module(s): Makefile
#
# Revision 1.1  1993/10/22  21:45:00  jkearns
# Initial revision
#
# Revision 1.0  1993/10/19  17:30:00  glk
# Initial revision
#
#

SUBDIRS			= setup

PROGRAMS		= macd macpd

ILIST			= macd macpd

IDIR			= /usr/lib/macs/
IOWNER			= root

macd_IMODE		=  754
macpd_IMODE		=  754

MACSCFLAGS		= $(DBG) -DSMD_IP -DIPSC 
INCFLAGS                = -I../../include -I../../include/macs -I../../include/nx
CFLAGS			= ${MACSCFLAGS} ${${TARGET_MACHINE}CFLAGS}
LIBS			= -lmac -lrin  -lnx -lmach


macd_OFILES		= macd.o ports.o app.o app_misc.o job.o smd_req.o \
                	  job_misc.o upd.o log.o ttywrite.o

macpd_OFILES		= macpd.o ttywrite.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
