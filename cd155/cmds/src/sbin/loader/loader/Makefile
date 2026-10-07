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

VPATH			= ../utils

PROGRAMS		= genhash

EXPLOADER_TARGETS	= ${OBJECTS}

USE_STATIC_LIBRARIES	=

OBJECTS			= ldr_bootstrap.o ldr_context.o ldr_global_file.o \
			  ldr_known_pkg.o ldr_main.o ldr_preload.o \
			  ldr_symres.o region_alloc.o \
			  ldr_dyn_mgr.o ldr_errno.o ldr_lock.o ldr_brk.o

genhash_OFILES		= genhash.o open_hash.o hash_string.o oh_nelem.o

INCFLAGS		= -I../include -I../ldr_include

CFLAGS			=  ${${TARGET_MACHINE}CFLAGS}

genhash.o_CCTYPE	= host
genhash.o_CFLAGS	= -D_NO_PROTO
genhash.o_INCFLAGS	= ${INCFLAGS} -I${MAKETOP}usr/include/loader
open_hash.o_CCTYPE	= host
open_hash.o_CFLAGS	= -D_NO_PROTO
open_hash.o_INCFLAGS	= ${INCFLAGS} -I${MAKETOP}usr/include/loader
hash_string.o_CCTYPE	= host
hash_string.o_CFLAGS	= -D_NO_PROTO
hash_string.o_INCFLAGS	= ${INCFLAGS} -I${MAKETOP}usr/include/loader
oh_nelem.o_CCTYPE	= host
oh_nelem.o_CFLAGS	= -D_NO_PROTO
oh_nelem.o_INCFLAGS	= ${INCFLAGS} -I${MAKETOP}usr/include/loader
genhash_CCTYPE		= host
genhash_LIBFLAGS	=
genhash_LIBS		=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
-include ${TARGET_MACHINE}/machdep.mk

ldr_bootstrap.o:	ldr_bootstrap_exports.tab

ldr_bootstrap_exports.tab:	ldr_bootstrap_exports.exp genhash
	./genhash -s -o $@ ldr_bootstrap_exports.exp
include ${MAKEFILEPATH}/depend.mk
-include Makedep
