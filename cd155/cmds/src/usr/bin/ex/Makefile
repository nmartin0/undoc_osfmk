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
# OSF/1 Release 1.0.2

VERSION			= 3.7

SEC_PROGRAMS		= expreserve
PROGRAMS		= ex exrecover expreserve ctags
MSGHDRS			= ex_msg.h
CATFILES		= ex.cat

SEC_ILIST		= ${SEC_PROGRAMS}
ILIST			= ${PROGRAMS}
IDIR			= /usr/bin/
LIB_IDIR		= /usr/lbin/
ex_ILINKS		= ${IDIR}edit ${IDIR}e \
			  ${IDIR}vi ${IDIR}view \
			  ${IDIR}vedit
ex_IMODE		= 1755
exrecover_IOWNER	= root
exrecover_IMODE		= 4711
exrecover_IDIR		= ${LIB_IDIR}
exrecover_ILINKS	= ${LIB_IDIR}ex${VERSION}recover
expreserve_IOWNER	= root
expreserve_IMODE	= 4711
expreserve_IDIR		= ${LIB_IDIR}
expreserve_ILINKS	= ${LIB_IDIR}ex${VERSION}preserve

# removed OSF_MMAP until mmap() syscall works.
# -DSTDIO -DUSG -DFLOCKFILE -DOSF_MMAP

CFLAGS			= -DLISPCODE -DVMUNIX -UVFORK -DCHDIR \
			  -DSTDIO -DUSG -DFLOCKFILE \
			  -DEXPRESERVE=\"${LIB_IDIR}ex${VERSION}preserve\" \
			  -DEXRECOVER=\"${LIB_IDIR}ex${VERSION}recover\" \
			  -DUNIX_SBRK -DVERSION=${VERSION} ${SEC_DEFS}

#CFLAGS			= -DTRACE -DUNDOTRACE -DADEBUG -DMDEBUG -DFDEBUG \
#			  -DIDEBUG -DMYSIG -DSUSPTRACE -DMYALLOC -DSTDIO \
#			  -DBSDNEW -DLISPCODE -DSINGLE -DTABS=8 -DUSG \
#			  -DVMUNIX -UMYDEBUG -URDEBUG -UUCBV7 -UV6 -UVFORK

ctags.o_CFLAGS		= -DBSDNEW
ex_OFILES		= ex.o ex_addr.o ex_cmds.o ex_cmds2.o \
			  ex_cmdsub.o ex_data.o ex_extern.o \
			  ex_get.o ex_io.o ex_put.o ex_re.o \
			  ex_set.o ex_subr.o \
			  ex_temp.o ex_tty.o ex_unix.o ex_v.o \
			  ex_vadj.o ex_vget.o ex_vmain.o \
			  ex_voper.o ex_vops.o ex_vops2.o \
			  ex_vops3.o ex_vput.o ex_vwind.o \
			  printf.o
ex_GARBAGE              = ex_vars.h
ex_LINTFILES		= ${ex_OFILES:.o=.c}
exrecover_OFILES	= exrecover.o
expreserve_OFILES	= expreserve.o ${SEC_DEFS?expre_sec.o:}
expreserve_LIBS         = ${SEC_LIBS}
ex_HFILES		= ex.h ex_argv.h ex_re.h ex_temp.h ex_tty.h ex_tune.h \
			  ex_vars.h ex_vis.h ex_msg.h
ex_LIBS			= -lcurses

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${ex_OFILES}: ${ex_HFILES}

expreserve.o: ex_msg.h

ex_vars.h: makeoptions ex_data.c
	sh makeoptions ex_data.c ${_CCFLAGS_}

${ctags_OFILES}: ex_msg.h

exrecover.o: ex_msg.h ex.h ex_temp.h ex_tty.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
