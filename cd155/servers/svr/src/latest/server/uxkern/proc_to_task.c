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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: proc_to_task.c,v $
 * Revision 1.14  1995/04/21  20:15:29  toman
 * Replaced panic after send right deallocation failure with a bootnode_printf()
 * in vproc_port_deallocate().
 *
 *  Reviewer: Brent Olsen, Suri Brahmaroutu
 *  Risk: Low
 *  Benefit or PTS #: 12902
 *  Testing: VSTNC
 *  Module(s): server/uxkern/proc_to_task.c
 *
 * Revision 1.13  1995/02/10  23:51:57  yazz
 * Reviewer: Jerry Toman
 * Risk: Lo
 * Benefit or PTS #: instrumentation to catch #11366 C-0 (not a fix)
 * Testing: EATs controlc
 * Module(s): server/tnc/dvp_vpops.c
 * 	    server/uxkern/proc_to_task.c
 * For TNC only, add much more info to some vproc deallocation panics.
 *
 * Revision 1.12  1994/11/18  20:49:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/10/25  00:15:38  jlitvin
 * Print the MK error return when failures occur in
 * vproc_port_deallocate().  This will make PTS #11366 easier to solve.
 *
 *  Reviewer: yazz
 *  Risk: very low
 *  Benefit or PTS #: 11376
 *  Testing: developer
 *  Module(s): server/uxkern/proc_to_task.c
 *
 * Revision 1.10  1994/09/29  23:00:42  yazz
 *  Author of fix: Chris Peak
 *  Reviewer: Bob Yasi, Nandini Ajmani
 *  Risk: Medium
 *  Benefit or PTS #: 9346 (plus 10485, 6266, 7916, 9265, 8194)
 *  Testing: control-c EAT case 3 (20 iterations), other EATs os_interfaces,
 * 		xtrnl, controlc, rmcall, fileio, message
 *  Module(s):
 * 	server/sys/vproc.h
 * 	server/uxkern/proc_to_task.h
 * 	server/uxkern/proc_to_task.c
 * 	server/tnc/dvp_vpops.c
 * 	server/tnc/rvp_subr.c
 *
 * Abolish the renaming of vproc ports for send rights on remote nodes.
 * Assign vproc port name at vproc alloc time and clear it at dealloc time.
 * Make vproc_to_port() lookup macro return the contents of the new vproc
 * port name field in the vproc structure.
 *
 * Revision 1.9  1994/06/18  00:22:01  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.8  1994/03/17  16:59:23  jlitvin
 * Add more debug information for "can't acquire send rights" panic.
 *
 * Revision 1.7  1993/10/28  03:16:11  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.6  1993/10/11  18:34:39  cfj
 * Change the panic in proc_to_task_enter() when mach_port_allocate_name()
 * returns an error it prints the error code and the port name it was
 * attempting to use.
 *
 * Revision 1.5  1993/07/14  18:43:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:05:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:32:08  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:53:20  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2  1992/11/06  20:34:24  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:44:28  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.11  92/10/16  11:30:01  chrisp
 * In vproc_port_deallocate(), add extra parameter indicating whether a
 * 	receive right is held locally. Now use mach_port_mod_refs()
 * 	and mach_port_destroy() rather than mach_port_destroy() so
 * 	that additional send rights extant may be given away (as
 * 	DEADNAMEs).
 * 
 * Revision 2.13  93/06/16  13:54:38  klh
 * 	Revision 2.10  93/05/16  20:59:40  loverso
 * 		Revision 1.3  1992/10/21  13:04:14  devrcs
 * 		Added debugging diagnostics to find out why task_to_port_enter()
 * 		cannot allocate the port name it wants.
 * 		[1992/10/19  17:02:40  emcmanus]
 * 
 * Revision 2.12  93/03/27  17:04:55  yazz
 * Added more info to two panic messages.
 *
 * Revision 2.11  92/10/16  11:30:01  chrisp
 * In vproc_port_deallocate(), add extra parameter indicating whether a
 * 	receive right is held locally. Now use mach_port_mod_refs()
 * 	and mach_port_destroy() rather than mach_port_destroy() so
 * 	that additional send rights extant may be given away (as
 * 	DEADNAMEs).
 * 
 * Revision 2.10  92/10/01  10:36:48  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.9  92/06/05  14:00:55  klh
 * 	Revision 2.8  92/05/24  13:59:46  pjg
 * 		Revision 3.4  92/03/24  21:04:09  barbou
 * 		Fix for bug #102: port leak when dumping core.
 * 
 * Revision 2.8  92/04/20  15:17:37  chrisp
 * Minor change to formatting to improve readability of vproc_port_allocate().
 * 
 * Revision 2.7  92/02/14  09:04:10  roman
 * Change text on panic strings.
 * 
 * Revision 2.6  91/12/17  10:33:48  roy
 * 	91/10/09  18:41:21  emcmanus
 * 	Added task_first_thread function to pick a thread from a task; this 
 * 	thread is essentially random and indicates incomplete support for 
 * 	multi-threaded applications.
 * 
 * Revision 2.5  91/11/22  15:20:33  rabii
 * 	locus merge
 * 	Cause system to panic if the deallocation of a vproc port fails. (roman)
 * 
 * Revision 2.4  91/10/04  15:24:01  chrisp
 * Add Locus copyright.
 * 
 * Revision 2.3  91/09/17  09:15:04  sjs
 * integrate Locus changes	roman
 * Changed mach_port_allocate() followed by mach_port_rename() to
 * a single mach_port_allocate_name() call. Added routines for creating
 * and destroying vproc ports.
 * 
 * Revision 2.2  91/08/31  14:27:19  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/27  15:38:59  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.1  91/06/25  17:13:21  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:06:14  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  91/08/12  22:38:27  rvb
 * 	Changed task_to_proc_enter to use mach_port_allocate_name.
 * 	Suggestion from Keith Loepere.
 * 	[91/04/14            rpd]
 * 
 * Revision 2.3  90/06/02  15:28:13  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:22:03  rpd]
 * 
 * Revision 2.2  89/12/08  20:16:46  rwd
 * 	Rename port to pointer to proc struct to eliminate hash.
 * 	[89/11/01            rwd]
 * 
 * $EndLog$
 */
/*
 * Task->process and request_port->process routines.
 */

#include <sys/proc.h>
#include <sys/vproc.h>
#include <kern/queue.h>

#include <uxkern/import_mach.h>
#include <uxkern/proc_to_task.h>

extern void	ux_server_add_port();
extern void	ux_server_remove_port();

#define	NTASKHASH	32

#if	((NTASKHASH-1)&NTASKHASH) == 0
#define	TASKHASH(t)	(((unsigned int)t) & (NTASKHASH-1))
#else
#define	TASKHASH(t)	(((unsigned int)t) % NTASKHASH)
#endif

struct proc_hash {
	queue_chain_t	task_to_proc_chain;
	task_t		task;
	struct proc *	proc;
};

queue_head_t	task_to_proc_hash[NTASKHASH];

struct mutex	task_to_proc_lock = MUTEX_INITIALIZER;

void task_to_proc_init()
{
	register int	i;

	for (i = 0; i < NTASKHASH; i++)
	    queue_init(&task_to_proc_hash[i]);
}

static void
showport(port)
    mach_port_t port;
{
#if __STDC__ == 1
#define ptentry(n) {MACH_PORT_TYPE_##n, #n}
#else
#define ptentry(n) {MACH_PORT_TYPE_/**/n, "n"}
#endif
    static struct {
	mach_port_type_t ptype;
	char *pname;
    } names[] = {
	ptentry(SEND), ptentry(RECEIVE), ptentry(SEND_ONCE), ptentry(PORT_SET),
	ptentry(DEAD_NAME), ptentry(DNREQUEST)
    };
    int i;
    mach_port_type_t type;
    kern_return_t kr;
    kr = mach_port_type(mach_task_self(), port, &type);
    if (kr != KERN_SUCCESS) {
	printf("port_type(%x) -> %d (0x%x)\n", port, kr, kr);
	return;
    }
    printf("port %x type:", port);
    for (i = 0; i < sizeof names / sizeof names[0]; i++)
	if (type & names[i].ptype)
	    printf(" %s", names[i].pname);
    printf("\n");
}

/*
 * Enter task and process in hash table.
 * Allocate request port for process, and return it.
 */
mach_port_t
task_to_proc_enter(task, p)
	task_t		task;
	struct proc *	p;
{
	register struct proc_hash *ph;
	register queue_t q;
	mach_port_t	req_port;
	kern_return_t	kr;

	req_port = (mach_port_t) p;

	kr = mach_port_allocate_name(mach_task_self(),
				     MACH_PORT_RIGHT_RECEIVE,
				     req_port);
	if (kr != KERN_SUCCESS) {
		if (kr == KERN_NAME_EXISTS)
			showport(req_port);
		panic("task_to_proc_enter: can't alloc request port "
				"kr=0x%x name=0x%x", kr, req_port);
	}

	kr = mach_port_insert_right(mach_task_self(),
				    req_port, req_port,
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
	    panic("task_to_proc_enter: can't acquire send rights");

	ux_server_add_port(req_port);

	ph = (struct proc_hash *)malloc(sizeof(struct proc_hash));

	ph->task = task;
	ph->proc = p;

	mutex_lock(&task_to_proc_lock);

	q = &task_to_proc_hash[TASKHASH(task)];
	queue_enter(q, ph, struct proc_hash *, task_to_proc_chain);

	mutex_unlock(&task_to_proc_lock);

	return (req_port);
}

struct proc *
task_to_proc_lookup(task)
	register task_t	task;
{
	struct proc *p = 0;

	register struct proc_hash *ph;
	register queue_t q;

	mutex_lock(&task_to_proc_lock);

	q = &task_to_proc_hash[TASKHASH(task)];
	for (ph = (struct proc_hash *)queue_first(q);
	     !queue_end(q, (queue_entry_t)ph);
	     ph = (struct proc_hash *)queue_next(&ph->task_to_proc_chain)) {
	    if (ph->task == task) {
		p = ph->proc;
		break;
	    }
	}

	mutex_unlock(&task_to_proc_lock);
	return (p);
}

void
task_to_proc_remove(task)
	register task_t	task;
{
	register struct proc_hash *ph;
	register queue_t q;

	register struct proc *p;
	mach_port_t req_port;

	mutex_lock(&task_to_proc_lock);

	q = &task_to_proc_hash[TASKHASH(task)];
	for (ph = (struct proc_hash *)queue_first(q);
	     !queue_end(q, (queue_entry_t)ph);
	     ph = (struct proc_hash *)queue_next(&ph->task_to_proc_chain)) {
	    if (ph->task == task) {
		p = ph->proc;
		break;
	    }
	}
	if (p == 0)
	    panic("task_to_proc_remove");

	req_port = (mach_port_t) p;

	queue_remove(q, ph, struct proc_hash *, task_to_proc_chain);

	mutex_unlock(&task_to_proc_lock);

	free((char *)ph);

	(void) mach_port_destroy(mach_task_self(), req_port);
}

kern_return_t
task_first_thread(task, threadp)
	register task_t task;
	register thread_t *threadp;
{
	thread_array_t thread_table;
	unsigned int table_size, i;

	if (task_threads(task, &thread_table, &table_size) != KERN_SUCCESS
	    || table_size <= 0) 
		return KERN_FAILURE;
	*threadp = thread_table[0];
	for (i = 1; i < table_size; i++) {
		(void) mach_port_deallocate(mach_task_self(),
					    thread_table[i]);
	}
	(void) vm_deallocate(mach_task_self(), 
			     (vm_address_t) thread_table, 
			     table_size * sizeof(*thread_table));
	return KERN_SUCCESS;
}

void
vproc_port_allocate(v)
	struct vproc *v;
{
	mach_port_t req_port;
	int kr;

	req_port = (mach_port_t) v;
	kr = mach_port_allocate_name(mach_task_self(), 
				     MACH_PORT_RIGHT_RECEIVE,
				     req_port);
	if (kr != KERN_SUCCESS) {
		if (kr == KERN_NAME_EXISTS)
			showport(req_port);
		panic("vproc_port_allocate: can't alloc vproc port "
				"kr=0x%x name=0x%x", kr, req_port);
	}

	kr = mach_port_insert_right(mach_task_self(), 
				    req_port, req_port,
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
	    panic("vproc_port_allocate: can't acquire send rights "
				"kr=0x%x name=0x%x", kr, req_port);

	v->vp_port_name = req_port;
	ux_server_add_port(req_port);
}

void
vproc_port_deallocate(v, has_receive_rt)
	struct vproc *v;
	boolean_t	has_receive_rt;
{
	int kr;

#ifdef TNC
	extern void vproc_debug_msg1();
#endif

	if (has_receive_rt) {
		kr = mach_port_mod_refs(mach_task_self(),
					vproc_to_port_lookup(v),
					MACH_PORT_RIGHT_RECEIVE,
					-1);
		if (kr != KERN_SUCCESS) {
			char buf[4096];
#ifdef TNC
			vproc_debug_msg1(v, buf);	/* fill buf w/msg */
#else
			buf[0] = '\0';			/* null for RAMDISK */
#endif

			panic("vproc_port_deallocate: mach_port_mod_refs fail "
				"(RCV RT) kr=0x%x\n%s", kr, buf);
		}
	}

	/*
	 * Use mach_port_deallocate() to loose the send right
	 * (or DEADNAME following the possible mod ref above)
	 */
	kr = mach_port_deallocate(mach_task_self(),
				  vproc_to_port_lookup(v));
	if (kr != KERN_SUCCESS) {

		char buf[4096];

#ifdef TNC
		vproc_debug_msg1(v, buf);	/* fill buf w/msg */
#else
		buf[0] = '\0';			/* null for RAMDISK */
#endif

		bootnode_printf("vproc_port_deallocate: "
			"mach_port_deallocate failure "
			"(SND RT) kr=0x%x\n%s", kr, buf);
	}

	v->vp_port_name = MACH_PORT_NULL;
}
