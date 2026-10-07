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
#
# OSF/1 Release 1.0.1

IPSC860CFLAGS =

IPSC860COBJS =

IPSC860SOBJS = setjmp.o signal.o

INCFLAGS = -I../libc/AT386

setjmp.o_CCTYPE = traditional

${IPSC860SOBJS:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${IPSC860SOBJS}: $${@:.o=.S}
	cc ${_CCFLAGS_} -ES $*.S > `basename $@ .o`.i
	${ANSI_AS} -o $@ `basename $@ .o`.i
	${RM} -f `basename $@ .o`.i

#	${_CC_} ${_CCFLAGS_} -traditional -O -c $*.S
#	${LD} -x -r $*.o
#	${MV} -f a.out $*.o
