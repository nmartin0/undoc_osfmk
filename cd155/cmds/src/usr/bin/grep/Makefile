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

PROGRAMS		= egrep fgrep grep
MSGHDRS			= egrep_msg.h fgrep_msg.h grep_msg.h
CATFILES		= egrep.cat fgrep.cat grep.cat

ILIST			= egrep fgrep grep
IDIR			= /usr/bin/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

${MSGHDRS:_msg.h=.o}: $${@:.o=_msg.h}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
