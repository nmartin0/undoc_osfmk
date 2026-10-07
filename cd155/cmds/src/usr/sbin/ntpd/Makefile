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

PROGRAMS		= ntpd ntpdc
MSGHDRS                 = ntpd_msg.h
CATFILES                = ntpd.cat

ILIST			= ntpd ntpdc
IDIR			= /usr/sbin/

CFLAGS                  = -DSETTICKADJ -DDEBUG -DSUN_FLT_BUG

HFILES			= ntp.h pathnames.h patchlevel.h

ntpd_OFILES 		= ntpd.o ntpsubs.o ntp_proto.o ntp_sock.o \
                         ntp_adjust.o read_local.o read_psti.o read_heath.o

ntpdc_OFILES		= ntpdc.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
