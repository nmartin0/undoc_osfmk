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
# @OSF_COPYRIGHT@
#
# $Id: machdep.mk,v 1.3 1994/11/19 01:34:47 mtm Exp $
#
# HISTORY
# $Log: machdep.mk,v $
# Revision 1.3  1994/11/19  01:34:47  mtm
# Copyright additions/changes
#
# Revision 1.2  1994/01/11  18:58:14  dleslie
#  Reviewer: Shala Arshi
#  Risk: low
#  Benefit or PTS #: use as860 instead of as860.exe
#  Testing: built all of cmds/libs
#  Module(s): machdep.mk
#
# Revision 1.1  1992/05/06  18:48:42  stans
# OSF-1.0.4
#
# $EndLog$

SFILES				= server.o
${TARGET_MACHINE}_OFILES	= ${SFILES}

server.o_CCTYPE			= traditional

.SUFFIXES: .S .s

${IPSC860_OFILES:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${SFILES}: $${@:.o=.S}
	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES ${@:.o=.S} | as860 -o $@
	rm -f ${@:.o=.S}

