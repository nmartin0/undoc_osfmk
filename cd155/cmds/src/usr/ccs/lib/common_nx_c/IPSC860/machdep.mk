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
#
# HISTORY:
# $Log: machdep.mk,v $
# Revision 1.6  1994/11/19  01:56:44  mtm
# Copyright additions/changes
#
# Revision 1.5  1994/01/11  18:57:43  dleslie
#  Reviewer: Shala Arshi
#  Risk: low
#  Benefit or PTS #: use as860 instead of as860.exe
#  Testing: built all of cmds/libs
#  Module(s): machdep.mk
#
# Revision 1.4  1993/10/05  21:41:47  stans
# added HISTORY
#
#

INCFLAGS = -I..

IPSC860SOBJS =	execv.o __eseek.o _estat.o _festat.o _lestat.o \
		statpfs.o fstatpfs.o fork.o execvenx.o

${IPSC860SOBJS:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${IPSC860SOBJS}: $${@:.o=.S}
	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES $*.S | as860 -o $@
