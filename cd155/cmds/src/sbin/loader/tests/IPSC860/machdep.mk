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
# 

IPSC860CFLAGS		=

# Uncomment the following lines to build tload2 statically linked against
# the loader, and comment out the lines for dynamic linking below.

tload2.o_CFLAGS	= -DLDR_STATIC_LINK
tload2_OFILES		= ${tload2_COMMON_OFILES} \
			  ${LOADER_COMMON_OFILES} \
			  ../build/switch.o \
			  ../coff/ldr_abs.o \
			  ../macho/mo_ldr.o \
			  ../macho/relocate.o \
			  ../macho/standard.o
tload2_LIBS		= -lld

# The following lines build tload2 linked dynamically against the loader

#tload2_OFILES		= ${tload2_COMMON_OFILES}
#tload2_LDFLAGS		= -T 74000000 -Tdata 75000000 -%ld,"-export tload2_package:printf,sscanf,perror,gets"
#tload2_LIBS		= -lld ../build/loader

texport_OFILES		= ${texport_COMMON_OFILES} \
			  ${LOADER_COMMON_OFILES} \
			  ../coff/ldr_abs.o \
			  ../macho/mo_ldr.o \
			  ../macho/relocate.o \
			  ../macho/standard.o

TLOAD2PROGS		= ltest1 ltest2

# Support for test programs

LD			= ${MACHO_GCC_EXEC_PREFIX}ld

ltest1_LDFLAGS		= -R

ltest2_LDFLAGS		= -R

ltest3_LDFLAGS		= -R

kmod1.o_CFLAGS		= -pic-names ${VMUNIX_PICNAMES}
kmod1_LDFLAGS		= -R -e _kmod1
kmod1_LIBS		= ${LIBVMUNIX_SO}

${TLOAD2PROGS}:
	${LD} ${_LDFLAGS_} -o $@ $@.o tload2

${KTLOAD2PROGS}:
	${LD} ${_LDFLAGS_} -o $@.X $@.o ${_LIBS_}
	${MV} $@.X $@
