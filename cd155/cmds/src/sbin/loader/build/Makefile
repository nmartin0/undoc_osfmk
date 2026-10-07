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
#
# OSF/1 Release 1.0

VPATH			= ${TARGET_MACHINE}

PROGRAMS		= loader lib_admin
MSGHDRS			= lib_admin_msg.h
CATFILES		= lib_admin.cat

ILIST			= loader lib_admin
IDIR			= /sbin/

EXPLOADER_TARGETS	= export_libloader.so
EXPDIR			= /usr/shlib/

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I../include -I../ldr_include

CFLAGS			= ${${TARGET_MACHINE}CFLAGS}

loader_COMMON_OFILES	= ../loader/ldr_bootstrap.o \
			  ../loader/ldr_context.o \
			  ../loader/ldr_global_file.o \
			  ../loader/ldr_known_pkg.o \
			  ../loader/ldr_main.o \
			  ../loader/ldr_preload.o \
			  ../loader/ldr_symres.o \
			  ../loader/region_alloc.o \
			  ../loader/ldr_dyn_mgr.o \
			  ../loader/ldr_errno.o \
			  ../loader/ldr_lock.o \
			  ../loader/ldr_brk.o

# WARNING: the following list of loader exports must be kept in sync with
# the ldr_bootstrap_exports.exp file.

