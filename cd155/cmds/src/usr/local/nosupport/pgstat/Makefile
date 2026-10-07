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
# OSF/1 Release 1.0

IDIR 			= /usr/bin/

ILIST			= pgstat

PROGRAMS		= pgstat

OFILES			= pgstat.o

INCFLAGS		= -I${TARGET_MACHINE}

CFLAGS			= -MD -DOSF1AD -DOSF1_ADFS


include ${MAKETOP}/usr/lib/makefiles/standard.mk
include ${MAKETOP}/usr/lib/makefiles/programs.mk
include ${MAKETOP}/usr/lib/makefiles/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
