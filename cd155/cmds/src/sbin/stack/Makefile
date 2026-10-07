# $Copyright
# Copyright 1992, 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
#
#      $Header:
#
#


PROGRAMS		= stack
MSGHDRS			= 
CATFILES		= 

ILIST			= stack
IDIR			= /sbin/
IOWNER                  = root
IMODE                   = 4755

HFILES			= bootmagic.h

INCFLAGS                = 

CFLAGS                	= -g

LIBS                    = -lnx -lmach -lnetname 

OFILES                  = main.o magic.o kernops.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
