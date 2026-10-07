# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# @OSF_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.5  1994/11/18  20:33:20  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/07/14  18:06:36  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  19:26:34  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:19:03  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:32:19  cfj
# Initial 1.0.3 code drop
#
# Revision 1.2  1992/11/30  22:25:59  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:24:41  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.5  1992/07/29  08:30:57  rabii
# 	Add stdarg.h to the machine dependent file list
#
# Revision 2.4  92/03/09  12:12:09  durriya
# 	[Revision 3.1  91/12/24  12:38:02  sp]
# 	eflags.h is now defunct
# 
# Revision 2.3  91/12/19  12:30:57  roy
# 	Add i860 includes. (rabii)
# 
# Revision 2.2  91/12/16  12:53:22  roy
# 	Initial check-in.
# 
# Revision 3.0  91/11/15  15:31:09  condict
# Copy the correct machine-dependent header files from server/${target_cpu}
# src dir to server/machine object dir, allowing cross-compilation on non-AFS
# systems without any "machine" symbolic links in the src tree.
# 
# 
# $EndLog$

PMAX_INCFILES		= replace.h this.h with.h the.h real.h list.h
I386_INCFILES		= addrconf.h asm.h cpu.h endian.h fpreg.h \
			  inline_lock.h machlimits.h machparam.h machtime.h \
			  pcb.h psl.h reg.h second_SYS.h vmparam.h stdarg.h
I860_INCFILES		= addrconf.h asm.h cpu.h endian.h inline_lock.h \
			  machlimits.h machparam.h machtime.h pcb.h \
			  second_SYS.h va_list.h varargs.h varargs.pgi.h \
			  vmparam.h stdarg.h

# In the OSF/1 build, all of these would be defined externally:
RM=/bin/rm
_RMFLAGS_= -f

all:	${${TARGET_CPU}_INCFILES}

${${TARGET_CPU}_INCFILES}: ../${target_cpu}/$${@}
	-if [ -f $@ ] && cmp -s ../${target_cpu}/$@ $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} ../${target_cpu}/$@ $@; fi
