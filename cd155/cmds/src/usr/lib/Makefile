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

SUBDIRS			= diction \
			  learn me ms nls tabset tmac nqs sat macs

DATAFILES		= Mail.help Mail.rc Mail.tildehelp \
			  lib.b units

ILIST			= ${PROGRAMS} ${SCRIPTS} ${DATAFILES}
IDIR			= /usr/lib/

SHARELIB_IDIR		= /usr/share/lib/
Mail.help_IDIR		= ${SHARELIB_IDIR}
Mail.help_IMODE		= 644
Mail.rc_IDIR		= ${SHARELIB_IDIR}
Mail.rc_IMODE		= 644
Mail.tildehelp_IDIR	= ${SHARELIB_IDIR}
Mail.tildehelp_IMODE	= 644
lib.b_IDIR		= ${SHARELIB_IDIR}
lib.b_IMODE		= 644
units_IDIR		= ${SHARELIB_IDIR}
units_IMODE		= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
