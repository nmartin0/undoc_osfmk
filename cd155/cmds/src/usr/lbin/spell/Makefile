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
# "@(#)$RCSfile: Makefile,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:48:05 $"
#

PROGRAMS		= spellprog hashcheck hashmake spellinprg spellout
SCRIPTS			= compress
DATAFILES		= american british extra list local \
			  spellhist stop Mkf.native
MSGHDRS			= spell_msg.h
CATFILES		= spell.cat

ILIST			= ${PROGRAMS} ${SCRIPTS} ${DATAFILES}
IDIR			= /usr/lbin/spell/
BIN_IDIR		= /usr/bin/
spellout_IDIR		= ${BIN_IDIR}

HFILES			= ${MSGHDRS}
spellprog_OFILES	= spellprog.o hashlook.o hash.o huff.o
spellinprg_OFILES	= spellinprg.o huff.o
spellout_OFILES		= spellout.o hashlook.o hash.o huff.o
hashcheck_OFILES	= hashcheck.o hash.o huff.o
hashmake_OFILES		= hashmake.o hash.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/scripts.mk
include ${MAKEFILEPATH}/datafiles.mk

hash.o: hash.h
huff.o: huff.h
spellout.o: spell_msg.h
spellinprg.o: spell_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
