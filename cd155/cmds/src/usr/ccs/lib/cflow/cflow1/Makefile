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

VPATH			= ${MAKETOP}/usr/ccs/bin/common:${MAKETOP}/usr/ccs/bin

PROGRAMS		= cflow1
MSGHDRS			= ctools_msg.h
CATFILES		= ctools.cat

ILIST			= cflow1
IDIR			= /usr/ccs/lib/

INCFLAGS		= -I${MAKETOP}/usr/ccs/bin/common
CFLAGS	=	\
	-DHOSTIEEE	\
	-DCFLOW		\
	-DNEWCALL	\
	-DHOSTIEEE	\
	-DSDBDEBUG	\
	-DTWOPASS	\
	-DSINGLE_PRECISION	\
	-D_COMPILER
HFILES			= ${MSGHDRS} mfile1.h error.h common.h
OFILES			= cgram.o code.o comm1.o local.o messages.o optim.o \
			  pftn.o scan.o tftn.o trees.o xdefs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
