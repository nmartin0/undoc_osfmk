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

#
#            Loader Cross Process Interface Test Programs
#
# This makefile builds some utility programs that are generally useful
# and help in testing the cross process interfaces.  The programs are
# ldr_region and dbg.  They both call the cmd_info() function as
# stolen from the loader test program tload2.  For example, the following
# output was produced by running ldr_region.
#
# % ldr_region $$
# 2	"/sbin/loader"	(Nregion 0 Flags 0x00000021):
# 3	"/usr/bin/csh"	(Nregion 4 Flags 0x00000080):
#	0	text
#		VA 0x00400000	MA 0x00400000	SZ   155648	Prot 0x005
#	1	rdata
#		VA 0x00426000	MA 0x00426000	SZ     8192	Prot 0x001
#	2	data
#		VA 0x10000000	MA 0x10000000	SZ     8192	Prot 0x003
#	3	bss
#		VA 0x10002000	MA 0x10002000	SZ    20480	Prot 0x007
# 4	"/usr/shlib/libc.so"	(Nregion 4 Flags 0x00000000):
#	0	text
#		VA 0x73000000	MA 0x73000000	SZ   299008	Prot 0x005
#	1	rdata
#		VA 0x73049000	MA 0x73049000	SZ    20480	Prot 0x001
#	2	data
#		VA 0x7304e000	MA 0x7304e000	SZ    49152	Prot 0x003
#	3	bss
#		VA 0x7305a000	MA 0x7305a000	SZ   131072	Prot 0x007
# %
#
# The dbg test program produces the same output,but won't work with an
# arbitrary process.  The dbg test programs accepts a program name, forks
# a child to run the program, but has the child enable tracing by calling
# ptrace(0), before exec'ing the program.  Thus the dbg program is ideal
# for testing the cross process interfaces # when they use ptrace().

PROGRAMS		= ldr_region dbg

LIBS			= -lloader -lld -lmach

ldr_region_OFILES	= ldr_region.o info.o
ldr_region_LIBS		= -lld -lxproc -lmach -lloader

dbg_OFILES		= dbg.o info.o
dbg_LIBS		= -lld -lxproc -lmach -lloader

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
