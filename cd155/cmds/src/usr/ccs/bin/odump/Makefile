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

VPATH		= ${TARGET_MACHINE}
INCFLAGS	= -I${TARGET_MACHINE}

PROGRAMS 	= odump
MSGHDRS		= odump_msg.h
CATFILES	= odump.cat

IDIR		= /usr/ccs/bin/
ILIST		= odump

HFILES		= ${MSGHDRS}
OFILES		= dump.o macho.o rose_names.o interp_tpfl.o
LIBS		= -lld

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

dump.o: dump_defs.h
macho.o: rose_namegrp.h
rose_names.o: rose_namegrp.h rose_names.h rose_namesmd.h tpfl_desc.h
interp_tpfl.o: tpfl_desc.h

include ${MAKEFILEPATH}/depend.mk
-include Makedep
