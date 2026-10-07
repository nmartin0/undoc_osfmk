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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/Makefile,v 1.34 1994/11/19 03:04:01 mtm Exp $
#
#


PROGRAMS		= allocator
MSGHDRS                 = allocator_msg.h
CATFILES                = allocator.cat

ILIST			= allocator
IDIR			= /usr/sbin/
IOWNER                  = root
IMODE                   = 755

ALLOC_PATH              = ${OSFTOP}/src/usr/sbin/allocator
VPATH                   = ${TARGET_MACHINE}:${ALLOC_PATH}
MIGFLAGS                = ${INCFLAGS} -I${ALLOC_PATH} -DPARACORE -user /dev/null
MIG_DEFS                = alloc.defs
MIG_OFILES              = allocServer.o

CFLAGS                  = -g -DiPSC860_COMPAT -DTNC -DPARACORE # -DSIGNAL_BUG
LDFLAGS			= -g

HFILES                  = alloc.defs alloc_types.defs \
			macs.h debug.h macros.h conflict.h allocutils.h \
			tiles.h

INCFLAGS                = -I. 

LIBS                    = ../../ccs/lib/libnx/libnx.a  -lmach -lnetname -lc -lmach_sa -lm

allocator_OFILES        = allocator.o ${MIG_OFILES} conflict.o schedule.o \
                          init_appl.o misc_rpcs.o pspart_rpc.o mkpart_rpc.o    \
                          rmpart_rpc.o tiles.o server_loop.o smd.o \
			  allocutils.o macs.o attributes.o debug.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
