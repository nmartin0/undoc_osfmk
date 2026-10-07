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
#             Loader Cross Process Interfaces (-lxproc)
#
# This is an experimental implementation of the loader cross process
# interfaces that work on an arbitrary process, that is on a process
# other than ldr_my_process() and ldr_kernel_process().  To get a
# an appropriate ldr_process_t simply obtain a pid_t for the desired
# target process and cast that value to a ldr_process_t.
# Because changes need to be made to ldr_xcalls.o found in -lc
# to support this new functionality, -lxproc contains its own version
# of ldr_xcalls.o with the necessary changes.  Always link programs that
# link with -lxproc, with -lld and -lloader.  nlist() needs -lld and the
# cross process interfaces call the loader to do some of their work.
# If -lxproc is built with the USE_VM_CALLS set, also link programs
# that link with -lxproc, with -lmach.
#
# Configuration Options
#
#	-DUSE_VM_CALLS
#		Causes the cross process interfaces to use the Mach VM
#		calls, such as vm_read(), to read from the target process's
#		address space.  Note that a task_t for the target process
#		is obtained by task_by_unix_pid() and its access
#		restrictions are applicable.  Note also that when
#		USE_VM_CALLS is set, programs that link with libxproc.a
#		must also link with -lmach.
#
#	-DUSE_PTRACE
#		Causes the cross process interfaces to use the ptrace()
#		system call to read from the target process's address
#		space.  Note that with ptrace(), the target process must
#		be a child and must be traced (that is, it has called
#		ptrace(0)).
#
#	-DUSE_MACHO_NLIST
#		Causes the cross process interfaces to call macho_nlist(),
#		rather than nlist(), to find the addresses of variables
#		in the loader.
#
#	-DPREFIX_UNDERSCORE
#		Causes the cross process interfaces to prefix and
#		underscore to the names of symbols it looks up in
#		the loader.  Note that the PREFIX_UNDERSCORE option
#		is typically machine dependent and as such, is typically
#		set in ${${TARGET_MACHINE}CFLAGS} in the machine
#		dependent makefile ${TARGET_MACHINE}/machdep.mk.
#		For example, the PREFIX_UNDERSCORE option is set when
#		building for the Multimax.

LIBS		=  -lloader -lld -lmach

INCFLAGS	= -I${MAKETOP}sbin/loader/include \
		  -I${MAKETOP}sbin/loader/include/${TARGET_MACHINE} \
		  -I${MAKETOP}sbin/loader/ldr_include

LIBRARIES	= libxproc.a

EXPLIB_TARGETS	= export_libxproc.a

ILIST		= libxproc.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

OFILES		= ldr_xcalls.o ldr_xproc.o macho_nlist.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
-include ${TARGET_MACHINE}/machdep.mk

CFLAGS		= -DUSE_VM_CALLS -DUSE_MACHO_NLIST \
		  ${${TARGET_MACHINE}CFLAGS}
include ${MAKEFILEPATH}/depend.mk
-include Makedep
