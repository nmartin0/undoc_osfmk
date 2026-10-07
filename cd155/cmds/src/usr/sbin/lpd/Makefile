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

SEC_PROGRAMS		= lpd lpr lpq lprm lpc
PROGRAMS		= ${SEC_PROGRAMS} pac lp cancel lpstat lptest
MSGHDRS			= printer_msg.h
CATFILES		= printer.cat

SEC_ILIST		= lpd lpr lpq lprm lpc
ILIST			= ${SEC_ILIST} pac lp cancel lpstat lptest
IDIR			= /usr/sbin/
BIN_IDIR		= /usr/bin/
cancel_IDIR		= ${BIN_IDIR}
lp_IDIR			= ${BIN_IDIR}
lpq_IDIR		= ${BIN_IDIR}
lpr_IDIR		= ${BIN_IDIR}
lprm_IDIR		= ${BIN_IDIR}
lpstat_IDIR		= ${BIN_IDIR}
lpc_IOWNER		= root
lpc_IMODE		= 4711
lpd_IOWNER		= root
lpd_IMODE		= 4711
lpq_IOWNER		= root
lpq_IMODE		= 4711
lpr_IOWNER		= root
lpr_IMODE		= 4711
lprm_IOWNER		= root
lprm_IMODE		= 4711

CFLAGS			= ${SEC_DEFS}
lpr.o_CFLAGS		= -D${OBJECT_FORMAT} ${CFLAGS}
HFILES			= ${MSGHDRS}
lpc_OFILES		= lpc.o cmds.o cmdtab.o startdaemon.o common.o \
			  printcap.o
lpd_OFILES		= lpd.o printjob.o recvjob.o displayq.o rmjob.o \
			  startdaemon.o lpdchar.o common.o printcap.o \
			  ${SEC_DEFS?lpd_sec.o:}
lpq_OFILES		= lpq.o printcap.o common.o displayq.o
lpq_LIBS		= -ltermlib ${LIBS}
lpr_OFILES		= lpr.o startdaemon.o common.o printcap.o
lprm_OFILES		= lprm.o rmjob.o startdaemon.o common.o printcap.o
pac_OFILES		= pac.o printcap.o
lpstat_OFILES		= lpstat.o printcap.o
LIBS			= -lresolv ${SEC_LIBS}
lpd_LIBS		= -lutil ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

lpd.o lpr.o lpq.o lprm.o pac.o: lp.h lp.local.h
recvjob.o printjob.o displayq.o rmjob.o common.o: lp.h lp.local.h
lp.o lpstat.o cancel.o: lp.h lp.local.h
startdaemon.o: lp.local.h
lpc.o cmdtab.o: lpc.h
cmds.o: lp.h lp.local.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
