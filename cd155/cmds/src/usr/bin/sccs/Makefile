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

SUBDIRS			= lib
PROGRAMS		= admin cdc comb delta get prs sccs sccshelp \
			  rmdel sact unget val what
SCRIPTS			= sccsdiff
MSGHDRS 		= cm_msg.h co_msg.h sccshelp_msg.h admin_msg.h \
			  comb_msg.h delta_msg.h get_msg.h prs_msg.h \
			  rmchg_msg.h unget_msg.h val_msg.h bsd_msg.h
CATFILES		= cm.cat co.cat sccshelp.cat admin.cat comb.cat \
			  delta.cat get.cat prs.cat rmchg.cat unget.cat \
			  val.cat bsd.cat sccsdiff.cat

ILIST			= admin cdc comb delta get prs sccs sccshelp \
			  rmdel sact unget val what sccsdiff
IDIR			= /usr/bin/

rmdel_OFILES		= rmchg.o
cdc_OFILES		= rmchg.o
sact_OFILES		= unget.o
LIBFLAGS		= -Llib
LIBS                    = -lsccs -lPW

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/scripts.mk

admin.o: defines.h
admin.o: had.h
admin.o: admin_msg.h
auxf.o: defines.h
comb.o: defines.h
comb.o: had.h
comb.o: comb_msg.h
defines.h: co_msg.h
defines.h: cm_msg.h
delta.o: defines.h
delta.o: had.h
delta.o: delta_msg.h
dohist.o: delta_msg.h
get.o: defines.h
get.o: had.h
get.o: get_msg.h
prs.o: defines.h
prs.o: had.h
prs.o: prs_msg.h
rmchg.o: defines.h
rmchg.o: had.h
rmchg.o: rmchg_msg.h
sccs.o: defines.h
sccs.o: bsd_msg.h
sccshelp.o: defines.h
sccshelp.o: sccshelp_msg.h
sccsdiff: sccsdiff.cat
unget.o: defines.h
unget.o: had.h
unget.o: unget_msg.h
val.o: defines.h
val.o: had.h
val.o: val_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
