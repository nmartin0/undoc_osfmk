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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2

PROGRAMS		= dprog style1 style2 style3
DATAFILES		= dict.d explain.d
MSGHDRS			= dprog_msg.h nwords_msg.h
CATFILES		= dprog.cat nwords.cat

ILIST			= ${PROGRAMS} ${DATAFILES}
IDIR			= /usr/lbin/

dict.d_IDIR		= /usr/share/lib/
explain.d_IDIR		= /usr/share/lib/
dprog.o_CFLAGS		= -DDICT=\"/usr/share/lib/dict.d\"
style1_OFILES		= nwords.o
style1_LIBS		= -ll
style2_OFILES		= end.o
style2_LIBS		= -ll
style3_OFILES		= part.o pscan.o outp.o
style3_LIBS		= -ll
GARBAGE			= end.c nwords.c part.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

dprog.o:	dprog_msg.h
end.o:		end.c names.h ehash.c edict.c
nwords.o:	nwords.c names.h nhash.c dict.c ydict.c abbrev.c nwords_msg.h
outp.o:		style.h names.h conp.h
part.o:		part.c style.h names.h conp.h
pscan.o:	names.h conp.h
