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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
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
 * $Log: genassym.c,v $
 * Revision 2.9  1995/04/04  21:22:58  lenb
 *         define THREAD_PCB and I860_STACK_COOKIES
 *
 * Revision 2.8  1994/11/18  20:39:01  mtm
 * Copyright additions/changes
 *
 * Revision 2.7  1993/06/30  22:30:51  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.6  1992/09/30  16:02:14  stans
 * added "KSTACK_SIZE" for ttrap.s; top-of-kernel stack.
 *
 * Revision 2.5  1992/09/24  17:04:58  stans
 * added THREAD_RECOVER for copy{in/out}() rtns in user.s
 *
 * Revision 2.3  91/06/18  20:51:21  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:54:28  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:26  jsb
 * 	First checkin.
 * 	[90/12/03  21:28:56  jsb]
 * 
 */

/*
 * Program to generate constants (e.g., field offsets) to assembly code.
 */

#include <platforms.h>
#include <cpus.h>
#include <mach_kdb.h>
#include <stat_time.h>

/*
 * Pass field offsets to assembly code.
 */
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/lock.h>
#include <kern/syscall_emulation.h>
#include <i860/thread.h>
#include <mach/i860/vm_param.h>
#include <i860/pmap.h>
#if     MACH_KDB
#include <sys/reboot.h>
#endif

#define	SOFF(a,b) &(((struct a *)0)->b)
#define	OFF(a,b) &(((a)0)->b)

/*ARGSUSED*/
main(argc,argv)
	int	argc;
	char	**argv;
{

	printf("#ifndef __ASSYM_S__\n\n#define __ASSYM_S__ 1\n\n");

	printf("#ifdef __GENASSYM_FRAME_DEFS__\n");
	/*
	 * i860 exception state
	 */
	printf("#define uintreg %d\n", SOFF(i860_saved_state,r0));
	printf("#define ur1 %d\n", SOFF(i860_saved_state,r1));
	printf("#define usp %d\n", SOFF(i860_saved_state,sp));
	printf("#define ufp %d\n", SOFF(i860_saved_state,fp));

	/*
	 * generate symbols for the integer registers which start at the
	 * beginning of an i860_svaed_state struct; First 32 longwords.
	 */
	{
	register int	j, *regs;

	regs = (int *)0;
	/* r0..fp already done */
	for(j=4; j < 32; j++)
		printf("#define ur%d %d\n", j, &regs[j]);
	}

	printf("#define ufltreg %d\n", SOFF(i860_saved_state,f0));
	printf("#define ut %d\n", SOFF(i860_saved_state,spc_t));
	printf("#define uki %d\n", SOFF(i860_saved_state,spc_ki));
	printf("#define ukr %d\n", SOFF(i860_saved_state,spc_kr));
	printf("#define umerge %d\n", SOFF(i860_saved_state,spc_merge));
	printf("#define ul1 %d\n", SOFF(i860_saved_state,psv_l1));
	printf("#define ul2 %d\n", SOFF(i860_saved_state,psv_l2));
	printf("#define ul3 %d\n", SOFF(i860_saved_state,psv_l3));
	printf("#define ua1 %d\n", SOFF(i860_saved_state,psv_a1));
	printf("#define ua2 %d\n", SOFF(i860_saved_state,psv_a2));
	printf("#define ua3 %d\n", SOFF(i860_saved_state,psv_a3));
	printf("#define um1 %d\n", SOFF(i860_saved_state,psv_m1));
	printf("#define um2 %d\n", SOFF(i860_saved_state,psv_m2));
	printf("#define um3 %d\n", SOFF(i860_saved_state,psv_m3));
	printf("#define ui1 %d\n", SOFF(i860_saved_state,psv_i1));
	printf("#define ufsr1 %d\n", SOFF(i860_saved_state,fsr1));
	printf("#define ufsr2 %d\n", SOFF(i860_saved_state,fsr2));
	printf("#define ufsr %d\n", SOFF(i860_saved_state,fsr));
	printf("#define upsr %d\n", SOFF(i860_saved_state,psr));
	printf("#define ufir %d\n", SOFF(i860_saved_state,pc));
	printf("#define udirbase %d\n", SOFF(i860_saved_state,dirbase));
	printf("#define udb %d\n", SOFF(i860_saved_state,db));
	printf("#define uepsr %d\n", SOFF(i860_saved_state,epsr));
	printf("#define ufp1 %d\n", SOFF(i860_saved_state,fp1));
	printf("#define ufp2 %d\n", SOFF(i860_saved_state,fp2));
	printf("#define ufp3 %d\n", SOFF(i860_saved_state,fp3));
	printf("#define ufp4 %d\n", SOFF(i860_saved_state,fp4));
	printf("#define utrapped_opcode %d\n", SOFF(i860_saved_state,trapped_opcode));
	printf("#define urdest %d\n", SOFF(i860_saved_state,rdest));
	printf("#define usrc1 %d\n", SOFF(i860_saved_state,src1));
	printf("#define usaved_r13 %d\n", SOFF(i860_saved_state,save_r13));
	printf("#define usaved_r14 %d\n", SOFF(i860_saved_state,save_r14));
	printf("#define ufir_inst %d\n", SOFF(i860_saved_state,save_r15));
	printf("#define ukrki %d\n", SOFF(i860_saved_state,krki));
	printf("#define utrapno %d\n", SOFF(i860_saved_state,trapno));
	printf("#define uvaddr %d\n", SOFF(i860_saved_state,vaddr));
	printf("#define ucode %d\n", SOFF(i860_saved_state,code));
	printf("#endif /* __GENASSYM_FRAME_DEFS__ */\n");
	printf("\n");

/* This is the number of bytes above an initial kernel stack  */
#define I860_STACK_COOKIES (sizeof(struct i860_exception_link) + \
                        sizeof(struct i860_kernel_state))

	printf("#define KSTACK_SIZE %d\n",KERNEL_STACK_SIZE - I860_STACK_COOKIES);
	printf("\n");

	printf("#define THREAD_KSTACK %d\n", OFF(thread_t,kernel_stack));
	printf("#define THREAD_TASK %d\n", OFF(thread_t,task));
	printf("#define THREAD_RECOVER %d\n", OFF(thread_t,recover));
	printf("#define THREAD_PCB %d\n", OFF(thread_t,pcb));
	printf("\n");

	printf("#define TASK_EML_DISPATCH %d\n", SOFF(task,eml_dispatch));
	printf("\n");

	printf("#define EMLDISP_REFCNT %d\n", OFF(eml_dispatch_t,ref_count));
	printf("#define EMLDISP_CNT %d\n", OFF(eml_dispatch_t,disp_count));
	printf("#define EMLDISP_MIN %d\n",
		 &(((eml_dispatch_t)0)->disp_min));
	printf("#define EMLDISP_VECT %d\n", OFF(eml_dispatch_t,disp_vector[0]));
	printf("\n");

	printf("/* redo VM_MIN_KERNEL_ADDRESS, C constructs in mach/i860/vm_param.h definition */\n");
	printf("#ifdef VM_MIN_KERNEL_ADDRESS\n#undef VM_MIN_KERNEL_ADDRESS\n#endif\n");
	printf("#define VM_MIN_KERNEL_ADDRESS 0x%x\n", VM_MIN_KERNEL_ADDRESS);

	printf("#define I860_STACK_COOKIES 0x%x\n",
	       (sizeof(struct i860_exception_link) +
		sizeof(struct i860_kernel_state)));

	printf("\n\n#endif\t/* __ASSYM_S__ */\n");

	return 0;
}

