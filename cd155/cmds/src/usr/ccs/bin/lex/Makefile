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

PROGRAMS		= lex
MSGHDRS			= lex_msg.h
CATFILES		= lex.cat

ILIST			= lex
IDIR			= /usr/ccs/bin/

OFILES			= main.o parser.o sub1.o sub2.o header.o
HFILES			= ldefs.h lex_msg.h

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

main.o: once.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
