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

VPATH           = ${SVROBJ}/include/netinet

DATAFILES		= inet_config.h \
			  icmp_var.h if_ether.h in.h in_pcb.h \
			  in_systm.h in_var.h ip.h ip_icmp.h ip_var.h \
			  tcp.h tcp_debug.h tcp_fsm.h tcp_seq.h \
			  tcp_timer.h tcp_var.h tcpip.h \
			  udp.h udp_var.h
INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/netinet/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
