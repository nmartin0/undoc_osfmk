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

#
# @(#)Makefile	4.2 09:46:58 7/12/90 SecureWare
#

#
# We get cron.h, permit.c, funcs.c, cronsub.c from here
#
VPATH			= ${MAKETOP}/usr/sbin/cron

SEC_PROGRAMS		= at atq atrm
PROGRAMS		= at atq atrm 
SCRIPTS			= batch
DATAFILES		= at.deny
MSGHDRS 		= cron_msg.h
CATFILES 		= cron.cat

SEC_ILIST		= at atq atrm
ILIST			= at atq atrm batch at.deny
IDIR			= /usr/bin/
ADM_IDIR		= /var/adm/cron/
at_IMODE		= 4755
at_IOWNER		= root
at.deny_IDIR		= ${ADM_IDIR}

INCFLAGS		= -I${MAKETOP}/usr/sbin/cron
at.o_CFLAGS		= ${SEC_DEFS}
at_secure.o_CFLAGS	= ${SEC_DEFS}
atq.o_CFLAGS		= ${SEC_DEFS}
atrm.o_CFLAGS		= ${SEC_DEFS}
cronsub.o_CFLAGS	= ${SEC_DEFS} -DBUILDING_AT
YFLAGS			= -d
HFILES			= cron_msg.h
at_OFILES 		= at.o att1.o permit.o funcs.o cronsub.o \
			  ${SEC_DEFS?at_secure.o:}
atq_OFILES 		= atq.o cronsub.o funcs.o ${SEC_DEFS?at_secure.o:}
atrm_OFILES 		= atrm.o cronsub.o funcs.o ${SEC_DEFS?at_secure.o:}
LIBS			= ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/scripts.mk
include ${MAKEFILEPATH}/objects.mk

att1.h: att1.c
	-if [ -f y.tab.h ]; then ${MV} -f y.tab.h att1.h; fi

att1.o: att1.h
at.o atq.o:	cron.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
