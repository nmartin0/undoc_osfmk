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
#
# @(#)$RCSfile: Makefile,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:26:16 $

SCRIPTS			= install installbsd

CATFILES		= install.cat installbsd.cat 
MSGHDRS			= install_msg.h installbsd_msg.h

ILIST			= install installbsd
IDIR			= /usr/bin/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/scripts.mk

#
# a little hack to trigger the rules to build the cat file.
#
install: install_msg.h
installbsd: installbsd_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
