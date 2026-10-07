/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: exec.c,v $
 * Revision 2.9  1994/11/18  20:38:52  mtm
 * Copyright additions/changes
 *
 * Revision 2.8  1993/06/30  22:30:35  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.7  1993/04/27  20:28:27  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 2.6.6.2  1993/04/22  18:28:46  dleslie
 * First R1_0 release
 *
 * Revision 2.7  1993/03/12  22:51:33  stans
 * changed strcmp() calls with 3 args to strncmp() which what was intended.
 * The gcc compiler caught this while PGC did not....
 *
 * Revision 2.6  1992/10/12  22:56:36  andyp
 * Added new support routines for acessing a pre-stage copy of an
 * executable (like the server and the emulator).
 *
 * Revision 2.5  1992/09/22  17:16:35  SSD
 * *** empty log message ***
 *
 * Revision 2.5  91/12/10  16:30:43  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:45  jsb]
 * 
 * Revision 2.4  91/06/18  20:50:59  jsb
 * 	New copyright from Intel. Corrected setting of regs.sp.
 * 	Printfs ifdef'd out.
 * 	[91/06/18  18:52:42  jsb]
 * 
 * Revision 2.3  91/06/17  15:44:30  jsb
 * 	First functional version.
 * 	[91/06/17  10:27:30  jsb]
 * 
 */
/*
 * i860-specific routines for loading coff files.
 */

#include <mach/mach_interface.h>
#include <mach/mach_user_internal.h>

#include <boot_ufs/file_io.h>
#include <boot_ufs/loader_info.h>

#include <i860/coff.h>
#include <i860/syms.h>
#include <i860/exec.h>

#if	0
print_lp(lp)
	struct loader_info *lp;
{
	printf("format\t0x%x\n", lp->format);
	printf("text_start\t0x%x\n", lp->text_start);
	printf("text_size\t0x%x\n", lp->text_size);
	printf("text_offset\t0x%x\n", lp->text_offset);
	printf("data_start\t0x%x\n", lp->data_start);
	printf("data_size\t0x%x\n", lp->data_size);
	printf("data_offset\t0x%x\n", lp->data_offset);
	printf("bss_size\t0x%x\n", lp->bss_size);
	printf("entry_1\t0x%x\n", lp->entry_1);
	printf("entry_2\t0x%x\n", lp->entry_2);
}
#endif	0


/*
 *	ld860 has an annoying habit of silently permitting overlapped
 *	text and data sections.  Check for that condition here and
 *	refuse to load the executable...
 */
static int sanity_check(lp)
	struct loader_info *lp;
{
	if ((lp->data_start >= lp->text_start) &&
	    (lp->data_start < (lp->text_start + lp->text_size))) {
		printf("text section overlaps data section\n");
		return EX_NOT_EXECUTABLE;
	}

	if ((lp->text_start >= lp->data_start) &&
	    (lp->text_start < (lp->data_start + lp->data_size + lp->bss_size))){
		printf("data section overlaps text section\n");
		return EX_NOT_EXECUTABLE;
	}

	return 0;

}


int ex_get_preloaded_header(prog_base, lp)
	unsigned char		*prog_base;
	struct loader_info	*lp;
{
	struct exec_hdrs	x;

	bcopy(prog_base, &x,  sizeof(x));
	return _ex_get_header(&x, lp);

}


int ex_get_header(fp, lp)
	struct file		*fp;
	struct loader_info	*lp;
{
	struct exec_hdrs	x;
	int			result;
	vm_size_t		resid;

	result = read_file(fp, 0, (vm_offset_t)&x, sizeof(x), &resid);
	if (result)
		return (result);
	if (resid)
		return (EX_NOT_EXECUTABLE);

	return _ex_get_header(&x, lp);
}



int _ex_get_header(x, lp)
	struct exec_hdrs	*x;
	struct loader_info 	*lp;
{
	struct scnhdr	*sh;
	int	i;

	sh = (struct scnhdr *) ((char *)x + sizeof(x->fh) + sizeof(x->ah) + 8);

	switch ((int) x->ah.magic) {
	case 0407:
		lp->format = EX_READIN;
		break;

	case 0410:
		lp->format = EX_SHAREABLE;
		break;

	case 0413:
		lp->format = EX_PAGEABLE;
		break;

	default:
		printf("_ex_get_header(): bad magic=0%o, oops\n", x->ah.magic);
		return EX_NOT_EXECUTABLE;
	}

	lp->text_size = -1;
	lp->data_size = -1;
	lp->bss_size = -1;

	if (x->fh.f_nscns > 3) {
		x->fh.f_nscns = 3; /* struct exec only has room for three */
	}
	for (i = 0; i < x->fh.f_nscns; i++, sh++) {
		if (! strncmp(sh->s_name, ".text", 5)) {
			lp->text_start = sh->s_vaddr;
			lp->text_size = sh->s_size;
			lp->text_offset = sh->s_scnptr;
			continue;
		}
		if (! strncmp(sh->s_name, ".data", 5)) {
			lp->data_start = sh->s_vaddr;
			lp->data_size = sh->s_size;
			lp->data_offset = sh->s_scnptr;
			continue;
		}
		if (! strncmp(sh->s_name, ".bss", 4)) {
			lp->bss_size = sh->s_size;
			continue;
		}
	}

	if (lp->text_size == -1 || lp->data_size == -1 || lp->bss_size == -1) {
		printf("some section not found\n");
		return EX_NOT_EXECUTABLE;
	}

	lp->entry_1 = x->ah.entry;
	lp->entry_2 = 0;

	if (sanity_check(lp)) {
		return EX_NOT_EXECUTABLE;
	}

	return 0;
}

#define	STACK_SIZE	(64*1024)

char *set_regs(user_task, user_thread, lp, arg_size)
	mach_port_t	user_task;
	mach_port_t	user_thread;
	struct loader_info *lp;
	int		arg_size;
{
	vm_offset_t	stack_start;
	vm_offset_t	stack_end;
	struct i860_thread_state	regs;
	unsigned int		reg_size;

	/*
	 * Allocate stack.
	 */
	stack_end = VM_MAX_ADDRESS;
	stack_start = VM_MAX_ADDRESS - STACK_SIZE;
	(void)vm_allocate(user_task,
			  &stack_start,
			  (vm_size_t)(stack_end - stack_start),
			  FALSE);

	reg_size = i860_THREAD_STATE_COUNT;
	(void)thread_get_state_KERNEL(user_thread,
				i860_THREAD_STATE,
				(thread_state_t)&regs,
				&reg_size);

	regs.pc = lp->entry_1;
	regs.sp = (int)((stack_end - arg_size) & ~0xf);

	(void)thread_set_state_KERNEL(user_thread,
				i860_THREAD_STATE,
				(thread_state_t)&regs,
				reg_size);

	return ((char *)regs.sp);
}


boolean_t
get_preload_symtab(prog_base, prog_size, symoff_p, symsize_p, header_p, header_size_p)
	unsigned char	*prog_base;
	unsigned long	prog_size;
	vm_offset_t	*symoff_p;	/* out */
	vm_size_t	*symsize_p;	/* out */
	vm_offset_t	*header_p;	/* out */
	vm_size_t	*header_size_p;	/* out */
{
	int             result;
	long            string_tbl_offset;
	long            string_tbl_size;
	long            file_size;

	ldf_hdr_t      *ldf_hdr;


	*symoff_p = 0;
	*symsize_p = 0;

	ldf_hdr = (ldf_hdr_t *)header_p;
	*header_size_p = sizeof(ldf_hdr_t);

	bcopy(prog_base + 0, (char *) header_p, sizeof(ldf_hdr_t));

	if (ldf_hdr->coff.mainhdr.f_nsyms == 0) {
		*header_size_p = 0;
		return TRUE;
	}

	*symoff_p = ldf_hdr->coff.mainhdr.f_symptr;
	string_tbl_offset = ldf_hdr->coff.mainhdr.f_symptr +
		(ldf_hdr->coff.mainhdr.f_nsyms * SYMESZ);

	bcopy(prog_base + string_tbl_offset,
		(char *) &string_tbl_size, sizeof(string_tbl_size));
	file_size = prog_size;
	*symsize_p = file_size - *symoff_p;
	string_tbl_size + sizeof(string_tbl_size);
	return TRUE;

}


boolean_t
get_symtab(fp, symoff_p, symsize_p, header_p, header_size_p)
	struct file	*fp;
	vm_offset_t	*symoff_p;	/* out */
	vm_size_t	*symsize_p;	/* out */
	vm_offset_t	*header_p;	/* out */
	vm_size_t	*header_size_p;	/* out */
{
	int             result;
	vm_size_t	resid;
	long            string_tbl_offset;
	long            string_tbl_size;
	long            file_size;

	ldf_hdr_t      *ldf_hdr;


	*symoff_p = 0;
	*symsize_p = 0;

	ldf_hdr = (ldf_hdr_t *)header_p;
	*header_size_p = sizeof(ldf_hdr_t);
	result = read_file(fp, 0, (vm_offset_t)header_p, sizeof(ldf_hdr_t), &resid);
#if 0
	printf("read hdr result:%d resid:%d\n", result, resid);
#endif
	if (result || resid)
		return (FALSE);

	if (ldf_hdr->coff.mainhdr.f_nsyms == 0) {
	    *header_size_p = 0;
	    return TRUE;
	}

	*symoff_p = ldf_hdr->coff.mainhdr.f_symptr;
	string_tbl_offset = ldf_hdr->coff.mainhdr.f_symptr +
	    (ldf_hdr->coff.mainhdr.f_nsyms * SYMESZ);

	result = read_file(fp, string_tbl_offset, 
			   (vm_offset_t)&string_tbl_size, 
			   sizeof(string_tbl_size), &resid);
#if 0
	printf("read str size result:%d resid:%d\n", result, resid);
#endif
	if (result || resid)
		return (FALSE);
	file_size = fp->i_size;
	*symsize_p = file_size - *symoff_p;
	    string_tbl_size + sizeof(string_tbl_size);
	return (TRUE);
}





