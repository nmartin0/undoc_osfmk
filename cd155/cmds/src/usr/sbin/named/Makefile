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

PROGRAMS		= named named-xfer
MSGHDRS			= named_msg.h
CATFILES		= named.cat

ILIST			= named named-xfer
IDIR			= /usr/sbin/

CCTYPE			= writable_strings
#CFLAGS			= -D_BSD  -D_SOCKADDR_LEN -DDEBUG -DSTATS
CFLAGS			= -D_BSD  -D_SOCKADDR_LEN -UDEBUG -DSTATS
HFILES			= ${MSGHDRS} db.h ns.h pathnames.h
#ALLOC			= storage.o
named_OFILES		= db_dump.o db_load.o db_glue.o db_lookup.o \
			  db_reload.o db_save.o db_update.o ns_forw.o \
			  ns_init.o ns_main.o ns_maint.o ns_req.o ns_resp.o\
			  ns_sort.o ns_stats.o\
			  ${ALLOC} version.o
named-xfer_OFILES	= named-xfer.o db_glue.o
LIBS			= -lbsd -lutil
GARBAGE			= version version.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

version.c: newvers.sh Version.c version.ucb
	sh newvers.sh Version.c version.ucb
include ${MAKEFILEPATH}/depend.mk
-include Makedep
