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
#
# derived from PMAX machdep.mk

S_OBJS		=

EMUL_S_OBJS	= mul.o

IPSC860SOBJS	= ${S_OBJS} ${EMUL_S_OBJS}

IPSC860COBJS	=

${S_OBJS:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${S_OBJS}: $${@:.o=.S}
	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES $*.S | as860 -o $@

${EMUL_S_OBJS}:
	for i in ${EMUL_S_OBJS}; do cp ${EMULOBJ}/$$i .; done
