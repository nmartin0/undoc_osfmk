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
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: signal.h,v $
 * Revision 1.7  1995/02/01  21:37:23  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:41:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:27:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/04/03  03:07:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/01/09  00:05:04  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.3  1992/12/17  19:55:28  cfj
 * Use __i860__ instead of just i860 in ifdefs.
 *
 * Revision 1.2  1992/11/30  22:43:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:19  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/02/17  14:43:33  klh
 * For OSF merge, update version # to match LCC #
 *
 * Revision 2.4  92/02/11  22:06:35  pjg
 * 	Add SIGMIGRATE signal (TNC only) (roman@locus).
 * 
 * Revision 2.3  91/11/13  14:44:14  rabii
 * 	Added i860 support
 * 
 *
 * [Mon Oct 21 16:19:28 PDT 1991] cfj@ssd.intel.com
 *      Added i860 signal context definitions.
 *
 * Revision 2.2  91/08/31  14:12:08  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:45:05  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.5.2  91/06/20  09:24:02  hermi
 * 	Copied sig_atomic_t fix from osc1.1 build.
 * 	Also moved SecureWare copyright.
 * 	 bug 1439: removed volatile from sig_atomic_t typedef.
 * 	 [91/01/02  10:56:56  dwm]
 * 	[91/06/20  09:17:59  hermi]
 * 
 * Revision 1.12  90/10/07  14:53:57  devrcs
 * 	moved sigprocmask() flags from ANSI C to POSIX section
 * 	[90/10/04  17:06:24  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:44:49  gm]
 * 
 * Revision 1.11  90/08/24  12:27:14  devrcs
 * 	HP/Apollo M68K
 * 	[90/08/13  17:37:30  mcg]
 * 
 * 	Removed unsupported signals: SIGCLD, SIGPWR, SIGDANGER, SIGMSG, SIGEMSG.
 * 	[90/08/07  14:47:20  coren]
 * 
 * 	Removed unsupported signals: SIGCLD, SIGPWR, SIGDANGER, SIGMSG, SIGEMSG.
 * 	[90/08/07  14:47:20  coren]
 * 
 * Revision 1.10  90/07/27  09:07:52  devrcs
 * 	Put definition of Mach IPC signals under #if.
 * 	Fixed SIGISMEMBER macro.
 * 	[90/07/17  13:00:23  coren]
 * 
 * 	Fixed merge problem.
 * 	[90/07/14  10:07:41  jeffc]
 * 
 * 	Fixed modifiable lvalue problem.
 * 	[90/07/13  16:54:18  jeffc]
 * 
 * Revision 1.9  90/07/17  11:42:27  devrcs
 * 	Removed _NONSTD_TYPES declarations.
 * 	[90/07/07  21:36:36  gm]
 * 
 * Revision 1.8  90/07/05  23:14:28  devrcs
 * 	Add support for per thread synchronous signals
 * 	[90/06/29  09:15:21  sp]
 * 
 * Revision 1.7  90/06/22  20:54:00  devrcs
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:46:22  seiden]
 * 
 * Revision 1.6  90/05/24  23:06:49  devrcs
 * 	Added STREAMS version of SIGIO: SIGPOLL.
 * 	[90/05/18  13:10:58  nolting]
 * 
 * Revision 1.5  90/04/27  19:27:08  devrcs
 * 	Clone SIG_IGN, etc under _NONSTD_TYPES with _NO_PROTO too!
 * 	[90/04/20  18:18:58  tmt]
 * 
 * 	Changes for ANSI compliance.
 * 	[90/04/19  08:23:56  coren]
 * 
 * 	Fixed order of fields in sigaction structure.
 * 	[90/04/16  10:33:47  coren]
 * 
 * Revision 1.4  90/03/13  20:49:46  mbrown
 * 	getting sigvec right!
 * 	[90/02/14  15:14:43  mbrown]
 * 
 * 	Moved location of sigvec struct.
 * 	[90/02/12  18:27:08  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/09  18:04:03  tom]
 * 
 * Revision 1.3  90/02/16  16:52:36  devrcs
 * 	Changed kernel back to being more POSIX like.
 * 	[90/02/14  21:59:00  coren]
 * 
 * 	Changed remaining void declarations to be int unless _POSIX_SOURCE.
 * 	[90/02/13  21:11:35  coren]
 * 
 * 	Change non-POSIX BSD code back to being non-POSIX.
 * 	[90/02/13  14:45:07  coren]
 * 
 * 	Fixed typo in signal declaration.
 * 	[90/02/13  11:22:04  coren]
 * 
 * 	Upgraded for POSIX 1003.1 compliance.
 * 	[90/01/25  18:52:59  coren]
 * 
 * Revision 1.2  90/01/03  12:43:58  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:39  gm]
 * 
 * $EndLog$
 */
/* @(#)signal.h	2.2 10:41:13 5/9/90 SecureWare */
/* @(#)signal.h 1.33  com/inc/sys,3.1,9013 3/8/90 13:48:54 */
/*
 * COMPONENT_NAME: SYSPROC - signal.h
 *                                                                    
 * ORIGIN: 27
 *
 * Copyright International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   


#ifndef _SYS_SIGNAL_H_
#define _SYS_SIGNAL_H_

#include <standards.h>

/*
 *
 *      The ANSI standard requires that certain values be in signal.h.
 *	The ANSI standard allows additional signals and pointers to 
 *	undeclarable functions with macro definitions beginning with
 * 	the letters SIG or SIG_ and an upper case letter.
 *      However, it also requires that if _ANSI_C_SOURCE is defined then 
 *      no other function definitions are present
 *
 *      This header includes all the ANSI required entries.  In addition
 *      other entries for the OSF system are included.
 *
 */


#ifdef _ANSI_C_SOURCE

#if	!defined(_KERNEL) && !defined(ASSEMBLER)
#if   defined (_NO_PROTO)
extern void (*signal())();
#else   /* ~_NO_PROTO */
extern void (*signal(int, void (*func)(int)))(int);
#endif  /* _NO_PROTO */

#ifdef _NO_PROTO
extern int raise();
#else /* ~ _NO_PROTO */
extern int raise(int );
#endif /* _NO_PROTO */

typedef int sig_atomic_t; /* accessable as an atomic entity (ANSI) */

#endif /* !_KERNEL && !ASSEMBLER */

/*
 * valid signal values: all undefined values are reserved for future use 
 * note: POSIX requires a value of 0 to be used as the null signal in kill()
 */
#define	SIGHUP	   1	/* hangup, generated when terminal disconnects */
#define	SIGINT	   2	/* interrupt, generated from terminal special char */
#define	SIGQUIT	   3	/* (*) quit, generated from terminal special char */
#define	SIGILL	   4	/* (*) illegal instruction (not reset when caught)*/
#define	SIGTRAP	   5	/* (*) trace trap (not reset when caught) */
#define	SIGABRT    6	/* (*) abort process */
#define SIGEMT	   7	/* EMT instruction */
#define	SIGFPE	   8	/* (*) floating point exception */
#define	SIGKILL	   9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	  10	/* (*) bus error (specification exception) */
#define	SIGSEGV	  11	/* (*) segmentation violation */
#define	SIGSYS	  12	/* (*) bad argument to system call */
#define	SIGPIPE	  13	/* write on a pipe with no one to read it */
#define	SIGALRM	  14	/* alarm clock timeout */
#define	SIGTERM	  15	/* software termination signal */
#define	SIGURG 	  16	/* (+) urgent contition on I/O channel */
#define	SIGSTOP	  17	/* (@) stop (cannot be caught or ignored) */
#define	SIGTSTP	  18	/* (@) interactive stop */
#define	SIGCONT	  19	/* (!) continue (cannot be caught or ignored) */
#define SIGCHLD   20	/* (+) sent to parent on child stop or exit */
#define SIGTTIN   21	/* (@) background read attempted from control terminal*/
#define SIGTTOU   22	/* (@) background write attempted to control terminal */
#define SIGIO	  23	/* (+) I/O possible, or completed */
#define SIGXCPU	  24	/* cpu time limit exceeded (see setrlimit()) */
#define SIGXFSZ	  25	/* file size limit exceeded (see setrlimit()) */
#define SIGVTALRM 26	/* virtual time alarm (see setitimer) */
#define SIGPROF   27	/* profiling time alarm (see setitimer) */
#define SIGWINCH  28	/* (+) window size changed */
#define SIGINFO   29    /* information request */
#define SIGUSR1   30	/* user defined signal 1 */
#define SIGUSR2   31	/* user defined signal 2 */
#ifdef 	TNC
#define SIGMIGRATE 32	/* tell task to migrate */
#endif	/* TNC */
/*
 * additional signal names supplied for compatibility, only 
 */
#define SIGIOINT SIGURG	/* printer to backend error signal */
#define SIGAIO	SIGIO	/* base lan i/o */
#define SIGPTY  SIGIO	/* pty i/o */
#define	SIGPOLL	SIGIO	/* STREAMS version of this signal */
#define SIGIOT  SIGABRT /* abort (terminate) process */ 
#define	SIGLOST	SIGIOT	/* old BSD signal ?? */
/*
 * valid signal action values; other values => pointer to handler function 
 */
#ifndef	_NO_PROTO
#define SIG_ERR		(void (*)(int))-1
#define	SIG_DFL		(void (*)(int))0
#define	SIG_IGN		(void (*)(int))1
#define SIG_HOLD        (void (*)(int))2        /* not valid as argument
                                                   to sigaction or sigvec */
#define SIG_CATCH       (void (*)(int))3        /* not valid as argument
                                                   to sigaction or sigvec */
#else	/* _NO_PROTO */
#define SIG_ERR		(void (*)())-1
#define	SIG_DFL		(void (*)())0
#define	SIG_IGN		(void (*)())1
#define SIG_HOLD        (void (*)())2           /* not valid as argument
                                                   to sigaction or sigvec */
#define SIG_CATCH       (void (*)())3           /* not valid as argument
                                                   to sigaction or sigvec */
#endif	/* _NO_PROTO */

#endif /* _ANSI_C_SOURCE */

/*
 *   The following are values that have historically been in signal.h.
 *
 *   They are a part of the POSIX defined signal.h and therefore are
 *   included when _POSIX_SOURCE is defined.
 *
 */

#ifdef _POSIX_SOURCE

/*
 * values of "how" argument to sigprocmask() call
 */
#define SIG_BLOCK	1
#define SIG_UNBLOCK	2
#define SIG_SETMASK	3

#include <sys/types.h>	/* Cannot be in ANSI - name space pollution */

/*
 * valid flag define for sa_flag field of sigaction structure - POSIX
 */
#define SA_NOCLDSTOP	0x00000004	/* do not set SIGCHLD for child stops*/

#ifndef ASSEMBLER
/*
 * sigaction structure used in sigaction() system call 
 * The order of the fields in this structure must match those in
 * the sigvec structure (below).
 */
struct sigaction {
#ifdef _NO_PROTO
	void	(*sa_handler)();	/* signal handler, or action value */
#else	/* _NO_PROTO */
	void	(*sa_handler)(int);	/* signal handler, or action value */
#endif	/* _NO_PROTO */
	sigset_t sa_mask;		/* signals to block while in handler */
	int	sa_flags;		/* signal action flags */
};


#ifndef _KERNEL
#ifdef _NO_PROTO
extern int kill();
extern int sigaction();
extern int sigprocmask();
extern int sigsuspend();
extern int sigemptyset();
extern int sigfillset();
extern int sigaddset();
extern int sigdelset();
extern int sigismember();
extern int sigpending();
#else	/* _NO_PROTO */

/*
 * function prototypes for signal functions 
 */
/* system calls */
extern int kill(pid_t , int ); 
extern int sigaction(int , struct sigaction *, struct sigaction *); 
extern int sigprocmask(int , sigset_t *, sigset_t *);
extern int sigsuspend(sigset_t *);  
/* library routines */
extern int sigemptyset(sigset_t *);
extern int sigfillset(sigset_t *);
extern int sigaddset(sigset_t *, int );
extern int sigdelset(sigset_t *, int );
extern int sigismember(sigset_t *, int );
extern int sigpending(sigset_t *);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#ifndef lint
#ifdef _KERNEL
/*
 * These are library routines.  Let the kernel use these until they get
 * their POSIX story straight.
 */
#define sigemptyset(set)        { *(set) = 0; }
#define sigfillset(set)         { *(set) = ~(sigset_t)0; }
#define sigaddset(set, signo)   ( *(set) |= 1 << ((signo) - 1), 0)
#define sigdelset(set, signo)   ( *(set) &= ~(1 << ((signo) - 1)), 0)
#define sigismember(set, signo) ( (*(set) & (1 << ((signo) - 1))) != 0)
#endif /* _KERNEL */
#endif /* lint */

#endif 	/* ASSEMBLER */

#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE


/*---------------------------------------------------------------*/
/* include file needed for Floating Point emulation. We can take */
/* this out when we get real hardware.  			 */
/*---------------------------------------------------------------*/
#ifdef _IBMRT
#include <ieeetrap.h>
#endif /* _IBMRT */

/*
 * Macro for converting signal number to a mask suitable for
 * sigblock().
 */
#define sigmask(m)	(1 << ((m)-1))

#define sigcantmask     (sigmask(SIGKILL)|sigmask(SIGSTOP))

#ifdef	OSF1_SERVER
#define	cantmask	(sigcantmask | sigmask(SIGCONT))
#define	stopsigmask	(sigmask(SIGSTOP)|sigmask(SIGTSTP)| \
			sigmask(SIGTTIN)|sigmask(SIGTTOU))
#endif	OSF1_SERVER

/*
 * sigvec structure used in sigvec compatibility interface.
 * The order of the fields in this structure must match those in
 * the sigaction structure (above).
 */
#ifndef ASSEMBLER
struct	sigvec {
#ifdef _NO_PROTO
	void	(*sv_handler)();	/* signal handler */
#else	/* _NO_PROTO */
	void	(*sv_handler)(int);	/* signal handler */
#endif	/* _NO_PROTO */
	int     sv_mask;        /* signal mask to apply */
	int     sv_flags;    
};                           
#endif	/* ASSEMBLER */
#define sv_onstack sv_flags     /* isn't compatibility wonderful! */

/*
 * function prototypes for signal functions 
 */

#ifndef _KERNEL
#ifdef _NO_PROTO
extern int sigvec();
#else	/* _NO_PROTO */
extern int sigvec(int, struct sigvec *, struct sigvec *);
#endif	/* _NO_PROTO */
#endif	/* !_KERNEL */


/*
 *      signals delivered on a per-thread basis.
 */
#ifdef _KERNEL
#define threadmask                                              \
        (sigmask(SIGILL)|sigmask(SIGTRAP)|sigmask(SIGIOT)|      \
         sigmask(SIGEMT)|sigmask(SIGFPE)|sigmask(SIGBUS)|       \
         sigmask(SIGSEGV)|sigmask(SIGSYS)|sigmask(SIGPIPE))

#define thread_signal_disposition(signo) (sigmask(signo)&threadmask)

#define signal_disposition(signo) (*((thread_signal_disposition(signo) ? u.u_tsignal : u.u_signal)+signo))

/*
 * get signal action for process and signal; currently only for current process
 */
#define SIGACTION(p, sig)       (signal_disposition(sig)))

#endif /* _KERNEL */

/*
 * sigstack structure used in sigstack() system call 
 */

#ifndef ASSEMBLER
struct  sigstack {
        char    *ss_sp;                 /* signal stack pointer */
        int     ss_onstack;             /* current status */
};

/*
 * Information pushed on stack when a signal is delivered.
 * This is used by the kernel to restore state following
 * execution of the signal handler.  It is also made available
 * to the handler to allow it to properly restore state if
 * a non-standard exit is performed.
 */
struct  sigcontext {
        int     sc_onstack;             /* sigstack state to restore */
        int     sc_mask;                /* signal mask to restore */
#ifdef  vax
        int     sc_sp;                  /* sp to restore */
        int     sc_fp;                  /* fp to restore */
        int     sc_ap;                  /* ap to restore */
        int     sc_pc;                  /* pc to restore */
        int     sc_ps;                  /* psl to restore */
#endif  /* vax */
#if     defined(mc68000) || defined(__mc68000)
        int     sc_sp;                  /* sp to restore */
        int     sc_pc;                  /* pc to restore */
        int     sc_ps;                  /* psl to restore */
#endif  /* mc68000 */
#ifdef  multimax
        int     sc_r0;                  /* r0 to restore */
        int     sc_r1;                  /* r1 to restore */
        int     sc_r2;                  /* r2 to restore */
        int     sc_sp;                  /* sp to restore */
        int     sc_pc;                  /* pc to restore */
        int     sc_ps;                  /* psl to restore */
#endif  /* multimax */
#ifdef  balance
        int     sc_sp;                  /* sp to restore */
        int     sc_modpsr;              /* mod/psr to restore */
        int     sc_sp;                  /* sp to restore */
        int     sc_pc;                  /* pc to restore */
        int     sc_ps;                  /* psl to restore */
#endif  /* balance */
#ifdef  ibmrt
#define MAXSIGREGS      33      /* (INFO-R0+1) num regs sent to sighndlr */
        char    *sc_floatsave;          /* -> (struct floatsave *) */
        int     sc_sp;                  /* sp to restore */
        int     sc_fp;                  /* fp to restore */
        int     sc_ap;                  /* ap to restore */
        int     sc_iar;                 /* ibmrt iar equivalent */
        int     sc_icscs;               /* ibmrt psl equivalent */
#define sc_pc   sc_iar
#define sc_ps   sc_icscs
        int     sc_saveiar;             /* saved copy of IAR for RTFL */
        int     sc_regs[MAXSIGREGS];    /* must be after sc_icscs */
#define sc_flags        sc_onstack
#define SC_ONSTACK      SV_ONSTACK      /* sc_flags: on signal stack */
#define SC_FLOATSAVE    0x8000          /* sc_flags: floating pt saved */
#define SC_EXCEPTION    0x4000          /* sc_flags: excpt packets saved */
#define SC_RTFL         0x2000          /* sc_flags: executing RTFL seq */
#endif  /* ibmrt */
#ifdef  i386
#define MAXSIGREGS      19
        int     sc_gs;
        int     sc_fs;
        int     sc_es;
        int     sc_ds;
        int     sc_edi;
        int     sc_esi;
        int     sc_ebp;
        int     sc_esp;
        int     sc_ebx;
        int     sc_edx;
        int     sc_ecx;
        int     sc_eax;
        int     sc_trapno;      /* err & trapno keep the context */
        int     sc_err;         /* switching code happy */
        int     sc_eip;
        int     sc_cs;
        int     sc_efl;
        int     sc_uesp;
        int     sc_ss;
#endif  /* i386 */
#ifdef  mips
        int     sc_pc;                  /* pc at time of signal */
        /*
         * General purpose registers
         */
        int     sc_regs[32];    /* processor regs 0 to 31 */
        int     sc_mdlo;        /* mul/div low */
        int     sc_mdhi;        /* mul/div high */
        /*
         * Floating point coprocessor state
         */
        int     sc_ownedfp;     /* fp has been used */
        int     sc_fpregs[32];  /* fp regs 0 to 31 */
        int     sc_fpc_csr;     /* floating point control and status reg */
        int     sc_fpc_eir;     /* floating point exception instruction reg */
        /*
         * END OF REGION THAT MUST AGREE WITH setjmp.h
         */
        /*
         * System coprocessor registers at time of signal
         */
        int     sc_cause;       /* cp0 cause register */
        int     sc_badvaddr;    /* cp0 bad virtual address */
        int     sc_badpaddr;    /* cpu bd bad physical address */
#endif  /* mips */
#ifdef sparc
        int     sc_sp;                  /* sp to restore */
        int     sc_pc;                  /* pc to retore */
        int     sc_npc;                 /* next pc to restore */
        int     sc_psr;                 /* psr to restore */
        int     sc_g1;                  /* register that must be restored */
        int     sc_o0;
#endif /* sparc */
#ifdef __i860__
	/*
	 * DON'T EVEN THINK ABOUT REARRANGING THIS WITHOUT CONSULTING
	 * emul_vector.s and sys/signal.h !!
	 */
	int	sc_regs[32];	/* integer registers 0 thru 31 */
#define sc_r0	sc_regs[0]	/* always 0 */
#define sc_r1	sc_regs[1]	/* user's return address */
#define sc_sp	sc_regs[2]	/* stack pointer */
#define sc_fp	sc_regs[3]	/* frame pointer */
#define sc_r4	sc_regs[4]	/* non-volatile */
#define sc_r5	sc_regs[5]	/* non-volatile */
#define sc_r6	sc_regs[6]	/* non-volatile */
#define sc_r7	sc_regs[7]	/* non-volatile */
#define sc_r8	sc_regs[8]	/* non-volatile */
#define sc_r9	sc_regs[9]	/* non-volatile */
#define sc_r10	sc_regs[10]	/* non-volatile */
#define sc_r11	sc_regs[11]	/* non-volatile */
#define sc_r12	sc_regs[12]	/* non-volatile */
#define sc_r13	sc_regs[13]	/* non-volatile */
#define sc_r14	sc_regs[14]	/* non-volatile */
#define sc_r15	sc_regs[15]	/* non-volatile */
#define sc_r16	sc_regs[16]	/* syscall param  1 */
#define sc_r17	sc_regs[17]	/* syscall param  2 */
#define sc_r18	sc_regs[18]	/* syscall param  3 */
#define sc_r19	sc_regs[19]	/* syscall param  4 */
#define sc_r20	sc_regs[20]	/* syscall param  5 */
#define sc_r21	sc_regs[21]	/* syscall param  6 */
#define sc_r22	sc_regs[22]	/* syscall param  7 */
#define sc_r23	sc_regs[23]	/* syscall param  8 */
#define sc_r24	sc_regs[24]	/* syscall param  9 */
#define sc_r25	sc_regs[25]	/* syscall param 10 */
#define sc_r26	sc_regs[26]	/* syscall param 11 */
#define sc_r27	sc_regs[27]	/* syscall param 12 */
#define sc_r28	sc_regs[28]	/* for structure passing or spills */
#define sc_r29	sc_regs[29]	/* emulator will return thru here */
#define sc_r30	sc_regs[30]	/* volatile */
#define sc_r31	sc_regs[31]	/* system call number */
	int	sc_fpregs[8];	/* 8 non-volatile floating point regs */
#define sc_f0	sc_fpregs[0]	/* always 0 */
#define sc_f1	sc_fpregs[1]	/* always 0 */
#define sc_f2	sc_fpregs[2]	/* non-volatile */
#define sc_f3	sc_fpregs[3]	/* non-volatile */
#define sc_f4	sc_fpregs[4]	/* non-volatile */
#define sc_f5	sc_fpregs[5]	/* non-volatile */
#define sc_f6	sc_fpregs[6]	/* non-volatile */
#define sc_f7	sc_fpregs[7]	/* non-volatile */
	int	sc_psr;		/* processor status reg */
	int	sc_fsr;		/* floating-point status reg */
	int	sc_epsr;	/* extended processor status reg */
	int	sc_fir;		/* pc, sometimes... */ 
#endif /* __i860__ */
};
#endif  /* ASSEMBLER */



/*
 * valid signal action values; other values => pointer to handler function 
 */
#define BADSIG		SIG_ERR
/*
 * valid flags define for sa_flag field of sigaction structure 
 */
#define	SA_ONSTACK	0x00000001	/* run on special signal stack */
#define SA_RESTART	0x00000002	/* restart system calls on sigs */

/*
 * flag bits defined for parameter to psig and sendsig; bits indicate
 *  what context (where and how much) should be saved on signal delivery
 */       
#define	NO_VOLATILE_REGS	0x0001
#define	USE_SAVE_AREA		0x0002
/* 
 * macros to manipulate signal sets
 */
#define	SIGINITSET(set)		(set) = (sigset_t)0;

#define	SIGMASK(s)		sigmask(s)

#define	SIGEMPTYSET(set)	(set) = (sigset_t)0;

#define	SIGFILLSET(set)		(set) = (sigset_t)~0;

#define SIGDELSET(set, s)	(set) &= ~SIGMASK(s);

#define SIGADDSET(set, s)	(set) |= SIGMASK(s);

#define SIGISMEMBER(set,s)	(((set) & SIGMASK(s)) != 0)

#define SIGMASKSET(dest, mask)	((dest) &= ~(mask))

#define SIGORSET(dest, mask)	((dest) |= (mask))


/*
 * values in sv_onstack are interpreted identically to values in
 * sa_onstack for sigaction();  however, the following additional
 * names are defined for values in sv_onstack to be compatible with
 * old usage of sigvec() 
 */
#define NSIG	32			/* maximum number of signals */
#define SIG_STK		0x00000001	/* bit for using sigstack stack */
#define SIG_STD		0x00000002	/* bit for old style signals */
#define SV_ONSTACK	SA_ONSTACK  /* take signal on signal stack */
#define SV_INTERRUPT	SA_RESTART /* do not restart system on signal return */

#define _OLDSTYLE (2)
#define _ONSTACK  (1)
#define _teststyle(n)   ((n) & _OLDSTYLE) /** TRUE if Bell style signals. **/
#define _testonstack(n) ((n) & _ONSTACK)  /** TRUE if on user-sig stack.  **/
#define _setoldstyle(n) ((n) | _OLDSTYLE)
#define _setnewstyle(n) ((n) & ~_OLDSTYLE)
#define _setonstack(n)  ((n) | _ONSTACK)
#define _clronstack(n)  ((n) & ~_ONSTACK)

#define SIGMAX	NSIG

#if     defined(mc68000) || defined(__mc68000)
#define     ILL_ILLINSTR_FAULT  0x10    /* illegal instruction fault */
#define     ILL_PRIVVIO_FAULT   0x20    /* privilege violation fault */
#define     ILL_COPROCERR_FAULT 0x34    /* [coprocesser protocol error fault] */
#define     ILL_TRAP1_FAULT     0x84    /* trap #1 fault */
#define     ILL_TRAP2_FAULT     0x88    /* trap #2 fault */
#define     ILL_TRAP3_FAULT     0x8c    /* trap #3 fault */
#define     ILL_TRAP4_FAULT     0x90    /* trap #4 fault */
#define     ILL_TRAP5_FAULT     0x94    /* trap #5 fault */
#define     ILL_TRAP6_FAULT     0x98    /* trap #6 fault */
#define     ILL_TRAP7_FAULT     0x9c    /* trap #7 fault */
#define     ILL_TRAP8_FAULT     0xa0    /* trap #8 fault */
#define     ILL_TRAP9_FAULT     0xa4    /* trap #9 fault */
#define     ILL_TRAP10_FAULT    0xa8    /* trap #10 fault */
#define     ILL_TRAP11_FAULT    0xac    /* trap #11 fault */
#define     ILL_TRAP12_FAULT    0xb0    /* trap #12 fault */
#define     ILL_TRAP13_FAULT    0xb4    /* trap #13 fault */
#define     ILL_TRAP14_FAULT    0xb8    /* trap #14 fault */
#else   /* mc68000 */
#define     ILL_RESAD_FAULT     0x0     /* reserved addressing fault */
#define     ILL_PRIVIN_FAULT    0x1     /* privileged instruction fault */
#define     ILL_RESOP_FAULT     0x2     /* reserved operand fault */
#endif  /* mc68000 */
#ifdef  multimax
#define     ILL_FLAG_TRAP       0x3     /* flag trap taken */
#endif  /* multimax */
#ifdef  mips
#define     ILL_COPR_UNUSABLE   0x3     /* coprocessor unusable */
#endif  /* mips */

#if     defined(mc68000) || defined(__mc68000)
#define     EMT_EMU1010         0x28    /* line 1010 emulator trap */
#define     EMT_EMU1111         0x2c    /* line 1111 emulator trap */
#define     FPE_INTDIV_TRAP     0x14    /* integer divide by zero */
#define     FPE_CHKINST_TRAP    0x18    /* CHK [CHK2] instruction */
#define     FPE_TRAPV_TRAP      0x1c    /* TRAPV [cpTRAPcc TRAPcc] instr */
#define     FPE_FLTBSUN_TRAP    0xc0    /* [branch or set on unordered cond] */
#define     FPE_FLTINEX_TRAP    0xc4    /* [floating inexact result] */
#define     FPE_FLTDIV_TRAP     0xc8    /* [floating divide by zero] */
#define     FPE_FLTUND_TRAP     0xcc    /* [floating underflow] */
#define     FPE_FLTOPERR_TRAP   0xd0    /* [floating operand error] */
#define     FPE_FLTOVF_TRAP     0xd4    /* [floating overflow] */
#define     FPE_FLTNAN_TRAP     0xd8    /* [floating Not-A-Number] */
#else   /* mc68000 */
#ifdef  sparc
#define     FPE_INTDIV_TRAP     0x14    /* integer divide by zero */
#define     FPE_FLTINEX_TRAP    0xc4    /* [floating inexact result] */
#define     FPE_FLTDIV_TRAP     0xc8    /* [floating divide by zero] */
#define     FPE_FLTUND_TRAP     0xcc    /* [floating underflow] */
#define     FPE_FLTOPERR_TRAP   0xd0    /* [floating operand error] */
#define     FPE_FLTOVF_TRAP     0xd4    /* [floating overflow] */
#else   /* sparc */
#define     FPE_INTOVF_TRAP     0x1     /* integer overflow */
#define     FPE_INTDIV_TRAP     0x2     /* integer divide by zero */
#define     FPE_FLTOVF_TRAP     0x3     /* floating overflow */
#define     FPE_FLTDIV_TRAP     0x4     /* floating/decimal divide by zero */
#define     FPE_FLTUND_TRAP     0x5     /* floating underflow */
#define     FPE_DECOVF_TRAP     0x6     /* decimal overflow */
#define     FPE_SUBRNG_TRAP     0x7     /* subscript out of range */
#define     FPE_FLTOVF_FAULT    0x8     /* floating overflow fault */
#define     FPE_FLTDIV_FAULT    0x9     /* divide by zero floating fault */
#define     FPE_FLTUND_FAULT    0xa     /* floating underflow fault */
#endif  /* sparc */
#endif  /* mc68000 */
#if     sun
#define     FPE_FPA_ENABLE      0x400   /* [FPA not enabled] */
#define     FPE_FPA_ERROR       0x404   /* [FPA arithmetic exception] */
#endif  /* sun */
#ifdef  multimax
#define     FPE_ILLINST_FAULT   0xb     /* Illegal FPU instruction */
#define     FPE_INVLOP_FAULT    0xc     /* Invalid operation */
#define     FPE_INEXACT_FAULT   0xd     /* Inexact result */
#define     FPE_OPERAND_FAULT   0xe     /* Operand fault        */
#endif  /* multimax */
#ifdef  sparc
#define     BUS_ALIGN           0x1     /* alignment error */
#define     EMT_TAG             0x0a    /* tag overflow */
#define     ILL_STACK           0x00    /* bad stack */
#endif  /* sparc */
#ifdef  mips
#define     FPE_UNIMP_FAULT     0xb     /* Unimplemented FPU instruction */
#define     FPE_INVALID_FAULT   0xc     /* Invalid operation */
#define     FPE_INEXACT_FAULT   0xd     /* Inexact result */
#endif  /* mips */



/*
 * Special signal number for intermediate signal handler.
 */
#ifdef  multimax
#define SIGCATCHALL 0x400
#endif  /* multimax */

/*
 * Codes for the mips break instruction.
 * (probably belong in a machine-specific header file)
 */
#ifdef  mips
#define BRK_USERBP      0       /* user bp (used by debuggers) */
#define BRK_KERNELBP    1       /* kernel bp (used by prom) */
#define BRK_ABORT       2       /* no longer used */
#define BRK_BD_TAKEN    3       /* for taken bd emulation */
#define BRK_BD_NOTTAKEN 4       /* for not taken bd emulation */
#define BRK_SSTEPBP     5       /* user bp (used by debuggers) */
#define BRK_OVERFLOW    6       /* overflow check */
#define BRK_DIVZERO     7       /* divide by zero check */
#define BRK_RANGE       8       /* range error check */
#endif  /* mips */

#ifndef _KERNEL
#ifdef  _NO_PROTO 
extern int sigblock();
extern int sigpause();
extern int sigreturn();
extern int sigsetmask();
extern int sigstack();
extern int siginterrupt();
#else
extern int sigblock(int);
extern int sigpause(int);
extern int sigreturn(struct sigcontext *);
extern int sigsetmask(int);
extern int sigstack(struct sigstack *, struct sigstack *);
extern int siginterrupt(int, int);
#endif  /* _NO_PROTO */
#endif 	/* ! _KERNEL */

#endif /* _OSF_SOURCE */
#endif /* _SYS_SIGNAL_H_ */
