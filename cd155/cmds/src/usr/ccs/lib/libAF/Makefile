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

LIBRARIES	= libAF.a

EXPLIB_TARGETS	= export_libAF.a

ILIST		= libAF.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

OFILES		= AFopen.o	\
		  AFrewind.o	\
		  AFclose.o	\
		  AFsetdflt.o	\
		  AFread.o	\
		  AFgetval.o	\
		  AFnxtval.o	\
		  AFgetent.o	\
		  AFnxtent.o	\
		  AFgetatr.o	\
		  AFnxtatr.o	

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
