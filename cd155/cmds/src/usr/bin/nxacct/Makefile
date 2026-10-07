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
# Revision 1.5  1994/11/19  01:32:33  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/12/20  19:00:25  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.3  1993/10/27  18:09:33  shala
# Added list of objects.
#
# Revision 1.2  1993/10/27  16:10:42  jkearns
# Added "-lmach" to LIBS.
#
# Revision 1.1  1992/10/01  18:36:54  rkl
# Initial revision
#
#
#

PROGRAMS		= nxacct
CATFILES		= nxacct.cat

ILIST			= nxacct
IDIR			= /usr/bin/
IOWNER			= ${ROOT_OWNER}
IMODE			= ${SETUID_MODE}

CFLAGS                  = 
LIBS               	= -lnx -lmach

OFILES             	= nxacct.o nxacct_sec.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
