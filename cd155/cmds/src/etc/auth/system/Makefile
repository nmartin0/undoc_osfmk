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
# Makefile for the etc/auth/system directory
#

# @(#)Makefile	3.1 09:52:49 6/7/90 SecureWare

FILES		= authorize default devassign files lps subsystems ttys
OTHERS		= ${SEC_DEFS?${FILES}:}
SEC_OTHERS	= ${FILES}


SEC_ILIST	= ${FILES}
ILIST		= ${OTHERS}
IDIR		= /etc/auth/system/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk

${OTHERS}: $${@:=.${SEC_LEVEL}}
	${RM} ${_RMFLAGS_} $@
	${CP} ${@:=.${SEC_LEVEL}} $@

include ${MAKEFILEPATH}/depend.mk
-include Makedep
