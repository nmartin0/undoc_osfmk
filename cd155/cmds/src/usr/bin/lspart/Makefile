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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/lspart/Makefile,v 1.18 1994/11/19 01:29:57 mtm Exp $
#
#

PROGRAMS		= lspart
MSGHDRS			= lspart_msg.h
CATFILES		= lspart.cat

ILIST			= lspart
IDIR			= /usr/bin/

IOWNER                  = root
IMODE                   = 755

HFILES			= 

CFLAGS			= #-DDEBUG

OFILES                  = lspart.o 

LIBS                    = -lnx  -lmach -lnetname -lc -lc_r -lmach_sa -lm

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
