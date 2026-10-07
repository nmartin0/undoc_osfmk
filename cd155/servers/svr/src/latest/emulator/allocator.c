/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: allocator.c,v $
 * Revision 1.7  1994/11/18  20:22:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  17:30:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:21:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:14:25  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:16:58  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:17:02  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:56:29  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:51:02  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:08:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:24  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.8  92/11/03  10:38:17  loverso
 * 	No longer use "htg_vm_map" with special reply port; just use
 * 	vm_map directly.  Have vm_allocate and emul_vm_map panic
 * 	if vm_map failed (i.e., they will return EMULATOR_BASE).
 * 	(loverso)
 * 
 * Revision 2.7  92/05/12  10:52:19  loverso
 * 	Fix panic check again in emul_vm_map. (roy)
 * 
 * Revision 2.6  92/05/01  09:37:46  rabii
 * 	Use end of allocated range in panic check.
 * 	[92/04/30            roy]
 * 
 * Revision 2.5  92/03/20  11:53:25  pjg
 * 	Panic if cannot allocate memory in the emulator space.
 * 
 * Revision 2.4  91/10/14  12:54:06  srl
 * (repair header comments)
 * 91/09/13  15:52:28  sp
 * rename vm_map hack to emul_vm_map so that we can compile with __STDC__
 * 
 * Revision 2.3  91/10/09  22:39:59  srl
 * changed name of vm_map routine to emul_vm_map,  for V2.0 -> V2.2. merge
 * 
 * Revision 2.2  91/08/30  16:39:46  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:02  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  90/06/02  15:20:11  rpd
 * 	Removed vm_allocate_with_pager.
 * 	Added reply_port argument to vm_map.
 * 	[90/04/08            rpd]
 * 
 * Revision 2.3  90/05/29  20:22:22  rwd
 * 	if anywhere in vm_allocate set addr to EMULATOR_BASE
 * 	[90/05/29            rwd]
 * 
 * Revision 2.2  89/11/29  15:26:01  af
 * 	RCS-ed.
 * 	[89/11/29            af]
 * 
 */
/*
 * Replacement for vm_allocate, to keep memory within emulator address space.
 */

#include <mach/mach.h>
#include <machine/vmparam.h>

#ifdef	mips
#define VM_PROT_DEFAULT (VM_PROT_READ|VM_PROT_WRITE)
#endif	mips

kern_return_t
vm_allocate(
	task_t		task,
	vm_offset_t	*addr,
	vm_size_t	size,
	boolean_t	anywhere)
{
	kern_return_t error;

	if (anywhere)
		*addr = EMULATOR_BASE;

	error = vm_map(task, addr, size, (vm_offset_t)0, anywhere,
			MEMORY_OBJECT_NULL, (vm_offset_t)0, FALSE,
			VM_PROT_DEFAULT, VM_PROT_ALL, VM_INHERIT_COPY);

	if (anywhere && (*addr + size > EMULATOR_END))
		emul_panic("vm_allocate: cannot allocate memory in the emulator region");
	if (error && anywhere)
		emul_panic("vm_allocate: vm_map failed in emulator");

	return (error);
}

kern_return_t
emul_vm_map(
	vm_task_t	task,
	vm_address_t	*address,
	vm_size_t	size,
	vm_address_t	mask,
	boolean_t	anywhere,
	memory_object_t	memory_object,
	vm_offset_t	offset,
	boolean_t	copy,
	vm_prot_t	cur_protection,
	vm_prot_t	max_protection,
	vm_inherit_t	inheritance)
{
	kern_return_t 	error;
	boolean_t     	in_emul;

	if (anywhere && *address < EMULATOR_BASE) {
		*address = EMULATOR_BASE;
		in_emul = TRUE;
        } else
		in_emul = FALSE;

	error = vm_map(task, address, size, mask, anywhere,
			memory_object, offset, copy,
			cur_protection, max_protection, inheritance);

	if (in_emul && (*address + size > EMULATOR_END))
		emul_panic("emul_vm_map: cannot allocate memory in the emulator region");
	if (error && anywhere)
		emul_panic("emul_vm_map: vm_map failed in emulator");
	return(error);
}
