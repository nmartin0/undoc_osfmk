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
 * 28-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Fixed comments about MIG interface
 *
 * $Log: profile.c,v $
 * Revision 1.6  1994/11/18  20:52:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/03/14  19:03:00  stans
 *  msg_return_t --> mach_msg_return_t; replaced old Mach 2.5 IPC name.
 *
 * Revision 1.4  1993/06/30  22:45:26  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:36:23  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:36:55  dleslie
 * First R1_0 release
 *
 * Revision 2.1.6.2  92/09/15  17:21:43  jeffreyh
 * 	Changes to support kernel task & threads profiling.
 * 	Fixed kernel interface (now uses mig and uses
 * 	a single port). The size of buffers is now
 * 	invisible to users. Detection of last sample is
 * 	possible using no_more_senders notification.
 * 	Some locks are still missing for MP.
 * 	[92/07/17            bernadat]
 * 
 * Revision 2.1.6.1  92/02/18  19:09:45  jeffreyh
 * 	Fixed Profiling code. [emcmanus@gr.osf.org]
 * 
 * Revision 2.1.1.1  91/09/26  04:48:14  bernadat
 * 	Profiling support
 * 	(Bernard Tabib & Andrei Danes @ gr.osf.org)
 * 
 */


#include	<kern/thread.h>
#include	<kern/queue.h>
#include	<kern/profile.h>
#include	<kern/sched_prim.h>
#include	<ipc/ipc_space.h>

extern vm_map_t kernel_map; /* can be discarded, defined in <vm/vm_kern.h> */

thread_t profile_thread_id = THREAD_NULL;

prof_data_t pbuf_alloc();

void profile_thread() 
{
    struct message {
	mach_msg_header_t	head;
	mach_msg_type_t		type;
	int			arg[SIZE_PROF_BUFFER+1];
    } msg;

    register int    s;
    buffer_t	    buf_entry;
    queue_entry_t   prof_queue_entry;
    prof_data_t	    pbuf;
    mach_msg_return_t mr;
    kern_return_t   kr;
    int		    j;



    /* Initialise the queue header for the prof_queue */
    mpqueue_init(&prof_queue);

    while (TRUE) {

	/* Dequeue the first buffer. */
	s = splsched();
	mpdequeue_head(&prof_queue, &prof_queue_entry);
	splx(s);

	if ((buf_entry = (buffer_t) prof_queue_entry) == NULLPBUF) { 
	    assert_wait((int) profile_thread, FALSE);
	    thread_block((void (*)()) 0);
	    if (current_thread()->wait_result != THREAD_AWAKENED)
		break;
	} else {
	    register int    *sample;
	    int		    imax;

	    pbuf = buf_entry->p_prof;
	    kr = send_samples(pbuf->prof_port, buf_entry->p_zone, buf_entry->p_index);
	    if (kr != KERN_SUCCESS)  
	      printf("send_samples(%x, %x, %d) error %x\n", pbuf->prof_port, buf_entry->p_zone, buf_entry->p_index, kr); 
		  
	    /* Indicate you've finished the dirty job */
	    buf_entry->p_full = FALSE;
	    if (buf_entry->p_wakeme)
	      thread_wakeup((int) &buf_entry->p_wakeme);
	}

    }
    /* The profile thread has been signalled to exit.  Any threads waiting
       for the last buffer of samples to be acknowledged should be woken
       up now.  */
    profile_thread_id = THREAD_NULL;
    while (1) {
	s = splsched();
	mpdequeue_head(&prof_queue, &prof_queue_entry);
	splx(s);
	if ((buf_entry = (buffer_t) prof_queue_entry) == NULLPBUF)
	    break;
	if (buf_entry->p_wakeme)
	    thread_wakeup((int) &buf_entry->p_wakeme);
    }
    thread_halt_self();
}



#include <mach/message.h>

void
send_last_sample_buf(pbuf)
prof_data_t pbuf;
{
    register int s;
    buffer_t buf_entry;

    if (pbuf == NULLPROFDATA)
	return;

    /* Ask for the sending of the last PC buffer.
     * Make a request to the profile_thread by inserting
     * the buffer in the send queue, and wake it up. 
     * The last buffer must be inserted at the head of the
     * send queue, so the profile_thread handles it immediatly. 
     */ 
    buf_entry = pbuf->prof_area + pbuf->prof_index;
    buf_entry->p_prof = pbuf;

    /* Watch out in case profile thread exits while we are about to
       queue data for it.  */
    s = splsched();
    if (profile_thread_id == THREAD_NULL)
	splx(s);
    else {
	buf_entry->p_wakeme = 1;
	mpenqueue_tail(&prof_queue, &buf_entry->p_list);
	thread_wakeup((int) profile_thread);
	assert_wait((int) &buf_entry->p_wakeme, TRUE);
	splx(s); 
	thread_block((void (*)()) 0);
    }
}


/*
 * Add a profile sample for the current pbuf.
 */

profile(pc, pbuf)
prof_data_t pbuf;
{
    int inout_val = pc; 
    buffer_t buf_entry;
    int *val;

    if (pbuf == NULLPROFDATA)
	return;
    
    /* Inserts the PC value in the buffer of the thread */
    set_pbuf_value(pbuf, &inout_val); 
    switch(inout_val) {
    case 0: 
	if (profile_thread_id == THREAD_NULL) {
	  reset_pbuf_area(pbuf);
	} else printf("profile: full buffer unsent\n");
	break;
    case 1: 
	/* Normal case, value successfully inserted */
	break;
    case 2 : 
	/*
	 * The value we have just inserted caused the
	 * buffer to be full, and ready to be sent.
	 * If profile_thread_id is null, the profile
	 * thread has been killed.  Since this generally
	 * happens only when the O/S server task of which
	 * it is a part is killed, it is not a great loss
	 * to throw away the data.
	 */
	if (profile_thread_id == THREAD_NULL) {
	  reset_pbuf_area(pbuf);
	  break;
	}

	buf_entry = (buffer_t) &pbuf->prof_area[pbuf->prof_index];
	buf_entry->p_prof = pbuf;
	mpenqueue_tail(&prof_queue, &buf_entry->p_list);
	
	/* Switch to another buffer */
	reset_pbuf_area(pbuf);
	
	/* Wake up the profile thread */
	if (profile_thread_id != THREAD_NULL)
	  thread_wakeup((int) profile_thread);
	break;
      
      default: 
	printf("profile : unexpected case\n"); 
    }
}


kern_return_t
thread_sample (thread, reply)
thread_t	thread;
ipc_port_t	reply;
{
    /* 
     * This routine is called every time that a new thread has made
     * a request for the sampling service. We must keep track of the 
     * correspondance between its identity (thread) and the port
     * we are going to use as a reply port to send out the samples resulting 
     * from its execution. 
     */
    prof_data_t	    pbuf;
    vm_offset_t	    vmpbuf;

    if (reply != MACH_PORT_NULL) {
	if (thread->thread_profiled) 
		return KERN_INVALID_ARGUMENT;
	/* Start profiling this thread, do the initialization. */
	pbuf = pbuf_alloc();
	if ((thread->profil_buffer = pbuf) == NULLPROFDATA) {
	    printf("mach_sample_thread: cannot allocate pbuf\n");
	    return KERN_RESOURCE_SHORTAGE;
	} 
	else {
	    if (!set_pbuf_nb(pbuf, NB_PROF_BUFFER-1)) {
		printf("mach_sample_thread: cannot set pbuf_nb\n");
		return KERN_FAILURE;
	    }
	    reset_pbuf_area(pbuf);
	}
	pbuf->prof_port = reply;
	thread->thread_profiled = TRUE;
	thread->thread_profiled_own = TRUE;
	if (profile_thread_id == THREAD_NULL)
	    profile_thread_id = kernel_thread(kernel_task, profile_thread);
    } else {
	if (!thread->thread_profiled)
		return(KERN_INVALID_ARGUMENT);

	thread->thread_profiled = FALSE;
	/* do not stop sampling if thread is not profiled by its own */

	if (!thread->thread_profiled_own)
	    return KERN_SUCCESS;
	else
	    thread->thread_profiled_own = FALSE;

	send_last_sample_buf(thread->profil_buffer);
	pbuf_free(thread->profil_buffer);
	thread->profil_buffer = NULLPROFDATA;
    }
    return KERN_SUCCESS;
}

kern_return_t
task_sample (task, reply)
ipc_port_t	reply;
task_t		task;
{
    prof_data_t	    pbuf=task->profil_buffer;
    vm_offset_t	    vmpbuf;
    int		    turnon = (reply != MACH_PORT_NULL);

    if (turnon) {
	if (task->task_profiled)
		return(KERN_INVALID_ARGUMENT);
	pbuf = pbuf_alloc();
	if (pbuf == NULLPROFDATA)
	  return KERN_RESOURCE_SHORTAGE;
	task->profil_buffer = pbuf;
	
	if (!set_pbuf_nb(pbuf, NB_PROF_BUFFER-1))
	    return KERN_FAILURE;
	reset_pbuf_area(pbuf);
	pbuf->prof_port = reply;
    } else
	if (!task->task_profiled)
		return(KERN_INVALID_ARGUMENT);


    if (turnon != task->task_profiled) {
	int actual, i;
	thread_t thread;

	if (turnon && profile_thread_id == THREAD_NULL)
	    profile_thread_id =
		kernel_thread(kernel_task, profile_thread);
	task->task_profiled = turnon;  
	actual = task->thread_count; 
	for (i = 0, thread = (thread_t) queue_first(&task->thread_list);
	     i < actual;
	     i++, thread = (thread_t) queue_next(&thread->thread_list)) {
		  if (!thread->thread_profiled_own) {
		    thread->thread_profiled = turnon;
		    if (turnon) {
			thread->profil_buffer = task->profil_buffer;
		  	thread->thread_profiled = TRUE;
		    } else {
			thread->thread_profiled = FALSE;
		        thread->profil_buffer = NULLPROFDATA;
		    }
		  }
	}
	if (!turnon) {
	    send_last_sample_buf(task->profil_buffer);
	    pbuf_free(task->profil_buffer);
	    task->profil_buffer = NULLPROFDATA;
	}
    }

    return KERN_SUCCESS;
}

prof_data_t
pbuf_alloc()
{
	register prof_data_t pbuf;
	register i;
	register int *zone;

	pbuf = (prof_data_t)kalloc(sizeof(struct prof_data));
	if (!pbuf)
		return(NULLPROFDATA);
	pbuf->prof_port = MACH_PORT_NULL;
	for (i=0; i< NB_PROF_BUFFER; i++) {
		zone = (int *)kalloc(SIZE_PROF_BUFFER*sizeof(int));
		if (!zone) {
			i--;
			while (i--)
				kfree(pbuf->prof_area[i].p_zone);
			kfree(pbuf);
			return(NULLPROFDATA);
		}
		pbuf->prof_area[i].p_zone = zone;
		pbuf->prof_area[i].p_full = FALSE;
	}
	pbuf->prof_port = MACH_PORT_NULL;
	return(pbuf);
}

pbuf_free(pbuf)
prof_data_t pbuf;
{
	register i;

	if (pbuf->prof_port)
		ipc_port_release_send(pbuf->prof_port);
	
	for(i=0; i < NB_PROF_BUFFER ; i++)
		kfree(pbuf->prof_area[i].p_zone, SIZE_PROF_BUFFER*sizeof(int));
	kfree(pbuf, sizeof(struct prof_data));
}



