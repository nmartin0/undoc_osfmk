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
 * $Log: scsi_cpu.c,v $
 * Revision 1.7  1995/03/14  23:48:56  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.6  1994/11/18  21:00:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/16  18:12:17  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.4  1993/06/30  22:54:01  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:48:07  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:53:37  dleslie
 * First R1_0 release
 *
 * Revision 2.7.2.1  92/03/28  10:15:58  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:32:54  jeffreyh]
 * 
 * Revision 2.8  92/02/23  22:44:52  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:29:47  af]
 * 
 * Revision 2.7  91/08/24  12:28:34  af
 * 	Filled in with what was missing.  Works, too.
 * 	[91/08/02  03:46:32  af]
 * 
 * Revision 2.6  91/06/19  11:57:39  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:30:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:45:39  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:23  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:18:04  mrt]
 * 
 * Revision 2.3  90/12/05  23:35:09  af
 * 
 * 
 * Revision 2.1.1.1  90/11/01  03:39:46  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_cpu.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *
 * This file contains code for SCSI commands for PROCESSOR devices.
 */

#include <mach/std_types.h>
#include <scsi/compat_30.h>

#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>

char *sccpu_name(internal)
	boolean_t	internal;
{
	return internal ? "sc" : "cpu";
}

scsi_send( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_write_t	*cmd;
	unsigned		len;	/* in bytes */
	unsigned int		max_dma_data;

#ifdef PARAGON860	/* performance mod */
	max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;
	if (len < tgt->block_size)
		len = tgt->block_size;

	cmd = (scsi_cmd_write_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_SEND;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >> 8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_SEND;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.write.requests++;
		tgt->statistics.write.xfer_count += len;
	}
#endif	SCSI_STATISTICS
}

scsi_receive( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_read_t		*cmd;
	register unsigned	len;
	unsigned int		max_dma_data;

#ifdef PARAGON860	/* performance mod */
	max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;
	if (len < tgt->block_size)
		len = tgt->block_size;

	cmd = (scsi_cmd_read_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_RECEIVE;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_RECEIVE;

	scsi_go(tgt, sizeof(*cmd), len, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.read.requests++;
		tgt->statistics.read.xfer_count += len;
	}
#endif	SCSI_STATISTICS
}
