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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: device_utils.c,v $
 * Revision 1.8  1994/11/18  20:47:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/18  00:19:33  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.6  1994/05/27  00:34:51  suri
 *  Reviewer: jlitvin
 *  Risk: lo
 *  Benefit or PTS #: 9545
 *  Testing: Tested by richardg
 *  Module(s): uxkern/device_utils.c (dev_error_to_errno())
 *  Solution: Server will now return ENOSPC as opposed to EOT when the
 * 	   physical end of tape is reached. This will enable devices
 * 	   such as 3480 model tape drive to function appropriately.
 *
 * Revision 1.5  1993/07/14  18:40:48  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:00:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:26:33  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/05/04  16:25:26  cfj
 * Modified dev_error_to_errno() to translate D_DEVICE_DOWN to EIO instead
 * of ENETDOWN.
 *
 * Revision 1.2  1992/11/30  22:53:45  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:30  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/14  18:14:39  roy
 * Revision 2.4  1993/01/08  14:37:14  durriya
 * 	add node to dev_entry struct. Add node # as arg to dev_lookup,
 * 	dev_enter & dev_remove
 *
 * Revision 2.3  91/10/14  18:14:39  roy
 * 	Revision 2.2.1.1  91/09/26  19:07:14  roy
 * 		Make type of device (block or char) an explicit field
 * 		(rather than overloading dev_t).
 * 
 * Revision 2.2  91/08/31  14:25:48  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/27  15:38:02  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.2  91/06/27  15:52:31  sp
 * Use OSF/1 zinit interface
 * 
 * Revision 3.1  91/06/25  17:12:02  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:06:00  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  91/03/20  15:05:06  dbg
 * 	Distinguish block and character devices.
 * 	[91/02/21            dbg]
 * 
 * Revision 2.3  90/11/05  16:57:51  rpd
 * 	Added per-bucket locks and counts.
 * 	[90/10/31            rpd]
 * 
 * Revision 2.2  90/10/25  15:07:19  rwd
 * 	Change hash function.
 * 	[90/10/15            rwd]
 * 
 */
/*
 * Support routines for device interface in out-of-kernel kernel.
 */

#include <uxkern/device_utils.h>
#include <sys/errno.h>

#include <kern/queue.h>
#include <kern/zalloc.h>
#include <cthreads.h>

/*
 * device_number to pointer hash table.
 */
#define	NDEVHASH	8
#define	DEV_NUMBER_HASH(dev)	\
		((major(dev) ^ minor(dev)) & (NDEVHASH-1))

struct dev_number_hash {
	struct mutex lock;
	queue_head_t queue;
	int count;
} dev_number_hash_table[NDEVHASH];

typedef struct dev_entry {
	queue_chain_t	chain;
	dev_t		dev;
        node_t          node;
	int		type;  		/* block or char */
	char *		object;		/* anything */
} dev_entry_t;

zone_t	dev_entry_zone;

void dev_utils_init()
{
	register int	i;

	for (i = 0; i < NDEVHASH; i++) {
	    mutex_init(&dev_number_hash_table[i].lock);
	    queue_init(&dev_number_hash_table[i].queue);
	    dev_number_hash_table[i].count = 0;
	}

	dev_entry_zone =
		zinit((vm_size_t)sizeof(dev_entry_t),
		      (vm_size_t)sizeof(dev_entry_t) * 4096,
		      vm_page_size,
		      "device to device_request port");
}

/*
 * Enter a (dev, node, type)-->object mapping in the dev table.  
 * 'type' specifies block or char device (BLOCK_DEV or CHAR_DEV)
 */
void dev_enter(dev, node, type, object)
	dev_t			dev;
        node_t                  node;
	int			type;
	char *			object;
{
	register dev_entry_t	*de;
	register struct 	dev_number_hash *b;

	de = (dev_entry_t *) zalloc(dev_entry_zone);
	de->dev = dev;
        de->node = node;
	de->object = object;
	de->type = type;

	b = &dev_number_hash_table[DEV_NUMBER_HASH(dev)];
	mutex_lock(&b->lock);
	enqueue_tail(&b->queue, (queue_entry_t)de);
	b->count++;
	mutex_unlock(&b->lock);
}

/*
 * Remove a device entry from the dev table.
 * 'type' specifies block or char device (BLOCK_DEV or CHAR_DEV)
 */
void dev_remove(dev, node, type)
	dev_t			dev;
        node_t                  node;
	int			type;
{
	register dev_entry_t 	*de;
	register struct 	dev_number_hash *b;

	b = &dev_number_hash_table[DEV_NUMBER_HASH(dev)];

	mutex_lock(&b->lock);

	for (de = (dev_entry_t *)queue_first(&b->queue);
	     !queue_end(&b->queue, (queue_entry_t)de);
	     de = (dev_entry_t *)queue_next(&de->chain)) {
	    if ((de->dev == dev) && (de->node == node) && 
                (de->type == type)) {
		queue_remove(&b->queue, de, dev_entry_t *, chain);
		b->count--;
		zfree(dev_entry_zone, (vm_offset_t)de);
		break;
	    }
	}

	mutex_unlock(&b->lock);
}

/*
 * Map a device specified by (dev, type) to an object.
 * 'type' specifies block or char device (BLOCK_DEV or CHAR_DEV)
 */
char *
dev_lookup(dev, node, type)
	dev_t			dev;
        node_t                  node;
	int			type;
{
	register dev_entry_t	*de;
	register struct 	dev_number_hash *b;
	char		  	*object = NULL;

	b = &dev_number_hash_table[DEV_NUMBER_HASH(dev)];
	mutex_lock(&b->lock);

	for (de = (dev_entry_t *)queue_first(&b->queue);
	     !queue_end(&b->queue, (queue_entry_t)de);
	     de = (dev_entry_t *)queue_next(&de->chain)) {
	    if ((de->dev == dev) && (de->node == node) && 
                (de->type == type)) {
		object = de->object;
		break;
	    }
	}

	mutex_unlock(&b->lock);
	return (object);
}

/*
 * Map kernel device error codes to BSD error numbers.
 */
int
dev_error_to_errno(err)
	int	err;
{
	switch (err) {
	    case D_SUCCESS:
		return (0);

	    case D_IO_ERROR:
		return (EIO);

	    case D_WOULD_BLOCK:
		return (EWOULDBLOCK);

	    case D_NO_SUCH_DEVICE:
		return (ENXIO);

	    case D_ALREADY_OPEN:
		return (EBUSY);

	    case D_DEVICE_DOWN:
		return (EIO);

	    case D_INVALID_OPERATION:
		return (ENOTTY);    /* weird, but ioctl() callers expect it */

	    case D_INVALID_RECNUM:
	    case D_INVALID_SIZE:
		return (EINVAL);

	    case D_NO_SPACE:
		return (ENOSPC);

	    default:
		return (EIO);
	}
	/*NOTREACHED*/
}
