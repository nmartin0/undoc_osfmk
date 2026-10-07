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
# flags passed to sub-invokations of make
#
_MAKEFLAGS_=${RELEASE_OPTIONS?'RELEASE_OPTIONS=${RELEASE_OPTIONS}':} ${TOSTAGE?'TOSTAGE=${TOSTAGE}':} ${FROMSTAGE?'FROMSTAGE=${FROMSTAGE}':}

#
#  Change into a directory and make recursively
#
#
#  __DO_<action>__PASS_<pass>__SUBDIR_<subdir>__
#
#	echo where we are
#	create path to subdir is needed
#	cd into subdir
#	exec make with pass and action
#
__DO_%__PASS_%2__SUBDIR_%3__: ${ALWAYS}
	@echo "[ ${MAKEDIR:/=}/%3 ]"
	makepath %3/. && cd %3 && \
	exec ${MAKE} ${_MAKEFLAGS_} ${%3_SUBMAKEFLAGS?${%3_SUBMAKEFLAGS}:${SUBMAKEFLAGS}} MAKEFILE_PASS=%2 ${_%_ACTION_}_all
