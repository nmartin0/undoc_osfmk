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
# (c) Copyright 1992, INTEL , CORP.
# ALL RIGHTS RESERVED
#
#
# INTEL  1.0
#
#
# History
# $Log: Makefile,v $
# Revision 1.10  1994/11/19  02:19:48  mtm
# Copyright additions/changes
#
# Revision 1.9  1994/04/13  15:46:34  dleslie
# Accommodate new emulator object tree architecture
#
# Revision 1.8  1994/02/04  20:29:00  brad
# Modified extended math support so that the routines that actually
# implement the math are pulled in from the server/emulator source
# tree, rather than duplicating the same code in the libesize source.
#  Reviewer: None
#  Risk: Low
#  Benefit or PTS #:
#  Testing: Ran PFS EATs, ran emath tests.
#  Module(s): Makefile _eadd.c _eadd1.c _ecmp.c _ediv.c _emod.c _emul.c
#             _esub.c _esub1.c _etos.c _etoxs.c _stoe.c _xstoe.c
#
# Revision 1.7  1994/01/12  17:33:13  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create link for libesize.a in obj tree, so utilities
# 	can link with it
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.6  1993/12/20  19:41:03  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.5  1993/11/18  20:17:01  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	get mcmsg and nx headers out of export tree
#  Testing: built on Suns and 486
#  Module(s): Makefile
#
# Revision 1.4  1993/11/13  06:30:05  brad
# Added _etoxs and _xstoe routines for manipulating ASCII hex representations
# of extended numbers.
#
#  Reviewer: None.
#  Risk: Low.
#  Benefit or PTS #: Added for tar support, for manipulation of extended hex
# 		   strings used in the extended format of the tar header.
#  Testing: Tested creating/extracting tar archives that included extended files.
#  Module(s): usr/ccs/lib/libesize/{Makefile,_etoxs.c,_xstoe.c}
#
# Revision 1.3  1993/09/28  18:02:05  dleslie
# Removed ILIST et al so libesize is not installed into release tree
#
# Revision 1.2  1993/09/28  01:32:55  brad
# Fixed line wrap.
#
# Revision 1.1  1993/09/28  00:48:31  brad
# Initial revision
#
#

VPATH		= ${TARGET_MACHINE}

EMULOBJ		= ${SVROBJ}/../emulator/${SVRCONFIG}

FC		= if77
FFLAGS		= -Mcoff -Y0,$(OSFTOP)/tools/ipsc860/hostbin \
		  -Ya,$(OSFTOP)/tools/ipsc860/hostbin -c  -O

OTHERS		= libesize.a
OBJECTS		=


INCFLAGS	= -I${TARGET_MACHINE}

CFLAGS		= -DCOMPAT_43 -DLANGUAGE_C -DLIBC_SCCS -DCUBE -DTNC \
		  -DNX -DPFS ${${TARGET_MACHINE}CFLAGS}

OFILES		= ${C_OBJS} \
		  ${EMUL_C_OBJS} \
		  ${F_OBJS} \
		  ${${TARGET_MACHINE}SOBJS} \
		  ${${TARGET_MACHINE}COBJS}

GARBAGE		= *.S *.as tags

C_OBJS		= _eadd.o eadd.o _eadd1.o eadd1.o _ecmp.o ecmp.o \
		  _ediv.o ediv.o _emod.o emod.o _emul.o emul.o \
		  _esub.o esub.o _esub1.o esub1.o _etos.o etos.o \
		  _etoxs.o _stoe.o stoe.o _xstoe.o

EMUL_C_OBJS	= pfs_emath.o

F_OBJS		= eadd_.o eadd1_.o ecmp_.o ediv_.o emod_.o emul_.o \
		  esub_.o esub1_.o etos_.o stoe_.o

-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c

# i860 COFF doesn't require lorder ...
#COFF_LORDER=`lorder *.o | tsort`
COFF_LORDER=`ls *.o`
A_OUT_LORDER=${OFILES}
MACHO_LORDER=${OFILES}

libesize.a: libesize.a(${OFILES})
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${AR} ${DEF_ARFLAGS} $@ _cvs_id.o $?
	${RM} -rf tmp 
	mkdir tmp
	cd tmp && { \
		${AR} x ../libesize.a; ${RM} -f __.SYMDEF __________ELELX; \
		${AR} cr libesize.a ${${OBJECT_FORMAT}_LORDER}; }
	${MV} -f tmp/libesize.a .
	${RM} -rf tmp
	${RANLIB} $@
	${RM} -f _cvs_id.c _cvs_id.o ../libesize.a
	ln -s libesize/libesize.a ../libesize.a

${EMUL_C_OBJS}: ${EMULOBJ}/$$@
	for i in ${EMUL_C_OBJS}; do cp ${EMULOBJ}/$$i .; done

include ${MAKEFILEPATH}/depend.mk
-include Makedep
