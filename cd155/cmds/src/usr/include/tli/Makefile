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

VPATH                   = ${SVROBJ}/include/tli

DATAFILES		= common.h iostate.h tdbg.h tihdr.h timod.h tlistate.h \
			  xtiso_config.h
INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/tli/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
env:
	@echo SVROBJ is ${SVROBJ}
	@echo VPATH is ${VPATH}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
