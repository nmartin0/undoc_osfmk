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

REUSE_CODE_PATH		= ${OSFTOP}/src/usr/ccs/lib/libnx/${TARGET_MACHINE}

VPATH			= ${TARGET_MACHINE}:${REUSE_CODE_PATH}

SFILES			= async.o lock.o rfork.o rforkmulti.o

${TARGET_MACHINE}_OFILES = ${SFILES} machdep.o

async.o_CCTYPE		= traditional
lock.o_CCTYPE		= traditional

rfork.o_CFLAGS		= -D_PTHREADS -I${REUSE_CODE_PATH}
rforkmulti.o_CFLAGS	= -D_PTHREADS -I${REUSE_CODE_PATH}

.SUFFIXES: .S .s

.s.S:
	rm -f $@
	cp $< $@

#
# NO support for '.S' files in our cc heaad.
#
${SFILES}: $${@:.o=.S}
	cc ${_CCFLAGS_} -ES ${@:.o=.S} > `basename $@ .o`.i
	${ANSI_AS} -o $@ `basename $@ .o`.i
	${RM} -f `basename $@ .o`.i

