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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * Copyright 1991 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * HISTORY
 * 24-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Minor fixes in case thread/task deallocated before profiling stopped
 *
 * $Log: profile.h,v $
 * Revision 1.5  1994/11/18  20:52:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:45:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:36:25  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:36:59  dleslie
 * First R1_0 release
 *
 * Revision 2.1.2.1  92/09/15  17:22:51  jeffreyh
 * 	Moved from mach/profil.h to kern/profile.h
 * 	Removed mach/profilparam.h
 * 	[92/07/24            bernadat]
 * 
 * Revision 2.1.6.1  92/02/18  19:13:54  jeffreyh
 * 	Fixed Profiling code. [emcmanus@gr.osf.org]
 * 	Data structures and macros used for the  profiling service.
 * 	(Bernard Tabib & Andrei Danes @ gr.osf.org)
 * 	[91/09/26  04:49:23  bernadat]
 * 
 */

#ifndef _KERN_PROFILE_H
#define _KERN_PROFILE_H

#include <mach_prof.h>
#include <mach/boolean.h>
#include <vm/vm_kern.h> 

#if MACH_PROF
	
#define	NB_PROF_BUFFER		2	/* number of buffers servicing a 
					 *	profiled thread */
#define	SIZE_PROF_BUFFER	200	/* size of a profil buffer (in int) */
					/* -> at most 1 packet every 2 secs */

struct	prof_data {
	struct ipc_port *prof_port;	/* where to send a full buffer */

	struct buffer {
	    queue_chain_t p_list;
	    int	*p_zone;		/* points to the actual storage area */
	    int p_index;		/* next slot to be filled */
	    boolean_t p_full;		/* is the current buffer full ? */ 
	    struct prof_data *p_prof;	/* base to get prof_port */
	    char p_wakeme;		/* do wakeup when sent */
	} prof_area[NB_PROF_BUFFER];

	int		prof_index;	/* index of the buffer structure
					 *   currently in use */

};

typedef struct prof_data	*prof_data_t;
#define NULLPROFDATA ((prof_data_t) 0)
typedef struct buffer		*buffer_t;
#define NULLPBUF ((buffer_t) 0)

/* Macros */

#define	set_pbuf_nb(pbuf, nb) \
         (((nb) >= 0 && (nb) < NB_PROF_BUFFER) \
	 ? (pbuf)->prof_index = (nb), 1 \
	 : 0)


#define	get_pbuf_nb(pbuf) \
	(pbuf)->prof_index


extern vm_map_t kernel_map; 

/* MACRO set_pbuf_value 
** 
** enters the value 'val' in the buffer 'pbuf' and returns the following
** indications:     0: means that a fatal error occured: the buffer was full
**                       (it hasn't been sent yet)
**                  1: means that a value has been inserted successfully
**		    2: means that we'v just entered the last value causing 
**			the current buffer to be full.(must switch to 
** 			another buffer and signal the sender to send it)
*/ 
	  
#define set_pbuf_value(pbuf, val) \
	 { \
	  register buffer_t a = &((pbuf)->prof_area[(pbuf)->prof_index]); \
	  register int i = a->p_index++; \
	  register boolean_t f = a->p_full; \
			  \
	  if (f == TRUE ) \
             *(val) = 0; \
	  else { \
	    a->p_zone[i] = *(val); \
	    if (i == SIZE_PROF_BUFFER-1) { \
               a->p_full = TRUE; \
               *(val) = 2; \
            } \
            else \
		*(val) = 1; \
          } \
	}

         
#define	reset_pbuf_area(pbuf) \
	{ \
	 register int *i = &((pbuf)->prof_index); \
					      \
	 *i = (*i == NB_PROF_BUFFER-1) ? 0 : ++(*i); \
	 (pbuf)->prof_area[*i].p_index = 0; \
	}


/*
** Global variable: the head of the queue of buffers to send 
** It is a queue with locks (uses macros from queue.h) and it
** is shared by hardclock() and the sender_thread() 
*/

mpqueue_head_t prof_queue; 

#define task_prof_init(task) \
	task->task_profiled = FALSE; \
     	task->profil_buffer = NULLPROFDATA;

#define thread_prof_init(thread, task) \
	thread->thread_profiled = task->task_profiled;	\
	thread->profil_buffer = task->profil_buffer;

#define task_prof_deallocate(task) \
	if (task->profil_buffer) \
		task_sample(task, MACH_PORT_NULL); \

#define thread_prof_deallocate(thread) \
	if (thread->thread_profiled_own && thread->profil_buffer)  \
		thread_sample(thread, MACH_PORT_NULL); \

#else	MACH_PROF

#define task_prof_init(task)
#define thread_prof_init(thread, task)
#define task_prof_deallocate(task)
#define thread_prof_deallocate(thread)

#endif	MACH_PROF
#endif	_KERN_PROFILE_H
