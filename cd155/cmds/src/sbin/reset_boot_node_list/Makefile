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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/reset_boot_node_list/Makefile,v 1.6 1994/11/21 16:47:34 mtm Exp $
#
#

VPATH			= ../getmagic:../initpart

PROGRAMS		= reset_bnl disablemesh
DATAFILES               = reset_boot_node_list.sh

MSGHDRS			= 
CATFILES		= 

ILIST			= ${DATAFILES} reset_bnl disablemesh
IDIR			= /sbin/

HFILES			= 

INCFLAGS                = -I../getmagic

#CFLAGS 			= -g -DDEBUG 
CFLAGS 			= -O

LIBS                    = -lnx -lmach -lnetname 

reset_bnl_OFILES 		        = reset_bnl.o kernops.o magic.o sort.o node.o parsenode.o

disablemesh_OFILES 		        = disablemesh.o kernops.o magic.o sort.o node.o parsenode.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
