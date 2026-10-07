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
NROFF=	nroff
COL=	col
EPS=	eps
TBL=	tbl
TROFF=	troff

#
# definitions for build
_DOCUMENTS_=${DOCUMENTS?${DOCUMENTS}:_DOCUMENTS_}
_DOCFILES_=${DOCUMENTS/*/${&_DOCFILES?${&_DOCFILES}:&.doc}

#
# default flags to roff
#
DEF_NROFFFLAGS=${DEF_NROFFFLAGS?${DEF_NROFFFLAGS}:${MANSECTION?-man -h:-mm}}
DEF_TROFFFLAGS=${DEF_TROFFFLAGS?${DEF_TROFFFLAGS}:-mm -Tps}
DEF_COLFLAGS=${DEF_COLFLAGS?${DEF_COLFLAGS}:-b}

#
# all flags for roff
#
_NROFFFLAGS_=${${@}_DEF_NROFFFLAGS?${${@}_DEF_NROFFFLAGS}:${DEF_NROFFFLAGS}} ${${@}_NROFFENV?${${@}_NROFFENV}:${NROFFENV}} ${${@}_NROFFFLAGS?${${@}_NROFFFLAGS}:${NROFFFLAGS}} ${${@}_NROFFARGS?${${@}_NROFFARGS}:${NROFFARGS}}
_TROFFFLAGS_=${${@}_DEF_TROFFFLAGS?${${@}_DEF_TROFFFLAGS}:${DEF_TROFFFLAGS}} ${${@}_TROFFENV?${${@}_TROFFENV}:${TROFFENV}} ${${@}_TROFFFLAGS?${${@}_TROFFFLAGS}:${TROFFFLAGS}} ${${@}_TROFFARGS?${${@}_TROFFARGS}:${TROFFARGS}}
_COLFLAGS_=${${@}_DEF_COLFLAGS?${${@}_DEF_COLFLAGS}:${DEF_COLFLAGS}} ${${@}_COLENV?${${@}_COLENV}:${COLENV}} ${${@}_COLFLAGS?${${@}_COLFLAGS}:${COLFLAGS}} ${${@}_COLARGS?${${@}_COLARGS}:${COLARGS}}
_TBLFLAGS_=${${@}_DEF_TBLFLAGS?${${@}_DEF_TBLFLAGS}:${DEF_TBLFLAGS}} ${${@}_TBLENV?${${@}_TBLENV}:${TBLENV}} ${${@}_TBLFLAGS?${${@}_TBLFLAGS}:${TBLFLAGS}} ${${@}_TBLARGS?${${@}_TBLARGS}:${TBLARGS}}
_EPSFLAGS_=${${@}_DEF_EPSFLAGS?${${@}_DEF_EPSFLAGS}:${DEF_EPSFLAGS}} ${${@}_EPSENV?${${@}_EPSENV}:${EPSENV}} ${${@}_EPSFLAGS?${${@}_EPSFLAGS}:${EPSFLAGS}} ${${@}_EPSARGS?${${@}_EPSARGS}:${EPSARGS}}

#
#  Default single suffix compilation rules
#

.SUFFIXES:	.out ${MANSECTION?.0:} .ps .doc ${MANSECTION?.${MANSECTION}:}

#
#  Default double suffix compilation rules
#
.doc.ps:
	${TBL} ${_TBLFLAGS_} $*.doc | ${TROFF} ${_TROFFFLAGS_} - | ${EPS} ${_EPSFLAGS_} > $*.X && ${MV} $*.X $@

.doc.out:
	${TBL} ${_TBLFLAGS_} $*.doc | ${NROFF} ${_NROFFFLAGS_} - | ${COL} ${_COLFLAGS_} >$*.X && ${MV} $*.X $@

${MANSECTION?.${MANSECTION}.0:}:
	${NROFF} ${_NROFFFLAGS_} $*.${MANSECTION} >$*.0.out
	${MV} -f $*.0.out $*.0

#
# Build Rules
#
${_DOCUMENTS_}: $${_DOCFILES_}
