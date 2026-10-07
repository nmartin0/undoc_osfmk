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

PROGRAMS		= top
MSGHDRS			= top_msg.h
CATFILES		= top.cat

ILIST			= top
IDIR			= /usr/bin/
IOWNER			= ${ROOT_OWNER}
IMODE			= ${SETUID_MODE}


# values for two defaults in "top"
#       TOPN    - default number of processes to display
#       DELAY   - default delay between updates
TOPN = 10
DELAY = 5

CCTYPE			= writable_strings
CFLAGS			= -D_BSD -DDefault_TOPN=$(TOPN) -DDefault_DELAY=$(DELAY)

HFILES			= 
OFILES			= top.o getname.o
LIBS			= -lcurses -ltermcap -ltermlib -lm -lbsd -lmach

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
