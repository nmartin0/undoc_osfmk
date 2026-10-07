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

VPATH			= ${TARGET_MACHINE}

OTHERS			= magic

ILIST			= magic
IDIR			= /etc/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk

magic: magic.common magic.mips magic.ns32000 magic.i386 magic.i860
	cat magic.common magic.i860 magic.mips magic.ns32000 magic.i386 > magic
include ${MAKEFILEPATH}/depend.mk
-include Makedep
