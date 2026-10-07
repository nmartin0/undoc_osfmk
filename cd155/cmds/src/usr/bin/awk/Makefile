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

PROGRAMS		= awk

ILIST			= awk
IDIR			= /usr/bin/

# i860: icc is ANSI ONLY
#CCTYPE			= traditional
#CFLAGS			= -D_NO_PROTO -Dvolatile=__volatile__
#
YFLAGS			= -d
FILES			= awk.lx.o b.o main.o token.o tran.o lib.o\
			  run.o parse.o proctab.o freeze.o
OFILES			= awk.g.o ${FILES}
LIBS			= -lm
GARBAGE			= awk.h token.c proc proc.o proctab.c\
			  y.tab.c y.tab.h yacc* temp

proc.o_CCTYPE		= host
proc.o_INCFLAGS		= -I.
#proc.o_CFLAGS		= -DPROC
proctoken.o_CCTYPE	= host
proctoken.o_INCFLAGS	= -I.
#proctoken.o_CFLAGS	= -DPROC
proc_CCTYPE		= host
proc_LIBS		=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

awk.g.o: awk.def

proc.o:	awk.h

y.tab.h: awk.g.o

awk.h: awk.g.o
	-if [ -f y.tab.h ]; then ${MV} -f y.tab.h awk.h; fi

${FILES}: awk.h awk.def

token.c: awk.h token.c.ucb
	sed -n -e '1,/= *{'\$$'/p' token.c.ucb > ./token.pre
	sed -e 's/\# *define *//' \
	    -e 's/^/'\"'/' \
	    -e 's/ /'\"', /' \
	    -e 's/'\$$'/,/' awk.h > ./token.mid
	sed -n -e '/^};'\$$'/,'\$$'p' token.c.ucb > ./token.end
	cat ./token.pre ./token.mid ./token.end > $@.X
	${MV} $@.X $@

proctab.c: proc
	${RM} -f $@
	./proc > $@

proctoken.c: token.c
	${RM} -f $@
	${CP} token.c $@

proc.o: awk.h

proc:	proc.o proctoken.o
	${_CC_} ${_LDFLAGS_} -o $@.X proc.o proctoken.o
	${MV} $@.X $@
include ${MAKEFILEPATH}/depend.mk
-include Makedep
