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
# default nroff program to run
#
NROFF=nroff

#
# default flags to nroff
#
DEF_NROFFFLAGS=${DEF_NROFFFLAGS?${DEF_NROFFFLAGS}:-man -h}

#
# all flags for nroff
#
_NROFFFLAGS_=${${@}_DEF_NROFFFLAGS?${${@}_DEF_NROFFFLAGS}:${DEF_NROFFFLAGS}} ${${@}_NROFFENV?${${@}_NROFFENV}:${NROFFENV}} ${${@}_NROFFFLAGS?${${@}_NROFFFLAGS}:${NROFFFLAGS}} ${${@}_NROFFARGS?${${@}_NROFFARGS}:${NROFFARGS}}

#
#  Default single suffix compilation rules
#
${MANSECTION?.SUFFIXES\: .0 .${MANSECTION}:}

#
#  Default double suffix compilation rules
#
${MANSECTION?.${MANSECTION}.0:}:
	${NROFF} ${_NROFFFLAGS_} $*.${MANSECTION} >$*.0.X
	${MV} -f $*.0.X $*.0
