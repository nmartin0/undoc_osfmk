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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_select.c,v 1.20 1994/11/18 20:47:55 mtm Exp $
 */

/*
 * msgp_select.c
 *
 * Selection
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>


/*
 * Routine:
 *	mcmsg_select_init()
 *
 * Purpose:
 *	Selector module initialization
 *	Currently null.
 *
 * Returns:
 *	none
 */

mcmsg_select_init()
{

	return;
}

/*
 * Routine:
 *	mcmsg_selector_init(sel, method)
 *
 * Arguments:
 *	sel:	Selector pointer
 *	method:	New method
 *
 * Purpose:
 *	Clears a selector structure and sets the method.
 *
 * Returns:
 *	none
 */

mcmsg_selector_init(sel, method)
	select_t	*sel;
	unsigned long	method;
{

	bzero(sel, sizeof(select_t));
	sel->method = method;
}

/*
 * Routine:
 *	mcmsg_selector_install(mt, sel, value, item, method)
 *
 * Arguments:
 *	mt:	mcmsg_task pointer
 *	sel:	Selector pointer
 *	value:	Key value
 *	item:	Usually a data structure pointer
 *	method:	New method
 *
 * Purpose:
 *	Installs a new item in the given selector. The item can be found in
 *	the selector by looking it up using the key value.
 *	Allocates and links a select_item to the hash table in the selector.
 *
 * Returns:
 *	none
 */

mcmsg_selector_install(mt, sel, value, item, method)
	mcmsg_task_t		*mt;
	register select_t	*sel;
	register unsigned long	value;
	register void		*item;
	register unsigned long	method;
{
	register select_item_t	*sihead;	/* Head item */
	register select_item_t	*sitail;	/* Tail item */
	register select_item_t	*si;		/* New item */
	register unsigned long	hi;		/* Hash index */

	/*
	 * Allocate and fill in select item
	 */

	si = mcmsg_alloc_select_item();
	assert(si != 0);
	si->value = value;
	si->item = item;
	si->method = method;
	si->link = si;
	si->mcmsg_task = mt;

	/*
	 * Install select item in selector
	 */

	if (value == 0) {
		/* Install zero item */
		sitail = sel->zero;
		sel->zero = si;
	} else {
		/* Start installation of arbitrary item */
		hi = SELECT_HASH_FUN(value);
		assert(hi < SELECT_HASH_LEN);
		sitail = sel->hash[hi];
		sel->hash[hi] = si;
	}
	if (sitail != 0) {
		/* Finish installation */
		sihead = sitail->link;
		si->link = sihead;
		sitail->link = si;
	}
}

/*
 * Routine:
 *	mcmsg_selector_install_si(mt, sel, si, value, method)
 *
 * Arguments:
 *	mt:	mcmsg_task pointer
 *	sel:	Selector pointer
 *	si:	Select item
 *	value:	Key value
 *	method:	New method
 *
 * Purpose:
 *	Installs a new item in the given selector.
 *	The select_item to be used is passed in from the caller.
 *	Links the select_item to the hash table in the selector.
 *
 * Returns:
 *	none
 */

mcmsg_selector_install_si(mt, sel, si, value, method)
	mcmsg_task_t		*mt;
	register select_t	*sel;
	register select_item_t	*si;
	register unsigned long	value;
	register unsigned long	method;
{
	register select_item_t	*sihead;	/* Head item */
	register select_item_t	*sitail;	/* Tail item */
	register unsigned long	hi;		/* Hash index */

	/*
	 * Fill in select item
	 */

	si->link = si;
	si->mcmsg_task = mt;
	si->value = value;
	si->method = method;
	si->item = (void *)si;

	if (value == 0) {
		/* Install zero item */
		sitail = sel->zero;
		sel->zero = si;
	} else {
		/* Start installation of arbitrary item */
		hi = SELECT_HASH_FUN(value);
		assert(hi < SELECT_HASH_LEN);
		sitail = sel->hash[hi];
		sel->hash[hi] = si;
	}
	if (sitail != 0) {
		/* Finish installation */
		sihead = sitail->link;
		si->link = sihead;
		sitail->link = si;
	}
}

/*
 * Routine:
 *	mcmsg_selector_lookup_si(sel, value)
 *
 * Arguments:
 *	sel:	Selector pointer
 *	value:	Key value
 *
 * Purpose:
 *	Finds a select_item in a selector by key value.
 *
 * Returns:
 *	Select item pointer
 */

#if	HANDCODE
/* See msgp_sel.s */
#else	HANDCODE

select_item_t *
mcmsg_selector_lookup_si(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*si;		/* Search item */
	register select_item_t	*sitail;	/* Tail item */
	register unsigned long	hi;		/* Hash index */

	if (value == 0) {

		/*
		 * Return the zero item
		 */

		return sel->zero;

	} else {
		unsigned long t;

		/*
		 * Look up hash entry
		 */

		hi = SELECT_HASH_FUN(value);
		assert(hi < SELECT_HASH_LEN);
		sitail = sel->hash[hi];
		if (sitail == 0) {
			return 0;
		}

		/*
		 * Search the linked list from head to tail
		 * for the desired value
		 */

		assert((t = MAXLOOP) != 0);
		for (si = sitail->link; ; si = si->link) {
			assert(si != 0);
			if (si->value == value)
				break;
			if (si == sitail) {
				return 0;
			}
			assert(t-- != 0);
		}
	}
	return si;
}
#endif	HANDCODE


/*
 * Routine:
 *	mcmsg_selector_detach(sel, value)
 *
 * Arguments:
 *	sel:	Selector pointer
 *	value:	Key value
 *
 * Purpose:
 *	Deinstalls an item in the given selector by key value.
 *	This code depends on the fact that the link is the
 *	first field of the select_item structure.
 *
 * Returns:
 *	The select_item pointer for the deinstalled item.
 */

select_item_t *
mcmsg_selector_detach(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*si;		/* Search item */
	register select_item_t	*siprev;	/* Item before si on the list */
	register select_item_t	*sitail;	/* Tail item */
	register void		**seltail;	/* Pointer to head entry */
	register unsigned long	hi;		/* Hash index */

	if (value == 0) {
		/* Zero entry */
		sitail = sel->zero;
		if (sitail) {
		    if (sitail == sitail->link) {
			sel->zero = 0;
		    } else {
			si = sitail->link;
			sitail->link = si->link;
			return(si);
		    }
		}
		return sitail;
	}

	/*
	 * List pointed to by hash table entry
	 */

	hi = SELECT_HASH_FUN(value);
	assert(hi < SELECT_HASH_LEN);
	seltail = &sel->hash[hi];

	/*
	 * Get tail item
	 */

	sitail = *seltail;
	if (sitail == 0) {
		return 0;
	}

	/*
	 * Search list from head to tail with si
	 * siprev follows si
	 */

	siprev = sitail;
	si = sitail->link;
	if (si == sitail) {
		/* Special case for one item in list */
		if (si->value != value)
			return 0;
		*seltail = 0;
	} else {
		unsigned long t;

		/*
		 * Search the list
		 */

		assert((t = MAXLOOP) != 0);
		for (;;) {
			if (si->value == value)
				break;
			if (si == sitail)
				return 0;
			siprev = si;
			si = si->link;
			assert(t-- != 0);
		}

		/*
		 * Relink si out of the list
		 */

		siprev->link = si->link;
		if (si == sitail) {
			*seltail = siprev;
		}
	}
	/* Clear the link when asserts enabled */
	assert((si->link = 0) == 0);
	return si;
}


/*
 * Routine:
 *	mcmsg_select_recvtype_detach(sel, value, node, ptype)
 *
 * Arguments:
 *	sel:	Selector pointer
 *	value:	Key value
 *	node: node selector 
 *	ptype: ptype selector 
 *
 * Purpose:
 *	Deinstalls an item representing a recv() in the given selector 
 * 	by key value, node select mask and ptype select mask..
 *	This code depends on the fact that the link is the
 *	first field of the select_item structure.
 *
 * Returns:
 *	The select_item pointer for the deinstalled item.
 */

select_item_t *
mcmsg_select_recvtype_detach(sel, value, node, ptype)
	register select_t	*sel;
	register unsigned long	value;
	register unsigned long	node;
	register unsigned long	ptype;
{
	register select_item_t	*si;		/* Search item */
	register select_item_t	*siprev;	/* Item before si on the list */
	register select_item_t	*sitail;	/* Tail item */
	register void		**seltail;	/* Pointer to head entry */
	register unsigned long	hi;		/* Hash index */
	register unsigned long t;

	if (value == 0) {
		/* Zero entry */
		sitail = sel->zero;
		if (sitail) {
		    if (sitail == sitail->link) {
		    	if ((sitail->method == SELMETH_RECV_TYPE) ||
			    (((sitail->nxrq.dest_node == -1) ||
			      (sitail->nxrq.dest_node == node)) &&
			     ((sitail->nxrq.dest_ptype == -1) ||
			      (sitail->nxrq.dest_ptype == ptype)))) {
				sel->zero = 0;
			} else {
				sitail = 0;
			}
		    } else {
			si = sitail;
			assert((t = MAXLOOP) != 0);
			do {
				siprev = si;
				si = si->link;
				if ((si->method == SELMETH_RECV_TYPE) ||
					/* SELMETH_RECV_TYPE */
				    (((si->nxrq.dest_node == -1) ||
                              	      (si->nxrq.dest_node == node)) &&
                             	     ((si->nxrq.dest_ptype == -1) ||
                              	      (si->nxrq.dest_ptype == ptype)))) {
					/*
					 * see if item is tail item; if
					 * so then must update sel->zero;
					 */
					if(si == sitail) {
						sel->zero = siprev;
					}
					siprev->link = si->link;
					assert((si->link = 0) == 0);
					return(si);
				}
				assert(t-- != 0);
			} while (si != sitail);
			sitail = 0;
		    }
		}
		return sitail;
	}

	/*
	 * List pointed to by hash table entry
	 */

	hi = SELECT_HASH_FUN(value);
	assert(hi < SELECT_HASH_LEN);
	seltail = &sel->hash[hi];

	/*
	 * Get tail item
	 */

	sitail = *seltail;
	if (sitail == 0) {
		return 0;
	}

	/*
	 * Search list from head to tail with si
	 * siprev follows si
	 */

	siprev = sitail;
	si = sitail->link;
	if (si == sitail) {
		/* Special case for one item in list */
		if ((si->value == value) &&
		    ((si->method == SELMETH_RECV_TYPE) ||
			/* SELMETH_RECV_TYPE */
		     (((si->nxrq.dest_node == -1) ||
                       (si->nxrq.dest_node == node)) &&
                      ((si->nxrq.dest_ptype == -1) ||
                       (si->nxrq.dest_ptype == ptype))))) {
			*seltail = 0;
		} else {
			return 0;
		}
	} else {
		unsigned long t;

		/*
		 * Search the list
		 */

		assert((t = MAXLOOP) != 0);
		for (;;) {
			if ((si->value == value) &&
		            ((si->method == SELMETH_RECV_TYPE) ||
				/* SELMETH_RECV_TYPE */
		             (((si->nxrq.dest_node == -1) ||
                               (si->nxrq.dest_node == node)) &&
                              ((si->nxrq.dest_ptype == -1) ||
                               (si->nxrq.dest_ptype == ptype))))) {
				break;
			}
			if (si == sitail) {
				return 0;
			}
			siprev = si;
			si = si->link;
			assert(t-- != 0);
		}

		/*
		 * Relink si out of the list
		 */

		siprev->link = si->link;
		if (si == sitail) {
			*seltail = siprev;
		}
	}
	/* Clear the link when asserts enabled */
	assert((si->link = 0) == 0);
	return si;
}

/*
 * Routine:
 *	mcmsg_selector_remove(sel, value)
 *
 * Arguments:
 *	sel:	Selector pointer
 *	value:	Key value
 *
 * Purpose:
 *	Deinstalls an item in the given selector by key value.
 *	Deallocates the associated select_item.
 *
 * Returns:
 *	none
 */

mcmsg_selector_remove(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*si;	/* Select item */

	/*
	 * Find and deinstall the select_item
	 */

	si = mcmsg_selector_detach(sel, value);
	if (si == 0) {
		return;
	}

	/*
	 * Free it
	 */

	mcmsg_free_select_item(si);
}

/*
 * Routine:
 *	mcmsg_clear_item_list(sistart)
 *
 * Arguments:
 *	sistart:	Pointer to a select item in the list
 *
 * Purpose:
 *	Clears all select items from a circular list and frees them.
 *
 * Returns:
 *	none
 */

mcmsg_clear_item_list(sistart)
	select_item_t	*sistart;
{
	register select_item_t	*si;		/* Scan item */
	register select_item_t	*sinext;	/* Next item */
	register unsigned long	t;

	/*
	 * Return if the list was empty
	 */

	if (sistart == 0) {
		return;
	}

	/*
	 * Scan the list from the next item back around to start
	 */

	assert((t = MAXLOOP) != 0);
	si = sistart->link;
	for (;;) {

		/*
		 * Save pointer to next item
		 */

		sinext = si->link;

		/*
		 * Free this item
		 */

		mcmsg_free_select_item(si);

		/*
		 * If it was start, we are done
		 */

		if (si == sistart) {
			break;
		}

		/*
		 * Go on to the next one
		 */

		assert(sinext != si);
		si = sinext;
		assert(t-- != 0);
	}
}

/*
 * Routine:
 *	mcmsg_selector_clear(sel)
 *
 * Arguments:
 *	sel:	Selector pointer
 *
 * Purpose:
 *	Clears all items from the selector.
 *	Does not affect fail pointer.
 *
 * Returns:
 *	none
 */

mcmsg_selector_clear(sel)
	register select_t	*sel;
{
	register int		hi;		/* Hash index */
	register void		**seltail;	/* Pointer to head entry */
	register select_item_t	*si;		/* Scan item */
	register select_item_t	*sitail;	/* Tail item */
	register select_item_t	*selnext;	/* Next item */
	unsigned long		t;

	mcmsg_trace_debug("sel clear", 1, sel, 0, 0, 0);

	/*
	 * Loop through all slots in the selector
	 */

	for (hi = 0; hi <= SELECT_HASH_LEN; hi++) {

		/*
		 * Get a pointer to the slot. The zero slot is last.
		 */

		if (hi == SELECT_HASH_LEN) {
			seltail = &sel->zero;
		} else {
			seltail = &sel->hash[hi];
		}

		/*
		 * Clear this list
		 */

		sitail = *seltail;
		if (sitail != 0) { /* Check for zero, a small speed opt. */

			/*
			 * Clear the list
			 */

			mcmsg_clear_item_list(sitail);


			/*
			 * Zero the hash entry
			 */

			*seltail = 0;
		}
	}
}
