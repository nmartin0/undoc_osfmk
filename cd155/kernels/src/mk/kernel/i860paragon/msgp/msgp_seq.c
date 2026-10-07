/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_seq.c,v 1.5 1995/01/11 22:36:38 joel Exp $
 */

/*
 * mcmsg_seq.c
 *
 * NX sequence selector management
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>

/*
 * The sequence selector has a more complex list structure than
 * regular selectors.
 *
 * Selection of a particular NX message requires the source pid and message
 * sequence number. The source pid is looked up first, then the sequence.
 *
 * The selector mcmsg_seq_sel selects on pid to NX request items.
 * Each NX request item represents a message in progress from some process.
 * The NX request item for the first message from a particular process is
 * directly in the selection list. Other messages from the same process are
 * linked from the first on the nxrq.seq_link field in a singly linked
 * null-terminated list.
 */

/*
 * Routine:
 *	mcmsg_install_sequence(mt, source_pid, si)
 *
 * Arguments:
 *	mt:		mcmsg_task pointer
 *	source_pid:	PID of source process
 *	si:		Select item to install
 *
 * Purpose:
 *	Installs a select item in the sequence selector.
 *
 * Returns:
 *	none
 */

mcmsg_install_sequence(mt, source_pid, si)
	mcmsg_task_t		*mt;
	register long		source_pid;
	register select_item_t	*si;
{
	register select_item_t	*seqhead; /* Head message for source_pid */

	/*
	 * Look up source_pid
	 */

	seqhead = mcmsg_selector_lookup_si(&mcmsg_seq_sel, source_pid);
	if (seqhead != 0) {

		/*
		 * Add into existing list
		 * The order doesn't matter, so put it in the fastest way
		 */

		si->nxrq.seq_link = seqhead->nxrq.seq_link;
		seqhead->nxrq.seq_link = si;
	} else {

		/*
		 * Install as the first message from source_pid
		 */

		si->nxrq.seq_link = 0;
		mcmsg_selector_install_si(mt,
					  &mcmsg_seq_sel,
					  si,
					  source_pid,
					  si->method);
	}
}

/*
 * Routine:
 *	mcmsg_lookup_sequence(source_pid, sequence)
 *
 * Arguments:
 *	source_pid:	PID of source process
 *	sequence:	Message sequence number
 *
 * Purpose:
 *	Looks up a select item in the sequence selector.
 *
 * Returns:
 *	Select item pointer or zero if not found.
 */

#if	HANDCODE
/* See msgp_sel.s */
#else	HANDCODE

select_item_t *
mcmsg_lookup_sequence(source_pid, sequence)
	register long		source_pid;
	register unsigned long	sequence;
{
	register select_item_t	*si;
	int			t;

	/*
	 * Look for first active message from source_pid
	 */

	si = mcmsg_selector_lookup_si(&mcmsg_seq_sel, source_pid);
	if (si != 0) {

		assert(si->method != SELMETH_TASK); /* beware of old code */

		/*
		 * There is at least one message
		 * Search the list for desired sequence number
		 */

		assert((t = MAXLOOP) != 0);
		while (si != 0) {
			if (si->nxrq.sequence == sequence) {
				break;
			}
			si = si->nxrq.seq_link;
			assert(t-- != 0);
		}
	}
	return si;
}
#endif	HANDCODE

/*
 * Routine:
 *	mcmsg_remove_sequence(mt, source_pid, si)
 *
 * Arguments:
 *	mt:		mcmsg_task pointer
 *	source_pid:	PID of source process
 *	si:		Select item to remove
 *
 * Purpose:
 *	Removes a select item from the sequence selector and frees it.
 *
 * Returns:
 *	none
 */

mcmsg_remove_sequence(mt, source_pid, si)
	mcmsg_task_t		*mt;
	register long		source_pid;
	register select_item_t	*si;
{
	register select_item_t	*seqhead;
	int			t;

	/*
	 * Look up the first message from source_pid
	 */

	seqhead = mcmsg_selector_lookup_si(&mcmsg_seq_sel, source_pid);
	assert(seqhead != 0);

	if (seqhead == si) {

		/*
		 * Remove the first one from the selector
		 * Frees the item as a side effect
		 * [This code could be faster with a mcmsg_selector_replace()]
		 */

		si = si->nxrq.seq_link;
		mcmsg_selector_remove(&mcmsg_seq_sel, source_pid);

		/*
		 * If there is another, install it in the selector
		 */

		if (si != 0) {
			mcmsg_selector_install_si(mt,
						  &mcmsg_seq_sel,
						  si,
						  source_pid,
						  si->method);
		}

	} else {

		/*
		 * Scan the list for the desired item
		 * Look at the link so to stop at the previous item
		 */

		assert((t = MAXLOOP) != 0);
		while (seqhead->nxrq.seq_link != si) {
			seqhead = seqhead->nxrq.seq_link;
			assert(t-- != 0);
		}

		/*
		 * Unlink the target item from the previous item
		 */

		seqhead->nxrq.seq_link = si->nxrq.seq_link;

		/*
		 * Free the item
		 */

		mcmsg_free_select_item(si);
	}
}

/*
 * Routine:
 *	mcmsg_selector_clear_task_seq(mt)
 *
 * Arguments:
 *	mt: Task pointer
 *
 * Purpose:
 *	Free all the message sequence select items for a task.
 *
 * Returns:
 *	none
 */

mcmsg_selector_clear_task_seq(mt)
	mcmsg_task_t		*mt;
{
	register int		hi;		/* Hash index */
	register void		**selhead;	/* Pointer to head entry */
	register select_item_t	*seqhead;	/* Main scan item */
	register select_item_t	*siprev;  /* Item before seqhead in main list */
	register select_item_t	*sitail;	/* Main list tail */
	register select_item_t	*sinext;	/* Main list next */
	register select_item_t	*seqnext;	/* Sub list next */
	register select_item_t	*seqprev;  /* Item before seqnext in sub list */
	unsigned long		t;

	mcmsg_trace_debug("sel clear seq", 2, &mcmsg_seq_sel, mt, 0, 0);

	/*
	 * Loop thru hash table.
	 */

	for (hi = 0; hi <= SELECT_HASH_LEN; hi++) {

		/*
		 * Get a pointer to the slot. The zero slot is last.
		 */

		if (hi == SELECT_HASH_LEN) {
			selhead = &mcmsg_seq_sel.zero;
		} else {
			selhead = &mcmsg_seq_sel.hash[hi];
		}
		
		/*
		 * Clear this list
		 */

		sitail = *selhead;
		if (sitail == 0)
			continue;
		siprev = sitail;
		seqhead = sitail->link;
		assert((t = MAXLOOP) != 0);

		/*
		 * Loop thru list of first messages
		 */

		for (;;) {

			/*
			 * Loop thru the sublist
			 */

			for (seqprev = seqhead; ; ) {

				/*
				 * Inspect the next one in the list
				 */

				seqnext = seqprev->nxrq.seq_link;
				if (seqnext == 0)
					break;

				/*
				 * Remove if it matches
				 * otherwise skip
				 */

				if (seqnext->mcmsg_task == mt) {
					seqprev->nxrq.seq_link =
						seqnext->nxrq.seq_link;
					mcmsg_free_select_item(seqnext);
				} else {
					seqprev = seqnext;
				}
			}

			/*
			 * Dispose of the first message
			 * if it matches
			 */

			if (seqhead->mcmsg_task == mt) {

				/*
				 * Pick up links
				 * and free it
				 */

				sinext = seqhead->link;
				seqnext = seqhead->nxrq.seq_link;
				mcmsg_free_select_item(seqhead);

				/*
				 * Correct for case where only one remains
				 */

				if (sinext == seqhead) {
/* PTS 11446 don't just set sinext to seqnext if seqnext is NULL.
   sinext is on a circular list, seqnext is on a NULL terminated list */
					if (seqnext) {
						sinext = seqnext;
					} else { 
					/* nothing left on seq chain or list */
						*selhead = 0;
						break;
					}
				}

				/*
				 * If there are other linked messages, replace
				 * this first one with the next.
				 */

				if (seqnext != 0) {

					seqnext->link = sinext;
					siprev->link = seqnext;

					/*
					 * If last one, reset tail pointer
					 * and break from loop
					 */

					if (seqhead == sitail) {
						*selhead = seqnext;
						break;
					}

					/*
					 * Otherwise replace link
					 * and skip to next
					 */

					siprev = seqnext;

				} else {

					/*
					 * Only seqhead remains in this
					 * sequence chain.
					 */
/* this next line truncates the free list in the case where there was 
   only one item on list and one item left in sequence chain.  Code
   above that notices one item on list sets sinext to next item on 
   sequence chain which could have been NULL.  With only one item on
   list siprev == seqhead which was already freed. So seting siprev-link
   to 0 truncates free list.  fixed above. PTS 11446 */
					siprev->link = sinext;
					if (seqhead == sitail) {
						*selhead = sinext;
						break;
					}
				}

			} else {

				/*
				 * This first message stays. Skip over it.
				 */

				siprev = seqhead;
			}

			/*
			 * Break out at tail
			 */

			if (seqhead == sitail) {
				break;
			}

			/*
			 * Go on to next first message
			 */

			seqhead = siprev->link;
		}
	}
}
