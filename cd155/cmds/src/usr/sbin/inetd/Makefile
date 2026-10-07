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

PROGRAMS		= inetd
MSGHDRS			= inetd_msg.h
CATFILES		= inetd.cat

LLHDRS			= load_level_com.h

ILIST			= inetd
IDIR			= /usr/sbin/

CCTYPE			= writable_strings

#CFLAGS			= -D_BSD

#
# DO_LOAD_LEVEL flag causes inetd to distribute services across nodes of
# the service partition.
#
# USE_REXEC flag causes inetd to use the TNC rexec feature for distribution.
#
# USE_RFORK flag causes inetd to use the TNC rfork feature for distribution.
#
# SKIP_ROOT_FS_NODE causes inetd to skip ROOT_FS_NODE.
#
# USE_FAST_NODE flag causes inetd to use the load information from the load
# leveling daemon to choose the destination node.
#
CFLAGS			= -DTNC -DDO_LOAD_LEVEL -DUSE_RFORK -DOSF1AD -D_BSD -DSKIP_ROOT_FS_NODE -DUSE_FAST_NODE

HFILES			= ${MSGHDRS} ${LLHDRS}

#
# Need one more include directory.
#
INCFLAGS		= 


#LIBS			= -lbsd -lutil

#
# Need to link to libnx if USE_REXEC flag is defined
#
LIBS			= -lnx -lmach  -lbsd -lutil

#
# Need some more objets
#
OFILES                  = inetd.o next_node.o fast_node.o getbootenv.o

#
# Path to the LL include file in the server
#
SERVERLLSRC		= ../load_level/src

#
# How to get the LL include file
#
load_level_com.h: $(SERVERLLSRC)/load_level_com.h
	rm -f $@
	cp $? $@

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
