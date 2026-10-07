/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*++ nqs_upd.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upd.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS queue destination state update module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	June 17, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:26 $ $State: Exp $)
 * $Log: nqs_upd.c,v $
 * Revision 1.3  1994/11/19  02:53:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:35  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:43  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:01:45  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */

extern int errno;		/* System call error number */

extern void free();		/* Deallocate memory */
extern char *malloc();		/* Allocate memory */
extern void nqs_abort();	/* Exit */
extern struct queue *nqs_fndnnq();
				/* Find non-network queue by name */
extern void nqs_vtimer();	/* Virtual timer */
extern void nqs_wakdes();	/* Wakeup a pipe queue destination */
extern int strcmp();		/* String comparison */
extern char *strcpy();		/* String copy */
extern time_t time();		/* Get current time */
extern void udb_addquedes();	/* Add a pipe-queue to destination mapping */
				/* to the NQS database image */
extern void udb_credes();	/* Create a pipe queue destination in */
				/* the NQS database image */
extern void udb_deldes();	/* Delete a pipe queue destination from */
				/* the NQS database image */
extern void udb_delque();	/* Delete a queue descriptor and any */
				/* associated queue-ordering or queue- */
				/* access file from the NQS database */
extern void udb_delquedes();	/* Delete a pipe-queue to destination */
				/* mapping from the NQS database */
extern void udb_destination();	/* Update pipe-queue destination in */
				/* the NQS database */


/*** upd_addquedes
 *
 *
 *	long upd_addquedes():
 *	Add a queue destination for the specified local pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_ALREADEXI:	if the destination is already in the
 *				destination set for the specified queue.
 *		TCML_INSUFFMEM:	if insufficient memory to create queue
 *				destination descriptor.
 *		TCML_MAXQDESTS: if the maximum allowable number of
 *				destinations has already been reached
 *				for this pipe queue.
 *		TCML_NOSUCHQUE:	if the named local queue did not exist.
 *		TCML_SELREFDES: if the new destination to be added was
 *				self-referential (prohibited).
 *		TCML_WROQUETYP:	if the queue is not a pipe queue.
 */
long upd_addquedes (lquename, rquename, rhost_mid)
char *lquename;			/* Local queue name */
char *rquename;			/* Remote queue name */
mid_t rhost_mid;		/* Machine-id of destination machine */
{
	struct pipeto *upd_credes();		/* Create a destination */

	register struct queue *queue;
	register struct qdestmap *mapdest;
	register struct qdestmap *prevmapdest;
	int res;				/* Result code */
	int dest_count;				/* #of destinations */
						/* for this queue */

	queue = nqs_fndnnq (lquename);
	if (queue == (struct queue *) 0) {
		return (TCML_NOSUCHQUE);	/* No such local queue */
	}
	if (queue->q.type != QUE_PIPE) return (TCML_WROQUETYP);
	/*
	 *  Make sure that the destination is not self-referential.
	 */
	if (strcmp (rquename, queue->q.namev.name) == 0 &&
	    rhost_mid == Locmid) {
		/*
		 *  The destination is self-referential!
		 */
		return (TCML_SELREFDES);
	}
	/*
	 *  Search the existing destination set to see if the destination
	 *  is already present for the specified queue.
	 */
	dest_count = 0;
	prevmapdest = (struct qdestmap *) 0;
	mapdest = queue->v1.pipe.destset;
	while (mapdest != (struct qdestmap *) 0) {
		if (mapdest->pipeto->rhost_mid < rhost_mid) {
			/*
			 *  Keep searching by machine-id.
			 */
			prevmapdest = mapdest;
			mapdest = mapdest->next;
			dest_count += 1;	/* One more destination */
		}
		else if (mapdest->pipeto->rhost_mid > rhost_mid) {
			/*
			 *  The destination to add does not exist.
			 *  Tally the number of remaining destinations
			 *  and exit the outer loop.
			 *
			 *  Prevmapdest already points at the proper
			 *  place....
			 */
			do {
				mapdest = mapdest->next;
				dest_count += 1;	/* One more */
			} while (mapdest != (struct qdestmap *) 0);
		}
		else {
			/*
			 *  Queue(s) going to the machine as specified in
			 *  the new destination already exist.  Now search
			 *  by queue name.
			 */
			do {
				/*
				 *  We are still dealing with queues
				 *  going to the same machine.
				 */
				res = strcmp (mapdest->pipeto->rqueue,
					      rquename);
				if (res < 0) {
					/*
					 *  Keep searching.
					 */
					prevmapdest = mapdest;
					mapdest = mapdest->next;
					dest_count += 1;/* One more dest */
				}
				else if (res == 0) {
					/*
					 *  The queue destination is
					 *  already present in the
					 *  destination set for the
					 *  specified queue.
					 */
					return (TCML_ALREADEXI);
				}
				else {
					/*
					 *  The destination to add does
					 *  not exist.  Tally the number
					 *  of remaining destinations
					 *  and exit the loop.
					 */
					do {
						mapdest = mapdest->next;
						dest_count += 1;/* One more */
					} while (mapdest!=(struct qdestmap*)0);
				}
			} while (mapdest != (struct qdestmap *) 0 &&
				 mapdest->pipeto->rhost_mid == rhost_mid);
		}
	}
	/*
	 *  The destination is not present in the destination set for the
	 *  specified queue, and when added, the destination should be added
	 *  with "prevmapdest" as its predecessor in the set (unless
	 *  "prevmapdest" is null, in which case the new destination must be
	 *  added at the very beginning of the set.
	 */
	if (dest_count >= MAX_QDESTS) {
		/*
		 *  Too many queue destinations would exist, if the
		 *  new destination were added.
		 */
		return (TCML_MAXQDESTS);
	}
	mapdest = (struct qdestmap *) malloc (sizeof (struct qdestmap));
	if (mapdest == (struct qdestmap *) 0) {
		return (TCML_INSUFFMEM);	/* Insufficient memory */
	}
	mapdest->pipeto = upd_credes (rquename, rhost_mid);
	if (mapdest->pipeto == (struct pipeto *) 0) {
		free ((char *) mapdest);	/* Free mapping structure */
		return (TCML_INSUFFMEM);	/* Insufficient memory */
	}
	mapdest->pipeto->refcount++;		/* Increment reference count */
	/*
	 *  Add the destination to the destination set for the specified
	 *  queue in lexicographic order.
	 */
	if (prevmapdest == (struct qdestmap *) 0) {
		mapdest->next = queue->v1.pipe.destset;	/* Add to set head */
		queue->v1.pipe.destset = mapdest;	/* Update head ptr */
	}
	else {					/* Add to set */
		mapdest->next = prevmapdest->next;
		prevmapdest->next = mapdest;
	}
	if (Booted) {
		/*
		 *  NQS is completely "booted", and so this queue/destination
		 *  mapping needs to be explicitly added to the NQS database.
		 *  Furthermore, it may be necessary to spawn a pipe queue
		 *  request....
		 */
		udb_addquedes (queue, mapdest);	/* Update the database image */
		if ((queue->q.status & QUE_RUNNING) && queue->q.queuedcount &&
		    (mapdest->pipeto->status & DEST_ENABLED)) {
			psc_spawn();		/* Maybe spawn a pipe req */
		}
	}
	else New_qdestmap = mapdest;		/* Set ptr to new dest map */
	return (TCML_COMPLETE);			/* for the benefit of */
}						/* ../src/nqs_ldconf.c */


/*** upd_credes
 *
 *
 *	struct pipeto *upd_credes():
 *
 *	Create a global pipe queue destination, if it is not already
 *	present in the global pipe queue destination set.
 *
 *	Returns:
 *		A pointer to the pipeto structure allocated for the
 *		destination, if successful.  Otherwise, NIL is returned.
 */
struct pipeto *upd_credes (rquename, rhost_mid)
register char *rquename;		/* Name of remote queue */
register mid_t rhost_mid;		/* Machine-id of destination */
{
	register struct pipeto *dest;		/* Destination descr walking */
	register struct pipeto *prevdest;	/* Previous dest descr */
	register short res;			/* Result of strcmp() */

	prevdest = (struct pipeto *) 0;
	dest = Pipetoset;			/* Global destination set */
	while (dest != (struct pipeto *) 0) {
		if (dest->rhost_mid < rhost_mid) {
			/*
			 *  Keep searching by machine-id.
			 */
			prevdest = dest;
			dest = dest->next;
		}
		else if (dest->rhost_mid > rhost_mid) {
			/*
			 *  The destination to add does not exist.
			 *  Prevdest points at the proper place....
			 */
			dest = (struct pipeto *) 0;	/* Exit loop */
		}
		else {
			/*
			 *  There are one or more pipe queue destinations
			 *  already in existence for the machine in question.
			 *  Now search by queue name.
			 */
			do {
				/*
				 *  We are still dealing with queues
				 *  going to the same machine.
				 */
				res = strcmp (dest->rqueue, rquename);
				if (res < 0) {
					/*
					 *  Keep searching.
					 */
					prevdest = dest;
					dest = dest->next;
				}
				else if (res == 0) {
					/*
					 *  The pipe queue destination already
					 *  exists.
					 */
					return (dest);
				}
				else {
					/*
					 *  The destination to add does
					 *  not exist.  Prevdest points
					 *  at the proper place.
					 */
					dest = (struct pipeto *) 0;
				}
			} while (dest != (struct pipeto *) 0 &&
				 dest->rhost_mid == rhost_mid);
		}
	}
	/*
	 *  The destination is not present in the global destination set, and
	 *  when added, the destination should be added with "prevdest" as
	 *  its predecessor in the set (unless "prevdest" is null, in which
	 *  case the new destination must be added at the very beginning of
	 *  the set).
	 */
	dest = (struct pipeto *) malloc (sizeof (struct pipeto));
	if (dest == (struct pipeto *) 0) {
		/*
		 *  There is not sufficient memory to create the
		 *  global destination descriptor.
		 */
		return ((struct pipeto *) 0);	/* Return NIL pointer */
	}
	dest->refcount = 0;		/* No references yet made */
	dest->status = DEST_ENABLED;	/* New destinations are enabled */
	dest->retry_at = 0;		/* Not in retry mode */
	dest->retrytime = 0;		/* Not in retry mode */
	dest->retrydelta = 0;		/* No retry delta defined */
	strcpy (dest->rqueue, rquename);/* Save name of destination queue */
	dest->rhost_mid = rhost_mid;	/* Remember destination machine */
	/*
	 *  Now, add the destination to the global destination set.
	 */
	if (prevdest == (struct pipeto *) 0) {
		dest->next = Pipetoset;	/* Add to set head */
		Pipetoset = dest;	/* Update head ptr */
	}
	else {				/* Add to set */
		dest->next = prevdest->next;
		prevdest->next = dest;
	}
	if (Booted) {
		/*
		 *  NQS is completely "booted", and so this destination
		 *  descriptor needs to be explicitly added to the NQS
		 *  database.
		 */
		udb_credes (dest);	/* Update the database image */
	}
	return (dest);			/* Return ptr to destination */
}


/*** upd_deldes
 *
 *
 *	void upd_deldes():
 *
 *	Delete a pipe queue destination from the global pipe queue
 *	destination set.
 */
void upd_deldes (pipeto)
register struct pipeto *pipeto;		/* Destination to delete */
{
	register struct pipeto *dest;
	register struct pipeto *prevdest;

	/*
	 *  Locate the pipe queue destination.
	 */
	prevdest = (struct pipeto *) 0;
	dest = Pipetoset;
	while (dest != (struct pipeto *) 0 && dest != pipeto) {
		prevdest = dest;	/* Keep searching */
		dest = dest->next;	/* destination set */
	}
	if (dest != (struct pipeto *) 0) {
		/*
		 *  Delete the queue destination.
		 */
		if (dest->refcount) {
			printf ("F$NQS internal error: ");
			printf ("upd_deldes() ref != 0.\n");
			errno = 0;	/* Not a system call error */
			nqs_abort();	/* Abort execution */
		}
		udb_deldes (dest);	/* Update the NQS database image */
		if (prevdest == (struct pipeto *) 0) {
			Pipetoset = dest->next;
		}
		else prevdest->next = dest->next;
		free ((char *) dest);
	}
	else {
		printf ("F$NQS internal error: upd_deldes() no exist.\n");
		errno = 0;		/* Not a system call error */
		nqs_abort();		/* Abort execution */
	}
}


/*** upd_delquedes
 *
 *
 *	long upd_delquedes():
 *	Delete a pipe queue destination.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if no such queue exists.
 *		TCML_NOSUCHDES:	if no such destination exists for queue.
 *		TCML_WROQUETYP:	if the queue is not a pipe queue.
 */
long upd_delquedes (lquename, rquename, rhost_mid)
char *lquename;			/* Local queue name */
char *rquename;			/* Remote queue name */
mid_t rhost_mid;		/* Machine-id for destination machine */
{
	register struct queue *queue;
	register struct qdestmap *mapdest;
	register struct qdestmap *prevmapdest;
	register short found;
	register short res;

	queue = nqs_fndnnq (lquename);
	if (queue == (struct queue *)0) return (TCML_NOSUCHQUE);
	if (queue->q.type != QUE_PIPE) return (TCML_WROQUETYP);
	/*
	 *  Locate the queue destination.
	 */
	prevmapdest = (struct qdestmap *) 0;
	mapdest = queue->v1.pipe.destset;
	found = 0;
	while (mapdest != (struct qdestmap *) 0 && !found) {
		if (mapdest->pipeto->rhost_mid > rhost_mid) {
			mapdest = (struct qdestmap *) 0;
		}
		else if (mapdest->pipeto->rhost_mid == rhost_mid) {
			/*
			 *  We have the correct machine-destination.
			 *  Now, try to find the queue.
			 */
			res = strcmp (mapdest->pipeto->rqueue, rquename);
			if (res < 0) {
				prevmapdest = mapdest;	/* Keep searching */
				mapdest = mapdest->next;/* destination set */
			}
			else if (res == 0) found = 1;	/* Found! */
			else {				/* No such dest */
				mapdest = (struct qdestmap *) 0;
			}
		}
		else {
			prevmapdest = mapdest;
			mapdest = mapdest->next;
		}
	}
	if (!found) return (TCML_NOSUCHDES);
	/*
	 *  Delete the queue/destination mapping.
	 */
	udb_delquedes (mapdest);	/* Update the NQS database image */
	if (--mapdest->pipeto->refcount == 0) {
		/*
		 *  No more references exist to this pipe queue destination.
		 *  Delete it.
		 */
		upd_deldes (mapdest->pipeto);
	}
	if (prevmapdest == (struct qdestmap *) 0) {
		queue->v1.pipe.destset = mapdest->next;
	}
	else prevmapdest->next = mapdest->next;
	free ((char *) mapdest);
	return (TCML_COMPLETE);
}


/*** upd_destination
 *
 *
 *	long upd_destination():
 *	Update the state of a pipe queue destination.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHDES:	if no such pipe queue destination.
 */
long upd_destination (rquename, rhost_mid, cmd, retry_wait)
register char *rquename;		/* Name of remote queue */
register mid_t rhost_mid;		/* Machine-id of destination */
int cmd;				/* Command: 0=enable; 1=fail; */
					/* 2=schedule for retry */
long retry_wait;			/* Schedule retry time */
{
	register struct pipeto *dest;	/* Pipe queue destination */
	register short found;		/* Found flag */
	register short res;		/* strcmp() result */
	register time_t timenow;	/* Current time */

	/*
	 *  Locate the pipe queue destination.
	 */
	dest = Pipetoset;
	found = 0;
	while (dest != (struct pipeto *) 0 && !found) {
		if (dest->rhost_mid > rhost_mid) dest = (struct pipeto *) 0;
		else if (dest->rhost_mid == rhost_mid) {
			/*
			 *  We have the correct machine-destination.
			 *  Now, try to find the queue.
			 */
			res = strcmp (dest->rqueue, rquename);
			if (res < 0) dest = dest->next;	/* Next destination */
			else if (res == 0) found = 1;	/* Found! */
			else dest = (struct pipeto *) 0;/* No such dest */
		}
		else dest = dest->next;	/* Next destination */
	}
	if (found) {
		/*
		 *  Update the queue destination state.
		 */
		if (cmd == 0) dest->status = DEST_ENABLED;
		else if (cmd == 1) dest->status = DEST_FAILED;
		else {			/* Schedule destination for retry */
			timenow = time ((time_t *) 0);
			if (!(dest->status & DEST_RETRY)) {
				/*
				 *  The destination was not previously in
				 *  retry mode.  Record the current time as
				 *  the time at which this destination entered
				 *  the retry state.
				 */
				dest->retrytime = timenow;
			}
			dest->status = DEST_RETRY; /* In retry wait state */
			dest->retry_at = timenow + retry_wait;
			/*
			 *  Set timer to re-enable the destination.
			 */
			nqs_vtimer (&dest->retry_at, nqs_wakdes);
		}
		udb_destination (dest);	/* Update the NQS database image */
		return (TCML_COMPLETE);	/* Update complete */
	}
	return (TCML_NOSUCHDES);	/* No such destination */
}


/*** upd_machine
 *
 *
 *	long upd_machine():
 *	Update the state of a machine destination.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHMAC:	if no such machine is mentioned
 *				in any pipe queue destination.
 */
long upd_machine (rhost_mid, cmd, retry_wait)
register mid_t rhost_mid;		/* Machine-id of destination */
int cmd;				/* Command: 0=enable; 1=fail; */
					/* 2=schedule for retry */
long retry_wait;			/* Schedule retry time */
{
	register struct pipeto *dest;	/* Pipe queue destination */
	register short found;		/* Boolean */
	register time_t timenow;	/* Current time */

	/*
	 *  Locate and operate upon all pipe queue destinations
	 *  at the stated machine.
	 */
	found = 0;
	dest = Pipetoset;
	while (dest != (struct pipeto *) 0) {
		if (dest->rhost_mid == rhost_mid) {
			/*
			 *  Perform update on this destination.
			 */
			found = 1;
			if (cmd == 0) dest->status = DEST_ENABLED;
			else if (cmd == 1) dest->status = DEST_FAILED;
			else {			/* Schedule destination */
				timenow = time ((time_t *) 0);
				if (!(dest->status & DEST_RETRY)) {
					/*
					 *  The destination was not previously
					 *  in retry mode.  Record the current
					 *  time as the time at which this
					 *  destination entered the retry
					 *  state.
					 */
					dest->retrytime = timenow;
				}
				dest->status = DEST_RETRY;
				dest->retry_at = timenow + retry_wait;
				/*
				 *  Set timer to re-enable the destination.
				 */
				nqs_vtimer (&dest->retry_at, nqs_wakdes);
			}
			udb_destination (dest);	/* Update NQS database image */
		}
		dest = dest->next;		/* Next destination */
	}
	if (found) return (TCML_COMPLETE);	/* Update complete */
	return (TCML_NOSUCHMAC);		/* No such destination */
}


/*** upd_setquedes
 *
 *
 *	long upd_setquedes():
 *	Set the queue destination set for the specified local pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_INSUFFMEM:	if insufficient memory to create queue
 *				destination descriptor.
 *		TCML_NOSUCHQUE:	if the named local queue did not exist.
 *		TCML_SELREFDES: if the the destination was self-
 *				referential (prohibited).
 *		TCML_WROQUETYP:	if the queue is not a pipe queue.
 */
long upd_setquedes (lquename, rquename, rhost_mid)
char *lquename;			/* Local queue name */
char *rquename;			/* Remote queue name */
mid_t rhost_mid;		/* Machine-id for destination machine */
{
	register struct queue *queue;
	register struct qdestmap *mapdest;
	register struct qdestmap *walkmapdest;
	register struct qdestmap *nextmapdest;
	register short found;
	register short res;

	queue = nqs_fndnnq (lquename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such local queue */
	}
	if (queue->q.type != QUE_PIPE) return (TCML_WROQUETYP);
	/*
	 *  Make sure that the destination is not self-referential.
	 */
	if (strcmp (rquename, queue->q.namev.name) == 0 &&
	    rhost_mid == Locmid) {
		/*
		 *  The destination is self-referential!
		 */
		return (TCML_SELREFDES);
	}
	/*
	 *  Determine if the new single destination for the queue already
	 *  exists in the destination set.
	 */
	mapdest = queue->v1.pipe.destset;
	found = 0;
	while (mapdest != (struct qdestmap *) 0 && !found) {
		if (mapdest->pipeto->rhost_mid > rhost_mid) {
			mapdest = (struct qdestmap *) 0;
		}
		else if (mapdest->pipeto->rhost_mid == rhost_mid) {
			/*
			 *  We have the correct machine-destination.
			 *  Now, try to find the queue.
			 */
			res = strcmp (mapdest->pipeto->rqueue, rquename);
			if (res < 0) {		/* Keep searching */
				mapdest = mapdest->next;
			}
			else if (res == 0) found = 1;	/* Found! */
			else {				/* No such dest */
				mapdest = (struct qdestmap *) 0;
			}
		}
		else mapdest = mapdest->next;
	}
	if (!found) {
		/*
		 *  The specified queue destination is not already present
		 *  within the destination set for the pipe queue.
		 *  Add the required destination.
		 */
		mapdest = (struct qdestmap *) malloc (sizeof (struct qdestmap));
		if (mapdest == (struct qdestmap *) 0) {
			return (TCML_INSUFFMEM);/* Insufficient memory */
		}
		mapdest->pipeto = upd_credes (rquename, rhost_mid);
		if (mapdest->pipeto == (struct pipeto *) 0) {
			free ((char *) mapdest);/* Free mapping structure */
			return (TCML_INSUFFMEM);/* Insufficient memory */
		}
		mapdest->pipeto->refcount++;	/* Increment reference count */
		udb_addquedes (queue, mapdest);	/* Update the database image */
		/*
		 *  Now, complete the process of adding the destination to
		 *  the destination set for this pipe queue (the new
		 *  destination is not added in lexicographic order, but
		 *  this does not matter, since we are are going to delete
		 *  all other destinations anyway....).
		 */
		mapdest->next = queue->v1.pipe.destset;	/* Add to set */
		queue->v1.pipe.destset = mapdest;	/* Update head ptr */
	}
	/*
	 *  Now, walk the destination set to delete ALL destinations with
	 *  the exception of the chosen single destination.
	 */
	walkmapdest = queue->v1.pipe.destset;
	while (walkmapdest != (struct qdestmap *) 0) {
		nextmapdest = walkmapdest->next;
		if (walkmapdest != mapdest) {
			/*
			 *  Delete the queue/destination mapping.
			 */
			udb_delquedes (walkmapdest);	/* Update database */
			if (--walkmapdest->pipeto->refcount == 0) {
				/*
				 *  No more references exist to this pipe queue
				 *  destination.  Delete it.
				 */
				upd_deldes (walkmapdest->pipeto);
			}
			free ((char *) walkmapdest);
		}
		walkmapdest = nextmapdest;	/* Keep walking the set */
	}					/* until done */
	queue->v1.pipe.destset = mapdest;
	mapdest->next = (struct qdestmap *) 0;
	if ((queue->q.status & QUE_RUNNING) && queue->q.queuedcount &&
	    (mapdest->pipeto->status & DEST_ENABLED)) {
		psc_spawn();			/* Maybe spawn a pipe req */
	}
	return (TCML_COMPLETE);			/* Return completion code */
}
