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
# (c) Copyright 1992, INTEL, CORP.
# ALL RIGHTS RESERVED
# $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/mach3/Makefile,v 1.9 1994/11/19 02:42:49 mtm Exp $
#
# INTEL Release 1.0

MK_INC			= ${MAKETOP}/mk/release/i860_mach/include
VPATH			= ${MK_INC}

SUBDIRS          	= device mach mach_debug i860paragon ${MACHINE}
EXPINC_SUBDIRS          = ${SUBDIRS}

DATAFILES		= cthreads.h errorlib.h mach.h mach_error.h \
			  mach_init.h mig_errors.h msg_type.h

OTHERS                  = machine

INCLUDES		= ${DATAFILES} ${OTHERS}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/mach3/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk

machine: ${ALWAYS}
	${RM} -rf machine
	ln -s ${MACHINE} machine
include ${MAKEFILEPATH}/depend.mk
-include Makedep
