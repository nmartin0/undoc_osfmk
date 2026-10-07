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
 * $Log: emul_stack_alloc.c,v $
 * Revision 1.11  1995/04/05  06:57:04  johannes
 * use of new define EMUL_INIT_STACK_SIZE
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 11007
 *  Testing: simple test with kill; kill from pthread; corefile EATs
 *  Module(s): server/paracore/core.c (adjust_thread_info)
 *             emulator/emul_stack.h (define for emulator stack size)
 *             emulator/emul_stack_alloc.c (use of new define)
 *
 * Revision 1.10  1994/11/18  20:23:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/08/31  22:45:46  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.7.2.1  1994/08/10  18:52:56  rlg
 * The queue limit for the server's root vnode port was increased to
 * MACH_PORT_QLIMIT_MAX.  This change, coupled with the changes to the
 * emulator for the reply port, work around a deadlock problem with the
 * new NORMA implementation.
 *
 * Committing in R1.3 WW33 to fix PTS #10409
 *
 * Modified Files:   server/bsd/init_main.c
 *                   emulator/emul_stack.h
 *                   emulator/emul_stack_alloc.c
 *                   emulator/pfs2_user_side.c
 *
 * Revision 1.7  1993/12/23  01:46:50  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: None.
 *  Testing: Booted and ran minimal PFS tests.
 *  Module(s): emulator/emul_stack_alloc.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.6  1993/07/14  17:31:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:22:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:14:54  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:35  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:17:42  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  05:57:02  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:51:39  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:08:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:00:48  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/05  22:15:45  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.12  1992/11/02  21:51:34  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 2.11  1992/10/23  00:15:50  cfj
 * Bump the size of an emulator stack to 32K.
 *
 * Revision 2.11  92/11/23  15:59:19  klh
 * 	Revision 2.7  92/11/03  11:01:56  loverso
 * 		Fix panic messages.  No longer need to pass reply port to
 * 		emul_vm_map.  Fix some comments.  Implement mig_dealloc_reply_port.
 * 		(loverso)
 * 
 * Revision 2.10  92/10/05  13:41:58  klh
 * 	Revision 2.6  92/08/26  12:09:26  loverso
 * 		Use EMUL_STACK_BASE to compute base of emulator stack.
 * 		Add new counter on_emul_stack, which counts the number of
 * 		"emulator" threads.
 * 		(loverso)
 * 
 * Revision 2.9  92/02/17  14:50:49  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.8  92/02/17  14:50:41  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.7  92/02/17  14:50:32  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.6  92/02/17  14:50:07  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.5  92/02/11  18:59:36  pjg
 * 	Split emul_stack_alloc() into two routines that may or may
 * 	not allocate a new mig_reply_port. Don't set emul_stack_next
 * 	to USRTEXT anymore.
 * 
 * 	Add in a kludge version of emul_stack_reinit() (needed by 
 * 	migrate) (TNC only) (roman@locus.com).
 * 
 * Revision 2.4  92/01/17  17:17:02  roy
 * 	Arg to emul_stack_alloc determines whether to allocate 
 * 	reply port (srl). 
 * 
 * Revision 2.3  91/10/14  13:10:43  srl
 * (Grenoble V2.2 merge)
 * 	91/09/13  15:52:59  sp
 * 	rename vm_map hack to emul_vm_map so that we can compile with __STDC__
 * 
 * Revision 2.2  91/08/30  16:40:22  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:11  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.5  90/11/05  15:31:50  rpd
 * 	Remove stack limit problems.
 * 	[90/11/01            rwd]
 * 
 * Revision 2.4  90/10/25  15:06:31  rwd
 * 	Add code to support STACK_GROWTH_UP.
 * 	[90/10/12            rwd]
 * 
 * Revision 2.3  90/06/02  15:20:44  rpd
 * 	Revised reply port allocation and added an explicit reply port
 * 	argument to vm_map.  (It is called before we are on an emulator stack.)
 * 	[90/04/08            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  19:28:56  rpd]
 * 
 * Revision 2.2  89/11/29  15:26:46  af
 * 	RCS-ed.
 * 	[89/11/29            af]
 * 
 * $EndLog$
 */
/*
 * Maintain a separate stack for the user thread
 * to use while in the emulator.
 */

#include <mach/mach.h>
#include <machine/vmparam.h>
#include "emul_stack.h"
#include "emul.h"

extern	char	end[];

extern void	emul_panic();

vm_size_t	emul_stack_size = EMUL_INIT_STACK_SIZE;
					/* size of one stack */
vm_offset_t	emul_stack_mask;	/* mask these bits to get to bottom */
vm_offset_t	emul_stack_next;
emul_stack_t	emul_stack_list = (emul_stack_t)0;

/*
 * Flag to indicate that we can use emulator stacks.
 */
boolean_t	stack_init_done = FALSE;

/*
 * Count of threads on emulator stack (i.e., count of threads in emulator)
 */
unsigned long	on_emul_stack = 0;

/*
 * Global reply port; used before the emulator stacks are initialized.
 */
mach_port_t	mig_reply_port = MACH_PORT_NULL;

/*
 * Forward declarations
 */
mach_port_t	mig_get_reply_port();
emul_stack_t	do_emul_stack_alloc();

/*
 * Clear the stack list on fork; children do NOT inherit the list
 * of emulator stacks.
 */
emul_stack_t
emul_stack_init()
{
	on_emul_stack = 0;

	emul_stack_mask = ~(emul_stack_size - 1);
	/*
	 * First emulator stack is allocated after the emulator's data
	 */
	emul_stack_next = ((vm_offset_t)end + emul_stack_size - 1)
				& emul_stack_mask;

	emul_stack_list = emul_stack_alloc();

	return emul_stack_list;
}


/*
 * Allocate a stack for the emulator and unconditionally allocate
 * a new reply port. Called to allocate a stack for threads internal
 * to the emulator. Must be called after the initial emulator stack was
 * created. Can be called on user's stack.
 */
emul_stack_t
emulthread_stack_alloc()
{
        mach_port_t    		reply_port;

	reply_port = mach_reply_port();
        if (reply_port == MACH_PORT_NULL)
                emul_panic("emulthread_stack_alloc: no reply port");

	return (do_emul_stack_alloc(reply_port));
}

/*
 * Allocate a stack for the emulator.  Called when
 * the stack list is empty.  Can be called on user's stack.
 */
emul_stack_t
emul_stack_alloc()
{
        mach_port_t    		reply_port;

        /*
         * Allocate a new reply port for the stack;
         * or take the global port if this is the first stack.
         */
        if (stack_init_done)
                reply_port = mach_reply_port();
        else
                reply_port = mig_get_reply_port();
        if (reply_port == MACH_PORT_NULL)
                emul_panic("emul_stack_alloc: no reply port");

	return (do_emul_stack_alloc(reply_port));
}

emul_stack_t
do_emul_stack_alloc(reply_port)
	mach_port_t	reply_port;
{
        vm_offset_t     	base;
        register emul_stack_t   new_stack;
	kern_return_t		ret;

        /*
         * Look for the next free region at the correct alignment.
         * We must pass in the reply port explicitly, because
         * we aren't running on the new stack yet, so mig_get_reply_port()
         * can't find an appropriate reply port.
         */
	base = emul_stack_next;

	ret = emul_vm_map(mach_task_self(),
		     &base, emul_stack_size, (emul_stack_size - 1),
		     TRUE, MEMORY_OBJECT_NULL, (vm_offset_t)0, FALSE,
		     VM_PROT_DEFAULT, VM_PROT_ALL, VM_INHERIT_NONE);

	if (ret != KERN_SUCCESS)
		emul_panic("emul_stack_alloc: emul_vm_map failed");

        emul_stack_next = base + emul_stack_size;

        /*
         * Set up top-of-stack structure with new reply port.
         */
#ifdef  STACK_GROWTH_UP
        new_stack = (emul_stack_t) (base);
        new_stack->link = 0;
        new_stack->reply_port = reply_port;
#ifdef	PFS
	new_stack->pfs_reply_port = MACH_PORT_NULL;
	new_stack->pfs_reply_port_q_limit = 0;
#endif		
        new_stack++;
#else   STACK_GROWTH_UP
        new_stack = (emul_stack_t) (base + emul_stack_size);
        new_stack--;
        new_stack->link = 0;
        new_stack->reply_port = reply_port;
#ifdef	PFS
	new_stack->pfs_reply_port = MACH_PORT_NULL;
	new_stack->pfs_reply_port_q_limit = 0;
#endif		
#endif  STACK_GROWTH_UP

        return (new_stack);
}


/*
 * Reinitialize the emulator stacks after a process migrates
 */
emul_stack_t
emul_stack_reinit()
{
	/*
	 * Currently this does NOT correctly preserve all the old
	 * reply ports or the old stack.
	 */
	return (emul_stack_init());
}


/*
 * The following routines are present to substitute for MiG library
 * routines, which do not do the right thing.
 */

/*
 * Use reply port at top of emulator stack, if possible.
 */
mach_port_t
mig_get_reply_port()
{
	register mach_port_t	port;

	if (stack_init_done) {
		register emul_stack_t	stack;
		volatile int		on_the_stack;

		stack = EMUL_STACK_BASE(&on_the_stack);

		if ((port = stack->reply_port) == MACH_PORT_NULL)
			stack->reply_port = port = mach_reply_port();
	} else {
		if ((port = mig_reply_port) == MACH_PORT_NULL)
			mig_reply_port = port = mach_reply_port();
	}

        return port;
}

void
mig_dealloc_reply_port()
{
	register mach_port_t	port;

	if (stack_init_done) {
		register emul_stack_t	stack;
		volatile int		on_the_stack;

		stack = EMUL_STACK_BASE(&on_the_stack);

		port = stack->reply_port;
		stack->reply_port = MACH_PORT_NULL;
	} else {
		port = mig_reply_port;
		mig_reply_port = MACH_PORT_NULL;
	}
	(void) mach_port_mod_refs(mach_task_self(), port,
				  MACH_PORT_RIGHT_RECEIVE, -1);
}

#ifdef	PFS
/*
 * Use PFS reply port at top of emulator stack.  This port is dedicated to
 * concurrent PFS operations initiated from a single thread, and avoids the
 * penalty of allocating a port per PFS file open.
 */
mach_port_t
pfs_get_reply_port(queue_limit)
mach_port_msgcount_t	queue_limit;
{
	register mach_port_t	port;
	register emul_stack_t	stack;
	volatile int		on_the_stack;

	stack = EMUL_STACK_BASE(&on_the_stack);

	if ((port = stack->pfs_reply_port) == MACH_PORT_NULL)
		stack->pfs_reply_port = port = mach_reply_port();

	if ((port != MACH_PORT_NULL) &&
	    (stack->pfs_reply_port_q_limit < queue_limit)) {
		/*
		 * increase the queue limit of the pfs_reply_port
		 * (add four to the requested value for some headroom):
		 */
		(void)mach_port_set_qlimit(mach_task_self(),
					   port,
					   (queue_limit+4));
	}

	return port;
}

void
pfs_dealloc_reply_port()
{
	register mach_port_t	port;
	register emul_stack_t	stack;
	volatile int		on_the_stack;

	stack = EMUL_STACK_BASE(&on_the_stack);

	port = stack->pfs_reply_port;
	stack->pfs_reply_port = MACH_PORT_NULL;
	stack->pfs_reply_port_q_limit = 0;

	(void) mach_port_mod_refs(mach_task_self(), port,
				  MACH_PORT_RIGHT_RECEIVE, -1);
}
#endif	PFS

/*
 * Called from mach_init() with initial==0 to force initialization,
 * and then explicitly from main/child_init/migrate_init to enable
 * the per-stack reply port.
 */
void
mig_init(initial)
	emul_stack_t initial;
{
	if (initial == 0) {
		stack_init_done = FALSE;
		mig_reply_port = MACH_PORT_NULL;
	} else {
		stack_init_done = TRUE;
		if (initial->reply_port != mig_reply_port)
			emul_panic("mig_init: no reply port");
	}
}
