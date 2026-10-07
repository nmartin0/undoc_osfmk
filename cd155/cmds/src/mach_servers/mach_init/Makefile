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
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
#
# OSF/1 Release 1.0

#
# Generate the User (and Server) sides of an RPC interface for the service
# server.  This is called by lib/libmach to get the interface into the mach
# library.
#
#  mach_init should be installed as /mach_servers/mach_init
#

VPATH			= ${MAKETOP}usr/include/servers

PROGRAMS		= mach_init

ILIST			= mach_init
IDIR			= /mach_servers/

USE_STATIC_LIBRARIES	=

MIG_DEFS		= service.defs
MIGFLAGS                = ${DEFINES} ${INCFLAGS} -nostdinc

OFILES			= service.o main.o quit.o serviceServer.o

LIBS			= -lcthreads -lmach

include ${MAKETOP}/usr/lib/makefiles/standard.mk
include ${MAKETOP}/usr/lib/makefiles/programs.mk
include ${MAKETOP}/usr/lib/makefiles/objects.mk

serviceServer.o: service.defs

include ${MAKEFILEPATH}/depend.mk
-include Makedep
