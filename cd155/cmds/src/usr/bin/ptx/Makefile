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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.1

PROGRAMS		= ptx
DATAFILES		= eign
MSGHDRS			= ptx_msg.h
CATFILES		= ptx.cat
HFILES			= ${MSGHDRS}

ILIST			= ptx eign
IDIR			= /usr/bin/
LIB_IDIR		= /usr/share/lib/
eign_IDIR		= ${LIB_IDIR}
ptx.o_CFLAGS		= -DDEFLTX='"${LIB_IDIR}eign"'

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
