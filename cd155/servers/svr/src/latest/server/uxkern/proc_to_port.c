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
/* #$# */
/*
 * HISTORY
 * $Log: proc_to_port.c,v $
 * Revision 1.5  1994/11/18  20:49:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:43:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:05:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:31:54  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:55:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:54:45  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1993/01/07  11:15:41  condict
 * 	Don't create send right for profile reply port, since it is never
 * 	used and fouls up the kernel's no-more-senders notification.
 * 	[1992/09/25  16:54:16  emcmanus]
 *
 * 	pport_to_proc_lookup() returns NULL if port does not correspond to a
 * 	table entry.  pport_to_proc_wakeup() now has a return value, to
 * 	indicate whether the port corresponded to a table entry; it does
 * 	nothing if not.
 * 	[1992/09/24  19:56:54  emcmanus]
 *
 * Revision 2.4  92/08/26  12:14:22  loverso
 * 	Allow to compile with ANSI compiler. (loverso)
 * 
 * Revision 2.3  92/05/24  13:59:39  pjg
 * 	Revision 3.1  92/03/31  15:41:21  emcmanus
 * 	Don't need to remove a port from a port set before destroying it.
 *
 * Revision 2.2  91/10/14  13:24:15  sjs
 * 	  * Revision 2.1  91/10/14  13:24:12  sjs
 * 	  * Created.
 * 	  * 
 * 
 * Revision 3.0  91/09/27  11:54:38  emcmanus
 * 	Installed for profiling.
 * 
 * 	91/05/24  10:22:46  adn
 * 	Created.
 * 	[91/04/01            adn]
 * 
 * $EndLog$
 */
/*************************************************************************/
/*
 * 	process->profiling_port and profiling_port->process routines.
 */
/*************************************************************************/

#include <sys/proc.h>
#include <kern/queue.h>

#include <uxkern/import_mach.h>



#ifndef	UX_PROFIL_REMOVE_PORT
#define	UX_PROFIL_REMOVE_PORT

#define	ux_profil_remove_port(port)  ux_server_remove_port(port)
extern	void	ux_server_remove_port();

#else
extern	void	ux_profil_remove_port();

#endif

extern	void	ux_profil_add_port();



#define	NPORTHASH	32

#if	((NPORTHASH-1)&NPORTHASH) == 0
#define	PORTHASH(t)	(((unsigned int)t) & (NPORTHASH-1))
#else
#define	PORTHASH(t)	(((unsigned int)t) % NPORTHASH)
#endif

typedef enum {NO_THREAD, RECEIVER_THREAD, SYSTEM_THREAD}   enum_data_t;

struct pproc_hash {
	queue_chain_t	  port_to_proc_chain;
	mach_port_t	  port;
	struct proc *	  proc;
	struct mutex	  lock;
	struct condition  sleepcond;
	enum_data_t	  blocked;
};


queue_head_t	port_to_proc_hash[NPORTHASH];

struct mutex	port_to_proc_lock = MUTEX_INITIALIZER;

void pport_to_proc_init()
{
	register int	i;

	for (i = 0; i < NPORTHASH; i++)
	    queue_init(&port_to_proc_hash[i]);
}


/*
 * Allocate profiling reply port for process.
 * Enter port and process in hash table, and return the port.
 */
mach_port_t
pport_to_proc_enter(p)
	struct proc *	p;
{
	register struct pproc_hash *ph;
	register queue_t q;
	mach_port_t	reply_port;
	kern_return_t	kr;

	kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&reply_port);
	if (kr != KERN_SUCCESS)
	    panic("port_to_proc_enter: can't allocate reply port");

 	ux_profil_add_port(reply_port);

	ph = (struct pproc_hash *)malloc(sizeof(struct pproc_hash));

	/* Some initialization */
	ph->port = reply_port;
	ph->proc = p;
	mutex_init(&ph->lock);
	condition_init(&ph->sleepcond);
	ph->blocked = NO_THREAD;

	mutex_lock(&port_to_proc_lock);

	q = &port_to_proc_hash[PORTHASH(reply_port)];
	queue_enter(q, ph, struct pproc_hash *, port_to_proc_chain);

	mutex_unlock(&port_to_proc_lock);

	return (reply_port);
}


#define pport_to_proc_entry(port, ph)    \
        /* mach_port_t  	port; */ \
	/* struct pproc_hash	*ph;  */ \
	{				 \
	   register queue_t q;		 \
					 \
	   q = &port_to_proc_hash[PORTHASH( (port) )]; \
	   for ((ph) = (struct pproc_hash *)queue_first(q); \
	        ! queue_end(q, (queue_entry_t)(ph)); \
	        (ph) = (struct pproc_hash *)queue_next(&(ph)->port_to_proc_chain)) { \
	       if ((ph)->port == (port)) \
		   break; \
	   } \
	}


struct proc *
pport_to_proc_lookup(port)
	register mach_port_t	port;
{
	struct proc *p = 0;

	register struct pproc_hash *ph;

	mutex_lock(&port_to_proc_lock);

	pport_to_proc_entry(port, ph); 
	if (ph)
	   p = ph->proc;

	mutex_unlock(&port_to_proc_lock);
	return (p);
}


#define pportprintf(str) /* nothing */

int
pport_to_proc_wakeup(port)
	register mach_port_t 	port;
{
	register struct pproc_hash *ph;

	mutex_lock(&port_to_proc_lock);
	pport_to_proc_entry(port, ph);
	mutex_unlock(&port_to_proc_lock);

	if (ph == NULL)
	    return 0;

	mutex_lock(&ph->lock);

	if (ph->blocked == NO_THREAD) {
pportprintf("@@@@@@@@@@@@  Receiver: first at the RV\n");
	   ph->blocked = RECEIVER_THREAD;
	   condition_wait(&ph->sleepcond, &ph->lock); 
	}
	else {
	/* ph->blocked == SYSTEM_THREAD */
pportprintf("@@@@@@@@@@@@  Receiver: second at the RV\n");

	   condition_signal(&ph->sleepcond);
	   condition_wait(&ph->sleepcond, &ph->lock); 
	}
pportprintf("@@@@@@@@@@@@  Receiver: waked up !\n");

	mutex_unlock(&ph->lock);

	return 1;
}

void
pport_to_proc_sleep(port)
	register mach_port_t 	port;
{
	register struct pproc_hash *ph;

	mutex_lock(&port_to_proc_lock);
	pport_to_proc_entry(port, ph);
	mutex_unlock(&port_to_proc_lock);


	mutex_lock(&ph->lock);

	if (ph->blocked == NO_THREAD) {
pportprintf("@@@@@@@@@@@@  Profil_get: first at the RV\n");
	   ph->blocked = SYSTEM_THREAD;
	   condition_wait(&ph->sleepcond, &ph->lock); 
pportprintf("@@@@@@@@@@@@  Profil_get: waked up !\n");
	}

	condition_signal(&ph->sleepcond);

	mutex_unlock(&ph->lock);

}	


/*
 * Remove the port from the profiling set, and destroy it.
 */
void
pport_to_proc_remove(port)
	register mach_port_t	port;
{
	register struct pproc_hash *ph;
	register queue_t q;

	register struct proc *p;

	mutex_lock(&port_to_proc_lock);

	q = &port_to_proc_hash[PORTHASH(port)];
	for (ph = (struct pproc_hash *)queue_first(q);
	     ! queue_end(q, (queue_entry_t)ph);
	     ph = (struct pproc_hash *)queue_next(&ph->port_to_proc_chain)) {
	    if (ph->port == port) {
		p = ph->proc;
		break;
	    }
	}
	if (p == 0)
	    panic("port_to_proc remove");

	queue_remove(q, ph, struct pproc_hash *, port_to_proc_chain);

	mutex_unlock(&port_to_proc_lock);

	free((char *)ph);

	(void) mach_port_destroy(mach_task_self(), port);
}

/* #$# */
