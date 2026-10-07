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
# @OSF_FREE_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.3  1994/11/19  02:38:27  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/12/20  19:42:43  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.1  1993/06/23  01:08:38  stans
# Initial revision
#
# Revision 1.1  1993/05/04  00:26:30  stans
# Initial revision
#
# Revision 1.3  1992/06/15  15:27:17  devrcs
# 	Changed targets for NORMA_MK9 ...
# 	[1992/06/11  08:21:42  david]
#
# Revision 1.2  1992/05/12  14:47:55  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  07:45:55  condict]
# 
# $EndLog$

VPATH			= ${MAKETOP}usr/include/servers

LIBRARIES		= libxmm.a

EXPLIB_TARGETS		= export_libxmm.a

ILIST			= libxmm.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

CFLAGS			= -MD

INCFLAGS                = -I.

MIGFLAGS		= ${DEFINES} ${INCFLAGS} -nostdinc

MIG_DEFS		= proxy.defs xmm_net.defs memory_object.defs

OFILES			= proxyServer.o \
			  xmm_netServer.o xmm_netUser.o \
			  memory_objectServer.o memory_objectUser.o \
			  proxy.o atrium.o hash.o kalloc.o \
			  mig_loop.o net_bsdtcp.o net_bsdudp.o \
			  vm_dirty.o xmm.o xmm_buffer.o xmm_copy.o \
			  xmm_debug.o xmm_flush.o xmm_interpose.o \
			  xmm_invalid.o xmm_luser.o xmm_maycache.o \
			  xmm_multiplex.o xmm_net.o xmm_pagingfile.o \
			  xmm_split.o xmm_svm.o xmm_vm.o xmm_write.o \
			  xmm_zero.o zalloc.o

GARBAGE			= proxy.h		proxyServer.c \
						proxyUser.c \
			  xmm_net.h		xmm_netServer.c \
						xmm_netUser.c \
			  memory_object.h	memory_objectServer.c \
						memory_objectUser.c


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
