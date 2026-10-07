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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.3

VPATH			= ${TARGET_MACHINE}

USE_STATIC_LIBRARIES	=

GLINE			=

EXPCOMMON_TARGETS	= export_dummy
IDIR			= /usr/ccs/lib/

CFLAGS			= -DCOMPAT_43 -DLANGUAGE_C -DLIBC_SCCS \
			  ${${TARGET_MACHINE}CFLAGS} 

OFILES			= ${STDOBJS}

GARBAGE			= *.S *.as tags

STDOBJS			= nx_setup.o execl.o execle.o execlp.o execvp.o \
			  dclock.o _dclock.o _myptype.o _mynode.o estat.o \
			  festat.o lestat.o _eseek.o _numnodes.o \
			  forknx.o execve.o data.o

IPSC860SOBJS	= execv.o __eseek.o _estat.o _festat.o _lestat.o \
			  statpfs.o fstatpfs.o fork.o execvenx.o

-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk

-include Makedep


${LOCALE}: ${LOCALEPATH}/$${@:=.ctab}
	${CTAB} -i ${LOCALEPATH}/${@:=.ctab} -o $@.X
	${MV} $@.X $@


#${STDOBJS}: $${@:.o=.c}
#${_CC_} ${_CCFLAGS_} -c $*.c
#	${LD} -x -r $*.o
#	${MV} -f a.out $*.o

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c

all:	${STDOBJS} ${IPSC860SOBJS}

dummy:	all
	touch dummy

tags:
	ctags *.c ${TARGET_MACHINE}/*.c
