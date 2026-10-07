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
# (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
# ALL RIGHTS RESERVED 
#
#
# OSF/1 Release 1.0.4

#
#  Definitions for rules using sed
#
_SED_STRIP_=-e '1s;^\#!;&;' -e t
_N_A_S_F_=THIS IS NOT A SOURCE FILE - DO NOT EDIT
_SED_SOURCEWARNING_=-e 's;^\#\(.*\)\@SOURCEWARNING\@;\1${_N_A_S_F_};' -e t
_SED_CSH_STRIP_=-e '/^[ 	]*\\#/d'
_SED_SH_STRIP_=-e '/^[ 	]*[\#]/d'

#
#  Default single suffix compilation rules
#
.csh:
	${SED}\
	 ${_SED_STRIP_}\
	 ${_SED_SOURCEWARNING_}\
	 ${${@}_SED_OPTIONS?${${@}_SED_OPTIONS}:${SED_OPTIONS}}\
	 ${_SED_CSH_STRIP_} $< >$*.X
	${CHMOD} +x $*.X
	${MV} -f $*.X $*

.sh:
	${SED}\
	 ${_SED_STRIP_}\
	 ${_SED_SOURCEWARNING_}\
	 ${${@}_SED_OPTIONS?${${@}_SED_OPTIONS}:${SED_OPTIONS}}\
	 ${_SED_SH_STRIP_} $< >$*.X
	${CHMOD} +x $*.X
	${MV} -f $*.X $*
