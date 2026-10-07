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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
# OSF/1 Release 1.0.1

IPSC860CFLAGS		=

# Select one of the following IPC mechanisms (must match mechanism
# used in building libc)

# IPC_CFILE		= kls_ipc_sysV_msg.c
# IPC_CFILE		= kls_ipc_sock_stream.c
# IPC_CFILE		= kls_ipc_mach.c

IPC_CFILE		= kls_ipc_sock_dgram.c

# Select the kernel loader managers for obtaining symbol information
# from the kernel

kloadsrv_KERNEL_MGRS	= kls_a.out.o \
 			  kls_macho.o \
			  kls_coff.o \
			  kls_coff_machdep.o

kls_coff.o_CFLAGS		= -D_COFF
kls_coff_machdep.o_CFLAGS	= -D_COFF

# Uncomment the following lines to statically link the kernel load
# server against the loader, and comment the dynamic link lines below.

kloadsrv.o_CFLAGS	= -DLDR_STATIC_LINK
kloadsrv_OFILES	= kloadsrv.o \
			  ${IPC_OFILES} \
			  ${KERNEL_LOADER_OFILES} \
			  ${LOADER_COMMON_OFILES} \
			  ${kloadsrv_KERNEL_MGRS} \
			  ../build/switch.o \
			  ../coff/ldr_abs.o \
			  ../macho/mo_ldr.o \
			  ../macho/relocate.o \
			  ../macho/standard.o

kloadsrv_LIBS		= -lld

# The following lines dynamically link the kernel load server against the loader

#kloadsrv_OFILES		= kloadsrv.o \
#			  ${IPC_OFILES} \
#			  ${KERNEL_LOADER_OFILES} \
#			  ${kloadsrv_KERNEL_MGRS}
#kloadsrv_LDFLAGS	= -%ld,-glue
#kloadsrv_LIBS		= -lld ../build/loader
#
