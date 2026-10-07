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

PROGRAMS		= gencat mkcatdefs dspmsg dspcat
SCRIPTS                 = runcat
MSGHDRS			= msgfac_msg.h
CATFILES		= msgfac.cat

ILIST			= gencat mkcatdefs dspmsg dspcat ${SCRIPTS}
IDIR			= /usr/bin/

HFILES			= ${MSGHDRS}
gencat_OFILES		= gencat.o catio.o
mkcatdefs_OFILES	= mkcatdefs.o symtab.o catio.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
