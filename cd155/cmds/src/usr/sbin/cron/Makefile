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

SEC_PROGRAMS 		= crontab cron
PROGRAMS 		= crontab cron
DATAFILES		= .proto cron.deny log queuedefs adm root sys
MSGHDRS 		= cron_msg.h
CATFILES 		= cron.cat

SEC_ILIST		= ${SEC_PROGRAMS}
ILIST			= ${PROGRAMS} ${DATAFILES}
IDIR			= /usr/sbin/
CRONTAB_IDIR		= /usr/bin/
SPOOL_IDIR		= /var/spool/cron/crontabs/
ADM_IDIR		= /var/adm/cron/
crontab_IDIR		= ${CRONTAB_IDIR}
crontab_IOWNER		= root
crontab_IMODE		= 4755
.proto_IDIR		= ${ADM_IDIR}
cron.deny_IDIR		= ${ADM_IDIR}
log_IDIR		= ${ADM_IDIR}
queuedefs_IDIR		= ${ADM_IDIR}
adm_IDIR		= ${SPOOL_IDIR}
root_IDIR		= ${SPOOL_IDIR}
sys_IDIR		= ${SPOOL_IDIR}

cron.o_CFLAGS 		= ${SEC_DEFS}
cron_sec.o_CFLAGS	= ${SEC_DEFS}
cronsub.o_CFLAGS 	= ${SEC_DEFS}
crontab.o_CFLAGS 	= ${SEC_DEFS}
crontab_sec.o_CFLAGS	= ${SEC_DEFS}
HFILES			= cron_msg.h cron.h
crontab_OFILES          = crontab.o permit.o funcs.o cronsub.o \
                          ${SEC_DEFS?crontab_sec.o:}
cron_OFILES             = cron.o funcs.o elm.o ${SEC_DEFS?cron_sec.o:}
LIBS                    = ${SEC_LIBS} -lutil

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
