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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: emul_machdep.c,v $
 * Revision 1.4  1994/11/18  20:26:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:34:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:26:50  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:10:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:59:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/03  23:59:03  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1991/12/16  09:59:48  roy
 * 	Remove CMUCS conditionals (sp).
 *
 * Revision 2.3  91/09/17  12:21:18  sjs
 * integrate Locus changes	yazz
 * e_fork() divided into machine dependent portion and machine
 * independent portion.  Machine dependent portion (in this file)
 * calls machine independent portion (in bsd_user_side.c).  Slight
 * change to interface to wait to now use a vproc port.
 * 
 * Revision 2.2  91/08/30  16:40:56  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.1  91/02/27  16:24:51  condict
 * Eliminate CMU syscalls.
 * 
 * Revision 3.0  91/01/17  12:05:22  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.8  90/08/06  15:31:07  rwd
 * 	Added pid_by_task.
 * 	[90/07/31            rwd]
 * 	Include sys/types.h
 * 	[90/07/17            rwd]
 * 
 * Revision 2.7  90/06/02  15:21:00  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  19:29:46  rpd]
 * 
 * Revision 2.6  90/05/29  20:22:39  rwd
 * 	Add set_arg_addr for new exec.  Make execv call execve -
 * 	it's not used very often.
 * 	[90/03/22            dbg]
 * 
 * Revision 2.5  90/05/21  13:48:35  dbg
 * 	Add set_arg_addr for new exec.  Make execv call execve -
 * 	it's not used very often.
 * 	[90/03/22            dbg]
 * 
 * Revision 2.4  90/03/14  21:24:34  rwd
 * 	Check for signals before system call.  Added mapped sigreturn.
 * 	[90/02/27            rwd]
 * 
 * Revision 2.3  89/10/17  11:24:31  rwd
 * 	Added syscall init_process(-41).  Mach_Init needs this.
 * 	[89/09/29            rwd]
 * 
 * Revision 2.2.1.1  89/09/21  20:35:43  dbg
 * 	Add interrupt return parameter to all calls.
 * 	[89/09/21            dbg]
 * 
 * 	Don't set return values if we will restart system call.
 * 	Remove INTERRUPT bit processing - if bit is set on otherwise
 * 	successful call, the out parameters are not returned.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.2  89/08/31  16:28:28  rwd
 * 	[89/08/24  13:42:24  rwd]
 * 
 * 	Added copyright to file by dbg.  Changed to reflect pushing of
 * 	syscall # by trampline code.
 * 	[89/08/23            rwd]
 * 
 *
 */
/*
 * Take/Return from signals - machine-dependent.
 */
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <vax/psl.h>
#include <vax/vmparam.h>
#include <uxkern/bsd_msg.h>	/* error code definitions */

#include <syscall_table.h>

#ifdef	MAP_UAREA
#include <sys/ushared.h>

extern int shared_enabled;
extern struct ushared_ro *shared_base_ro;
extern struct ushared_rw *shared_base_rw;
#endif	MAP_UAREA

extern mach_port_t	our_bsd_server_port;

#define	E_JUSTRETURN	255	/* return without changing registers */

/*
 * User's registers are stored on partially on the user's stack
 * and partially on the emulator's stack.
 */
struct emul_regs_2 {
	int	r0;
	int	r1;
	int	r2;
	int	r3;
	int	r4;
	int	r5;
	int	syscode;
	int	pc;
	int	psl;
};

struct emul_regs {
	int	ap;
	int	fp;
	int	em_pc;		/* (return PC from syscall_emul) */
	int	r6;
	int	r7;
	int	r8;
	int	r9;
	int	r10;
	int	r11;
	int	em_nargs;	/* (number of args in call to syscall_emul) */
	struct emul_regs_2 *usp;
				/* pointer to rest of user registers */
};
#define	USP_OFFSET	((int)&(((struct emul_regs *)0)->usp))

void	take_signals();	/* forward */

int	emul_low_entry = -9;
int	emul_high_entry = 181;

extern emul_common();

void
emul_setup(task)
	task_t	task;
{
	register int i;
	register kern_return_t	rc;

	for (i = emul_low_entry;
	     i <= emul_high_entry;
	     i++) {
		rc = task_set_emulation(task,
					emul_common,
					i);
	}
	rc = task_set_emulation(task,
			emul_common,
			-33);
	rc = task_set_emulation(task,
			emul_common,
			-34);
	rc = task_set_emulation(task,
			emul_common,
			-41);
	rc = task_set_emulation(task,
			emul_common,
			-52);
}

/*
 * System calls enter here.
 */
void
emul_syscall(usp_loc)
	int		usp_loc;	/* 'usp' field in emul_regs */
{
	register int	syscode;
	register int	error;
	register struct sysent *callp;
	int		rval[2];
	boolean_t	interrupt = FALSE;
	register struct emul_regs *regs;
	register struct emul_regs_2 *regs2;
	register int	*args;

	regs = (struct emul_regs *)((int)&usp_loc - USP_OFFSET);
	regs2 = regs->usp;
	args = ((int *)regs->ap) + 1;
		/* point to first argument */

	syscode = regs2->syscode;

#ifdef	MAP_UAREA
	if (shared_enabled) {
	    if (shared_base_ro->us_cursig) {
		error = ERESTART;
		goto signal;
	    }
	}
#endif	MAP_UAREA

	if (syscode == 0) {
	    /*
	     * Indirect system call.
	     */
	    syscode = *args++;
	}

	/*
	 * Find system call table entry for the system call.
	 */
	if (syscode >= nsysent)
	    callp = &sysent[63];	/* nosysent */
	else if (syscode >= 0)
	    callp = &sysent[syscode];
	else {
	    /*
	     * Negative system call numbers are CMU extensions.
	     */
	    if (syscode == -33)
		callp = &sysent_task_by_pid;
	    else if (syscode == -34)
		callp = &sysent_pid_by_task;
	    else if (syscode == -41)
		callp = &sysent_init_process;
	    else if (syscode == -59)
		callp = &sysent_htg_ux_syscall;
	    else
		callp = &sysent[63];	/* nosysent */
	}

	/*
	 * Set up the initial return values.
	 */
	rval[0] = 0;
	rval[1] = regs2->r1;

	/*
	 * Call the routine, passing arguments according to the table
	 * entry.
	 */
	switch (callp->nargs) {
	    case 0:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				rval);
		break;
	    case 1:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0],
				rval);
		break;
	    case 2:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1],
				rval);
		break;
	    case 3:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2],
				rval);
		break;
	    case 4:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2], args[3],
				rval);
		break;
	    case 5:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2], args[3], args[4],
				rval);
		break;
	    case 6:
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args[0], args[1], args[2],
				args[3], args[4], args[5],
				rval);
		break;

	    case -1:	/* generic */
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				syscode,
				args,
				rval);
		break;

	    case -2:	/* pass registers to modify */
		error = (*callp->routine)(our_bsd_server_port,
				&interrupt,
				args,
				rval,
				regs);
		regs2 = regs->usp;	/* if changed */
		break;
	}

	/*
	 * Set up return values.
	 */

#ifdef	MAP_UAREA
signal:
#endif	MAP_UAREA

	switch (error) {
	    case E_JUSTRETURN:
		/* Do not alter registers */
		break;

	    case 0:
		/* Success */
		regs2->psl &= ~PSL_C;
		regs2->r0 = rval[0];
		regs2->r1 = rval[1];
		break;

	    case ERESTART:
		/* restart call */
		syscode = regs2->syscode;	/* get original */
		if (syscode > 63 || syscode < 0)
		    regs2->pc -= 4;		/* not correct for 'chmk r0'
						   in syscall.c */
		else
		    regs2->pc -= 2;
		break;

	    default:
		/* error */
		regs2->psl |= PSL_C;
		regs2->r0 = error;
		break;
	}

	/*
	 * Handle interrupt request
	 */
	if (error == ERESTART || error == EINTR || interrupt)
	    take_signals(regs);
}

/*
 * Exec starts here to save registers.
 */
struct execa {
    char	*fname;
    char	**argp;
    char	**envp;
};

int
e_execv(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct execa	*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	struct execa		execa;

	execa.fname = argp->fname;
	execa.argp  = argp->argp;
	execa.envp  = (char **)0;

	return (e_execve(serv_port, interrupt, &execa, rval, regs));
}

int
e_execve(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register struct execa	*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	struct emul_regs_2	save_regs2;
	register struct emul_regs_2 *regs2;
	int		entry[2];
	unsigned int	entry_count;
	vm_offset_t	arg_addr;
	register int	error;

	/*
	 * Save anything from old stack.
	 */
	save_regs2 = *regs->usp;

	/*
	 * Call exec.  If error, return without changing registers.
	 */
	entry_count = 2;
	error = e_exec_call(serv_port,
			    interrupt,
			    argp->fname,
			    argp->argp,
			    argp->envp,
			    &arg_addr,
			    entry,
			    &entry_count);
	if (error)
	    return (error);

	/*
	 * Put new user stack just below arguments.
	 */
	regs2 = (struct emul_regs_2 *)arg_addr;

	/*
	 * Push old registers on user stack.
	 */
	*--regs2 = save_regs2;

	/*
	 * Set new pc, and clear frame pointer for traceback.
	 */
	regs->usp = regs2;
	regs2->pc = entry[0] + 2;
	regs->fp = 0;

	/*
	 * If second entry value, it is the initial parameter hack for
	 * /etc/init.
	 */
	if (entry_count > 1)
	    regs->r11 = entry[1];	/* HACK HACK HACK */

	/*
	 * Return to new stack.
	 */
	return (E_JUSTRETURN);
}


/*
 * Take a signal.
 */
void
take_signals(regs)
	register struct emul_regs *regs;
{
	struct emul_regs_2	save_regs2;
	register struct emul_regs_2	*regs2;

	register struct sigcontext *scp;
	register struct sigframe {
	    int		sf_signum;
	    int		sf_code;
	    struct sigcontext *sf_scp;
	    int		(*sf_handler)();
	    int		sf_argcount;
	    struct sigcontext *sf_scpcopy;
	} *fp;

	int	old_mask, old_onstack, sig, code, handler, new_sp;
	boolean_t	interrupt;

	/*
	 * Get anything valuable off user stack first.
	 */
	save_regs2 = *regs->usp;

	/*
	 * Get the signal to take from the server.  It also
	 * switches the signal mask and the stack, so we must
	 * be off the old user stack before calling it.
	 */
	(void) bsd_take_signal(our_bsd_server_port,
			&interrupt,
			&old_mask,
			&old_onstack,
			&sig,
			&code,
			&handler,
			&new_sp);

	/*
	 * If there really were no signals to take, return.
	 */
	if (sig == 0)
	    return;

	/*
	 * Put the signal context and signal frame on the signal stack.
	 */
	if (new_sp == 0) {
	    /*
	     * Build signal frame and context on user's stack.
	     */
	    new_sp = (int)regs->usp;
	}

	scp = ((struct sigcontext *)new_sp) - 1;
	fp  = ((struct sigframe *)scp) - 1;

	/*
	 * Build the argument list for the signal handler.
	 */
	fp->sf_signum = sig;
	fp->sf_code = code;
	fp->sf_scp = scp;
	fp->sf_handler = (int (*)()) handler;

	/*
	 * Build the calls argument frame to be used to call sigreturn.
	 */
	fp->sf_argcount = 1;
	fp->sf_scpcopy = scp;

	/*
	 * Build the signal context to be used by sigreturn.
	 */
	scp->sc_onstack = old_onstack;
	scp->sc_mask = old_mask;
	scp->sc_sp = (int)(regs->usp+1);	/* user sp after return */
	scp->sc_fp = regs->fp;
	scp->sc_ap = regs->ap;
	scp->sc_pc = save_regs2.pc;
	scp->sc_ps = save_regs2.psl;

	/*
	 * Set up the new stack and handler addresses.
	 */
	regs2 = ((struct emul_regs_2 *)fp) - 1;
	*regs2 = save_regs2;
	regs->usp = regs2;
	regs2->psl &= ~(PSL_CM|PSL_FPD);
	regs2->pc = SIGCODE_LOC;	/* trampoline code */

}

/*
 * New sigreturn.
 */
int
e_sigreturn(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	struct a {
	    struct sigcontext *sigcp;
	} 			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	struct sigcontext	sc;
	struct emul_regs_2	save_regs2;
	register struct emul_regs_2	*regs2;
	register int		rc;

	/*
	 * Copy in signal context and regs2.
	 */
	sc = *argp->sigcp;
	save_regs2 = *regs->usp;

	/*
	 * Validate return PSL.
	 */
	if ((sc.sc_ps & (PSL_MBZ|PSL_IPL|PSL_IS)) != 0 ||
	    (sc.sc_ps & (PSL_PRVMOD|PSL_CURMOD)) != (PSL_PRVMOD|PSL_CURMOD) ||
	    ((sc.sc_ps & PSL_CM) &&
	     (sc.sc_ps & (PSL_FPD|PSL_DV|PSL_FU|PSL_IV)) != 0)) {
	    return (EINVAL);
	}

	/*
	 * Change signal stack and mask.  If new signals are pending,
	 * do not take them until we switch user stack.
	 */
#ifdef	MAP_UAREA
    if (shared_enabled) {
	rc = e_shared_sigreturn(serv_port,
				interrupt,
				sc.sc_onstack & 01,
				sc.sc_mask);

    } else {
#endif	MAP_UAREA
	rc = bsd_sigreturn(serv_port,
			interrupt,
			sc.sc_onstack & 01,
			sc.sc_mask);

#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA
	/*
	 * Change registers.
	 */
	regs2 = ((struct emul_regs_2 *)sc.sc_sp) - 1;

	*regs2 = save_regs2;
	regs->ap  = sc.sc_ap;
	regs->fp  = sc.sc_fp;
	regs->usp  = regs2;
	regs2->pc  = sc.sc_pc;
	regs2->psl = sc.sc_ps;

	return (E_JUSTRETURN);
}

/*
 * Compatibility with BSD4.2 chmk $139 used by longjmp().
 */
int
e_osigcleanup(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct sigcontext	*scp;
	struct emul_regs_2		save_regs2;
	register struct emul_regs_2	*regs2;
	register int			rc;
	int				old_onstack,
					old_mask,
					old_sp;

	/*
	 * Copy in signal context and regs2.  Pointer to sigcontext is
	 * on top of user stack (unlike all other syscalls).
	 */
	save_regs2 = *regs->usp;
	scp = *((struct sigcontext **)(regs->usp + 1));
	old_onstack = scp->sc_onstack;
	old_mask    = scp->sc_mask;
	old_sp      = scp->sc_sp;

	/*
	 * Change signal stack and mask.  If new signals are pending,
	 * do not take them until we switch user stack.
	 */
#ifdef	MAP_UAREA
    if (shared_enabled) {
	rc = e_shared_sigreturn(serv_port, interrupt, 
				old_onstack & 01, old_mask);
    } else {
#endif	MAP_UAREA
	rc = bsd_sigreturn(serv_port, interrupt, old_onstack & 01, old_mask);
#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA

	/*
	 * Move registers to old stack.  Only change stack pointer.
	 */
	regs2 = ((struct emul_regs_2 *)old_sp) - 1;

	*regs2 = save_regs2;
	regs->usp = regs2;

	return (E_JUSTRETURN);
}

/*
 * Wait has a weird parameter passing mechanism.
 */
int
e_wait(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->usp;

	int	new_args[2];

	if ((regs2->psl & PSL_ALLCC) == PSL_ALLCC) {
	    new_args[0] = regs2->r0;	/* options */
	    new_args[1] = regs2->r1;	/* rusage_p */
	}
	else {
	    new_args[0] = 0;
	    new_args[1] = 0;
	}
	return (emul_vproc_generic(serv_port, interrupt,
			SYS_wait, &new_args[0], rval));
}

int
e_fork(serv_port, interrupt, argp, rval, regs)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			*argp;
	int			*rval;
	struct emul_regs	*regs;
{
	register struct emul_regs_2 *regs2 = regs->usp;
	register int error;

	struct vax_thread_state	child_regs;
	
	extern int	child_fork();

	/*
	 * Set up registers for child.  It resumes on its own stack.
	 */
	child_regs.r0  = regs2->r0;
	child_regs.r1  = regs2->r1;
	child_regs.r2  = regs2->r2;
	child_regs.r3  = regs2->r3;
	child_regs.r4  = regs2->r4;
	child_regs.r5  = regs2->r5;
	child_regs.r6  = regs->r6;
	child_regs.r7  = regs->r7;
	child_regs.r8  = regs->r8;
	child_regs.r9  = regs->r9;
	child_regs.r10 = regs->r10;
	child_regs.r11 = regs->r11;
	child_regs.ap  = regs->ap;
	child_regs.fp  = regs->fp;
	child_regs.sp  = (int)regs->usp;
	child_regs.pc  = (int)child_fork;	/* not a procedure */
	child_regs.ps  = regs2->psl;
	
	/*
	 * Create the child.
	 */
	return( e_fork_call(serv_port,
		interrupt,
		(thread_state_t)&child_regs,
		VAX_THREAD_STATE_COUNT,
		rval));
}

vm_offset_t
set_arg_addr(arg_size)
	vm_size_t	arg_size;
{
	/*
	 * Round argument size to fullwords
	 */
	arg_size = (arg_size + NBPW - 1) & ~(NBPW - 1);

	/*
	 * Put argument list at top of stack.
	 */
	return (USRSTACK - arg_size);
}

