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

SUBDIRS			= special
DATAFILES		= words american british stop README ${BINARIES}
BINARIES		= hlist hlista hlistb hstop

ILIST			= ${DATAFILES}
IDIR			= /usr/share/dict/
IMODE			= 444

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk

${BINARIES}: ${SOURCEBASE}/usr/share/dict/$$@
	cp -p ${SOURCEBASE}/usr/share/dict/$@ .

-include Makedep
