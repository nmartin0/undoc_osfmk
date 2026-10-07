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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * $Log: scsi_disk.c,v $
 * Revision 1.8  1995/03/28  17:33:35  jerrie
 * Added SCSI_CMD_QUEUE_DEPTH bootmagic to control tagged queue depth.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low
 *  Benefit or PTS #: Provides a way to change the queue depth.
 *  Testing:	   Developer
 *  Module(s):	   scsi_disk.c, scsi_tape.c
 *
 * Revision 1.7  1995/03/14  23:49:02  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.6  1994/11/18  21:00:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/16  18:12:26  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.4  1993/06/30  22:54:06  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:48:12  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:53:42  dleslie
 * First R1_0 release
 *
 * Revision 2.11.2.1  92/03/28  10:16:09  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:33:08  jeffreyh]
 * 
 * Revision 2.12  92/02/23  22:44:59  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:29:26  af]
 * 
 * Revision 2.11  91/08/24  12:29:08  af
 * 	Bcopy casts.
 * 
 * Revision 2.10  91/06/19  11:57:47  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.9  91/05/14  17:30:28  mrt
 * 	Correcting copyright
 * 
 * Revision 2.8  91/05/13  06:05:40  af
 * 	Enabled some group-1 commands needed on big disks.
 * 	Wrote all those that were missing, kept disabled until
 * 	we find a use for them (via ioctls, I am afraid).
 * 	[91/05/12  16:24:35  af]
 * 
 * Revision 2.7.1.1  91/03/29  17:13:10  af
 * 	Enabled some group-1 commands needed on big disks.
 * 	Wrote all those that were missing, kept disabled until
 * 	we find a use for them (via ioctls, I am afraid).
 * 
 * Revision 2.7  91/02/05  17:45:49  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:35  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:18:21  mrt]
 * 
 * Revision 2.6  90/12/05  23:35:14  af
 * 	Use and prefer BSD/OSF labels.
 * 	[90/12/03  23:48:09  af]
 * 
 * Revision 2.4.1.1  90/11/01  03:39:58  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_disk.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *
 * This file contains code for SCSI commands for DISK devices.
 */

#include <mach/std_types.h>
#include <scsi/compat_30.h>

#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>


char *scdisk_name(internal)
	boolean_t	internal;
{
	return internal ? "rz" : "disk";
}

scdisk_optimize(tgt)
	target_info_t	*tgt;
{
	int		i;
	scsi_ret_t	ret;

	/*
	 * Make sure it is up and about
	 */
	i = 0;
	while ((ret = scsi_test_unit_ready(tgt, 0)) != SCSI_RET_SUCCESS) {

		if (i++ == 5)
			printf(".. not yet ready ..");
		delay(1000000);
		if (i == 60) {
			printf(".. seems hopeless.");
			break;
		}
	}
	if ((i >= 5) && (i < 60)) {
		printf(" ready.");
	}

	if (ret != SCSI_RET_SUCCESS) {
		/*
		 * If the disk *still* is not ready, it might not return
		 * the mode sense data to set up the command queuing (below).
		 * Instead of returning an error here we just let it fall
		 * through in case it happens to work.
		 */
	}

	/*
	 * If the device supports tagged command queuing, set up the
	 * control mode page for the expected error handling behavior.
	 */
	if (tgt->flags & TGT_TAGGED_QUEUING) {
		scsi_mode_sense_data_t		*header;
		scsi_mode_sense_page10_t	*page10;
		int				depth;

		/*
		 * Read the current values
		 */
		ret = scsi_mode_sense(tgt, 10,
			sizeof(scsi_mode_sense_data_t) +
			sizeof(scsi_mode_sense_page10_t), 0);

		if (ret != SCSI_RET_SUCCESS) {
			if (scsi_debug) {
				printf("[scdisk_optimize: %s]\n",
				    "mode sense error - page 10 (changeable)");
			}

			/*
			 * In this case we can't support tagged
			 * command queuing on the device.
			 */
			BCLR(scsi_should_queue, tgt->masterno, tgt->target_id);

			return;
		}

		header = (scsi_mode_sense_data_t *)tgt->cmd_ptr;
		page10 = (scsi_mode_sense_page10_t *)(header + 1);

		/*
		 * Check the current values.  For disks, we want dque
		 * off (queuing enabled), qerr off (continue executing
		 * queued commands), and q_algorithm set for un-restriced
		 * re-ordering.
		 */
		if ((page10->dque != 0) ||
		    (page10->qerr != 0) ||
		    (page10->q_algorithm != SCSI_UNRESTRICTED_REORDERING)) {
			unsigned char		page10_buf
	[sizeof(scsi_mode_sense_data_t) + sizeof(scsi_mode_sense_page10_t)];

			/*
			 * Change the settings to our liking
			 */
			header->data_len    = 0;
			header->medium_type = 0;
			header->device_spec = 0;

			header->bdesc[0].density_code = 0;
			header->bdesc[0].no_blks_msb  = 0;
			header->bdesc[0].no_blks      = 0;
			header->bdesc[0].no_blks_lsb  = 0;
			header->bdesc[0].reserved     = 0;

			page10->ps	    = 0;
			page10->dque	    = 0;
			page10->qerr	    = 0;
			page10->q_algorithm = SCSI_UNRESTRICTED_REORDERING;

			/* Copy sense data to a temporary buffer */
			bcopy(header, page10_buf, sizeof(page10_buf));

			/* Select new values */
			ret = scdisk_mode_select(tgt, 0, 0, page10_buf,
				sizeof(page10_buf), FALSE);

			if (ret != SCSI_RET_SUCCESS) {
				if (scsi_debug) {
					printf("[scdisk_optimize: %s]\n",
						"mode select error - page 10");
				}

				/*
				 * In this case we can't support tagged
				 * command queuing on the device.
				 */
				BCLR(scsi_should_queue, tgt->masterno,
					tgt->target_id);

				return;
			}
		}

		/*
		 * Initialize the controller interface
		 * layer to support tagged command queuing,
		 * default to 64 queued commands for now
		 */
		depth = getbootint("SCSI_CMD_QUEUE_DEPTH", 64);
		if (scsi_softc[tgt->masterno]->tag_q_init) {
			(scsi_softc[tgt->masterno]->tag_q_init)(tgt, depth);
		}
	}
}

/*
 * SCSI commands partially specific to disks
 */
scdisk_read( tgt, secno, ior)
	register target_info_t	*tgt;
	register unsigned int	secno;
	io_req_t		ior;
{
	scsi_cmd_read_t		*cmd;
	register unsigned	len;
	unsigned int		max_dma_data;
	unsigned int		nblks;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;
	if (len < tgt->block_size)
		len = tgt->block_size;

	nblks = btodb(len);

	cmd = (scsi_cmd_read_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ;
	cmd->scsi_cmd_lun_and_lba1 = (secno>>16)&SCSI_LBA_MASK;
	cmd->scsi_cmd_lba2 	   = (secno>> 8)&0xff;
	cmd->scsi_cmd_lba3 	   = (secno    )&0xff;
	cmd->scsi_cmd_xfer_len     = nblks;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_READ;

	scsi_go(tgt, sizeof(*cmd), len, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.read.requests++;
		tgt->statistics.read.xfer_count += nblks;
	}
#endif	SCSI_STATISTICS
}

scdisk_write( tgt, secno, ior)
	register target_info_t	*tgt;
	register unsigned int	secno;
	io_req_t		ior;
{
	scsi_cmd_write_t	*cmd;
	unsigned		len;	/* in bytes */
	unsigned int		max_dma_data;
	unsigned int		nblks;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;
	if (len < tgt->block_size)
		len = tgt->block_size;

	nblks = btodb(len);

	cmd = (scsi_cmd_write_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_WRITE;
	cmd->scsi_cmd_lun_and_lba1 = (secno>>16)&SCSI_LBA_MASK;
	cmd->scsi_cmd_lba2 	   = (secno>> 8)&0xff;
	cmd->scsi_cmd_lba3 	   = (secno    )&0xff;
	cmd->scsi_cmd_xfer_len     = nblks;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_WRITE;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.write.requests++;
		tgt->statistics.write.xfer_count += nblks;
	}
#endif	SCSI_STATISTICS
}

scdisk_mode_select(tgt, lbn, ior, mdata, mlen, save)
	register target_info_t	*tgt;
	register int 		lbn;
	io_req_t		ior;
	char			*mdata;
{
	scsi_cmd_mode_select_t	*cmd;
	scsi_mode_select_param_t	*parm;
	register int			len;

	bzero(tgt->cmd_ptr, sizeof(*cmd) + sizeof(*parm));
	cmd = (scsi_cmd_mode_select_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_MODE_SELECT;
	cmd->scsi_cmd_lun_and_lba1 = SCSI_CMD_MSL_PF;	/* XXX only if... */
	cmd->scsi_cmd_xfer_len = sizeof(scsi_mode_select_param_t);/* no vuq */

	parm = (scsi_mode_select_param_t*) (cmd + 1);
	if (mdata) {
		cmd->scsi_cmd_xfer_len = mlen;
		bcopy(mdata, (char*)parm, mlen);
		if (save)
			cmd->scsi_cmd_lun_and_lba1 |= SCSI_CMD_MSL_SP;
	} else {
	 	/* parm->medium_type = if (floppy)disk.. */
		parm->desc_len = 8;
		/* this really is the LBN */
		parm->descs[0].density_code = 0;/* XXX default XXX */
		parm->descs[0].reclen1 = (lbn>>16)&0xff;
		parm->descs[0].reclen2 = (lbn>> 8)&0xff;
		parm->descs[0].reclen3 = (lbn    )&0xff;
		mlen = sizeof(*parm);
	}

	tgt->cur_cmd = SCSI_CMD_MODE_SELECT;

	scsi_go_and_wait(tgt, sizeof(*cmd) + mlen, 0, ior);

	return tgt->done;
}

/*
 * SCSI commands fully specific to disks
 */
scsi_read_capacity( tgt, lbn, ior)
	register target_info_t	*tgt;
	int			lbn;
	io_req_t		ior;
{
	scsi_cmd_read_capacity_t	*cmd;

	bzero(tgt->cmd_ptr, sizeof(*cmd));
	cmd = (scsi_cmd_read_capacity_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_CAPACITY;
	/* all zeroes, unless... */
	if (lbn) {
		cmd->scsi_cmd_rcap_flags = SCSI_CMD_RCAP_PMI;
		cmd->scsi_cmd_lba1 = (lbn>>24);
		cmd->scsi_cmd_lba2 = (lbn>>16)&0xff;
		cmd->scsi_cmd_lba3 = (lbn>> 8)&0xff;
		cmd->scsi_cmd_lba4 = (lbn    )&0xff;
	}
	
	tgt->cur_cmd = SCSI_CMD_READ_CAPACITY;

	scsi_go_and_wait(tgt, sizeof(*cmd), sizeof(scsi_rcap_data_t),ior);

	return tgt->done;
}

scsi_reassign_blocks( tgt, defect_list, n_defects, ior)
	register target_info_t	*tgt;
	unsigned int		*defect_list;	/* In ascending order ! */
	int			n_defects;
	io_req_t		ior;
{
	scsi_cmd_reassign_blocks_t	*cmd;
	scsi_Ldefect_data_t		*parm;
	unsigned		len;	/* in bytes */

	cmd = (scsi_cmd_reassign_blocks_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_REASSIGN_BLOCKS;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */

	parm = (scsi_Ldefect_data_t *) (cmd + 1);
	parm->res1 = parm->res2 = 0;
	n_defects *= 4;	/* in 4-byte-ints */
	parm->list_len_msb = n_defects >> 8;
	parm->list_len_lsb = n_defects;
	bcopy((char*)defect_list, (char*)parm->defects, n_defects);

	tgt->cur_cmd = SCSI_CMD_REASSIGN_BLOCKS;

	scsi_go(tgt, sizeof(*cmd) + sizeof(*parm) + (n_defects - 4), 0, FALSE);
}

scsi_medium_removal( tgt, allow, ior)
	register target_info_t	*tgt;
	boolean_t		allow;
	io_req_t		ior;
{
	scsi_cmd_medium_removal_t	*cmd;

	cmd = (scsi_cmd_medium_removal_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_PREVENT_ALLOW_REMOVAL;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_pa_prevent   = allow ? 0 : 1;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_PREVENT_ALLOW_REMOVAL;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}

scsi_format_unit( tgt, mode, vuqe, intlv, ior)
	register target_info_t	*tgt;
	register unsigned int	intlv;
	io_req_t		ior;
{
	scsi_cmd_format_t	*cmd;
	char			*parms;

	cmd = (scsi_cmd_format_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_FORMAT_UNIT;
	cmd->scsi_cmd_lun_and_lba1 =
		mode & (SCSI_CMD_FMT_FMTDATA|SCSI_CMD_FMT_CMPLIST|SCSI_CMD_FMT_LIST_TYPE);
	cmd->scsi_cmd_lba2 	   = vuqe;
	cmd->scsi_cmd_lba3 	   = intlv >>  8;
	cmd->scsi_cmd_xfer_len     = intlv;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */

	parms = (char*) cmd + 1;
	if (ior->io_count)
		bcopy(ior->io_data, parms, ior->io_count);
	else
		bzero(parms, 0xff - sizeof(*cmd));

	tgt->cur_cmd = SCSI_CMD_FORMAT_UNIT;

	scsi_go_and_wait(tgt, sizeof(*cmd) + ior->io_count, 0, ior);
	return tgt->done;
}


/* Group 1 Commands */

scsi_long_read( tgt, secno, ior)
	register target_info_t	*tgt;
	register unsigned int	secno; 
	io_req_t		ior;
{
	scsi_cmd_long_read_t	*cmd;
	register unsigned	len;
	unsigned int		max_dma_data;
	unsigned int		nblks;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;
	if (len < tgt->block_size)
		len = tgt->block_size;

	nblks = btodb(len);

	cmd = (scsi_cmd_long_read_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_LONG_READ;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = nblks >> 8;
	cmd->scsi_cmd_xfer_len_2     = nblks;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_LONG_READ;

	scsi_go(tgt, sizeof(*cmd), len, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.read.requests++;
		tgt->statistics.read.xfer_count += nblks;
	}
#endif	SCSI_STATISTICS
}


scsi_long_write( tgt, secno, ior)
	register target_info_t	*tgt;
	register unsigned int	secno;
	io_req_t		ior;
{
	scsi_cmd_long_write_t	*cmd;
	unsigned		len;	/* in bytes */
	unsigned int		max_dma_data;
	unsigned int		nblks;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;
	if (len < tgt->block_size)
		len = tgt->block_size;

	nblks = btodb(len);

	cmd = (scsi_cmd_long_write_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_LONG_WRITE;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = nblks >> 8;
	cmd->scsi_cmd_xfer_len_2     = nblks;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_LONG_WRITE;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.write.requests++;
		tgt->statistics.write.xfer_count += nblks;
	}
#endif	SCSI_STATISTICS
}

scdisk_verify( tgt, secno, nsectrs, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_verify_long_t	*cmd;
	int			len;

	len = ior->io_count;

	cmd = (scsi_cmd_verify_long_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_VERIFY_1;
	cmd->scsi_cmd_lun_and_relbit = len ? SCSI_CMD_VFY_BYTCHK : 0;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = (nsectrs) >> 8;
	cmd->scsi_cmd_xfer_len_2     = nsectrs;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_VERIFY_1;

	scsi_go_and_wait(tgt, sizeof(*cmd) + len, 0, ior);
	return tgt->done;
}


scsi_read_defect( tgt, mode, ior)
	register target_info_t	*tgt;
	register unsigned int	mode; 
	io_req_t		ior;
{
	scsi_cmd_long_read_t	*cmd;
	register unsigned	len;

	len = ior->io_count;
	if (len > 0xffff)
		len = 0xffff;

	cmd = (scsi_cmd_read_defect_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_DEFECT_DATA;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_lba1	     = mode & 0x1f;
	cmd->scsi_cmd_lba2	     = 0;
	cmd->scsi_cmd_lba3	     = 0;
	cmd->scsi_cmd_lba4	     = 0;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = (len) >> 8;
	cmd->scsi_cmd_xfer_len_2     = (len);
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	/* ++ HACK Alert */
/*	tgt->cur_cmd = SCSI_CMD_READ_DEFECT_DATA;*/
	tgt->cur_cmd = SCSI_CMD_LONG_READ;
	/* -- HACK Alert */

	scsi_go(tgt, sizeof(*cmd), len, FALSE);
	iowait(ior);
	return tgt->done;
}


#if	0 /* unused commands */
scsi_rezero_unit( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_rezero_t	*cmd;

	cmd = (scsi_cmd_rezero_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_REZERO_UNIT;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_REZERO_UNIT;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}

scsi_seek( tgt, where, ior)
	register target_info_t	*tgt;
	register unsigned int	where;
	io_req_t		ior;
{
	scsi_cmd_seek_t	*cmd;

	cmd = (scsi_cmd_seek_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_SEEK;
	cmd->scsi_cmd_lun_and_lba1 = (where >> 16) & 0x1f;
	cmd->scsi_cmd_lba2 	   = where >>  8;
	cmd->scsi_cmd_lba3 	   = where;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_SEEK;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}

scsi_reserve( tgt, len, id, mode, ior)
	register target_info_t	*tgt;
	register unsigned int	len;
	unsigned char		id;
	io_req_t		ior;
{
	scsi_cmd_reserve_t	*cmd;

	cmd = (scsi_cmd_reserve_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_RESERVE;
	cmd->scsi_cmd_lun_and_lba1 = mode & 0x1f;
	cmd->scsi_cmd_reserve_id   = id;
	cmd->scsi_cmd_extent_llen1 = len >>  8;
	cmd->scsi_cmd_extent_llen2 = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_RESERVE;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}

scsi_release( tgt, id, mode, ior)
	register target_info_t	*tgt;
	unsigned char		id, mode;
	io_req_t		ior;
{
	scsi_cmd_release_t	*cmd;

	cmd = (scsi_cmd_release_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_RELEASE;
	cmd->scsi_cmd_lun_and_lba1 = mode & 0x1f;
	cmd->scsi_cmd_reserve_id   = id;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_RELEASE;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}


/* Group 1 Commands */

scsi_long_seek( tgt, secno, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_long_seek_t	*cmd;

	cmd = (scsi_cmd_long_seek_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_LONG_SEEK;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = 0;
	cmd->scsi_cmd_xfer_len_2     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_LONG_SEEK;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);
}

scsi_write_verify( tgt, secno, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_write_vfy_t	*cmd;
	unsigned		len;	/* in bytes */
	unsigned int		max_dma_data;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;

	cmd = (scsi_cmd_write_vfy_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_WRITE_AND_VERIFY;
	cmd->scsi_cmd_lun_and_relbit = SCSI_CMD_VFY_BYTCHK;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = (btodb(len)) >> 8;
	cmd->scsi_cmd_xfer_len_2     = btodb(len);
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_WRITE_AND_VERIFY;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);
}

scsi_search_data( tgt, secno, how, flags, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_search_t	*cmd;
	unsigned		len;	/* in bytes */
	unsigned int		max_dma_data;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	if (how != SCSI_CMD_SEARCH_HIGH &&
	    how != SCSI_CMD_SEARCH_EQUAL &&
	    how != SCSI_CMD_SEARCH_LOW)
		panic("scsi_search_data");

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;

	cmd = (scsi_cmd_search_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = how;
	cmd->scsi_cmd_lun_and_relbit = flags & 0x1e;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = (btodb(len)) >> 8;
	cmd->scsi_cmd_xfer_len_2     = btodb(len);
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = how;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);
}


scsi_set_limits( tgt, secno, nblocks, inhibit, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_set_limits_t	*cmd;

	cmd = (scsi_cmd_set_limits_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_SET_LIMITS;
	cmd->scsi_cmd_lun_and_relbit = inhibit & 0x3;
	cmd->scsi_cmd_lba1	     = secno >> 24;
	cmd->scsi_cmd_lba2	     = secno >> 16;
	cmd->scsi_cmd_lba3	     = secno >>  8;
	cmd->scsi_cmd_lba4	     = secno;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = nblocks >> 8;
	cmd->scsi_cmd_xfer_len_2     = nblocks;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_SET_LIMITS;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);
}


#endif

#ifdef	SCSI2
scsi_lock_cache
scsi_prefetch
scsi_read_defect_data
scsi_sync_cache
scsi_write_same
#endif	SCSI2
