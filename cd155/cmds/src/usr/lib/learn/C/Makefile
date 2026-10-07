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

DATAFILES		= L0 L0.1a L1.1a L1.1b L1.1c L1.1d L1.1e L1.1f \
			  L10 L11.1a L11.2a L12.1a L12.1b L13.1a L14.1a \
			  L14.2a L14.2b L15.1a L15.1b L16.2a L16.2b \
			  L16.2c L17.1a L17.1c L18.1a L19.1a L2.1a \
			  L2.1b L2.1c L2.1d L2.1e L20.1a L3.1a L3.1b \
			  L30.1a L31.1a L32.1a L33.1a L35.1a L37.1a \
			  L4.1a L40.1a L41.1a L42.1a L43.1a L43.1b \
			  L5.1a L5.1b L5.1c L5.1d L5.1e L5.1f L5.1g \
			  L5.2a L5.2b L5.2e L5.3e L50.1a L9.1a \
			  getline.c getnum.c
OBJECTS			= getline.o getnum.o

ILIST			= ${DATAFILES} ${OBJECTS}
IDIR			= /usr/lib/learn/C/
IMODE			= 644
CFLAGS			= -D__i860__

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
