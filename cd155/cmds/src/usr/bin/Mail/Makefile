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

PROGRAMS		= Mail fmt
DATAFILES		= Mail.rc
MSGHDRS			= Mail_msg.h
CATFILES		= Mail.cat

ILIST			= ${PROGRAMS} ${DATAFILES}
IDIR			= /usr/bin/
Mail_IGROUP		= mail
Mail_IMODE		= 775
Mail_ILINKS		= ${IDIR}mailx
Mail.rc_IDIR		= /usr/lib/

CFLAGS			= -D_BSD
HFILES			= configdefs.h def.h glob.h \
			  local.h rcv.h sysexits.h ${MSGHDRS}
Mail_OFILES		= aux.o cmd2.o cmd3.o cmdtab.o collect.o config.o \
			  edit.o fio.o getname.o head.o lex.o list.o main.o \
			  names.o optim.o quit.o send.o strings.o temp.o \
			  tty.o v7.local.o vars.o cmd1.o version.o
fmt_OFILES		= fmt.o head.o
LIBS			= -lbsd

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
