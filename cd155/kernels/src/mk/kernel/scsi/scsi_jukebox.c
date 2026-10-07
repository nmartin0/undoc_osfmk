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
 * $Log: scsi_jukebox.c,v $
 * Revision 1.7  1995/03/14  23:49:07  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.6  1994/11/18  21:00:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/24  22:55:33  jerrie
 * Removed #ifdef to include this code as part of the standard SCSI driver.
 *
 * Revision 1.4  1993/06/30  22:54:10  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:48:16  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:53:46  dleslie
 * First R1_0 release
 *
 * Revision 2.6  91/06/19  11:57:53  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:30:46  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:45:58  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:47  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:18:31  mrt]
 * 
 * Revision 2.3  90/12/05  23:35:17  af
 * 	Cleanups.
 * 	[90/12/03  23:48:23  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:40:04  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_jukebox.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *
 * This file contains code for SCSI commands for MEDIA CHANGER devices.
 */

#include <mach/std_types.h>
#include <scsi/compat_30.h>

#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>

char *scjb_name(internal)
	boolean_t	internal;
{
	return internal ? "jz" : "jukebox";
}

scjb_optimize(tgt)
	target_info_t		*tgt;
{
}

scsi_init_element_status(tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_init_element_status_t	*cmd;

	cmd = (scsi_cmd_init_element_status_t *)(tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_INIT_ELEMENT_STATUS;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte	   = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_INIT_ELEMENT_STATUS;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

	return tgt->done;
}

scsi_read_element_status(tgt, type, tag, start, num, len, ior)
	register target_info_t	*tgt;
	unsigned char		type;
	boolean_t		tag;
	unsigned short		start;
	unsigned short		num;
	unsigned int		len;
	io_req_t		ior;
{
	scsi_cmd_read_element_status_t	*cmd;

	cmd = (scsi_cmd_read_element_status_t *)(tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_ELEMENT_STATUS;
	cmd->scsi_cmd_lun_and_relbit	   =
		(type & SCSI_CMD_ELEMENT_TYPE_MASK) |
		((tag) ? SCSI_CMD_RD_VOL_TAG : 0);
	cmd->scsi_cmd_rd_elm_start_msb	   = start >> 8;
	cmd->scsi_cmd_rd_elm_start_lsb	   = start >> 0;
	cmd->scsi_cmd_rd_elm_nelements_msb =   num >> 8;
	cmd->scsi_cmd_rd_elm_nelements_lsb =   num >> 0;
	cmd->scsi_cmd_xfer_len_1	   = 0;
	/*
	 * We only support 256 bytes max
	 *
	 * cmd->scsi_cmd_rd_elm_alloc_msb	   =   len >> 16;
	 * cmd->scsi_cmd_rd_elm_alloc_sb1	   =   len >> 8;
	 * cmd->scsi_cmd_rd_elm_alloc_lsb	   =   len >> 0;
	 */
	cmd->scsi_cmd_rd_elm_alloc_msb	   = 0;
	cmd->scsi_cmd_rd_elm_alloc_sb1	   = 0;
	cmd->scsi_cmd_rd_elm_alloc_lsb	   = 0xff;
	cmd->scsi_cmd_xxx1		   = 0;
	cmd->scsi_cmd_ctrl_byte		   = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_READ_ELEMENT_STATUS;

	scsi_go_and_wait(tgt, sizeof(*cmd), len, ior);

	return tgt->done;
}

scsi_position_to_element(tgt, trans, dst, invert, ior)
	register target_info_t	*tgt;
	register unsigned short	trans;
	register unsigned short	dst;
	boolean_t		invert;
	io_req_t		ior;
{
	scsi_cmd_position_to_element_t	*cmd;

	cmd = (scsi_cmd_position_to_element_t *)(tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_POSITION_TO_ELEMENT;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_posn_trans_msb = trans >> 8;
	cmd->scsi_cmd_posn_trans_lsb = trans >> 0;
	cmd->scsi_cmd_posn_dest_msb  =   dst >> 8;
	cmd->scsi_cmd_posn_dest_lsb  =   dst >> 0;
	cmd->scsi_cmd_xxx	     = 0;
	cmd->scsi_cmd_xfer_len_1     = 0;
	cmd->scsi_cmd_posn_invert    = (invert) ? SCSI_CMD_POSN_INVERT : 0;
	cmd->scsi_cmd_ctrl_byte	     = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_POSITION_TO_ELEMENT;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

	return tgt->done;
}

scsi_move_medium(tgt, trans, src, dst, invert, ior)
	register target_info_t	*tgt;
	register unsigned short	trans;
	register unsigned short	src;
	register unsigned short	dst;
	boolean_t		invert;
	io_req_t		ior;
{
	scsi_cmd_move_medium_t	*cmd;

	cmd = (scsi_cmd_move_medium_t *)(tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_MOVE_MEDIUM;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_move_trans_msb = trans >> 8;
	cmd->scsi_cmd_move_trans_lsb = trans >> 0;
	cmd->scsi_cmd_move_src_msb   =   src >> 8;
	cmd->scsi_cmd_move_src_lsb   =   src >> 0;
	cmd->scsi_cmd_move_dest_msb  =   dst >> 8;
	cmd->scsi_cmd_move_dest_lsb  =   dst >> 0;
	cmd->scsi_cmd_xfer_len_3     = 0;
	cmd->scsi_cmd_xfer_len_4     = 0;
	cmd->scsi_cmd_move_invert    = (invert) ? SCSI_CMD_MV_INVERT : 0;
	cmd->scsi_cmd_ctrl_byte	     = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_MOVE_MEDIUM;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

	return tgt->done;
}

scsi_exchange_medium(tgt, trans, src, dst1, dst2, invert1, invert2, ior)
	register target_info_t	*tgt;
	register unsigned short	trans;
	register unsigned short	src;
	register unsigned short	dst1;
	register unsigned short	dst2;
	boolean_t		invert1;
	boolean_t		invert2;
	io_req_t		ior;
{
	scsi_cmd_exchange_medium_t	*cmd;

	cmd = (scsi_cmd_exchange_medium_t *)(tgt->cmd_ptr);
	cmd->scsi_cmd_code		 = SCSI_CMD_EXCHANGE_MEDIUM;
	cmd->scsi_cmd_lun_and_relbit	 = 0;
	cmd->scsi_cmd_exchange_trans_msb = trans >> 8;
	cmd->scsi_cmd_exchange_trans_lsb = trans >> 0;
	cmd->scsi_cmd_exchange_src_msb   =   src >> 8;
	cmd->scsi_cmd_exchange_src_lsb   =   src >> 0;
	cmd->scsi_cmd_exchange_dest1_msb =  dst1 >> 8;
	cmd->scsi_cmd_exchange_dest1_lsb =  dst1 >> 0;
	cmd->scsi_cmd_exchange_dest2_msb =  dst2 >> 8;
	cmd->scsi_cmd_exchange_dest2_lsb =  dst2 >> 0;
	cmd->scsi_cmd_exchange_invert    = (invert1) ? SCSI_CMD_EX_INVERT_1 : 0;
	cmd->scsi_cmd_exchange_invert   |= (invert2) ? SCSI_CMD_EX_INVERT_2 : 0;
	cmd->scsi_cmd_ctrl_byte		 = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_EXCHANGE_MEDIUM;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

	return tgt->done;
}

#if 0
scsi_request_volume_address
scsi_send_volume_tag
#endif
