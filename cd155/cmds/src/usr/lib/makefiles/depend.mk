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

nodep_all: ${ALWAYS}
	${RM} -f Makedep

nodep_%: _nodep_prefix_%;@

_nodep_prefix_%:
	@echo "Must use nodep_<file>.o as target"

_nodep_prefix_%.o:
	${RM} -f %.o %.d
	echo "%.o: %.o" > %.d

.EXIT:
	@-${MD} -u Makedep ${_MDFLAGS_} *.d

