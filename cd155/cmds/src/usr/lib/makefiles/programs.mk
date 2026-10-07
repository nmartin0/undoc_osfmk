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
_PROGRAMS_=${PROGRAMS?${PROGRAMS}:_PROGRAMS_}
_PROG_OFILES_=${PROGRAMS/*/${&_OFILES?${&_OFILES}:&.o}}

#
#  Definitions for default .c.o rules used by programs or libraries
#
_PROGRAM_C_O_=${PROGRAMS?.c.o:${OBJECTS?.c.o:_PROGRAM_C_O_}}

#
#  Default single suffix compilation rules
#
.c:
	${_CC_} ${_LDFLAGS_} ${_CCFLAGS_} -o $*.X $*.c ${_LIBS_}
	${MV} $*.X $@

.p:
	${PC} ${_PCFLAGS_} -o $*.X $*.p ${_LIBS_}
	${MV} $*.X $@

.y:
	${YACC} ${_YFLAGS_} $<
	${_CC_} ${_LDFLAGS_} ${_CCFLAGS_} -o $*.X y.tab.c
	-${RM} -f y.tab.c
	${MV} -f $*.X $@

.l:
	${LEX} ${_YFLAGS_} $<
	${_CC_} ${_LDFLAGS_} ${_CCFLAGS_} -o $*.X lex.yy.c
	-${RM} -f lex.yy.c
	${MV} -f $*.X $@

#
#  Default double suffix compilation rules
#
${_PROGRAM_C_O_}:
	${_CC_} -c ${_CCFLAGS_} $*.c

#
#  Build rules
#
${_PROGRAMS_}: $${_OFILES_}
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c 
	${RM} -f _cvs_id.c
	-${_CC_} ${VERBOSE_LD} ${_LDFLAGS_} -o $@.X _cvs_id.o ${_OFILES_} \
		${_LIBS_} > _LOAD.$@.d.tmp 2>&1
	@if [ -f $@.X ] ; then \
	    grep -v 'warning:' _LOAD.$@.d.tmp | sed "s/^/$@: /"  > _LOAD.$@.d ; \
	    else \
		cat _LOAD.$@.d.tmp ; \
	fi
	@rm -f _LOAD.$@.d.tmp
	${MV} $@.X $@
