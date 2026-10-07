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

OSF1AD_LL	= ${MAKETOP}/usr/sbin/load_level/src
VPATH		= ${OSF1AD_LL}

DATAFILES		= cron motd paging sendmail uucp enlogin \
			  allocator kloadsrv loader loadlevel mach named \
                          preserve settime xdm inet recpasswd snmpd \
                          inetd nfs nfsmount rmtmpfiles syslog lpd ntpd \
			  initpart savecore smd nqs spv macs mklog

ILIST			= ${DATAFILES}
IDIR			= /sbin/init.d/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
