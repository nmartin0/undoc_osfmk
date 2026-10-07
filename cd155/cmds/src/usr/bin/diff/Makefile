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

PROGRAMS		= diff diffh
MSGHDRS			= diff_msg.h
CATFILES		= diff.cat

ILIST			= diff diffh
IDIR			= /usr/bin/
LIB_IDIR		= /usr/lbin/
diffh_IDIR		= ${LIB_IDIR}

DIFF			= ${IDIR}diff
DIFFH			= ${LIB_IDIR}diffh
PR			= ${IDIR}pr

CFLAGS			= -D${OBJECT_FORMAT}
diff.o_CFLAGS		= -DDIFF='"${DIFF}"' -DDIFFH='"${DIFFH}"'\
			  -DPR='"${PR}"' ${CFLAGS}
HFILES			= diff_msg.h diff.h
diff_OFILES		= diff.o diffdir.o diffreg.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
