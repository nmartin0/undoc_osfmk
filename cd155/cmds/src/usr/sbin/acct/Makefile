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
# @(#) $RCSfile: Makefile,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:02:58 $
#

SUBDIRS			= acctlib
SEC_PROGRAMS		= accton
PROGRAMS		= acctcms acctcom acctcon1 acctcon2 acctdisk \
			  acctdusg acctmerg accton acctprc1 acctprc2 \
			  acctwtmp diskusg fwtmp printpw wtmpfix
SCRIPTS			= chargefee ckpacct dodisk lastlogin monacct nulladm \
			  prctmp prdaily prtacct remove runacct shutacct \
			  startup turnacct
MSGHDRS			= acct_msg.h
CATFILES		= acct.cat
DATAFILES		= holidays ptecms.awk ptelus.awk

SEC_ILIST		= ${SEC_PROGRAMS}
ILIST			= ${PROGRAMS} ${SCRIPTS} ${DATAFILES}
IDIR			= /usr/sbin/acct/
acctcom_IDIR		= /usr/bin/
accton_IOWNER		= root
accton_IMODE		= 4750

accton.o_CFLAGS         = ${SEC_DEFS}
accton_LIBS             = ${SEC_LIBS}

HFILES			= ${MSGHDRS} acctdef.h ctmp.h ptmp.h table.h tacct.h

acctcms_LIBS		= acctlib/acctlib.a
acctcom_LIBS		= acctlib/acctlib.a
acctcon1_LIBS		= acctlib/acctlib.a
acctmerg_LIBS		= acctlib/acctlib.a
acctprc1_LIBS		= acctlib/acctlib.a
acctprc2_LIBS		= acctlib/acctlib.a

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/scripts.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
