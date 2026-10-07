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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS AS-IS
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
 * $Log: scsi_dma.h,v $
 * Revision 1.6  1994/11/18  21:01:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/16  18:12:44  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.4  1993/06/30  22:55:45  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:49:30  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:55:08  dleslie
 * First R1_0 release
 *
 * Revision 2.3  91/11/12  11:17:22  rvb
 * 	Added end_msgin().
 * 	[91/10/30  13:42:14  af]
 * 
 * Revision 2.2  91/08/24  12:27:29  af
 * 	Created.
 * 	[91/08/02  04:14:25  af]
 * 
 */
/*
 *	File: scsi_dma.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	DMA operations that an HBA driver might invoke.
 *
 */

/*
 * This defines much more than usually needed, mainly
 * to cover for the case of no DMA at all and/or only
 * DMA from/to a specialized buffer ( which means the
 * CPU has to copy data into/outof it ).
 */

typedef struct {
	opaque_t	(*init)(
				/* int unit,
				   vm_offset_t base */);

#ifdef PARAGON860	/* performance mod */
	void		(*new_target)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   unsigned char ntargets */);
#else
	void		(*new_target)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);
#endif

	void		(*map)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*start_cmd)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	void		(*end_xfer)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	void		(*end_cmd)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   io_req_t ior */);

	int		(*start_datain)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*start_msgin)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	void		(*end_msgin)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	boolean_t	(*start_dataout)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   unsigned *regp,
				   unsigned value */);

	int		(*restart_datain_1)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*restart_datain_2)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	void		(*restart_datain_3)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*restart_dataout_1)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	int		(*restart_dataout_2)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	int		(*restart_dataout_3)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   unsigned *regp */);

	void		(*restart_dataout_4)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	boolean_t	(*disconn_1)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	boolean_t	(*disconn_2)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

	boolean_t	(*disconn_3)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	boolean_t	(*disconn_4)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	boolean_t	(*disconn_5)(
				/* opaque_t dma_state,
				   target_info_t *tgt,
				   int xferred */);

	void		(*disconn_callback)(
				/* opaque_t dma_state,
				   target_info_t *tgt */);

} scsi_dma_ops_t;

