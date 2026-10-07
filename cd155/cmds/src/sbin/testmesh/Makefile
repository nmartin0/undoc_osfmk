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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/testmesh/Makefile,v 1.4 1994/11/21 16:48:14 mtm Exp $
#
#

VPATH                   = ../bootmesh
PROGRAMS		= testmesh
MSGHDRS			= 
CATFILES		= 

ILIST			= testmesh
IDIR			= /sbin/

HFILES			= bootmesh.h nic.h coff.h

INCFLAGS                = -I../bootmesh

CFLAGS 			= -g -DDEBUG -DTEST -DBUMPER -DSLOWDOWN -DMSGTEST

LIBS                    = -lnx -lmach -lnetname 

OFILES 		        = main.o boot.o kernops.o coff.o magic.o nic.o \
			  boot_send.o mcmsg.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
