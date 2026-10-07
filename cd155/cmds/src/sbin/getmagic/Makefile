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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/getmagic/Makefile,v 1.5 1994/11/21 16:41:09 mtm Exp $
#
#

PROGRAMS		= getmagic
MSGHDRS			= 
CATFILES		= 

ILIST			= getmagic
IDIR			= /sbin/
IOWNER                  = root
IMODE                   = 4755

HFILES			= bootmagic.h

INCFLAGS                = 

CFLAGS                	=

LIBS                    = -lnx -lmach -lnetname 

OFILES                  = main.o magic.o kernops.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
