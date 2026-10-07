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

PROGRAMS		= sendmail mailstats
DATAFILES		= sendmail.cf sendmail.hf aliases
MSGHDRS			= sendmail_msg.h mailstats_msg.h
CATFILES                = sendmail.cat mailstats.cat

ILIST			= ${PROGRAMS} ${DATAFILES}
IDIR			= /usr/sbin/
ADM_IDIR		= /var/adm/sendmail/
sendmail.cf_IDIR	= ${ADM_IDIR}
sendmail.hf_IDIR	= /usr/share/lib/
aliases_IDIR		= ${ADM_IDIR}
sendmail_IOWNER		= root
sendmail_IMODE		= 4755
sendmail_ILINKS		= ${IDIR}mailq ${IDIR}newaliases ${IDIR}smtpd
sendmail.cf_IOWNER	= root
sendmail.cf_IMODE	= 644
aliases_IOWNER		= root
aliases_IMODE		= 644

CCTYPE			= writable_strings
USE_STATIC_LIBRARIES	=
INCFLAGS		= -I.
CFLAGS			= -D_BSD -DNOCATMACRO -DVMUNIX -DNDBM  -D__PARAGON__


HFILES			= cdefs.h conf.h mailstats.h pathnames.h sendmail.h \
			  useful.h sysexits.h ${MSGHDRS}
sendmail_OFILES		= alias.o arpadate.o clock.o collect.o conf.o \
			  convtime.o daemon.o deliver.o domain.o envelope.o \
			  err.o headers.o macro.o main.o parseaddr.o \
			  queue.o readcf.o recipient.o savemail.o \
			  srvrsmtp.o stab.o stats.o sysexits.o trace.o \
			  usersmtp.o util.o version.o map.o mci.o

sendmail_LIBS		= -lbsd -ldbm -lutil

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
