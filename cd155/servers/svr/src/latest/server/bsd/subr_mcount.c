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
/* Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: subr_mcount.c,v $
 * Revision 1.5  1994/11/18  20:27:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:48:59  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:04:59  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 2.10  1993/04/15  22:47:30  condict
 * 	Various 860-specific changes to support server profiling.  Mainly
 * 	this concerns how to get self-pc and caller's pc into mcount.
 *
 * Revision 2.9  93/04/08  11:20:13  loverso
 * 	ux server threads are wired by default. (loverso)
 * 
 * Revision 2.8  93/03/22  23:58:04  condict
 * 	Partial implementation of i860 profiling.  More work needed.
 * 
 * Revision 2.7  93/01/07  11:16:17  condict
 * 	Replaced AD version of this file with latest version from MK.
 * 	[1992/12/04  14:19:00  condict]
 * 
 * Revision 1.4  1992/10/21  12:52:36  devrcs
 * 	Fixed up a merging mistake in the last submit.
 * 	[1992/09/25  18:33:39  emcmanus]
 *
 * 	Updated to new MK profiling interface.  Request no-more-senders
 * 	notification so we know when profiling has finished.
 * 	[1992/09/24  19:48:27  emcmanus]
 *
 * 	Fixed long descriptive comment to describe new mcount locking
 * 	scheme, and added changes for emulator profiling.
 * 	[1992/08/05  17:56:56  emcmanus]
 *
 * 	Greatly simplified design by getting rid of per-thread buffers and
 * 	instead introducing an mcount lock that is held for the minimum
 * 	amount of time.  This also requires non-profiled versions of mach
 * 	library functions.
 * 	[1992/06/10  17:47:02  emcmanus]
 *
 * Revision 1.2  1992/05/12  13:10:31  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/05  01:00:20  condict]
 * 
 * 	Enable i386 profiling under gcc
 * 	[90/02/26            rvb]
 * 
 * Revision 3.5  92/03/31  15:39:58  emcmanus
 * 	Set mcount thread name.  Check Mach return values.  Fix two thread exiting
 * 	bugs: tried to take a lock that was already taken, and forgot to destroy
 * 	the thread's mcount reply port on exit.
 * 
 * Revision 3.4  92/02/25  17:48:13  condict
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 
 * Revision 3.3  92/02/19  16:22:46  emcmanus
 * 	Deleted some old debugging code.
 * 
 * Revision 3.2  92/02/12  10:58:21  bernadat
 * 	Quit mcount if profiling is off
 * 
 * Revision 3.1  91/12/09  18:49:36  emcmanus
 * 	Major revision so that mcount works properly in the multi-threaded server.
 * 
 * Revision 3.0  91/09/27  11:48:45  emcmanus
 * 	Installed for profiling.
 * 
 * Revision 2.8  91/06/27  16:47:26  adn
 * 	Enable i386 Server profiling function.
 * 	[91/04/01            adn]
 * 
 * Revision 2.7  89/03/09  19:30:38  rpd
 * 	More cleanup.
 * 
 * Revision 2.6  89/02/25  14:45:40  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.5  89/02/09  04:32:50  mwyoung
 * 	Code cleanup cataclysm.
 * 
 * Revision 2.4  89/01/23  22:06:26  af
 * 	Changes for I386.
 * 		(But is this correct -- rvb)
 * 	[89/01/09            rvb]
 * 
 * 	Added Mips profiling code.
 * 	[89/01/08            af]
 * 
 * Revision 0.0  88/01/29            rvb
 * 	Sun must call mcount with frompc, and selfpc as args when we use
 * 	gcc since regs get reorganized.  The compiler would emit mcount
 * 	which would then call the "counting" code correctly.  BUT,
 * 	gprof knows enough to only subtract out the time accumulated by
 * 	mcount and not this new routine.  So for now I'll use pcc to build
 * 	this file.  The "correct mcount" is found in vax.s
 * 	[88/01/29            rvb]
 * 
 * Revision 0.0  87/12/14            rvb
 * 	Set up profiling for sun (68020) architecture.  And,
 * 	Fix splXXX calls so that we may call a special version, np_splXXX,
 * 	tp be sure not to profile the calls to splXXX in mcount.  This
 * 	is currently used only by the sun, and may go away totally some
 * 	day when the sun uses inline.
 * 	[87/12/14            rvb]
 * 
 * Revision 0.0  87/08/31            rvb
 * 	1) The label overflow did not splx(s) before it exited
 * 	2) "s=splhigh()" Does not work when s is static and the
 * 	expansion of s=splhigh() is smart enough to store the spl
 * 	directly into s.  An interrupt can now come and clobber s, before
 * 	the spl is set high.  This happens on the sequent.  So s must
 * 	either be a register (which we have no more of) or an automatic.
 * 	Whether this problem happens for your machine depends on how
 * 	s=splhigh() expands.
 * 	[87/08/31            rvb]
 * 
 * Revision 0.0  87/06/26            dlb
 * 	MULTIMAX: mcount --> mmax_mcount due to trampoline code.
 * 	[87/06/26            dlb]
 * 
 * Revision 0.0  87/04/02            avie
 * 	Fixed recent Sequent change to not return if we are not on the
 * 	master, instead, we must goto out (to allow Vaxen to rsb).  Also
 * 	moved the check for if we are profiling earlier in mcount to
 * 	attempt to curtail problems trying to profile bootstrapping code
 * 	(like trying to compare cpu_number against master_cpu before
 * 	setting either of them!).
 * 	[87/04/02            avie]
 * 
 * Revision 0.0  87/03/30            dlb
 * 	MULTIMAX changes -- a model for how to do this right.
 * 	[87/03/30            dlb]
 * 
 * Revision 0.0  87/03/20            rvb
 * 	munged to work on sequent.
 * 	[87/03/20            rvb]
 * 
 * Revision 0.0  87/02/27            sanzi
 * 	Added additional RT support for profiling.  Changes included
 * 	adding rt specific code under switch ibmrt and changing the value
 * 	of s_lowpc.
 * 	[87/02/27            sanzi]
 * 
 * Revision 0.0  86/05/29            jjc
 * 	Initialize "s_lowpc" to "start" for Sun.
 * 	[86/05/29            jjc]
 * 
 * Revision 0.0  86/02/14            bolosky
 * 	Added different definition of s_lowpc for Sailboat under switch
 * 	ibmrt.
 * 	[86/02/14            bolosky]
 * 
 * $EndLog$
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)subr_mcount.c	7.1 (Berkeley) 6/5/86
 */

/* last integrated from: gmon.c	4.10 (Berkeley) 1/14/83 */

/***
Multi-threaded mcount design.

When files are compiled with -pg, a call to mcount is inserted by
the compiler at the beginning of every function.  mcount records
the occurrence of this edge in the call graph (i.e., combination
of caller pc and callee pc), for later use by the gprof program.
Traditionally, it does this by finding or inserting the (caller,
callee) pair in a hash table.

In a multi-threaded program, more than one instance of mcount can be
active at a time, so concurrent access to the hash table must be
controlled.  We do this by taking a spinlock in the critical part of
the mcount function.  This code just adds the edge to a circular
buffer.  The work of merging these edges into the hash table is done
by a separate thread, mcount_thread, which wakes up periodically.
Thus the amount of time for which the lock is held is kept to a
minimum, which is important to avoid frequent collisions that would
distort the measurements.

The circular buffer has a head pointer, which is the first edge that
has not yet been transferred to the hash table, and a tail pointer,
which is where the next edge will be written.  The tail pointer is
only updated by mcount, where it is protected against concurrent
writes by the spinlock; the head pointer is only updated by
mcount_thread, which has no concurrency.  Since the tail is read by
mcount_thread and the head by mcount, there is an assumption that the
value read will be either the pre-increment or the post-increment
value.  If this assumption were not true, we could have mcount_thread
take the mcount lock too.

We have to make sure that mcount is not called at embarrassing times,
in particular recursively from itself.  Functions within this file do
not call mcount because of the way it is compiled.  We link with
versions of the cthreads and mach_sa libraries that have been compiled
with profiling, so we need unprofiled versions of any functions to be
called from here.  Currently, that means mach_msg and swtch_pri; the
libraries provide mach_msg_trap_noprof and swtch_pri_noprof.

The profiling status is controlled by the variable `profiling'.  This is
usually changed by tweaking it with the /etc/kgmon program (ugh).  To
detect such changes we check the value of `profiling' every time the
mcount_thread wakes up.  A change in status means that the microkernel
has to be told to start or stop sending server PC samples to us.

***/

#include <cputypes.h>
#include <sys/gprof.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <machine/cpu.h>
#include <machine/vmparam.h>
#include <uxkern/import_mach.h>
#include <kern/parallel.h>

int profiling = 3;
int oldprofiling = 3;
int emulator_profiling = 3, emulator_oldprofiling = 3;

#if	!defined(mips)

/*
 * Froms is actually a bunch of unsigned shorts indexing tos
 */
u_short *froms;
struct tostruct *tos = 0;
long tolimit = 0;
u_short *emulator_froms;
struct tostruct *emulator_tos;

#ifdef	i386
char	*s_lowpc = (char *) 0x0;
#define TOSFRACTION	100
#endif	i386

#ifdef	i860
char	*s_lowpc = (char *) 0x0;
#define TOSFRACTION	100
#endif	i860

#ifdef	vax
char	*s_lowpc = (char *)0x80000000;
#define TOSFRACTION	100
#endif	vax

#ifdef	ibmrt
char    *s_lowpc = (char *)0xe0000000;
#define TOSFRACTION	200
#endif	ibmrt

#ifdef	sun
char	*s_lowpc = (char *)0x0e000000;
#define TOSFRACTION	100
#endif	sun

#ifdef	ns32000
char 	*s_lowpc = (char *) 0x0;
#define TOSFRACTION	100
#endif	ns32000

#if	defined(sun)
#define spl_high()	np_splhigh()
#define spl_x(x)	np_splx(x)
#else	defined(sun)
#if	defined(i386)
#define spl_high()	splhigh()
#define spl_x(x)	splx(x)
#else	defined(i386)
#define spl_high() 
#define spl_x(x)
#endif	defined(i386)
#endif	defined(sun)

extern char etext;

extern int emulator_text_size;

char *s_highpc = &etext;
u_long	s_textsize = 0;

int ssiz;
u_short	*sbuf;
u_short	*kcount;

int emulator_ssiz;
u_short *emulator_sbuf;
char *emulator_s_lowpc = (char *) EMULATOR_BASE, *emulator_s_highpc;
u_long emulator_s_textsize = EMULATOR_SIZE;

#include <sys/user.h>
#if 0	/* EMcM: trying to avoid FP. */
#define         SCALE_1_TO_1    0x10000L
#else
#define		SQRT_SCALE_1_TO_1	0x100L
#define		SCALE_1_TO_1	(SQRT_SCALE_1_TO_1*SQRT_SCALE_1_TO_1)
#endif
struct uuprof server_prof_struct;
struct uuprof emulator_prof_struct;

spin_lock_t mcount_lock;

mach_port_t mcount_port;

unsigned int mcount_head, mcount_tail;
int mcount_dropped, last_mcount_dropped;
#if MCOUNT_STATS
int mcount_collisions, mcount_count;
#endif

#define MCOUNT_BUFSIZE 8192
char *selfpc_array[MCOUNT_BUFSIZE];
unsigned short *frompcindex_array[MCOUNT_BUFSIZE];

any_t mcount_thread();

void
spin_lock_solid_noprof(p)
	register spin_lock_t *p;
{
	while (spin_lock_locked(p) || !spin_try_lock(p)) {
		swtch_pri_noprof(0);
	}
}

kmstartup()
{
	u_long	fromssize, tossize;
        int bufsiz;
	kern_return_t kr;

	/*
	 *	round lowpc and highpc to multiples of the density we're using
	 *	so the rest of the scaling (here and in gprof) stays in ints.
	 */
	s_lowpc = (char *)
	    ROUNDDOWN((unsigned)s_lowpc, HISTFRACTION*sizeof(HISTCOUNTER));
	s_highpc = (char *)
	    ROUNDUP((unsigned)s_highpc, HISTFRACTION*sizeof(HISTCOUNTER));
	s_textsize = s_highpc - s_lowpc;
	printf("Profiling server, s_textsize=%x [%x..%x]\n",
		s_textsize, s_lowpc, s_highpc);
	ssiz = (s_textsize / HISTFRACTION) + sizeof(struct phdr);
	sbuf = (u_short *)malloc(ssiz);
	if (sbuf == 0) {
		printf("No space for monitor buffer(s)\n");
		return;
	}
	blkclr((caddr_t)sbuf, ssiz);
	fromssize = s_textsize / HASHFRACTION;
	froms = (u_short *)malloc(fromssize);
	if (froms == 0) {
		printf("No space for monitor buffer(s)\n");
/*		cfreemem(sbuf, ssiz); */
		sbuf = 0;
		return;
	}
	blkclr((caddr_t)froms, fromssize);
 
	tolimit = s_textsize * ARCDENSITY / TOSFRACTION;
	if (tolimit < MINARCS) {
		tolimit = MINARCS;
	} else if (tolimit > 65534) {
		tolimit = 65534;
	}
	tossize = tolimit * sizeof(struct tostruct);
	tos = (struct tostruct *)malloc(tossize);
	if (tos == 0) {
		printf("No space for monitor buffer(s)\n");
/*		cfreemem(sbuf, ssiz); */
		sbuf = 0;
/*		cfreemem(froms, fromssize); */
		froms = 0;
		return;
	}
	blkclr((caddr_t)tos, tossize);
	tos[0].link = 0;
	((struct phdr *)sbuf)->lpc = s_lowpc;
	((struct phdr *)sbuf)->hpc = s_highpc;
	((struct phdr *)sbuf)->ncnt = ssiz;
	kcount = (u_short *)(((int)sbuf) + sizeof(struct phdr));

 	if ((bufsiz = ssiz - sizeof(struct phdr)) <= 0) {
            panic("kmstartup: error in allocating the prof buffers\n");
            return; 
        }

	/* 
	 *	Integrate the profiling of the Server within the
	 *	general profiling scheme used for  Unix processes.
	 */
  	server_prof_struct.pr_base = (short *)  kcount;
	server_prof_struct.pr_size = (unsigned) bufsiz;
	server_prof_struct.pr_off  = (unsigned) s_lowpc;
        if (bufsiz < s_textsize) {
          bufsiz *= 10;
	  server_prof_struct.pr_scale=((bufsiz / s_textsize) * SCALE_1_TO_1)/10;
        }
        else 
#if 0	/* EMcM: don't do FP arithmetic. */
	  server_prof_struct.pr_scale= (bufsiz < s_textsize ) ?
			    	       ((float)  bufsiz / s_textsize ) * SCALE_1_TO_1:
                            	       SCALE_1_TO_1;
#else
	  {
	  server_prof_struct.pr_scale= (bufsiz < s_textsize ) ?
		       bufsiz*SQRT_SCALE_1_TO_1/s_textsize*SQRT_SCALE_1_TO_1 :
		       SCALE_1_TO_1;
	  printf("bufsiz=%d, textsize=%d, pr_scale=%d\n",
		 bufsiz, s_textsize, server_prof_struct.pr_scale);
	  }
#endif
	spin_lock_init(&mcount_lock);

	kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&mcount_port);
	/* Currently this port is never sent to, so the receive always times
	   out.  We could send messages to it to wake up the mcount thread
	   as necessary.  */
	if (kr != KERN_SUCCESS) {
	    printf("kmstartup: port allocate -> %x\n", kr);
	    panic("kmstartup");
	}
	ux_create_thread(mcount_thread);
}


emulator_prof_init()
{
    struct phdr *pp;
    emulator_s_lowpc = (char *)
	ROUNDDOWN((unsigned) emulator_s_lowpc,
		  HISTFRACTION * sizeof(HISTCOUNTER));
    emulator_s_highpc = (char *)
	ROUNDUP((unsigned) (emulator_s_lowpc + emulator_text_size),
		HISTFRACTION * sizeof(HISTCOUNTER));
    emulator_s_textsize = emulator_s_highpc - emulator_s_lowpc;
    printf("Profiling emulator, textsize=%x [%x..%x]\n",
	   emulator_s_textsize, emulator_s_lowpc, emulator_s_highpc);
    emulator_ssiz = (emulator_s_textsize / HISTFRACTION) +
	sizeof(struct phdr);
    emulator_sbuf = (u_short *) malloc(emulator_ssiz);
    pp = (struct phdr *) emulator_sbuf;
    pp->lpc = emulator_s_lowpc;
    pp->hpc = emulator_s_highpc;
    pp->ncnt = emulator_ssiz;
    emulator_prof_struct.pr_base = (short *) ((char *)pp + sizeof *pp);
    emulator_prof_struct.pr_size = emulator_ssiz - sizeof *pp;
    emulator_prof_struct.pr_off = (unsigned) emulator_s_lowpc;
    emulator_prof_struct.pr_scale =
	(emulator_prof_struct.pr_size * SQRT_SCALE_1_TO_1 /
	 emulator_s_textsize) * SQRT_SCALE_1_TO_1;
}

#if	defined(i386)
extern int mcount() __asm__ ("mcount");
#endif	defined(i386)

#if	defined(vax) || defined(balance) || defined(sun)
/*
 * This routine is massaged so that it may be jsb'ed to
 */
asm(".text");
asm("#the beginning of mcount()");
asm(".data");
#endif	defined(vax) || defined(balance) || defined(sun)

#if	defined(vax) || defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386) || defined(i860)

#if	multimax
mmax_mcount(selfpc, frompcindex)
register char		*selfpc;
register unsigned short	*frompcindex;
{
	register struct tostruct	*top;
	register struct tostruct	*prevtop;
	register long			toindex;
#if 0	/* Match braces for ctags and the like. */
}
#endif
#else	multimax


extern	mach_port_t	server_prof_port;

#if	defined(i860)

mcount(selfpc, frompcindex)
	char		*selfpc;
	unsigned short	*frompcindex;
{
#if 0	/* Match braces for ctags and the like. */
}
#endif

#else	defined(i860)

mcount()
{
	char		*selfpc;	/* r11 => r5 */
	unsigned short	*frompcindex;	/* r10 => r4 */

#endif	defined(i860)

	register unsigned int i;
#endif	multimax

	struct mcount_data	*mp;

	if (profiling)
		goto out;

#ifdef	lint
	selfpc = (char *)0;
	frompcindex = 0;
#else	not lint
	/*
	 *	find the return address for mcount,
	 *	and the return address for mcount's caller.
	 */
#ifdef	vax	 
	asm("	.text");		/* make sure we're in text space */
	asm("	movl (sp), r11");	/* selfpc = ... (jsb frame) */
	asm("	movl 16(fp), r10");	/* frompcindex =     (calls frame) */
#endif	vax	
#if	defined(balance)
	asm("	.text");		/* make sure we're in text space */
	asm("	movd 4(fp), r7");
	asm("	movd 4(0(fp)), r6");
#endif	defined(balance)
#if	defined(sun)
	asm("	.text");		/* make sure we're in text space */
	asm("	movl a6@(4), a5");	/* selfpc */
	asm("	movl a6@,a0");
	asm("	movl a0@(4), a4");	/* frompc */
#endif	defined(sun)
#if	defined(i386)
#define SelfPc() \
({ int _spl__, _tmp1__; \
	asm volatile("movl 4(%%ebp), %0" : "=r" (_spl__) : "r" (_tmp1__)); \
	_spl__; })
#define FromPc() \
({ int _spl__, _tmp1__; \
	asm volatile("movl (%%ebp), %0; movl 4(%0), %0" : "=r" (_spl__) : "r" (_tmp1__)); \
	_spl__; })

	selfpc = (char *)SelfPc();
	frompcindex = (unsigned short *)FromPc();
#endif	defined(i386)
#endif	not lint

	/*
	 *	check that frompcindex is a reasonable pc value.
	 *	for example:	signal catchers get called from the stack,
	 *			not from text space.  too bad.
	 */
	if (frompcindex == 0)
		goto out;
		/* This test should really be in the gprof program.  As it is,
		   gprof gets confused by a 0 caller pc (which we can get for
		   cthread_body after a cthread_fork) because it can't find
		   a symbol for it.  */
	frompcindex = (unsigned short *)((long)frompcindex - (long)s_lowpc);
	if ((unsigned long)frompcindex > s_textsize) {
		goto out;
	}
	frompcindex =
	    &froms[((long)frompcindex) / (HASHFRACTION * sizeof(*froms))];

	if (!spin_try_lock(&mcount_lock)) {
#if MCOUNT_STATS
	    mcount_collisions++;
#endif
	    spin_lock_solid_noprof(&mcount_lock);
	}

	i = mcount_tail;
	if ((mcount_tail = (i + 1) % MCOUNT_BUFSIZE) == mcount_head) {
	    mcount_tail = i;
	    mcount_dropped++;
	    spin_unlock(&mcount_lock);
	} else {
	    spin_unlock(&mcount_lock);
	    frompcindex_array[i] = frompcindex;
	    selfpc_array[i] = selfpc;
	}

out:

#ifdef	vax
	asm("	rsb");
#endif	vax

#if	defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386) || defined(i860)
	return;
#endif	defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386) || defined(i860)
}
#if	defined(vax) || defined(balance) || defined(sun)
asm(".text");
asm("#the end of mcount()");
asm(".data");
#endif	defined(vax) || defined(balance) || defined(sun)
#endif	defined(vax) || defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386) || defined(i860)


#define MCOUNT_PERIOD 251
int mcount_period = MCOUNT_PERIOD;

any_t
mcount_thread()
{
    struct mcount_data *mp;
    mach_msg_header_t msg;
    kern_return_t kr;
#define MCOUNT_REPORT 32
unsigned loopcount = 0;
    set_fixed_high_priority(mach_thread_self());
    cthread_set_name(cthread_self(), "mcount");
    while (1) {
	kr = mach_msg_trap_noprof(&msg, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0,
			     sizeof msg, mcount_port, mcount_period,
		      MACH_PORT_NULL);
	if (kr == KERN_SUCCESS || kr == MACH_RCV_TIMED_OUT) {
	    if (mcount_head != mcount_tail)
		add_mcounts();
	} else printf("mcount_thread: mach_msg -> %x\n", kr);
	if (profiling != oldprofiling)
	    profile_change();
	if (emulator_profiling != emulator_oldprofiling)
	    emulator_profile_change();
	if (mcount_dropped > last_mcount_dropped) {
	    int i = mcount_dropped;
	    /* Reduce sleep time: */
	    mcount_period = mcount_period * 2 / 3;
	    if (mcount_period < 10)
		    mcount_period = 10;

	    printf("mcount dropped %d edges.  Reducing mcount_period to %d\n",
					i - last_mcount_dropped, mcount_period);
	    last_mcount_dropped = i;
	}
#if MCOUNT_STATS
if (!profiling && ++loopcount % MCOUNT_REPORT == 0 && mcount_count > 0)
 printf("mcount: count %d collis %d ratio %d%%\n", mcount_count,
 mcount_collisions, mcount_collisions * 100 / mcount_count);
#endif
	    }
	return 0;
}


set_fixed_high_priority(th)
thread_t th;
{
    struct host_sched_info schedinfo;
    extern mach_port_t host_port;
    int schedinfosize = HOST_SCHED_INFO_COUNT;
    kern_return_t kr;

    set_thread_priority(th, 1);
    kr = host_info(host_port, HOST_SCHED_INFO, &schedinfo, &schedinfosize);
    if (kr != KERN_SUCCESS)
	printf("host_info -> %x\n", kr);
    else thread_policy(th, POLICY_FIXEDPRI, schedinfo.min_quantum);
}


add_mcounts()
{
    for ( ; mcount_head != mcount_tail;
	 mcount_head = (mcount_head + 1) % MCOUNT_BUFSIZE) {
	long toindex;
	struct tostruct *top, *prevtop;
	char *selfpc;
	unsigned short *frompcindex;
	selfpc = selfpc_array[mcount_head];
	if (selfpc == NULL)
		return;
#if MCOUNT_STATS
	mcount_count++;
#endif
	selfpc_array[mcount_head] = NULL;
	frompcindex = frompcindex_array[mcount_head];
	if ((toindex = *frompcindex) == 0) {
		/*
		 *	first time traversing this arc
		 */
		toindex = ++tos[0].link;
		if (toindex >= tolimit) {
			goto overflow;
		}
		*frompcindex = toindex;
		top = &tos[toindex];
		top->selfpc = selfpc;
		top->count = 1;
		top->link = 0;
		continue;
	}
	top = &tos[toindex];
	if (top->selfpc == selfpc) {
		/*
		 *	arc at front of chain; usual case.
		 */
		top->count++;
		continue;
	}
	/*
	 *	have to go looking down chain for it.
	 *	top points to what we are looking at,
	 *	prevtop points to previous top.
	 *	we know it is not at the head of the chain.
	 */
	for (;;) {
		if (top->link == 0) {
			/*
			 *	top is end of the chain and none of the chain
			 *	had top->selfpc == selfpc.
			 *	so we allocate a new tostruct
			 *	and link it to the head of the chain.
			 */
			toindex = ++tos[0].link;
			if (toindex >= tolimit) {
				goto overflow;
			}
			top = &tos[toindex];
			top->selfpc = selfpc;
			top->count = 1;
			top->link = *frompcindex;
			*frompcindex = toindex;
			break;
		}
		/*
		 *	otherwise, check the next arc on the chain.
		 */
		prevtop = top;
		top = &tos[top->link];
		if (top->selfpc == selfpc) {
			/*
			 *	there it is.
			 *	increment its count
			 *	move it to the head of the chain.
			 */
			top->count++;
			toindex = prevtop->link;
			prevtop->link = top->link;
			top->link = *frompcindex;
			*frompcindex = toindex;
			break;
		}
	}
    }
    return;
overflow:
    profiling = 3;
    mcount_head = mcount_tail;
    printf("mcount_thread: tos overflow\n");
    /*
     * Since we are no longer in mcount (which can potentially be called
     * from anywhere, including memory allocation functions), we could
     * allocate memory properly.  But we can afford not to fix this problem
     * until it actually arises.
     */
}


profile_change()
{
    /* 
     *  Change of the Server profiling status detected. 
     *  Let the micro-kernel know about this change (ON or OFF) 
     *
     *	For historical reasons, the profiling variable is used
     *	confusingly: a nonzero value means profiling is off.
     */
    kern_return_t kr;
    oldprofiling = profiling;
    if (profiling) { 
	/* profiling status disabled */
	kr = task_sample((mach_port_t) current_task(), MACH_PORT_NULL);
	if (kr != KERN_SUCCESS)
	    printf("sample_task -> %d\n", kr);
	kr = mach_port_deallocate(mach_task_self(),
				  server_prof_port);
	if (kr != KERN_SUCCESS)
	    printf("port_deallocate -> %d\n", kr);
    } else {
	/* 
	 *  Allocate the Server's own internal profiling port. 
	 *  Add it to the set of profiling ports.
	 */
	if (mach_port_allocate(mach_task_self(),
			       MACH_PORT_RIGHT_RECEIVE,
			       &server_prof_port) != KERN_SUCCESS) {
	    printf("server profile: can't allocate reply port");
	    profiling = oldprofiling = 3;
	    return;
	}

	if (mach_port_insert_right(mach_task_self(),
				   server_prof_port,
				   server_prof_port,
				   MACH_MSG_TYPE_MAKE_SEND) !=
	    KERN_SUCCESS) {
	    printf("server profile: can't acquire send rights");
	    profiling = oldprofiling = 3;
	    return;
	}

	ux_profil_add_port(server_prof_port);
	kr = task_sample((mach_port_t) current_task(), server_prof_port);
	if (kr == MIG_BAD_ID) {
	    printf("kernel does not support profiling - sampling data "
		   "will be missing\n");
	} else if (kr != KERN_SUCCESS) {
	    printf("sample_task -> %d\n", kr);
	} else {
	    mach_port_t old_notify_port;
	    kr = mach_port_request_notification(mach_task_self(),
		   server_prof_port, MACH_NOTIFY_NO_SENDERS,
		   (mach_port_mscount_t) 0, server_prof_port,
		   MACH_MSG_TYPE_MAKE_SEND_ONCE, &old_notify_port);
	    if (kr != KERN_SUCCESS)
		printf("server profile: request notify -> %d\n", kr);
	}
    }
}


emulator_profile_change()
{
    struct proc *p;
    unix_master();
    emulator_oldprofiling = emulator_profiling;
    if (emulator_profiling == 0) {	/* Turning on. */
	for (p = allproc; p; p = p->p_nxt) {
	    if (p->p_pid == 0)
		continue;
	    if (p->p_profport == MACH_PORT_NULL)
		p->p_profport = pport_to_proc_enter(p);
	    task_sample(mach_task_self(), p->p_profport,
			(mach_port_t) p->p_task);
	    p->p_taskprofed = 1;
	}
    } else {				/* Turning off. */
	for (p = allproc; p; p = p->p_nxt) {
	    if (p->p_pid == 0)
		continue;
	    if (p->p_profport == MACH_PORT_NULL)
		panic("emulator_profile_change");
	    task_sample(mach_task_self(), MACH_PORT_NULL,
			(mach_port_t) p->p_task);
	    p->p_taskprofed = 0;
	}
    }
    unix_release();
}
#endif	!defined(mips)


/* Rest of file is mips stuff. */
#ifdef	mips

#if	PROFTYPE != 1 && PROFTYPE != 2 && PROFTYPE != 3 && PROFTYPE != 4
#include "error: PROFTYPE incorrectly defined"
#endif

#include <sys/time.h>
#include <sys/kernel.h>

int profiling = 3;

unsigned int	*kcount;
char *s_lowpc;
u_long	s_textsize = 0;
struct phdr phdr;

extern char eprol[], etext[];

kmstartup()
{
	if (phdr.pc_buf == 0) {
		printf("No space for monitor buffer(s)\n");
		s_textsize = 0;
		return;
	}
	printf("pc_buf is at %x\n", phdr.pc_buf);
	bzero((caddr_t)phdr.pc_buf, phdr.pc_bytes);

	phdr.proftype = PC_SAMPLES;
	phdr.sample_hz = phz ? phz : hz;
	kcount = (unsigned int *)phdr.pc_buf;

#if	PROFTYPE == 1
	printf("Profiling type 1 (PC samples only)\n");
#endif	PROFTYPE == 1

#if	PROFTYPE == 2 || PROFTYPE == 3
	mipsprof_startup();
#endif	PROFTYPE == 2 || PROFTYPE == 3

#if	PROFTYPE == 4
	gprof_startup();
#endif	PROFTYPE == 4
}

#if	PROFTYPE == 2 || PROFTYPE == 3

/*
 * "_mcount" adds (return_address >> 1) to this to find the corresponding
 * counter within the array of counters. Don't set this nonzero till after
 * we've allocated storage for the counters, since it also prevents _mcount
 * from being called from within sbrk before we're ready.
 */
char * _mcountoff;

mipsprof_startup()
{
	if (phdr.bb_buf == 0) {
		printf("No space for bb counts\n");
		return;
	}
	printf("bb_buf is at %x\n", phdr.bb_buf);
	bzero((caddr_t)phdr.bb_buf, phdr.bb_bytes);

#if	PROFTYPE == 2
	printf("Profiling type 2 (Invocation counts)\n");
	phdr.proftype |= INV_COUNTS;
#else	PROFTYPE == 3
	printf("Profiling type 3 (Basic Block counts)\n");
	phdr.proftype |= BB_COUNTS;
#endif	PROFTYPE

	_mcountoff = 0;
	printf("bb_buf is at %x bb_buf[0] = %x\n", phdr.bb_buf, phdr.bb_buf[0]);
	_mcountoff = phdr.bb_buf/* - (((unsigned) phdr.lowpc) >> 1)*/; 
}
#endif	PROFTYPE == 2 || PROFTYPE == 3

#if	PROFTYPE == 4
/*
 * GPROF profiling initialization
 *
 * Froms is actually a bunch of unsigned shorts indexing tos
 */
u_short *froms;
struct tostruct *tos = 0;
long tolimit = 0;

gprof_startup(php)
struct phdr *php;
{
	u_long	fromssize, tossize;

	phdr.froms_bytes = s_textsize / HASHFRACTION;
	froms = (u_short *)(phdr.froms_buf = (char *)calloc(phdr.froms_bytes));
	if (froms == 0) {
		printf("No space for froms buffer\n");
		s_textsize = 0;
		return;
	}
	bzero((caddr_t)phdr.froms_buf, phdr.froms_bytes);
	tolimit = s_textsize * ARCDENSITY / 100;
	if (tolimit < MINARCS) {
		tolimit = MINARCS;
	} else if (tolimit > 65534) {
		tolimit = 65534;
	}
	phdr.tos_bytes = tolimit * sizeof(struct tostruct);
	tos = (struct tostruct *)(phdr.tos_buf=(char *)calloc(phdr.tos_bytes));
	if (tos == 0) {
		printf("No space for tos buffer(s)\n");
	/* 	cfreemem(froms, phdr.froms_bytes); */
		s_textsize = 0;
		return;
	}
	bzero((caddr_t)phdr.tos_buf, phdr.tos_bytes);
	tos[0].link = 0;
	phdr.proftype |= GPROF_COUNTS;
	printf("Profiling type 4 (GPROF)\n");
	return;
}
#endif	PROFTYPE == 4

#endif	mips
