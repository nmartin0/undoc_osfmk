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

# Uncomment the following lines to statically link against standalone
# loader. Comment out the lines for dynamic linking that follow.
# For a.out omit the -T and -Tdata flags because we can't force the
# linker to put text and/or data a specific address.  Just link
# programs at some address other than the default.  You should be
# able to do this with COFF or OSF/ROSE, but not a.out.

#loader_LDFLAGS		= -T 90000000

# Following lines are needed to dynamically link against loader.

loader_LDFLAGS		= -%ld," -export ${LOADER_CONTEXT_EXPORTS}" \
			  -%ld," -export ${LOADER_UTILITIES_EXPORTS}" \
			  -T 90000000

IPSC860_LOADER_LIBS	= -lld

loader_OFILES		= main.o switch.o ${loader_COMMON_OFILES} \
			  ../coff/ldr_abs.o \
			  ../macho/mo_ldr.o \
			  ../macho/relocate.o \
			  ../macho/standard.o

switch.o_CFLAGS		= -DLDR_PRELOAD_MGR -DLDR_MACHO_MGR -DLDR_COFF_MGR


# Uncomment the following lines to statically link lib_admin against the
# standalone loader, and comment out the dynamic link lines below.

lib_admin.o_CFLAGS	= -DLDR_STATIC_LINK
lib_admin_OFILES	= ${lib_admin_COMMON_OFILES} \
			  ${loader_COMMON_OFILES} \
			  switch.o \
			  ../coff/ldr_abs.o \
			  ../macho/mo_ldr.o \
			  ../macho/relocate.o \
			  ../macho/standard.o
lib_admin_LIBS		= ../utils/libutils.a -lld -lAF

# The following lines dynamically link lib_admin against the loader.

#lib_admin_OFILES	= ${lib_admin_COMMON_OFILES}
#lib_admin_LDFLAGS	= -%ld,-glue
#lib_admin_LIBS		= -lld -lAF loader
#
