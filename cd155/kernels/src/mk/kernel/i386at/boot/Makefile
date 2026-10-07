# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# @OSF_FREE_COPYRIGHT@
#
#
# HISTORY
# Makefile,v
# Revision 1.1.4.2  1993/02/12  17:35:18  dleslie
# Adding new code from vendor
#
# Revision 1.1  1992/09/30  02:38:32  robert
# Initial revision
#
# $EndLog$
#
# CMU_HIST 
#
# Revision 2.1.2.1  92/04/30  11:51:54  bernadat
# 	Changes to generate in the obj directory, and use kernel includes
# 	Copied from Main line
# 	[92/03/19            bernadat]
# 
# Revision 2.2  92/04/04  11:33:46  rpd
# 	Fabricated for 3.0
# 	[92/03/30            mg32]
# 
#
# CMU_ENDHIST 
#
# Mach Operating System
# Copyright (c) 1992, 1991 Carnegie Mellon University
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
#
#

CFLAGS	= -O
LIBS=
INC = -I. -I../..

# start.o should be first, table.o should be second
OBJS = start.o table.o boot2.o boot.o asm.o bios.o io.o disk.o sys.o gcc.o

boot: boot.x
	cp boot.x boot.tmp
	-strip boot.tmp
#	Remove a.out header
	dd if=boot.tmp of=boot ibs=32 skip=1 obs=1024b
	@ls -l boot

boot.x: $(OBJS)
	$(LD) -N -T 0 -o boot.x $(OBJS) $(LIBS)


clean:
	/bin/rm -f *.o *.d boot

.SUFFIXES: .s .c .o

.c.o:
	@echo $(CC) -c $(CFLAGS) $(INC) $*.c
	-@trap "/bin/rm -f $*.i $*.s; exit 0" 0 1 2 3 10 15; \
	$(CC) -MD $(CFLAGS) $(INC) -S $<;	\
	if [ $$? != 0 ]; then :; else \
		sed -e "s/^.*data/.text/" -e "/^.*ident/d" $*.s > $*.i;	\
		$(AS) $*.i -o $@; \
	fi

.s.o:
	@echo $(AS) -o $*.o $<
	-@trap "/bin/rm -f $*.i X$*.c; exit 0" 0 1 2 3 10 15;	\
	/bin/rm -f X$*.c;	\
	ln -s $*.s X$*.c;	\
	$(CC) -E $(CFLAGS) X$*.c > $*.i;	\
	if [ $$? != 0 ]; then :;	\
	else \
		$(AS) $*.i -o $@;	\
	fi

gcc.s: 	../../machine/gcc.s
	ln -s ../../machine/gcc.s .

boot.o: boot.c
boot.o: boot.h
boot.o: ../../boot_ufs/file_io.h
boot.o: ../../mach/machine/vm_param.h
boot.o: ../../device/device_types.h
boot.o: ../../mach/std_types.h
boot.o: ../../mach/boolean.h
boot.o: ../../mach/machine/boolean.h
boot.o: ../../mach/kern_return.h
boot.o: ../../mach/machine/kern_return.h
boot.o: ../../mach/port.h
boot.o: ../../mach/machine/vm_types.h
boot.o: ../../boot_ufs/defs.h
boot.o: ../../sys/types.h
boot.o: ../../device/param.h
boot.o: ../../boot_ufs/fs.h
boot.o: ../../boot_ufs/disk_inode.h
boot.o: ../../i386/exec.h
boot.o: ../../sys/reboot.h
disk.o: disk.c
disk.o: boot.h
disk.o: ../../boot_ufs/file_io.h
disk.o: ../../mach/machine/vm_param.h
disk.o: ../../device/device_types.h
disk.o: ../../mach/std_types.h
disk.o: ../../mach/boolean.h
disk.o: ../../mach/machine/boolean.h
disk.o: ../../mach/kern_return.h
disk.o: ../../mach/machine/kern_return.h
disk.o: ../../mach/port.h
disk.o: ../../mach/machine/vm_types.h
disk.o: ../../boot_ufs/defs.h
disk.o: ../../sys/types.h
disk.o: ../../device/param.h
disk.o: ../../boot_ufs/fs.h
disk.o: ../../boot_ufs/disk_inode.h
disk.o: ../../i386at/disk.h
io.o: io.c
io.o: ../../i386/pio.h
sys.o: sys.c
sys.o: boot.h
sys.o: ../../boot_ufs/file_io.h
sys.o: ../../mach/machine/vm_param.h
sys.o: ../../device/device_types.h
sys.o: ../../mach/std_types.h
sys.o: ../../mach/boolean.h
sys.o: ../../mach/machine/boolean.h
sys.o: ../../mach/kern_return.h
sys.o: ../../mach/machine/kern_return.h
sys.o: ../../mach/port.h
sys.o: ../../mach/machine/vm_types.h
sys.o: ../../boot_ufs/defs.h
sys.o: ../../sys/types.h
sys.o: ../../device/param.h
sys.o: ../../boot_ufs/fs.h
sys.o: ../../boot_ufs/disk_inode.h
sys.o: ../../boot_ufs/dir.h
sys.o: ../../sys/reboot.h
table.o: table.c

