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
 
/*++ nqs_upv.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upv.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS device state update module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	June 16, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:31 $ $State: Exp $)
 * $Log: nqs_upv.c,v $
 * Revision 1.3  1994/11/19  02:53:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:55  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:53  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:28  hender
 * Sterling version 4/22/87
 * 
 *
 *	
 *	Intergraph update B. Mar: 			12/11/89 TAC
 *		Try to spawn device in upv_setdevfor() 
 *		- as you are changing a form on a device
 *
 */

#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */

extern void bytezero();		/* Zero fill */
extern void dsc_spawn();	/* Maybe spawn some device reqs */
extern void free();		/* Deallocate memory */
extern char *malloc();		/* Allocate memory */
extern struct gendescr *nextdb();
				/* Return next allocated entry */
				/* from a database file */
extern struct queue *nqs_fndnnq();
				/* Find non-network queue by name */
extern void seekdbb();		/* Buffered seek on a database file */
extern int strcmp();		/* String comparison */
extern char *strcpy();		/* String copy */
extern void udb_addquedev();	/* Add a queue/device mapping */
				/* to the NQS database image */
extern void udb_credev();	/* Add a new device definition to the */
				/* NQS database image */
extern void udb_deldev();	/* Delete a device descriptor from the NQS */
				/* database */
extern void udb_delquedev();	/* Delete a queue/device mapping */
				/* for the specified device from */
				/* the NQS database */
extern void udb_device();	/* Update the state for a device as */
				/* stored in the NQS database */
extern int upf_valfor();	/* Boolean function: valid form? */


/*** upv_addquedev
 *
 *
 *	long upv_addquedev():
 *
 *	Add a queue to device mapping for the specified local device
 *	queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_ALREADEXI:	if the mapping is already present.
 *		TCML_INSUFFMEM:	if insufficient memory to create the
 *				queue/device mapping descriptor.
 *		TCML_NOSUCHDEV:	if the named local device did not exist.
 *		TCML_NOSUCHQUE:	if the named local queue did not exist.
 *		TCML_WROQUETYP:	if the named queue is not a device queue.
 */
long upv_addquedev (quename, devname)
char *quename;			/* Local queue name */
char *devname;			/* Local device name */
{
	struct device *finddev();	/* Return device structure for device */

	register struct queue *queue;
	register struct device *device;
	register struct qdevmap *map;
	register struct qdevmap *prevmap;
	register int res;			/* Result code */

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such local queue */
	}
	device = finddev (devname);
	if (device == (struct device *)0) {
		return (TCML_NOSUCHDEV); 	/* No such device */
	}
	if (queue->q.type != QUE_DEVICE) return (TCML_WROQUETYP);
	/*
	 *  Determine if the mapping is already present (in the lexicographic-
	 *  ally ordered queue-to-device mapping set.  If not, then remember
	 *  where to insert it into the mapping set so that the mapping set
	 *  remains in lexicographic order.
	 */
	prevmap = (struct qdevmap *) 0;
	map = queue->v1.device.qdmaps;
	while (map != (struct qdevmap *)0) {
		res = strcmp (map->device->name, devname);
		if (res < 0) {
			/*
			 *  Keep searching.
			 */
			prevmap = map;
			map = map->nextqd;	/* Get next queue/device map */
		}
		else if (res == 0) {
			/*
			 *  The mapping is already present.
			 */
			return (TCML_ALREADEXI);
		}
		else map = (struct qdevmap *)0;	/* Exit loop */
	}
	/*
	 *  The specified queue/device mapping does not exist, and when
	 *  the mapping is created, it should be added with "prevmap" as
	 *  its predecessor in the mapping set (unless "prevmap" is null,
	 *  in which case the mapping must be added at the very beginning
	 *  of the set).
	 */
	if ((map = (struct qdevmap *)
		    malloc (sizeof(struct qdevmap))) == (struct qdevmap *) 0) {
		return (TCML_INSUFFMEM);	/* Insufficient memory */
	}
	map->device = device;			/* Ptr to device */
	map->queue = queue;			/* Ptr to queue */
	/*
	 *  Add the mapping in the lexicographically ordered queue-to-device
	 *  mapping set.
	 */
	if (prevmap == (struct qdevmap *) 0) {
		map->nextqd = queue->v1.device.qdmaps;
						/* Add to mapping set head */
		queue->v1.device.qdmaps = map;	/* Update set header */
	}
	else {
		map->nextqd = prevmap->nextqd;	/* Add to mapping set */
		prevmap->nextqd = map;		/* Update next ptr */
	}
	/*
	 *  Add the inverse mapping in the UNORDERED device-to-queue
	 *  mapping set.
	 */
	map->nextdq = device->dqmaps;		/* Add to device/queue */
	device->dqmaps = map;			/* mapping set for device */
	if (Booted) {
		/*
		 *  NQS is completely "booted", and so this queue/device
		 *  mapping needs to be explicitly added to the NQS database.
		 *  Furthermore, it may be necessary to spawn a device queue
		 *  request....
		 */
		udb_addquedev (map);	/* Update the NQS database image */
					/* appropriately */
		if ((map->queue->q.status & QUE_RUNNING) &&
		     map->queue->q.queuedcount &&
		    (map->device->status & DEV_ENABLED) &&
		    (map->device->status & DEV_ACTIVE) == 0) {
			/*
			 *  The device is enabled, and is not active.
			 *  The queue is running, but other req(s) could run.
			 */
			dsc_spawn();	/* Maybe spawn a device req */
		}
	}
	else New_qdevmap = map;		/* Otherwise, update most recently */
					/* added map for the benefit of */
					/* nqs_ldconf.c */
	return (TCML_COMPLETE);
}


/*** upv_credev
 *
 *
 *	long upv_credev():
 *	Create an NQS device.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful.
 *		TCML_ALREADEXI:	 if the device already exists.
 *		TCML_INSUFFMEM:	 if insufficient memory to create device
 *				 descriptor.
 *		TCML_NOSUCHFORM: if the forms associated with the device
 *				 do not exist.
 *		TCML_TOOMANDEV:  if the creation of this device would
 *				 result in exceeding the maximum number
 *				 of devices allowed for the local host
 *				 (see MAX_NOOFDEVICES in ../h/nqs.h).
 */
long upv_credev (devname, formsname, fullname, servername)
char *devname;			/* Shorthand name for device */
char *formsname;		/* Forms in device */
char *fullname;			/* Fully qualified pathname of device */
char *servername;		/* Server and args for device */
{
	register struct device *device;
	register struct device *prevdevice;
	register int res;			/* Result code */

	if (formsname [0] != '\0') {		/* null name is valid */
		if (!upf_valfor (formsname)) {
			return(TCML_NOSUCHFORM);/* No such form */
		}
	}
	/*
	 *  Search the lexicographically ordered device set to see if the
	 *  device already exists.
	 */
	prevdevice = (struct device *)0;
	device = Devset;
	while (device != (struct device *)0) {
		res = strcmp (device->name, devname);
		if (res < 0) {
			/*
			 *  The device has not yet been found, keep
			 *  searching.
			 */
			prevdevice = device;
			device = device->next;
		}
		else if (res == 0) {
			/*
			 *  The device already exists.
			 */
			return (TCML_ALREADEXI);
		}
		else device = (struct device *)0;	/* Exit loop */
	}
	/*
	 *  Would the creation of this device result in too many devices,
	 *  exceeding the Runvars[] space allocated to device requests?
	 */
	if (Noofdevices >= MAX_NOOFDEVICES) return (TCML_TOOMANDEV);
	/*
	 *  The device does not already exist, and when the newly created
	 *  device is added to the device set, it should be added with
	 *  "prevdevice" as its predecessor (unless "prevdevice" is null,
	 *  in which case the device is added at the very beginning of the
	 *  device set.
	 */
	if ((device = (struct device *)
		       malloc (sizeof (struct device))) == (struct device *)0) {
		return (TCML_INSUFFMEM);	/* Insufficient memory */
	}
	bytezero ((char *) device, sizeof (struct device));
	device->status = 0;			/* Inactive, disabled, but */
						/* not failed */
	device->statmsg [0] = '\0';		/* No status message */
	strcpy (device->name, devname);		/* Save device name */
	if ((device->fullname = malloc (strlen (fullname) + 1)) == (char *)0) {
		free ((char *) device);		/* Free partial descriptor */
		return (TCML_INSUFFMEM);	/* Insufficient memory */
	}
	Noofdevices++;				/* One more device */
	strcpy (device->fullname, fullname);	/* Save full path name */
	strcpy (device->forms, formsname);	/* Save forms name */
	strcpy (device->server, servername);	/* Save server name */
	device->curreq = (struct request *)0;	/* No current request */
	device->curque = (struct queue *)0;	/* No current queue */
	device->dqmaps = (struct qdevmap *) 0;	/* No device/queue mappings */
	/*
	 *  The new device structure is now completely initialized; insert
	 *  it into the lexicographically ordered device set.
	 */
	if (prevdevice == (struct device *)0) {
		/*
		 *  Insert at the beginning of the list.
		 */
		device->next = Devset;
		Devset = device;
	}
	else {
		device->next = prevdevice->next;
		prevdevice->next = device;
	}
	if (Booted) {
		/*
		 *  NQS is completely "booted", and so this new device needs
		 *  to be added to the NQS database image.
		 */
		udb_credev (device);	/* Add the device descriptor to the */
					/* NQS database image. */
	}
	else New_device = device;	/* Otherwise, update most recently */
					/* added device for the benefit of */
					/* nqs_ldconf.c */
	return (TCML_COMPLETE);
}


/*** upv_deldev
 *
 *
 *	long upv_deldev():
 *	Delete an NQS device.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_DEVACTIVE:	if the device is active.
 *		TCML_DEVENABLE:	if the device is enabled; a device cannot
 *				be deleted unless it is disabled.
 *		TCML_NOSUCHDEV:	if no such device exists.
 */
long upv_deldev (devname)
char *devname;				/* Name of device */
{
	struct device *finddev();	/* Return device structure for device */

	register struct qdevmap *map;
	register struct qdevmap *prevmap;
	register struct qdevmap *walkmap;
	register struct device *prevdevice;
	register struct device *device;

	device = finddev (devname);
	if (device == (struct device *)0) return (TCML_NOSUCHDEV);
	if (device->status & DEV_ENABLED) return (TCML_DEVENABLE);
	if (device->status & DEV_ACTIVE) return (TCML_DEVACTIVE);
	/*
	 *  Delete the device/queue mapping set for the device.
	 */
	map = device->dqmaps;
	while (map != (struct qdevmap *)0) {
		/*
		 *  Remove this mapping from the device set
		 *  as threaded from the queue.
		 */
		prevmap = (struct qdevmap *)0;
		walkmap = map->queue->v1.device.qdmaps;
		while (walkmap != map) {
			prevmap = walkmap;
			walkmap = walkmap->nextqd;
		}
		if (prevmap == (struct qdevmap *)0) {
			map->queue->v1.device.qdmaps = map->nextqd;
		}
		else prevmap->nextqd = map->nextqd;
		/*
		 *  Delete the queue/device mapping.
		 */
		walkmap = map->nextdq;
		udb_delquedev (map);	/* Update database */
		free ((char *) map);	/* Update memory */
		map = walkmap;
	}
	udb_deldev (device);		/* Delete the device from */
					/* the NQS database image */
	free (device->fullname);	/* Delete device fullname */
	/*
	 *  Remove the device descriptor from the device set.
	 */
	if (Devset == device) Devset = device->next;
	else {
		prevdevice = Devset;
		while (prevdevice->next != device) {
			prevdevice = prevdevice->next;
		}
		prevdevice->next = device->next;
	}
	free ((char *) device);		/* Delete device structure */
	Noofdevices--;			/* One less device */
	return (TCML_COMPLETE);
}


/*** upv_delquedev
 *
 *
 *	long upv_delquedev():
 *	Delete a device queue to device mapping.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHQUE:	if no such queue exists.
 *		TCML_NOSUCHMAP:	if no such mapping exists for queue.
 *		TCML_WROQUETYP:	if the queue is not a device queue.
 */
long upv_delquedev (quename, devname)
char *quename;			/* Queue name */
char *devname;			/* Device name */
{
	register struct queue *queue;
	register struct qdevmap *map;
	register struct qdevmap *prevmap;
	register struct qdevmap *walkmap;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) return (TCML_NOSUCHQUE);
	if (queue->q.type != QUE_DEVICE) return (TCML_WROQUETYP);
	/*
	 *  Locate the device in the device set for the specified queue.
	 */
	prevmap = (struct qdevmap *) 0;
	map = queue->v1.device.qdmaps;
	while (map != (struct qdevmap *) 0 &&
	       strcmp (map->device->name, devname)) {
		prevmap = map;
		map = map->nextqd;
	}
	if (map == (struct qdevmap *) 0) return (TCML_NOSUCHMAP);
	/*
	 *  Delete the device from the device set for the specified queue.
	 */
	udb_delquedev (map);		/* Update the NQS database image */
	/*
	 *  Remove the map structure from the linked list as threaded
	 *  from the queue.
	 */
	if (prevmap == (struct qdevmap *) 0) {
		queue->v1.device.qdmaps = map->nextqd;
	}
	else prevmap->nextqd = map->nextqd;
	/*
	 *  Remove the map structure from the linked list as threaded
	 *  from the device.
	 */
	prevmap = (struct qdevmap *) 0;
	walkmap = map->device->dqmaps;
	while (walkmap != map) {
		prevmap = walkmap;
		walkmap = walkmap->nextdq;
	}
	if (prevmap==(struct qdevmap *) 0) map->device->dqmaps = map->nextdq;
	else prevmap->nextdq = map->nextdq;
	/*
	 *  Drop this mapping structure.
	 */
	free ((char *) map);
	return (TCML_COMPLETE);
}


/*** upv_disdev
 *
 *
 *	long upv_disdev():
 *	Disable an NQS device.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHDEV:	if no such device exists.
 */
long upv_disdev (devname)
char *devname;				/* Name of device */
{
	struct device *finddev();	/* Return device structure for device */

	register struct device *device;

	device = finddev (devname);	/* Get device by name */
	if (device == (struct device *)0) return (TCML_NOSUCHDEV);
	device->status &= ~DEV_ENABLED;	/* Mark device as disabled */
	udb_device (device);		/* Update the NQS database image */
	return (TCML_COMPLETE);
}


/*** upv_enadev
 *
 *
 *	long upv_enadev():
 *	Enable an NQS device.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_NOSUCHDEV:	if no such device exists.
 */
long upv_enadev (devname)
char *devname;				/* Name of device */
{
	struct device *finddev();	/* Return device structure for device */

	register struct device *device;

	device = finddev (devname);	/* Get device by name */
	if (device == (struct device *)0) return (TCML_NOSUCHDEV);
	if ((device->status & DEV_ACTIVE) == 0) {
		/*
		 *  The device is not active.
		 *  Enable it.
		 */
		device->status = DEV_ENABLED;
					/* Mark device as enabled */
					/* absolutely! */
		dsc_spawn();		/* Possibly spawn a device req */
	}
	else device->status |= DEV_ENABLED;
					/* Set the enabled bit, but */
					/* don't otherwise touch an */
					/* active device */
	udb_device (device);		/* Update NQS database image */
	return (TCML_COMPLETE);
}


/*** upv_setdevfor
 *
 *
 *	long upv_setdevfor():
 *	Set device forms.
 *	
 *	Intergraph update B. Mar: Try to spawn device  12/11/89 TAC
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHDEV:	 if the named device does not exist.
 *		TCML_NOSUCHFORM: if the specified form is not defined
 *				 in the local NQS forms set.
 */
long upv_setdevfor (devname, formsname)
char *devname;			/* Device name */
char *formsname;		/* New forms to place in device */
{
	struct device *finddev();	/* Return device structure for device */

	register struct device *device;

	device = finddev (devname);
	if (device == (struct device *)0) {
		return (TCML_NOSUCHDEV);	/* No such device */
	}
	if (upf_valfor (formsname)) {
		strcpy(device->forms,formsname);/* Save forms name */
		if ((device->status & DEV_ENABLED)  &&  /* Intergraph added */
		    (device->status & DEV_ACTIVE) == 0)
			dsc_spawn();
		udb_device (device);	/* Update NQS database image */
		return (TCML_COMPLETE);
	}
	return (TCML_NOSUCHFORM);		/* No such form */
}


/*** upv_setdevser
 *
 *
 *	long upv_setdevser():
 *	Set device server.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHDEV:	if the named device does not exist.
 */
long upv_setdevser (devname, servername)
char *devname;			/* Device name */
char *servername;		/* New server for device */
{
	struct device *finddev();	/* Return device structure for device */

	register struct device *device;

	device = finddev (devname);
	if (device == (struct device *)0) {
		return (TCML_NOSUCHDEV);	/* No such device */
	}
	strcpy (device->server, servername);	/* Copy new server */
	udb_device (device);			/* Update NQS database image */
	return (TCML_COMPLETE);
}


/*** upv_setquedev
 *
 *
 *	long upv_setquedev():
 *
 *	Set the queue to device mapping set for the specified local device
 *	queue to the single specified device.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 *		TCML_INSUFFMEM:	if insufficient memory to create
 *				queue/device mapping descriptor.
 *		TCML_NOSUCHDEV:	if the named local device did not exist.
 *		TCML_NOSUCHQUE:	if the named local queue did not exist.
 *		TCML_WROQUETYP:	if the named queue is not a device queue.
 */
long upv_setquedev (quename, devname)
char *quename;			/* Local queue name */
char *devname;			/* Local device name */
{
	struct device *finddev();	/* Return device structure for device */

	register struct queue *queue;
	register struct device *device;
	register struct qdevmap *map;
	register struct qdevmap *walkmap;
	register struct qdevmap *walkmap2;
	register struct qdevmap *prevmap;
	struct qdevmap *nextmap;

	queue = nqs_fndnnq (quename);
	if (queue == (struct queue *)0) {
		return (TCML_NOSUCHQUE);	/* No such local queue */
	}
	device = finddev (devname);
	if (device == (struct device *)0) {
		return (TCML_NOSUCHDEV); 	/* No such device */
	}
	if (queue->q.type != QUE_DEVICE) return (TCML_WROQUETYP);
	/*
	 *  Determine if the device is already in the device mapping set
	 *  for the specified queue.
	 */
	map = queue->v1.device.qdmaps;
	while (map != (struct qdevmap *) 0 &&
	       strcmp (map->device->name, devname) != 0) {
		map = map->nextqd;		/* Get next dev in set */
	}
	if (map == (struct qdevmap *) 0) {
		/*
		 *  The specified queue/device mapping does not exist.
		 *  Create it.
		 */
		map = (struct qdevmap *) malloc (sizeof (struct qdevmap));
		if (map == (struct qdevmap *) 0) {
			return (TCML_INSUFFMEM);/* Insufficient memory */
		}
		map->device = device;		/* Ptr to device */
		map->queue = queue;		/* Ptr to queue */
		/*
		 *  Now, complete the process of adding the device mapping to
		 *  the mapping set for this device queue.  The mapping is
		 *  not added added in lexicographic order, but this does
		 *  not matter, since was are going to delete all of the
		 *  queue to device mappings below....
		 */
		map->nextqd = queue->v1.device.qdmaps;
						/* Add to queue/device */
		queue->v1.device.qdmaps = map;	/* mapping set for queue */
		/*
		 *  Add the inverse mapping in the UNORDERED device-to-queue
		 *  mapping set.
		 */
		map->nextdq = device->dqmaps;	/* Add to device/queue */
		device->dqmaps = map;		/* mapping set for device */
		udb_addquedev (map);		/* Add the new mapping to */
						/* the NQS database image */
	}
	/*
	 *  Now, walk the device mapping set to delete ALL mappings with
	 *  the exception of the chosen single device mapping.
	 */
	walkmap = queue->v1.device.qdmaps;
	while (walkmap != (struct qdevmap *) 0) {
		nextmap = walkmap->nextqd;	/* Next queue/device mapping */
		if (walkmap != map) {
			/*
			 *  Remove the map structure from the linked list as
			 *  threaded from the device.
			 */
			prevmap = (struct qdevmap *) 0;
			walkmap2 = walkmap->device->dqmaps;
			while (walkmap2 != walkmap) {
				prevmap = walkmap2;
				walkmap2 = walkmap2->nextdq;
			}
			if (prevmap == (struct qdevmap *) 0) {
				walkmap->device->dqmaps = walkmap->nextdq;
			}
			else prevmap->nextdq = walkmap->nextdq;
			/*
			 *  Delete the queue/device mapping.
			 */
			udb_delquedev (walkmap);	/* Update database */
			free ((char *) walkmap);	/* Update memory */
		}
		walkmap = nextmap;
	}
	map->nextqd = (struct qdevmap *) 0;	/* Queue-to-device mapping */
	queue->v1.device.qdmaps = map;		/* set is a set of one entry */
	if ((map->queue->q.status & QUE_RUNNING) &&
	     map->queue->q.queuedcount &&
	    (map->device->status & DEV_ENABLED) &&
	    (map->device->status & DEV_ACTIVE) == 0) {
		/*
		 *  The device is enabled, and is not active.
		 *  The queue is running, but other req(s) could run.
		 */
		dsc_spawn();		/* Maybe spawn a device req */
	}
	return (TCML_COMPLETE);
}


/*** finddev
 *
 *
 *	struct device *finddev():
 *	Return pointer to device structure for the named device.
 *
 *	Returns:
 *		A pointer to the device structure for the named device,
 *		if the named device exists; otherwise (struct device *)0
 *		is returned.
 */
static struct device *finddev (devname)
register char *devname;			/* Name of device */
{
	register struct device *device;
	register int res;

	/*
	 *  We take advantage of the fact that the device set is
	 *  lexicographically ordered.
	 */
	device = Devset;
	while (device != (struct device *)0) {
		if ((res = strcmp (device->name, devname)) < 0) {
			/*
			 *  Keep searching.
			 */
			device = device->next;
		}
		else if (res == 0) return (device);	/* Found! */
		else return ((struct device *)0);	/* Not found */
	}
	return (device);				/* Not found */
}

