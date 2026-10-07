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

PROGRAMS		= pgs

ILIST			= pgs

IDIR			= /usr/mach/bin/

USE_STATIC_LIBRARIES	=

INCFLAGS                = -I.
CFLAGS			= -DOSF1AD=1 -O

LIBS			= -lmach

OFILES			= pgs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
