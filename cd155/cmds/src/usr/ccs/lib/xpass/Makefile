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

VPATH			= ../../bin/common:../../bin/cxref

PROGRAMS		= xpass
MSGHDRS			= ctools_msg.h cxref_msg.h
CATFILES		= ctools.cat cxref.cat

ILIST			= xpass
IDIR			= /usr/ccs/lib/

INCFLAGS		= -I../../bin/common
CFLAGS	=	\
	-DNEWCALL	\
	-DHOSTIEEE	\
	-DSDBDEBUG	\
	-DTWOPASS	\
	-DSINGLE_PRECISION	\
	-D_COMPILER	\
	-DCXREF

xpass_OFILES	= xpass.o cgram.o code.o comm1.o local.o messages.o	\
		  optim.o pftn.o scan.o tftn.o trees.o xdefs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

xpass.o:	cxref_msg.h

$(xpass_OFILES):	ctools_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
