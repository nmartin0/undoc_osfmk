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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/chpart/Makefile,v 1.13 1994/11/19 01:20:24 mtm Exp $
#
#
# HISTORY:
# $Log: Makefile,v $
# Revision 1.13  1994/11/19  01:20:24  mtm
# Copyright additions/changes
#
# Revision 1.12  1993/12/20  18:46:44  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.11  1993/11/18  20:13:09  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get mcmsg and nx headers out of export tree
# 	get libs out of export tree
#  Testing: built on Suns and 486
#  Module(s): Makefile
#
# Revision 1.10  1993/11/17  18:07:16  carbajal
#  Reviewer: None
#  Risk: Low
#  Benefit or PTS #: Added some .h files to HFILES
#  Testing:
#  Module(s):
#
# Revision 1.9  1993/09/09  18:20:57  carbajal
# Changed imode from 4755 to 755
#
# Revision 1.8  1993/07/14  14:21:50  shala
# Removed mach3 dependencies.
#
# Revision 1.7  1993/05/26  17:38:54  carbajal
# Moved allocUser.c from here to libnx
#
# Revision 1.6  1992/10/22  00:54:05  carbajal
# No changes
#
# Revision 1.5  1992/10/13  22:29:12  carbajal
# Added support for renaming partitions with -nm
# Also moved parsepart.c and parsesched.c into libnx
#
# Revision 1.4  1992/10/08  19:05:44  carbajal
# changes made for new .partinfo structure
#
# Revision 1.3  1992/09/22  00:00:23  carbajal
# got rid of challocator.c
#
# Revision 1.2  1992/09/17  00:35:36  carbajal
# Added history log for RCS
# reorg'd -I statement to avoid conflict between mach 2.5 include
# files and mach 3.0 include files. (mach_init.h was the problem)
#

PROGRAMS		= chpart
MSGHDRS			= chpart_msg.h
CATFILES		= chpart.cat

ILIST			= chpart
IDIR			= /usr/bin/

IOWNER                  = root
IMODE                   = 755

CFLAGS                	=  #-DDEBUG

LIBS                    = -lnx -lmach -lnetname -lc -lc_r -lmach_sa

OFILES                  = chpart.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
