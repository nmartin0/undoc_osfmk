/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: scsi_queue.c,v $
 * Revision 1.2.8.1  1995/06/11  18:34:24  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:49:11  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	scsi_queue.c
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	SCSI device driver tagged command queuing support functions
 */


#include <scsi.h>
#if	NSCSI > 0

#include <sys/types.h>
#include <vm/vm_kern.h>

#include <scsi/compat_30.h>
#include <scsi/scsi_defs.h>
#include <scsi/scsi_queue.h>
#include <scsi/scsi.h>

/* Forward declarations */
void            req_q_enter_first(tag_info_t *, target_info_t *);


tag_info_t *
scsi_queue_alloc(tgt, size, depth)
	target_info_t	*tgt;
	int		size;
	int		depth;
{
	int		i;
	int		bytes;
	unsigned char	*cmd_p;
	tag_info_t	*tag;

	assert(size > 0);
	assert(depth > 0);

	/*
	 * Free any existing queue tag data structures
	 */
	if (tgt->tag_q)
		scsi_queue_free(tgt);

	/*
	 * Save some information
	 */
	tgt->tag_q_size  = size;
	tgt->tag_q_depth = depth;

	/*
	 * Allocate and initialize tagged command queuing data structures
	 */
	bytes = size * depth;
	if (kmem_alloc_wired(kernel_map, &tgt->tag_q, bytes) != KERN_SUCCESS) {
		tgt->tag_q_size	 = 0;
		tgt->tag_q_depth = 0;
		tgt->tag_q	 = (tag_info_t *)NULL;
		return tgt->tag_q;
	}
	bzero(tgt->tag_q, bytes);
	
	/*
	 * Allocate and zero fill the command buffers
	 */
	bytes = depth * 256;	/* number of commands * 256 bytes per command */
	if (kmem_alloc_wired(kernel_map, &cmd_p, bytes) != KERN_SUCCESS) {
		bytes = size * depth;
		kmem_free(kernel_map, tgt->tag_q, bytes);
		tgt->tag_q_size	 = 0;
		tgt->tag_q_depth = 0;
		tgt->tag_q	 = (tag_info_t *)NULL;
		return tgt->tag_q;
	}
	bzero(cmd_p, bytes);

	/*
	 * Do the once only initialization
	 */
	queue_init(&tgt->free_tag_q);
	queue_init(&tgt->active_tag_q);

	for (i = 0; i < depth; i++) {
		tag = (tag_info_t *)((unsigned char *)tgt->tag_q + (size * i));

		tag->tag     = i;
		tag->state   = TAG_FREE;
		tag->cmd_ptr = cmd_p + (i * 256);
		tag->tgt     = tgt;

		enqueue_tail(&tgt->free_tag_q, (queue_entry_t)tag);
	}

	return tgt->tag_q;
}


scsi_queue_free(tgt)
	target_info_t	*tgt;
{
	/*
	 * Sanity check
	 */
	if ((tgt->tag_q == NULL)    ||
	    (tgt->tag_q_size  == 0) ||
	    (tgt->tag_q_depth == 0))
		return 0;

	/*
	 * Free the command queue data buffers
	 */
	kmem_free(kernel_map, tgt->tag_q, tgt->tag_q_size * tgt->tag_q_depth);

	/*
	 * Clear size, depth, and pointer fields
	 */
	tgt->tag_q_size  = 0;
	tgt->tag_q_depth = 0;
	tgt->tag_q	 = (tag_info_t *)NULL;

	return 0;
}


tag_info_t *
scsi_queue_reserve(tgt)
	target_info_t	*tgt;
{
	tag_info_t	*tag;

	tag = (tag_info_t *)dequeue_head(&tgt->free_tag_q);
	tag->state = TAG_BUSY;

	return tag;
}

scsi_queue_release(tgt, tag)
	target_info_t	*tgt;
	tag_info_t	*tag;
{
	tag->state = TAG_FREE;
	enqueue_tail(&tgt->free_tag_q, (queue_entry_t)tag);

	return 0;
}


void
scsi_queue_save(tgt, tag)
	target_info_t	*tgt;
	tag_info_t	*tag;
{
	unsigned char	*tmp;

	/*
	 * Move information from the tgt data structure
	 * to the tag queue data structure for safe keeping
	 */

	/* Just save these fields */
	tag->flags   = tgt->flags;
	tag->cur_cmd = tgt->cur_cmd;
	tag->lun     = tgt->lun;

	/* restore the real LUN value for the next command */
	tgt->lun = tgt->true_lun;

	/*
	 * Swap the command pointer in the tag with the
	 * command pointer in the target data structure
	 * This provides a new command buffer for the
	 * for the next request
	 */
	tmp = tag->cmd_ptr;
	tag->cmd_ptr = tgt->cmd_ptr;
	tgt->cmd_ptr = tmp;

	/*
	 * Save the target ior in the queue tag structure
	 * Set the target ior to the next request on the chain if 
	 * scsi_queue_enabled;
	 * If the next request is null, this allows the following
	 * request from the kernel to be queued up on the target
	 * (unless, of course, the queue is fulll)
	 */
	tag->ior = tgt->ior;
	if (is_scsi_queue_enabled(tgt)) {
	    if (tgt->ior) {
		tgt->ior = tag->ior->io_next;	/* remove to queue one cmd */
	    }
	}
}

void
scsi_queue_restore(tgt, tag)
	target_info_t	*tgt;
	tag_info_t	*tag;
{
	/*
	 * Move information from the tag queue data
	 * structure back into the target data structure
	 */

	/*
	 * The command buffer pointer should only be restored
	 * on commands other than read or write
	 */
	if (tag->cur_cmd != SCSI_CMD_READ      &&
	    tag->cur_cmd != SCSI_CMD_LONG_READ &&
	    tag->cur_cmd != SCSI_CMD_WRITE     &&
	    tag->cur_cmd != SCSI_CMD_LONG_WRITE) {
		unsigned char	*tmp;

		/*
		 * Swap the command pointer in the tag with the
		 * command pointer in the target data structure
		 * This restores the old command buffer for the
		 * for the request
		 */
		tmp = tgt->cmd_ptr;
		tgt->cmd_ptr = tag->cmd_ptr;
		tag->cmd_ptr = tmp;
	}

	/*
	 * We only want to restore the TGT_BBR_ACTIVE
	 * and TGT_OPTIONAL_CMD flags because these
	 * flags might change on a per reqest basis.
	 * Other flags are global for the target across
	 * all requests.
	 */
	tgt->flags &= ~(TGT_BBR_ACTIVE | TGT_OPTIONAL_CMD);
	tgt->flags |= tag->flags & (TGT_BBR_ACTIVE | TGT_OPTIONAL_CMD);

	/* Just restore this value */
	tgt->cur_cmd = tag->cur_cmd;

	/*
	 * NOTE that we do *not* restore the tgt->lun field here.
	 * 	The lun field was restored by the scsi_queue_save()
	 * 	function so that subsequent commands use the correct
	 * 	value.
	 */

	/*
	 * Put the request back on the head of the chain
	 */
        if (is_scsi_queue_enabled(tgt)) {
	    req_q_enter_first(tag,tgt);
	}
}

void
req_q_enter_first(tag, tgt)
    tag_info_t *tag;
    target_info_t *tgt;
{
    if (!tag->ior)
	return;
    tag->ior->io_next = tgt->ior;
    if (tgt->ior) {
	tgt->ior->io_prev = tag->ior;
    }
    tgt->ior = tag->ior;
}


tag_info_t *
scsi_queue_locate(tgt, lun, tag)
	target_info_t	*tgt;
	int		lun;
	int		tag;
{
	int		i;

	/*
	 * Sanity check
	 */
	if (tag >= tgt->tag_q_depth)
		return (tag_info_t *)NULL;

	/*
	 * We ignore the lun parameter and simply use
	 * the tag as an index into the tag_q structs
	 */
	i = tgt->tag_q_size * tag;
	return (tag_info_t *)((unsigned char *)tgt->tag_q + i);
}

#if	MACH_ASSERT

/*
 * Display the queue tag information
 *
 * Note that this function will only properly if the tag pointers
 * are in the format as created by scsi_queue_alloc().  Allocating
 * tags using an adapter specific function may not guarantee the
 * expected format.
 */
int
scsi_queue_dump(master, tgt_id, all)
	int		master;
	int		tgt_id;
	boolean_t	all;
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	tag_info_t	*tag;
	int		depth;
	int		size;
	int		lun;
	int		i;
	int		active;

	if (master >= NSCSI) {
		db_printf("Master too large:  0 <= master <= %d\n", NSCSI - 1);
		return 0;
	}

	if (!(sc = scsi_softc[master])) {
		db_printf("scsi_softc[%d] uninitialized\n", master);
		return 0;
	}

	for (lun = 0; lun < MAX_LUNS; lun++) {

		if (!(tgt = sc->target[tgt_id][lun])) {
			db_printf("sc->target[%d][%d] uninitialized\n",
				tgt_id, lun);
			return 0;
		}

		/*
		 * Weird stuff for disk.  We're not supporting LUNs
		 * on disks yet, so each target/lun structure points
		 * to target/lun 0.
		 */
		if (lun != 0 && tgt == sc->target[tgt_id][0])
			continue;

		depth = tgt->tag_q_depth;
		size  = tgt->tag_q_size;

		if (all) {
			db_printf(
		    "Controller %d Target %d LUN %d Total Queue Depth %d\n\n",
				master, tgt_id, lun, depth);
		}

		active = 0;
		for (i = 0; i < depth; i++) {
			tag =
		    (tag_info_t *)((unsigned char *)tgt->tag_q + (size * i));

			if (!all && (tag->state == TAG_FREE))
				continue;

			db_printf("Tag %d @ 0x%x\n",     tag->tag, tag);
			db_printf("\tlinks.prev 0x%x\n", tag->links.prev);
			db_printf("\tlinks.next 0x%x\n", tag->links.next);

			db_printf("\tstate      ");
			switch (tag->state) {
				case TAG_FREE:
					db_printf("Free");
					break;
				case TAG_BUSY:
					db_printf("BUSY");
					active++;
					break;
				default:
					db_printf("UNKNOWN 0x%x", tag->state);
					break;
			}
			db_printf("\n");

			db_printf("\tior        0x%x\n", tag->ior);
			db_printf("\tflags      0x%x\n", tag->flags);
			db_printf("\tcmd_ptr    0x%x\n", tag->cmd_ptr);
			db_printf("\tcmd_count  %d\n",   tag->cmd_count);
			db_printf("\tcur_cmd    0x%x\n", tag->cur_cmd);
			db_printf("\tcmd_only   0x%x\n", tag->cmd_only);
			db_printf("\tlun        %d\n",   tag->lun);
		}

		db_printf(
		    "\nController %d Target %d LUN %d Active Tag Count:  %d\n",
			master, tgt_id, lun, active);
	}

	return 0;
}

#endif	MACH_ASSERT

#endif	NSCSI > 0
