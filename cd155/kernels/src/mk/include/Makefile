# 
# $Copyright
# Copyright 1991, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# 
# Mach Operating System
# Copyright (c) 1992,1991,1990,1989 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon 
# the rights to redistribute these changes.
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.13  1995/03/14  23:39:22  jerrie
#  Reviewer:	   Jerrie Coffman, Vineet Kumar, Richard Griffiths
#  Risk:		   High
#  Benefit or PTS #: Add SCSI-16 daughter board support.
#  Testing:	   Ran PFS, RAID, fileio, and tape EATs.
#  Module(s):	   Too numerous to mention.  See Jerrie for details.
#
# Revision 1.12  1994/11/18  20:24:07  mtm
# Copyright additions/changes
#
# Revision 1.11  1994/07/20  16:53:41  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: copy ipi_status.h and hippi_status.h to the release
# 	tree, from where the commands/libs build can find them
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.10  1994/04/29  21:08:16  dleslie
#  Reviewer: Michael Goldsmith
#  Risk: low
#  Benefit or PTS #: allow kernel's disk_status.h to be visible for
# 	commands/libs builds.
#  Testing: it will work
#  Module(s): Makefile
#
# Revision 1.9  1993/11/18  17:29:54  dleslie
#  Reviewer: shala
#  Risk: low
#  Benefit or PTS #: copy headers to release tree so cmds/libs build can
# 	find them
#  Testing: built on 486 and Sun
#  Module(s): Makefile
#
# Revision 1.7  1993/09/22  18:45:30  stans
#    Export norma/node_status.h to the release area.
#
# Revision 1.6  1993/06/30  22:15:30  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.5  1993/06/08  21:34:04  regnier
# Generate include/i860paragon/mcmsg release area.
#
# Revision 1.4  1993/04/27  20:13:00  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.2.6.2  1993/04/22  18:12:31  dleslie
# First R1_0 release
#
#
# ***
#	Re-added profilparam.h because server still expects it.
#	1/21/93	Greg Regnier
#
# Revision 2.24.3.6  92/09/15  17:13:30  jeffreyh
# 	Replaced mach/profilparam.h with mach/prof_types.h
# 	[92/07/24            bernadat]
# 
# 	Added mach/prof.defs for profiling
# 	[92/07/24            bernadat]
# 
# Revision 2.24.3.5  92/04/30  11:44:38  bernadat
# 	Added missing mach_debug/page_info.h
# 	[92/04/27            bernadat]
# 
# 	Add mach/profilparam.h.
# 	[92/04/07            bernadat]
# 
# Revision 2.24.3.3  92/03/28  10:01:59  jeffreyh
# 	Pick up MK71 version of file and make changes to match NORMA
# 	kernel source.
# 	[92/03/16            jeffreyh]
# 
# Revision 2.33  92/03/05  22:45:49  rpd
# 	Added mach/bootstrap.defs, mach/bootstrap.h.
# 	[92/03/02            rpd]
# 
# 	Changed for new Makefile-include organization.
# 	[92/02/28            rpd]
# 
# Revision 2.32  92/02/19  15:06:05  elf
# 	Changed source location of */exec.h files, from bootstrap to kernel.
# 	Added {mac2,i860,luna88k}/exec.h.
# 	[92/02/19            rpd]
# 	Added Makefile-mkdirs and Makefile-mklinks.
# 	Added sys/varargs.h.
# 	[92/02/10            rpd]
# 
# Revision 2.31  92/02/18  22:15:50  elf
# 	Fixed type in a level 3 rule.
# 	[92/02/18            danner]
# 
# Revision 2.30  92/02/18  17:59:24  elf
# 	Added export of motorola/m88k/m88100/thread_status.h
# 	[92/01/22  18:18:25  danner]
# 
# Revision 2.29  92/01/28  14:31:11  rpd
# 	Added mach/norma_special_ports.h, mach/norma_task.defs,
# 	and mach/vm_param.h.
# 	[92/01/28            rpd]
# 
# Revision 2.28  92/01/24  00:49:28  rpd
# 	Fixed MAKEPSD uses, to fix shadowing.
# 	Removed VPATH.
# 	[92/01/23            rpd]
# 
# Revision 2.27  92/01/23  15:19:50  rpd
# 	Revised for new Makefile organization.
# 	[92/01/16            rpd]
# 

# This makefile installs machine-independent Mach include files.

include ${MAKETOP}Makefile-common
# more definitions, for recursive makes
include ${MAKETOP}Makefile-subdirs
# more definitions, for mkodirs/mkidirs/mkrdirs
include ${MAKETOP}Makefile-mkdirs
# more definitions, for mkilinks/mkrlinks
include ${MAKETOP}Makefile-mklinks

# Files which live in ${MAKETOP}kernel

KERNEL_1 = device/device.defs device/device_reply.defs \
	device/device_request.defs device/device_types.defs \
	device/device_types.h device/net_status.h device/tape_status.h \
	device/disk_status.h \
	device/tty_status.h norma/node_status.h mach/boolean.h \
	scsi/scsi_status.h \
	ipi/ipi_status.h \
	mach/error.h mach/exc.defs mach/exception.h mach/host_info.h \
	mach/kern_return.h mach/mach.defs mach/mach_host.defs \
	mach/mach_norma.defs mach/mach_param.h mach/mach_port.defs \
	mach/mach_traps.h mach/mach_types.defs mach/mach_types.h \
	mach/machine.h mach/memory_object.defs mach/memory_object.h \
	mach/memory_object_default.defs mach/message.h \
	mach/mig_errors.h mach/msg_type.h mach/notify.defs \
	mach/notify.h mach/policy.h mach/port.h mach/processor_info.h \
	mach/std_types.defs mach/std_types.h mach/syscall_sw.h \
	mach/task_info.h mach/task_special_ports.h mach/thread_info.h \
	mach/thread_special_ports.h mach/thread_status.h \
	mach/thread_switch.h mach/time_value.h mach/vm_attributes.h \
	mach/vm_inherit.h mach/vm_prot.h mach/vm_statistics.h \
	mach/norma_special_ports.h mach/norma_task.defs \
	mach_debug/hash_info.h mach_debug/ipc_info.h \
	mach_debug/mach_debug.defs mach_debug/mach_debug_types.defs \
	mach_debug/mach_debug_types.h mach_debug/vm_info.h \
	mach_debug/zone_info.h mach_debug/page_info.h sys/varargs.h sys/types.h \
	mach/prof.defs mach/prof_types.h mach/profilparam.h \
	mach/default_pager_object.defs #No longer in MK68+ kernels

#
#	Add mcmsg_nx.h and mcmsg_xmsg.h for norma13
#		Greg Regnier 10/16/92
#	Add i860paragon/mcmsg directories
#		Greg Regnier 6/7/93
#
KERNEL_2 = i860ipsc/mcmsg/mcmsg_info.defs i860ipsc/mcmsg/mcmsg_appl.h \
	i860ipsc/mcmsg/mcmsg_nx.h i860ipsc/mcmsg/mcmsg_xmsg.h \
	i860paragon/mcmsg/mcmsg_info.defs i860paragon/mcmsg/mcmsg_appl.h \
	i860paragon/mcmsg/mcmsg_nx.h i860paragon/mcmsg/mcmsg_xmsg.h \
	i860paragon/mcmsg/mcmsg_intf.h \
	i860paragon/hippi/hippi_status.h

# The files below do not yet exist in the NORMA system
# mach/boot_info.h \

# Files which live in ${MAKETOP}bootstrap

BOOTSTRAP_1 =
# mach/default_pager.defs mach/default_pager_types.defs \
#	mach/default_pager_types.h mach/bootstrap.defs

# Files which live in ${MAKETOP}user/libmach

LIBMACH_0 = errorlib.h mach.h mach_error.h mach_init.h setjmp.h strings.h

# Files which live in ${MAKETOP}user/threads

THREADS_0 = cthreads.h

# 'Dummy' files which we create directly in the installation directory

STRANGE_0 = varargs.h mig_errors.h msg_type.h

STRANGE_1 = mach/mach.h mach/vm_param.h

# Files which we create in the object directory

OBJECT_1 = device/device.h device/device_reply.h \
	device/device_request.h mach/exc.h mach/mach_host.h \
	mach/mach_port.h mach/mach_interface.h \
	mach/memory_object_default.h mach/memory_object_user.h \
	mach/notify_user.h mach/default_pager_object.h mach_debug/mach_debug.h 

# Not in NORMA yet
#mach/default_pager.h 	mach/bootstrap.h

# Directories which must exist in the install/release include hierarchies

# Added in i860ipsc and i860ipsc/mcmsg.  These had been dropped between
# norma12 and norma13.
#	Dave Leslie 10/15/92
# Added i860paragon/mcmsg
#   Greg Regnier 6/7/93
IDIRS = mach mach_debug sys device norma \
		i860paragon i860paragon/mcmsg  i860ipsc i860ipsc/mcmsg \
		ipi scsi i860paragon/hippi

# top-level targets
# We recurse into subdirectories for machine-dependent files.

ARCHS = mips i386 vax sun3 i860 luna88k mac2 i860paragon

RECURSE = ${RECURSE_TEMPLATE/RECURSE_DIRS/${ARCHS}}

preinstall ::
	+${RECURSE/RECURSE_TARGET/preinstall}

install ::
	+${RECURSE/RECURSE_TARGET/install}

release ::
	+${RECURSE/RECURSE_TARGET/release}

clean ::
	+${RECURSE/RECURSE_TARGET/clean}

# installation rules for dummy files

${INSTALLDIR}/include/mig_errors.h :
	${RM} $@
	echo '#include <mach/mig_errors.h>' > $@
	${CHMOD_INC} $@

${INSTALLDIR}/include/msg_type.h :
	${RM} $@
	echo '#include <mach/msg_type.h>' > $@
	${CHMOD_INC} $@

${INSTALLDIR}/include/varargs.h :
	${RM} $@
	echo '#include <sys/varargs.h>' > $@
	${CHMOD_INC} $@

${INSTALLDIR}/include/mach/mach.h :
	${RM} $@
	echo '#include <mach.h>' > $@
	${CHMOD_INC} $@

${INSTALLDIR}/include/mach/vm_param.h :
	${RM} $@
	echo '; DO NOT INCLUDE THIS FILE ;' > $@
	${CHMOD_INC} $@

# rules to create files in the object directory:
# These rules require that subdirectories exist in the object directory;
# the mkodirs target will create them if necessary.

MKODIRS = mach mach_debug device

.INIT :: mkodirs

# mig rules for normal kernel files

MIG_FILES = device/device.h device/device_reply.h \
	device/device_request.h mach/exc.h \
	mach/mach_host.h mach/mach_port.h mach/memory_object_default.h \
	mach_debug/mach_debug.h mach/default_pager_object.h

${MIG_FILES} : $${@;(.*)\.h;${MAKETOP}kernel/\1.defs}
	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${@;(.*)\.h;${MAKETOP}kernel/\1.defs}

# mig rules for files that don't fit the pattern

mach/mach_interface.h : ${MAKETOP}kernel/mach/mach.defs
	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${MAKETOP}kernel/mach/mach.defs

mach/memory_object_user.h : ${MAKETOP}kernel/mach/memory_object.defs
	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${MAKETOP}kernel/mach/memory_object.defs

mach/notify_user.h : ${MAKETOP}kernel/mach/notify.defs
	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${MAKETOP}kernel/mach/notify.defs

# Not in NORMA yet
#mach/default_pager.h : ${MAKETOP}bootstrap/mach/default_pager.defs
#	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${MAKETOP}bootstrap/mach/default_pager.defs

mach/bootstrap.h : ${MAKETOP}bootstrap/mach/bootstrap.defs
	${MIG} ${MIGFLAGS} -header $@ -user /dev/null -server /dev/null ${MAKETOP}bootstrap/mach/bootstrap.defs

# generic rules for installing/releasing include files

include Makefile-include

# We also make 'machine' links in the install and release directories.
# We do this last so that the linked-to directories have already
# been created.

MKILINKS = include include/mach
MKRLINKS = ${MKILINKS}

preinstall :: mkilinks

install :: mkilinks

release :: mkrlinks

-include Makedep
