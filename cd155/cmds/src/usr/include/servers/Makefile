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
# Mach 3.0 conversion
#
# env_mgr.h is MiG generated during the population of usr/include/servers

MIG_DEFS                = machid.defs machid_debug.defs env_mgr.defs \
                          netmemory.defs netname.defs service.defs

DATAFILES               = machid_types.defs machid_types.h netname_defs.h \
                          machid.h netname.h machid_debug.h service.h \
                          netmemory.h emdefs.h env_mgr.h \
                          ${MIG_DEFS}

INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/servers/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/objects.mk

#${MIG_HDRS}: $${@:.h=.defs}

include ${MAKEFILEPATH}/depend.mk
-include Makedep
