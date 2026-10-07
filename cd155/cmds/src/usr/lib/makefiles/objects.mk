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
# definitions for build
#
_ALL_OFILES_=${OFILES?${OFILES}:${_PROG_OFILES_:u}}

_OFILES_=${OFILES?${OFILES}:${${@}_OFILES?${${@}_OFILES}:${BINARIES?$@.o:}}}

#
# definitions for clean
#
_CLEAN_OFILES_=${OFILES?${OFILES}:${$%_OFILES?${$%_OFILES}:${BINARIES?$%.o:}}}

#
# definitions for lint
#
_LINT_OFILES_=${OFILES?${OFILES}:${$%_OFILES?${$%_OFILES}:${PROGRAMS?$%.o:}}}

#
# definitions for tags
#
_TAGS_OFILES_=${OFILES?${OFILES}:${$%_OFILES?${$%_OFILES}:${PROGRAMS?$%.o:}}}

#
#  Definitions for using xstr
#
_XSTR_C_O_=${XSTR_OFILES?%_x.o:_XSTR_C_O_}

#
#  Default double suffix compilation rules
#
${_XSTR_C_O_}: %.c
	${_CC_} -E ${_CCFLAGS_} %.c | ${XSTR} -c -
	${_CC_} ${_CCFLAGS_} -c x.c
	${MV} -f x.o %_x.o
	${RM} -f x.c

.y.o:
	${YACC} ${_YFLAGS_} $<
	${_CC_} ${_CCFLAGS_} -c y.tab.c
	-${RM} -f y.tab.c
	${MV} -f y.tab.o $@

.y.c:
	${YACC} ${_YFLAGS_} $<
	${MV} -f y.tab.c $@

.l.o:
	${LEX} ${_LFLAGS_} $<
	${_CC_} ${_CCFLAGS_} -c lex.yy.c
	-${RM} -f lex.yy.c
	${MV} -f lex.yy.o $@

.p.o:
	${PC} -c ${_PCFLAGS_} $*.p

.c.pp:
	${_CC_} ${_CCFLAGS_} -E $< >$@

${_ALL_OFILES_}: ${HFILES}

${_MIG_HDRS_}: $${@:.h=.defs}
	${MIG} ${_MIGFLAGS_} ${@:.h=.defs} -server /dev/null -user /dev/null

${_MIG_USRS_}: $${@:User.c=.defs}
	${MIG} ${_MIGFLAGS_} ${@:User.c=.defs} -server /dev/null

${_MIG_SRVS_}: $${@:Server.c=.defs}
	${MIG} ${_MIGFLAGS_} ${@:Server.c=.defs} -user /dev/null
