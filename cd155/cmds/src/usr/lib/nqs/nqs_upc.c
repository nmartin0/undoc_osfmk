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
 
/*++ nqs_upc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upc.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS queue complex update module.
 *
 *
 *	Author:
 *	-------
 *	Clayton D Andreasen, Cray Research, Incorporated.
 *	August 6, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:25 $ $State: Exp $)
 * $Log: nqs_upc.c,v $
 * Revision 1.3  1994/11/19  02:53:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:32  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:41  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:41  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <errno.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */

extern int  errno;		/* System call error number */
extern void bsc_spawn();	/* Maybe spawn some batch reqs */
extern void dsc_spawn();	/* Maybe spawn some device reqs */
extern void bytezero();		/* Zero fill */
extern void free();		/* Deallocate memory */
extern char *malloc();		/* Allocate memory */
extern struct queue *nqs_fndnnq();
				/* Find non-network queue by name */
extern int strcmp();		/* String comparison */
extern char *strcpy();		/* String copy */
extern void udb_crecom();	/* Update database for create queue complex */
extern void udb_delcom();	/* Update database for delete queue complex */
extern void udb_quecom();	/* Update database for misc queue complex */


 /*** upc_addcomque
  *
  *
  *    long upc_addcomque():
  *
  *    Add a queue to a complex.
  *
  *    Returns:
  *            TCML_COMPLETE:  if successful.
  *            TCML_ALREADEXI: if the mapping is already present.
  *            TCML_INSUFFMEM: if insufficient memory to create the
  *                            qcomplex descriptor.
  *            TCML_NOSUCHCOM: if the named local queue complex did not exist.
  *            TCML_NOSUCHQUE: if the named local queue did not exist.
  *            TCML_TOOMANQUE: if the qcomplex cannot accept another queue.
  *            TCML_TOOMANCOM: if the queue cannot accept another complex.
  *            TCML_WROQUETYP: if the named queue is not a device queue.
  */
 long upc_addquecom (que_name,complx_name)
 char *que_name;                        /* Local queue name */
 char *complx_name;                        /* Local queue complex name */
 {

        struct queue *queue;            /* Pointer to que structure */
        struct qcomplex *qcomplex;      /* Pointer to que complex structure */
        struct qcomplex **qcp;          /* Pointer to que complex pointer */
        register int i;
 
        /*
         *  Find the queue to be added.
         */
        queue = nqs_fndnnq (que_name);
        if (queue == (struct queue *)0) {
                return(TCML_NOSUCHQUE); /* No such local queue */
        }

	/* Complexes do not exist for pipe queues */
        if (queue->q.type == QUE_PIPE) 
                return(TCML_WROQUETYP);

        /*
         *  Find the queue complex to add to.
         */
        qcomplex = Qcomplexset;
        while (qcomplex != (struct qcomplex *)0) {
                if (strcmp(complx_name, qcomplex->name) == 0) break;
                qcomplex = qcomplex->next;
        }
        if (qcomplex == (struct qcomplex *)0) {
                return(TCML_NOSUCHCOM); /* No such queue complex */
        }

        /*
         *  Locate where to add the queue complex to the queue.
         */
        qcp = (struct qcomplex **)0;
        if (queue->q.type == QUE_BATCH) { 
        for (i = MAX_COMPLXSPERQ; --i >= 0;) {
                if (queue->v1.batch.qcomplex[i] == (struct qcomplex *)0) {
                        qcp = &queue->v1.batch.qcomplex[i];
                        continue;
                }
                if (queue->v1.batch.qcomplex[i] == qcomplex) {
                        return(TCML_ALREADCOM); /* Already in queue complex */
                }
         }
        } else if (queue->q.type == QUE_DEVICE) { 
             for (i = MAX_COMPLXSPERQ; --i >= 0;) {
                if (queue->v1.device.qcomplex[i] == (struct qcomplex *)0) {
                        qcp = &queue->v1.device.qcomplex[i];
                        continue;
                }
                if (queue->v1.device.qcomplex[i] == qcomplex) {
                        return(TCML_ALREADCOM); /* Already in queue complex */
                }
           }
 
	}	/* end of DEVICE type */
        if (qcp == (struct qcomplex **)0) {
                return (TCML_TOOMANCOM); /* Too many complexes for the queue */
        }

        /*
         *  Locate where to add the queue to the queue complex.
         *  Then link everything together.
         */
        for (i = 0; i < MAX_QSPERCOMPLX; i++) {
                if (qcomplex->queue[i] == (struct queue *)0) {
                        qcomplex->queue[i] = queue;
                        *qcp = qcomplex;
                        if (Booted) {
                                udb_quecom(qcomplex);
                        }
                        return (TCML_COMPLETE); /* Successful completion */
                }
        }
        return(TCML_TOOMANQUE);         /* Too many queues in queue complex */
}







/*** upc_crecom
 *
 *
 *	long upc_crecom():
 *
 *	Create a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_ALREADEXI:	if the named local queue complex already exists.
 *		TCML_INSUFFMEM: if there was insufficient memory to allocate
 *				the queue complex structure.
 */
long upc_crecom (complx_name)
char *complx_name;			/* Name of queue complex */
{
	struct qcomplex *qcomplex;	/* Pointer to queue complex structure */
	struct qcomplex *prevqcom;	/* Pointer to previous queue complex */

	/*
	 *  Find the end of the queue complex set and check for the 
	 *  possibility of the queue complex already existing.
	 */
	prevqcom = (struct qcomplex *)0;
	qcomplex = Qcomplexset;
	while (qcomplex != (struct qcomplex *)0) {
		if (strcmp(complx_name, qcomplex->name) == 0) {
			return(TCML_ALREADEXI);
		}
		prevqcom = qcomplex;
		qcomplex = qcomplex->next;
	}

	/*
	 *  The queue complex does not exist already.  Allocate one and
	 *  link it into the set.
	 */
	qcomplex = (struct qcomplex *)malloc( sizeof(struct qcomplex) );
	if (qcomplex == (struct qcomplex *)0) {
		return (TCML_INSUFFMEM);	/* insufficient memory */
	}
	if (prevqcom == (struct qcomplex *)0) {
		Qcomplexset = qcomplex;
	} else {
		prevqcom->next = qcomplex;
	}

	/*
	 *  Initialize the queue complex and then add the queue to it.
	 */
	bytezero( (char *)qcomplex, sizeof(struct qcomplex) );
	qcomplex->runlimit = 1;
	strcpy( qcomplex->name, complx_name );
	if (Booted) {
		udb_crecom(qcomplex);
	}
	else New_qcomplex = qcomplex;	/* Otherwise, update most recently */
					/* added queue complex for the */
					/* benefit of nqs_ldconf.c CERN Boissat */
	return (TCML_COMPLETE);		/* Successful completion */
}



/*** upc_delcom
 *
 *
 *	long upc_delcom():
 *
 *	Delete a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHCOM:	if the named local queue complex does not exist.
 */
long upc_delcom (complx_name)
char *complx_name;			/* Name of queue complex */
{
	struct queue *queue;		/* Pointer to queue structure */
	struct qcomplex *qcomplex;	/* Pointer to queue complex structure */
	struct qcomplex *prevqcom;	/* Pointer to previous queue complex */
	register int i;

	/*
	 *  Find the queue complex to delete.
	 */
	prevqcom = (struct qcomplex *)0;
	qcomplex = Qcomplexset;
	while (qcomplex != (struct qcomplex *)0) {
		if (strcmp(complx_name, qcomplex->name) == 0) break;
		prevqcom = qcomplex;
		qcomplex = qcomplex->next;
	}
	if (qcomplex == (struct qcomplex *)0) {
		return(TCML_NOSUCHCOM);	/* No such queue complex */
	}
	if (prevqcom == (struct qcomplex *)0) {
		Qcomplexset = qcomplex->next; /* =(struct qcomplex *)0; */
	} else {
		prevqcom->next = qcomplex->next;
	}

	/*
	 *  Now remove all references to the queue complex.
	 */
	queue = Nonnet_queueset;
	while (queue != (struct queue *)0) {
		for (i = MAX_COMPLXSPERQ; --i >= 0;) {
			if (queue->v1.batch.qcomplex[i] == qcomplex) {
				queue->v1.batch.qcomplex[i] =
					(struct qcomplex *)0;
				break;
			}
		}
		queue = queue->next;
	}
	udb_delcom(qcomplex);
	free( (char *)qcomplex );	/* Release the space used by complex */
	bsc_spawn(); 	/* Maybe spawn some batch requests CERN Boissat */
	return (TCML_COMPLETE);		/* Successful completion */
}



/*** upc_remquecom
 *
 *
 *	long upc_remquecom():
 *
 *	Remove a batch queue from a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if the named local queue does not exist.
 *		TCML_NOSUCHCOM:	if the named local queue complex does not exist.
 *		TCML_WROQUETYP:	if the named local queue is not a batch queue.
 *		TCML_NOTMEMCOM:	if the named local queue is not a member of
 *				the queue complex.
 */
long upc_remquecom (que_name, complx_name)
char *que_name;				/* Name of queue to be added */
char *complx_name;			/* Name of queue complex */
{
	struct queue *queue;		/* Pointer to queue structure */
	struct qcomplex *qcomplex;	/* Pointer to queue complex structure */
	register int i;

	/*
	 *  Find the queue to be removed from the complex.
	 */
	queue = nqs_fndnnq (que_name);
	if (queue == (struct queue *)0) {
		return(TCML_NOSUCHQUE);		/* No such local queue */
	}
	if (queue->q.type != QUE_BATCH) {
		return(TCML_WROQUETYP);
	}

	/*
	 *  Find the queue complex to remove from.
	 */
	qcomplex = Qcomplexset;
	while (qcomplex != (struct qcomplex *)0) {
		if (strcmp(complx_name, qcomplex->name) == 0) break;
		qcomplex = qcomplex->next;
	}
	if (qcomplex == (struct qcomplex *)0) {
		return(TCML_NOSUCHCOM);	/* No such queue complex */
	}

	/*
	 *  Remove the pointer from the queue to the queue complex.
	 */
	for (i = 0; i < MAX_COMPLXSPERQ; i++) {
		if (queue->v1.batch.qcomplex[i] == qcomplex) {
			queue->v1.batch.qcomplex[i] = (struct qcomplex *)0;
			break;
		}
	}

	/*
	 *  Remove the pointer from the queue complex to the queue.
	 */
	for (i = 0; i < MAX_QSPERCOMPLX; i++) {
		if (qcomplex->queue[i] == queue) {
			qcomplex->queue[i] = (struct queue *)0;
			if (Booted) {
				udb_quecom(qcomplex);
			}
			return (TCML_COMPLETE);	/* Successful completion */
		}
	}
	return (TCML_NOTMEMCOM);	/* Not a member of the queue complex */
}



/*** upc_setcomlim
 *
 *
 *	long upc_setcomlim():
 *
 *	Set a runlimit for a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHCOM:	if the named local queue complex does not exist.
 */
long upc_setcomlim (complx_name, limit)
char *complx_name;			/* Name of queue complex */
long limit;
{
	struct qcomplex *qcomplex;	/* Pointer to queue complex structure */
	long prevlim;			/* Previous runlimit for q complex */

	/*
	 *  Find the queue complex to add to.
	 */
	qcomplex = Qcomplexset;
	while (qcomplex != (struct qcomplex *)0) {
		if (strcmp(complx_name, qcomplex->name) == 0) break;
		qcomplex = qcomplex->next;
	}
	if (qcomplex == (struct qcomplex *)0) {
		return(TCML_NOSUCHCOM);	/* No such queue complex */
	}

	/*
	 *  Set the new limit for the queue complex and call the spawning
	 *  routine to possibly spawn new batch requests if the limit has
	 *  increased.
	 */
	prevlim = qcomplex->runlimit;
	qcomplex->runlimit = limit;
	if (Booted) {
		udb_quecom(qcomplex);
	}
	if (limit > prevlim) {
		bsc_spawn();
	}
	return (TCML_COMPLETE);		/* Successful completion */
}

 /*** nqs_fndcomplex
  *
  *
  *    struct qcomplex *nqs_fndcomplex():
  *
  *    Return pointer to queue structure for the named qcomplex.
  *
  *    Returns:
  *            A pointer to the qcomplex structure for the named
  *            qcomplex, if the qcomplex exists;
  *            otherwise (struct qcomplex *)0 is returned.
  */
struct qcomplex *nqs_fndcomplex (comname)
 register char *comname;
{
       register struct qcomplex *qcomplex;
       register int res;
 
        /*
         *  We take advantage of the fact that qcomplex set
         *  is lexicographically ordered.
         */

       qcomplex = Qcomplexset;
       while (qcomplex != (struct qcomplex *)0) {
               if ((res = strcmp (qcomplex->name, comname)) < 0) {
                       /*
                        *  We have not yet found the qcomplex, but we should
                        *  keep on searching.
                        */
                       qcomplex = qcomplex->next;
               }
               else if (res == 0) return (qcomplex);   /* Found! */
               else return ((struct qcomplex *)0);     /* Not found */
        }
       return (qcomplex);                                      /* Not found */  
}
