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
# Definitions for build
#
_LIBRARIES_=${LIBRARIES?${LIBRARIES}:_LIBRARIES_}
_SHARED_LIBRARIES_=${SHARED_LIBRARIES?${SHARED_LIBRARIES}:_SHARED_LIBRARIES_}
_SET_ANY_LIBRARIES_=${LIBRARIES?_ANY_LIBRARIES_:${SHARED_LIBRARIES?_ANY_LIBRARIES_:}}
${_SET_ANY_LIBRARIES_}=

_LIB_OFILES_=${LIBRARIES?${_OFILES_}:}

#
#  Definitions for default .c.o rules used by programs or libraries
#
_LIBRARY_C_O_=${_ANY_LIBRARIES_?.c.o:_LIBRARY_C_O_}

#
#  Default double suffix compilation rules
#
${_LIBRARY_C_O_}:
	${_CC_} -c ${_CCFLAGS_} $*.c
#	${LD} ${_LDFLAGS_} -x -r $*.o
#	${MV} -f a.out $*.o

#
#  Build rules
#
${_LIBRARIES_}: ${_LIBRARIES_}($${_LIB_OFILES_}) 
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${AR} ${DEF_ARFLAGS} $@ _cvs_id.o $?
	${RANLIB} $@
	${RM} -f _cvs_id.c _cvs_id.o $? ../$@
	ln -s `basename $@ .a`/$@ ../$@

${_SHARED_LIBRARIES_}: $${_OFILES_}
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${_LD_} ${_SHLDFLAGS_} -o $@.X _cvs_id.o ${_OFILES_} ${_LIBS_}
	${MV} $@.X $@

