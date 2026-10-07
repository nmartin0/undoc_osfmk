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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/watchdog/Makefile,v 1.3 1994/11/21 16:48:26 mtm Exp $
#
#

PROGRAMS		= watchdog
DATAFILES               = 

MSGHDRS			= 
CATFILES		= 

ILIST			= ${DATAFILES} watchdog
IDIR			= /sbin/

HFILES			= 

INCFLAGS                = 

CFLAGS 			= -g -DDEBUG -DTEST -DBUMPER -DSLOWDOWN

LIBS                    = 

watchdog_OFILES         = watchdog.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
