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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
# OSF/1 Release 1.0

VPATH			= ${TARGET_MACHINE}

SUBDIRS			= include
PROGRAMS		= kloadsrv

ILIST			= kloadsrv
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

LOADER_COMMON_OFILES	= ../loader/ldr_bootstrap.o \
			  ../loader/ldr_global_file.o \
			  ../loader/ldr_known_pkg.o \
			  ../loader/ldr_main.o \
			  ../loader/ldr_context.o \
			  ../loader/ldr_preload.o \
			  ../loader/ldr_symres.o \
			  ../loader/region_alloc.o \
			  ../loader/ldr_dyn_mgr.o \
			  ../loader/ldr_errno.o \
			  ../loader/ldr_lock.o \
			  ../loader/ldr_brk.o \
			  ../utils/libutils.a

KERNEL_LOADER_OFILES	= kernel_region_alloc.o \
			  kls_vm.o \
			  ldr_kernel_bootstrap.o \
			  ldr_kernel_main.o \
			  ldr_kernel_syscalls.o \
			  kls_subr.o

IPC_OFILES		= kls_message.o kls_ipc.o

INCFLAGS		= -I../include -I../ldr_include -Iinclude

CFLAGS			= ${${TARGET_MACHINE}CFLAGS}

include  ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include  ${MAKEFILEPATH}/programs.mk
include  ${MAKEFILEPATH}/objects.mk
include ${TARGET_MACHINE}/machdep.mk

kls_ipc.c: ${IPC_CFILE}
	${RM} -f kls_ipc.c
	${CP} ${IPC_CFILE} kls_ipc.c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
