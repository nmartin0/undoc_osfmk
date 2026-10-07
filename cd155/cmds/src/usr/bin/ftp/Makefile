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

VPATH			= ${MAKETOP}/usr/sbin/ftpd

PROGRAMS		= ftp
MSGHDRS			= ftp_msg.h
CATFILES		= ftp.cat

ILIST			= ftp
IDIR			= /usr/bin/

CCTYPE			= writable_strings
CFLAGS			= -D_BSD
HFILES			= ftp_var.h pathnames.h ftp_msg.h
OFILES			= cmds.o cmdtab.o ftp.o  glob.o main.o \
			  ruserpass.o domacro.o
LIBS			= -lbsd

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

cmds.o: pathnames.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
