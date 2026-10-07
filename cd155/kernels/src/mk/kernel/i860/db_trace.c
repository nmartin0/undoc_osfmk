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
 * $Log: db_trace.c,v $
 * Revision 2.12  1994/11/18  20:38:50  mtm
 * Copyright additions/changes
 *
 * Revision 2.11  1994/07/12  19:18:33  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 2.10  1994/02/01  01:53:03  raya
 * Added calls to db_check_task_addr() to check to see that page is
 * present in memory before attempting to read it.  This allows the
 * t/uT command to continue with the next thread when it encounters
 * a thread that is partially paged out.
 *
 *  Reviewer:         	Don B.
 *  Risk:                  Low
 *  Benefit or PTS #: 	7993
 *  Testing:          	Limited test of the t/uT command.
 *  Module(s):             i860/db_trace.c ddb/db_access.c
 *
 * Revision 2.9  1993/06/30  22:30:33  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.8  1993/05/10  20:56:31  andyp
 * Added code from donb@ssd.intel.com to traceback all threads within
 * a task (mostly useful to server people).  Use 'T' as a modifier to
 * the "trace" command (eg, "t/ut $task2.0") to get the new behavior.
 *
 * Revision 2.7  1992/10/19  21:57:11  prp
 * Hack to let you see any register set you can get an address of
 *
 * Revision 2.6  1992/09/22  17:16:00  SSD
 * *** empty log message ***
 *
 * Revision 2.5.2.2  92/03/28  10:08:01  jeffreyh
 * 	Small changes in call parameters logic.
 * 	[92/03/27            sjs]
 * 	Added logic to print call parameters on call trace.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.5.2.1  92/02/18  19:00:29  jeffreyh
 * 	[intel] disable error condition.
 * 	[92/02/13  12:40:17  jeffreyh]
 * 
 * Revision 2.5  91/12/10  16:30:38  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:41  jsb]
 * 
 * Revision 2.4  91/08/28  11:12:14  jsb
 * 	From Intel SSD: use pcb->k_context (not context) in db_stack_trace_cmd.
 * 	[91/08/26  16:10:05  jsb]
 * 
 * Revision 2.3  91/06/18  20:50:57  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:50:59  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:18  jsb
 * 	First checkin.
 * 	[90/12/03  21:28:19  jsb]
 * 
 */

/*
 * Machine-dependent tracing support for kernel debugger.
 * Derived from i386.
 */

#include <mach/boolean.h>
#include <vm/vm_map.h>
#include <kern/thread.h>
#include <kern/task.h>

#include <i860/psl.h>
#include <machine/db_machdep.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_variables.h>
#include <ddb/db_task_thread.h>

int db_i860_reg_value();


#define	DDB_I860_SHOW_FPREGS	0	/* display fp regs? */

/*
 * Machine register set.
 */
struct db_variable db_regs[] = {
	{ "psr",     (int *) &ddb_regs.psr,	db_i860_reg_value, 0,0,0,0 },
	{ "pc",	     (int *) &ddb_regs.pc,	db_i860_reg_value, 0,0,0,0 },
	/*{ "db",	     (int *) &ddb_regs.db,	db_i860_reg_value, 0,0,0,0 },*/
	{ "dirbase", (int *) &ddb_regs.dirbase,	db_i860_reg_value, 0,0,0,0 },
	{ "fsr",     (int *) &ddb_regs.fsr,	db_i860_reg_value, 0,0,0,0 },
	{ "epsr",    (int *) &ddb_regs.epsr,	db_i860_reg_value, 0,0,0,0 },
	{ "r0",	     (int *) &ddb_regs.r0,	db_i860_reg_value, 0,0,0,0 },
	{ "r1",	     (int *) &ddb_regs.r1,	db_i860_reg_value, 0,0,0,0 },
	{ "sp",	     (int *) &ddb_regs.sp,	db_i860_reg_value, 0,0,0,0 },
	{ "fp",	     (int *) &ddb_regs.fp,	db_i860_reg_value, 0,0,0,0 },
	{ "r4",	     (int *) &ddb_regs.r4,	db_i860_reg_value, 0,0,0,0 },
	{ "r5",	     (int *) &ddb_regs.r5,	db_i860_reg_value, 0,0,0,0 },
	{ "r6",	     (int *) &ddb_regs.r6,	db_i860_reg_value, 0,0,0,0 },
	{ "r7",	     (int *) &ddb_regs.r7,	db_i860_reg_value, 0,0,0,0 },
	{ "r8",	     (int *) &ddb_regs.r8,	db_i860_reg_value, 0,0,0,0 },
	{ "r9",	     (int *) &ddb_regs.r9,	db_i860_reg_value, 0,0,0,0 },
	{ "r10",     (int *) &ddb_regs.r10,	db_i860_reg_value, 0,0,0,0 },
	{ "r11",     (int *) &ddb_regs.r11,	db_i860_reg_value, 0,0,0,0 },
	{ "r12",     (int *) &ddb_regs.r12,	db_i860_reg_value, 0,0,0,0 },
	{ "r13",     (int *) &ddb_regs.r13,	db_i860_reg_value, 0,0,0,0 },
	{ "r14",     (int *) &ddb_regs.r14,	db_i860_reg_value, 0,0,0,0 },
	{ "r15",     (int *) &ddb_regs.r15,	db_i860_reg_value, 0,0,0,0 },
	{ "r16",     (int *) &ddb_regs.r16,	db_i860_reg_value, 0,0,0,0 },
	{ "r17",     (int *) &ddb_regs.r17,	db_i860_reg_value, 0,0,0,0 },
	{ "r18",     (int *) &ddb_regs.r18,	db_i860_reg_value, 0,0,0,0 },
	{ "r19",     (int *) &ddb_regs.r19,	db_i860_reg_value, 0,0,0,0 },
	{ "r20",     (int *) &ddb_regs.r20,	db_i860_reg_value, 0,0,0,0 },
	{ "r21",     (int *) &ddb_regs.r21,	db_i860_reg_value, 0,0,0,0 },
	{ "r22",     (int *) &ddb_regs.r22,	db_i860_reg_value, 0,0,0,0 },
	{ "r23",     (int *) &ddb_regs.r23,	db_i860_reg_value, 0,0,0,0 },
	{ "r24",     (int *) &ddb_regs.r24,	db_i860_reg_value, 0,0,0,0 },
	{ "r25",     (int *) &ddb_regs.r25,	db_i860_reg_value, 0,0,0,0 },
	{ "r26",     (int *) &ddb_regs.r26,	db_i860_reg_value, 0,0,0,0 },
	{ "r27",     (int *) &ddb_regs.r27,	db_i860_reg_value, 0,0,0,0 },
	{ "r28",     (int *) &ddb_regs.r28,	db_i860_reg_value, 0,0,0,0 },
	{ "r29",     (int *) &ddb_regs.r29,	db_i860_reg_value, 0,0,0,0 },
	{ "r30",     (int *) &ddb_regs.r30,	db_i860_reg_value, 0,0,0,0 },
	{ "r31",     (int *) &ddb_regs.r31,	db_i860_reg_value, 0,0,0,0 },
#if	DDB_I860_SHOW_FPREGS
	{ "f0",	     (int *) &ddb_regs.f2,	db_i860_reg_value, 0,0,0,0 },
	{ "f1",	     (int *) &ddb_regs.f2,	db_i860_reg_value, 0,0,0,0 },
	{ "f2",	     (int *) &ddb_regs.f2,	db_i860_reg_value, 0,0,0,0 },
	{ "f3",	     (int *) &ddb_regs.f3,	db_i860_reg_value, 0,0,0,0 },
	{ "f4",	     (int *) &ddb_regs.f4,	db_i860_reg_value, 0,0,0,0 },
	{ "f5",	     (int *) &ddb_regs.f5,	db_i860_reg_value, 0,0,0,0 },
	{ "f6",	     (int *) &ddb_regs.f6,	db_i860_reg_value, 0,0,0,0 },
	{ "f7",	     (int *) &ddb_regs.f7,	db_i860_reg_value, 0,0,0,0 },
	{ "f8",	     (int *) &ddb_regs.f8,	db_i860_reg_value, 0,0,0,0 },
	{ "f9",	     (int *) &ddb_regs.f9,	db_i860_reg_value, 0,0,0,0 },
	{ "f10",     (int *) &ddb_regs.f10,	db_i860_reg_value, 0,0,0,0 },
	{ "f11",     (int *) &ddb_regs.f11,	db_i860_reg_value, 0,0,0,0 },
	{ "f12",     (int *) &ddb_regs.f12,	db_i860_reg_value, 0,0,0,0 },
	{ "f13",     (int *) &ddb_regs.f13,	db_i860_reg_value, 0,0,0,0 },
	{ "f14",     (int *) &ddb_regs.f14,	db_i860_reg_value, 0,0,0,0 },
	{ "f15",     (int *) &ddb_regs.f15,	db_i860_reg_value, 0,0,0,0 },
	{ "f16",     (int *) &ddb_regs.f16,	db_i860_reg_value, 0,0,0,0 },
	{ "f17",     (int *) &ddb_regs.f17,	db_i860_reg_value, 0,0,0,0 },
	{ "f18",     (int *) &ddb_regs.f18,	db_i860_reg_value, 0,0,0,0 },
	{ "f19",     (int *) &ddb_regs.f19,	db_i860_reg_value, 0,0,0,0 },
	{ "f20",     (int *) &ddb_regs.f20,	db_i860_reg_value, 0,0,0,0 },
	{ "f21",     (int *) &ddb_regs.f21,	db_i860_reg_value, 0,0,0,0 },
	{ "f22",     (int *) &ddb_regs.f22,	db_i860_reg_value, 0,0,0,0 },
	{ "f23",     (int *) &ddb_regs.f23,	db_i860_reg_value, 0,0,0,0 },
	{ "f24",     (int *) &ddb_regs.f24,	db_i860_reg_value, 0,0,0,0 },
	{ "f25",     (int *) &ddb_regs.f25,	db_i860_reg_value, 0,0,0,0 },
	{ "f26",     (int *) &ddb_regs.f26,	db_i860_reg_value, 0,0,0,0 },
	{ "f27",     (int *) &ddb_regs.f27,	db_i860_reg_value, 0,0,0,0 },
	{ "f28",     (int *) &ddb_regs.f28,	db_i860_reg_value, 0,0,0,0 },
	{ "f29",     (int *) &ddb_regs.f29,	db_i860_reg_value, 0,0,0,0 },
	{ "f30",     (int *) &ddb_regs.f30,	db_i860_reg_value, 0,0,0,0 },
	{ "f31",     (int *) &ddb_regs.f31,	db_i860_reg_value, 0,0,0,0 },
#endif	DDB_I860_SHOW_FPREGS
};
struct db_variable *db_eregs = db_regs + sizeof(db_regs)/sizeof(db_regs[0]);

/*
 * Stack trace.
 */
#define	INKERNEL(va)	(((vm_offset_t)(va)) >= VM_MIN_KERNEL_ADDRESS)
#define SRC1(ir)	((ir >> 11) & 0x1F)

struct i860_frame {
	struct i860_frame	*f_frame;
	int			f_retaddr;
};

#define	TRAP		1
#define	INTERRUPT	2
#define	SYSCALL		3


struct i860_kregs {
	char	*name;
	int	offset;
} i860_kregs[] = {
	{  "r0", (int) &(((struct i860_kernel_state *) 0)->ks_r0) },
	{  "r1", (int) &(((struct i860_kernel_state *) 0)->ks_r1) },
	{  "sp", (int) &(((struct i860_kernel_state *) 0)->ks_sp) },
	{  "fp", (int) &(((struct i860_kernel_state *) 0)->ks_fp) },
	{  "r4", (int) &(((struct i860_kernel_state *) 0)->ks_r4) },
	{  "r5", (int) &(((struct i860_kernel_state *) 0)->ks_r5) },
	{  "r6", (int) &(((struct i860_kernel_state *) 0)->ks_r6) },
	{  "r7", (int) &(((struct i860_kernel_state *) 0)->ks_r7) },
	{  "r8", (int) &(((struct i860_kernel_state *) 0)->ks_r8) },
	{  "r9", (int) &(((struct i860_kernel_state *) 0)->ks_r9) },
	{ "r10", (int) &(((struct i860_kernel_state *) 0)->ks_r10) },
	{ "r11", (int) &(((struct i860_kernel_state *) 0)->ks_r11) },
	{ "r12", (int) &(((struct i860_kernel_state *) 0)->ks_r12) },
	{ "r13", (int) &(((struct i860_kernel_state *) 0)->ks_r13) },
	{ "r14", (int) &(((struct i860_kernel_state *) 0)->ks_r14) },
	{ "r15", (int) &(((struct i860_kernel_state *) 0)->ks_r15) },
#if	DDB_I860_SHOW_FPREGS
	{  "f0", (int) &(((struct i860_kernel_state *) 0)->ks_f0) },
	{  "f1", (int) &(((struct i860_kernel_state *) 0)->ks_f1) },
	{  "f2", (int) &(((struct i860_kernel_state *) 0)->ks_f2) },
	{  "f3", (int) &(((struct i860_kernel_state *) 0)->ks_f3) },
	{  "f4", (int) &(((struct i860_kernel_state *) 0)->ks_f4) },
	{  "f5", (int) &(((struct i860_kernel_state *) 0)->ks_f5) },
	{  "f6", (int) &(((struct i860_kernel_state *) 0)->ks_f6) },
	{  "f7", (int) &(((struct i860_kernel_state *) 0)->ks_f7) },
#endif	DDB_I860_SHOW_FPREGS
	{  0, 0 },
};
static db_expr_t	null_reg = 0xdeadc0de;


int *
db_lookup_i860_kreg(name, ks)
	char *name;
	struct i860_kernel_state	*ks;
{
	register struct i860_kregs *kp;

	for (kp = i860_kregs; kp->name; kp++) {
		if (strcmp(name, kp->name) == 0)
			return ((int *) ((int) ks + kp->offset));
	}
	return(0);
}


int
db_i860_reg_value(vp, valuep, flag, ap)
	struct	db_variable	*vp;
	db_expr_t		*valuep;
	int			flag;
	db_var_aux_param_t	ap;
{
        struct i860_kernel_state	*ks;
        struct i860_saved_state		*ss;
	int				*dp = 0;
	thread_t			thread = ap->thread;
 
	if (thread == THREAD_NULL) {
		if ((thread = db_default_thread) == THREAD_NULL) {
			if ((thread = current_thread()) == THREAD_NULL) {
				/*db_error("no current_thread()\n");*/
				/*NOTREACHED*/
			}
		}
	}

	/*
	 * 'k' modifier lets you look at the registers that might
	 * be saved with a kernel thread.
	 * 'u' modifier ensures that you're looking at user-mode
	 * registers.
	 * No modifier uses the current state of the registers at the time
	 * of the exception.
	 *
	 * Kluge: 'rXXXXXXXX' modifier lets you look at some other reg struct.
	 */
	if (db_option(ap->modif, 'k')) {
		if (thread->kernel_stack) {
			ks = STACK_I860KS(thread->kernel_stack);
			dp = db_lookup_i860_kreg(vp->name, ks);
		} else {
			/* only continuation func pointer is valid */
			if (vp->valuep == (int *) &ddb_regs.r1) {
				dp = (int *) (&thread->swap_func);
			}
		}
		if (!dp) {
			dp = &null_reg;
		}
	} else if (db_option(ap->modif, 'u')) {
		if (thread != current_thread()) {
			/* user regs are in pcb */
			if (thread->pcb != 0) {
				ss = USER_REGS(thread);
				dp = (int *)((int)(ss) +
					((int)vp->valuep - (int)&ddb_regs));
			} else {
				ss = 0;
				dp = &null_reg;
			}
		} else {
			/*
			 * thread is the current thread; if trapped from user
			 * mode, use defaults. otherwise get them from the pcb.
			 */
			if ((ddb_regs.psr & PSR_PU) == 0) {
				if (thread->pcb != 0) {
					ss = USER_REGS(thread);
					dp = (int *)((int)(ss) +
						((int)vp->valuep - (int)&ddb_regs));
				} else {
					ss = 0;
					dp = &null_reg;
				}
			}
		}
	} else if (ap->modif[0] == 'r') {
		unsigned long a;
		char *p = ap->modif + 1;
		unsigned char t;

		while (*p) {
			if (*p >= '0' && *p <= '9') {
				t = *p - '0';
			} else if (*p >= 'a' && *p <= 'f') {
				t = *p - 'a' + 10;
			} else {
				t = 0;
			}
			a = (a << 4) + t;
			p++;
		}
		dp = (int *)(a +
			     (unsigned long)(vp->valuep) -
			     (unsigned long)(&ddb_regs));
	}

	/*
	 * default to current exception frame
	 */
	if (!dp) {
		dp = vp->valuep;
	}

	if (flag == DB_VAR_SET) {
		if (dp != &null_reg) {
			*dp = *valuep;
		}
	} else {
		*valuep = *dp;
	}

	return (0);
}


/*
 * Figure out how many arguments were passed into the frame at "fp".
 */
int
db_numargs(fp, task)
	struct i860_frame *fp;
	task_t task;
{
	/* just return 0 for i860 since most args are in registers */
	return 0;
}

/*
 * Major ****** "HACK" alert ********
 *
 * this pseudo algorthum really doesn't work well without compiler support.
 *
 * return # of args pased to i860 rtn.
 *
 * skip over basic preamble, want to start looking at instruction with
 * "st.l r1,xxx".
 *
 * inputs:
 *	task		task of interest ptr.
 *	va		VA of function start.
 *
 * outputs:
 *	count of actual arguments.
 *
 */

db_hack(va)
{
	return( db_count_func_args(current_task(),va) );
}

#define	ST_X 0x1c000000
#define FST_X 0x2c000000
#define INST(a) ((a) & 0xfc000000)

db_count_func_args( task, va )
        task_t          task;
        db_addr_t       va;
{
        register int	instr, argc=0;
	register int	cur_arg_reg = 16;
	register int	prologue_ok=FALSE;

	/*
	 * skip over initial function prolog:
	 *	addu    -128, sp, sp
	 *	st.l    fp, 32(sp)
	 *	addu    32, sp, fp
	 *
	 * Start looking at the instruction "st.l    r1, 4(fp)"
	 */
	va += 12;

	/*
	 * arguments are not easily determined!
	 *
	 * Use a bottom up hueristic to determine the parameters.
	 * The current PGI compiler saves all of the parameters on
	 * the stack as part of the routine preamble.  We print out
	 * those registers starting with r16 and incrementing.  If
	 * we are at the top of a routine, just use the current
	 * register values.  If the compiler optimizes these saves
	 * out, this will probably not work as well.
	 */
	instr = db_get_task_value( (int)va, 4, FALSE, task );

	/*
	 * Make sure we have a st.l r1,xxx, may have been optimized out
	 * and we can't use this method.
	 */
	if ( ( INST(instr) == ST_X) && SRC1(instr) == 1 ) {
		va += 4;
		instr =  db_get_task_value((int)va,4,FALSE,task);
		prologue_ok = TRUE;

		/*
		 * while a st.x or fst.x instruction
		 */
		while( INST(instr) == ST_X || INST(instr) == FST_X ) {
		    register int	src_reg;

		    /*
		     * Ignore fst.x instructions, 0x1c000000 is st.x
		     */
		    src_reg = SRC1(instr);
		    if ( INST(instr) != FST_X &&  src_reg == cur_arg_reg ) {
			argc++;
			cur_arg_reg++;
		    }

		    va += 4;	/* next instruction */

		    instr =  db_get_task_value( (int)va, 4, FALSE, task );

		    /* don't get carried away */
		    if ( cur_arg_reg > 29 )
			break;
		}
	}

	/*
	 * attempt to catch those cases where you actually have args but
	 * the actuals save code was optimized aay or not generated. Default
	 * to display three args. We at least do the right thing for assembly
	 * lang. functions.
	 */
	if ( prologue_ok && argc == 0 )
	    argc = 3;

	return argc;
}


static void db_i860_print_user_boundary()
{
	db_printf("--== <user mode> ==--\n");
}


static void db_trace_debug(fp, pc, ufp, upc, th)
	struct i860_frame	*fp, *ufp;
	vm_offset_t		pc, upc;
	thread_t		th;
{
	db_printf("fp=0x%x pc=0x%x ufp=0x%x upc=0x%x th=0x%x\n",
		fp, pc, ufp, upc, th);
}


/*
 *	Use a bottom up heuristic to determine the parameters.
 *	The current PGI compiler saves all of the parameters on
 *	the stack as part of the routine preamble.  We print out
 *	those registers starting with r16 and incrementing.  If
 *	we are at the top of a routine, just use the current
 *	register values.  If the compiler optimizes these saves
 *	out, this will probably not work as well.
 */
static void db_i860_guess_parameters(th, task, fp, pc)
	thread_t		th;
	task_t			task;
	struct i860_frame	*fp;
	vm_offset_t		pc;
{
	int		reg, fpoff;
	vm_offset_t	ea;
	unsigned long	op, val;
	boolean_t	comma = FALSE;

	if (th == THREAD_NULL) {
		db_printf("<!>");
		return;
	}

	/*
	 *	skip over basic preamble, want to start with st.l r1,xxx
	 */
	reg = 16;
	pc += 0xc;

	/*
	 *	Make sure we have a st.l r1,xxx, else parameter saves
	 *	may have been optimized out and we can't use this method.
	 */
	if ( db_check_task_addr(pc, task, FALSE) != 0 )  {
		db_printf("-?-");
                return;
	}
	op = db_get_task_value(pc, 4, FALSE, task);
	if (((op & 0xfc000000) != 0x1c000000) || (SRC1(op) != 1)) {
		db_printf("?");
		return;
	}

	pc += 4;
	if ( db_check_task_addr(pc, task, FALSE) != 0 ) {
		db_printf("-?-");
		return;
	}
	op = db_get_task_value(pc, 4, FALSE, task);

	/*
	 *	Decode the function preamble, computing where
	 *	in the frame the parameter registers were saved.
	 */
	while (((op & 0xfc000000) == 0x1c000000)
	    || ((op & 0xfc000000) == 0x2c000000)) {

		if ((SRC1(op) == reg) && (op & 0xfc000000) != 0x2c000000) {

			/*
			 *	Get the immediate encoded offset into
			 *	the frame.
			 */
			fpoff = (op & 0x000007ff) | ((op & 0x001f0000) >> 5);
			if (fpoff & 0x8000) {
				fpoff |= 0xffff0000;
			}
			fpoff &= ~1;	/* knock off size bit */
			ea = ((vm_offset_t) fp) + fpoff;
			if (db_check_task_addr(ea, task, FALSE) != 0) {
				db_printf("-?-");
				return;
			}
			val = db_get_task_value(ea, 4, FALSE, task);
			if (comma) {
				db_printf(",");
			}
			db_printf("%x", val);
			if (++reg > 27) {
				db_printf("...");
				return;
			}
			comma = TRUE;
		}

		pc += 4;
		if ( db_check_task_addr(pc, task, FALSE) != 0 ) {
			db_printf("-?-");
			return;
		}
		op = db_get_task_value(pc, 4, FALSE, task);
	}
}


static boolean_t	show_underscores;
static boolean_t	trace_verbose;

/*======================================================================
 *
 *  NAME
 *       db_trace_thread
 *
 *  DESCRIPTION
 *       Print a stack traceback of the specified or default thread.
 *
 *  PARAMETERS
 *       addr:          - address specified on cmd line in std cmdlin format.
 *       have_addr:     - true if addr has valid value in it.
 *       count:         - ",count" specified in std cmdlin format.
 *       kernel_only:   - do not trace back into user space
 *       trace_thread:  - a thread was specified on the command line
 *
 *  RETURN VALUE
 *       --- none ---
 *
 *=====================================================================*/
void db_trace_thread( addr, have_addr, count, kernel_only, trace_thread )
        db_expr_t       addr;
        boolean_t       have_addr;
        db_expr_t       count;
        boolean_t	kernel_only;
	boolean_t	trace_thread;
{
	struct i860_frame 		*fp, *lfp, *ufp;
	struct i860_kernel_state	*iks;
	int		cnt;
	db_addr_t	pc, upc;
	char		*filename;
	int		linenum;
	task_t		task;
	thread_t	th;
	extern unsigned	db_maxoff;
	boolean_t	umode;
	char		*name;
	db_expr_t	offset;

	umode = FALSE;
	lfp = (struct i860_frame *) ~0;	/* to be different from "fp" */
	fp = 0;
	ufp = 0;
	pc = 0;
	upc = 0;
	th = THREAD_NULL;
	task = TASK_NULL;

	if (trace_verbose) {
		db_printf("db_trace_thread(a=%x, ha=%d, c=%d, ko=%d, tt=%d)\n",
			addr, have_addr, count, kernel_only, trace_thread);
	}

	/*
	 *	Compute a frame pointer to start from and a pc.
	 */
	if (trace_thread) {

		if (have_addr) {
			th = (thread_t) addr;
			if (!db_check_thread_address_valid(th)) {
				return;
			}
		} else {
			th = (db_default_thread)
				? db_default_thread
				: current_thread();
			if (th == THREAD_NULL) {
				db_printf("[ no thread ]\n");
				return;
			}
		}
		task = (th != THREAD_NULL) ? th->task : TASK_NULL;

		if (th == current_thread()) {
			fp = (struct i860_frame *) ddb_regs.fp;
			pc = (db_addr_t) ddb_regs.pc;
		} else {
			if (th->kernel_stack == 0) {
				if (th->pcb == 0) {
					db_printf("[ no pcb, no stack ]\n");
				}
				fp = 0;
				pc = (db_addr_t) th->swap_func;
			} else {
				iks = STACK_I860KS(th->kernel_stack);
				fp = (struct i860_frame *) iks->ks_fp;
				pc = iks->ks_r1;
			}
		}
	} else {
		if (have_addr) {
			th = (db_default_thread)
				? db_default_thread
				: current_thread();
			task = (th != THREAD_NULL)
				? th->task
				: TASK_NULL;
			fp = (struct i860_frame *) addr;
			if (db_check_task_addr((int) fp, task, 1) != 0) {
				db_printf("[ bad fp 0x%x ]\n", addr);
				return;
			}
			pc = (db_addr_t) db_get_task_value(
					(int) &fp->f_retaddr,
					4,
					FALSE,
					task);
		} else {
			th = current_thread();
			task = (th != THREAD_NULL)
				? th->task
				: TASK_NULL;
			fp = (struct i860_frame *) ddb_regs.fp;
			pc = (db_addr_t) ddb_regs.pc;
		}
	}

	if (th == THREAD_NULL) {
		kernel_only = TRUE;
	} else {

		/*
		 *	If there is a pcb, get the user-mode fp and pc.
		 */
		if (th->pcb == 0) {
			kernel_only = TRUE;
		} else {
			ufp = (struct i860_frame *) USER_REGS(th)->fp;
			upc = USER_REGS(th)->pc;
		}

		/*
		 *	If the thread is swapped and has a continuation,
		 *	show the small amount of kernel state that can
		 *	be displayed.
		 */
		if ((th->state & TH_SWAPPED) && (th->swap_func != 0)) {
			db_printf("[ %s: ",
				(th->state & TH_SWAPPED) ? "continuation"
							 : "ready-to-run");
			db_task_printsym(th->swap_func, DB_STGY_PROC, task);
			db_printf(" ]\n");
		}
	}

	if (!INKERNEL(pc) && !INKERNEL(fp)) {
		db_i860_print_user_boundary();
		if (kernel_only) {
			return;
		}
	}

	/*
	 *	print a frame and climb up.
	 */
	for (cnt = 0; cnt < count; cnt++) {

		if (trace_verbose) {
			db_trace_debug(fp, pc, ufp, upc, th);
		}

		/*
		 *	print items of interest for this frame
		 */
		if (fp != 0) {
			db_printf("%2d <%08x> ", cnt, fp);
			db_find_task_sym_and_offset(pc, &name, &offset, task);
			if ((name == 0) || (offset > db_maxoff)) {
				db_printf("0x%08x(", pc);
				offset = 0;
			} else {
				if (show_underscores == 0) {
					if (name[0] == '_') {
						name++;
					} else if (name[0] != '_') {
						db_printf("=");
					}
				}
				db_printf("%s(", name);
			}

			/*
			 *	Use the parameter-guessing gem orginally
			 *	from sjs@osf.org.  This only works when
			 *	the compiler hasn't optimized away
			 *	the stores of function arguments onto
			 *	the frame (-Mnoframe w/ PGI will render
			 *	the guesses wrong, too).
			 */
			db_i860_guess_parameters(th, task, fp, pc - offset);

			db_printf(")");
			if (offset) {
				db_printf("+0x%x", offset);
			}

			if (db_line_at_pc(0, &filename, &linenum, pc)) {
				db_printf(" [%s", filename);
				if (linenum > 0) {
					db_printf(":%d", linenum);
				}
				db_printf("]");
			}

			db_printf("\n");
		}


		/*
		 *	go up one frame.
		 *
		 *	If the current frame dead-ends, follow the frame
		 *	chain rooted in the pcb.
		 */
		if (fp == 0) {
			if (ufp == 0) {
				return;
			}
			if (kernel_only) {
				return;
			}
			if (umode == TRUE) {
				return;
			}

			db_i860_print_user_boundary();

			lfp = fp;
			fp = ufp;
			pc = upc;
			ufp = 0;
			upc = 0;
			umode = TRUE;

			continue;
		}

		if (th == THREAD_NULL) {
			pc = fp->f_retaddr;
			lfp = fp;
			fp = fp->f_frame;
		} else {
			if ( db_check_task_addr((int) fp, task, FALSE) != 0 ) {
				fp = 0;
			} else {
				pc = (db_addr_t) db_get_task_value(
					(int) &(fp)->f_retaddr,
					4,
					FALSE,
					task);
				lfp = fp;
				fp = (struct i860_frame *) db_get_task_value(
						(int) &(fp)->f_frame,
						4,
						FALSE,
						task);
			}
		}

		if (!INKERNEL(pc) && !INKERNEL(fp) && (umode == FALSE)) {
			db_i860_print_user_boundary();
			if (kernel_only) {
				return;
			}
			umode = TRUE;
		}

		if (lfp == fp) {
			db_printf("   <%08x> ... [lfp=%08x]", fp, lfp);
			return;
		}
	}
}


/*======================================================================
 *
 *  NAME
 *       db_stack_trace_cmd
 *
 *  DESCRIPTION
 *       Print a stack traceback of the specified thread(s).
 *
 *  PARAMETERS
 *       addr:        - address specified on cmd line in std cmdlin format.
 *       have_addr:   - true if addr has valid value in it.
 *       count:       - ",count" specified in std cmdlin format.
 *       modif:       - cmd modifiers specified in std cmdlin format "/x".
 *                      valid modifiers include:
 *                       trace [/] [t | T] [u] [_]
 *                      where
 *                       t  => thread
 *                       T  => all threads in task
 *                       u  => include user space
 *			 _  => toggle the display of leading underscores
 *
 *  RETURN VALUE
 *       --- none ---
 *
 *=====================================================================*/
void db_stack_trace_cmd(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	boolean_t	kernel_only = !db_option( modif, 'u' );
	boolean_t	trace_thread = db_option( modif, 't' );
        boolean_t	trace_task = db_option( modif, 'T' );
	boolean_t	toggle_underscores = db_option( modif, '_' );
	thread_t	thread = THREAD_NULL;
        task_t		task = TASK_NULL;
        int		thread_id = -1;
	int		task_id = -1;
        int		n = -1;
        db_expr_t       value;

	trace_verbose = db_option( modif, 'v' );

	if ( trace_task && trace_thread ) {
		db_error("Cannot specify both 'T' and 't'\n");
	}

	if ( count == -1 ) {
		count = 65535;
	}

	if (toggle_underscores) {
		show_underscores = !show_underscores;
	}

	if ( !trace_task ) {
		db_trace_thread( addr,
				have_addr,
				count,
				kernel_only,
				trace_thread );
		return;
	}


	/* find the specified task */
	if ( !have_addr ) {

		/* task not specified, use default */
		if ( db_default_task != TASK_NULL ) {
			task = db_default_task;
                } else {
			db_error( "No task\n" );
                }

	} else {  /* task or thread address was specified */

		task_id = db_lookup_task( (task_t)addr );
		if ( task_id < 0 ) {
			thread_id = db_lookup_thread( (thread_t)addr );
			if ( thread_id < 0 ) {
				db_error( "Bad task/thread address 0x%08lx\n",
					addr );
			}
			/* user specified a starting thread */
			task = ((thread_t)addr)->task;
			task_id = db_lookup_task( task );
			if ( task_id < 0 ) {
				db_error( "Bad task address 0x%08lx\n", task );
			}
		} else {
			task = (task_t)addr;
			thread_id = 0;
                }
	}

	/* trace each thread's stack */
	queue_iterate(&task->thread_list, thread, thread_t, thread_list) {

		db_printf( "\n**** task%d.%d ****\n", task_id, thread_id );
		db_trace_thread( thread,
				 TRUE,        /* have_addr */
				 count,
				 kernel_only,
				 TRUE );      /* trace_thread */

		thread_id++;
	}
}


