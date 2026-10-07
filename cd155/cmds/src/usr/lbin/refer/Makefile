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

PROGRAMS		= addbib deliv hunt inv lookbib mkey refer \
			  sortbib whatabout
SCRIPTS			= roffbib indxbib

ILIST			= ${PROGRAMS} ${SCRIPTS}
IDIR			= /usr/bin/
LIB_IDIR		= /usr/lbin/refer/
hunt_IDIR		= ${LIB_IDIR}
inv_IDIR		= ${LIB_IDIR}
mkey_IDIR		= ${LIB_IDIR}

deliv_OFILES		= deliv1.o deliv2.o
hunt_OFILES		= deliv2.o glue4.o glue5.o hunt1.o hunt2.o hunt3.o \
			  hunt5.o hunt6.o hunt7.o hunt8.o hunt9.o refer3.o \
			  shell.o tick.o
inv_OFILES		= inv1.o inv2.o inv3.o inv5.o inv6.o deliv2.o
mkey_OFILES		= mkey1.o mkey2.o mkey3.o deliv2.o
refer_OFILES		= deliv2.o glue1.o glue2.o glue3.o glue4.o \
			  glue5.o hunt2.o hunt3.o hunt5.o hunt6.o hunt7.o \
			  hunt8.o hunt9.o mkey3.o refer0.o refer1.o \
			  refer2.o refer4.o refer5.o refer6.o refer7.o \
			  refer8.o shell.o
whatabout_OFILES	= what1.o what2.o what3.o what4.o shell.o mkey3.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/scripts.mk

glue3.o hunt2.o hunt3.o refer0.o refer2.o refer3.o refer4.o refer5.o \
refer6.o refer7.o refer8.o: refer..c
what1.o what4.o: what..c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
