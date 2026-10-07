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
 * $Log: rz_labels.c,v $
 * Revision 1.7  1995/03/14  23:48:32  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.6  1994/11/18  20:59:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:53:41  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:47:49  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:16  dleslie
 * First R1_0 release
 *
 * Revision 2.3.2.2  92/04/30  12:00:40  bernadat
 * 	Changes from TRUNK:
 * 		Let "grab_bob_label" return failure so we take the default.
 * 		[92/04/01            rvb]
 * 
 * Revision 2.3.2.1  92/03/28  10:15:38  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:32:14  jeffreyh]
 * 
 * Revision 2.4  92/02/23  22:44:31  elf
 * 	Actually, scan for all possible (bogus) label formats.
 * 	This makes it possible to cross-mount disks even if
 * 	they do not have the standard BSD label.
 * 	[92/02/22            af]
 * 
 * Revision 2.3  91/08/24  12:28:06  af
 * 	Created, splitting out DEC-specific code from rz_disk.c and
 * 	adding some more.
 * 	[91/06/26            af]
 * 
 */
/*
 *	File: rz_labels.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	6/91
 *
 *	Routines for various vendor's disk label formats.
 */

#include <platforms.h>

#include <mach/std_types.h>
#include <scsi/compat_30.h>
#include <scsi/scsi_defs.h>
#include <scsi/rz_labels.h>

/*
 * Find and convert from a DEC label to BSD
 */
boolean_t
rz_dec_label(tgt, label, ior)
	target_info_t		*tgt;
	struct disklabel	*label;
	io_req_t		ior;	
{
	/* here look for a DEC label */
	register scsi_dec_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op    = IO_READ | IO_INTERNAL;
	ior->io_error = 0;

	/*
	 * If the controller supports scatter/gather set the io_op flag
	 */
	if (scsi_softc[tgt->masterno]->supports_sgio)
		ior->io_op |= IO_SGLIST;

	tgt->ior = ior;
	scdisk_read( tgt, DEC_LABEL_BYTE_OFFSET/tgt->block_size, ior);
	iowait(ior);

	if (ior->io_op & IO_ERROR)
		return FALSE;

	data = (char *)ior->io_data;
	part = (scsi_dec_label_t*)&data[DEC_LABEL_BYTE_OFFSET%tgt->block_size];
	if (part->magic == DEC_LABEL_MAGIC) {
		if (scsi_debug)
		printf("{Using DEC label}");
		for (i = 0; i < 8; i++) {
			label->d_partitions[i].p_size =
				part->partitions[i].n_sectors;
			label->d_partitions[i].p_offset =
				part->partitions[i].offset;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * Find and convert from a Omron label to BSD
 */
boolean_t
rz_omron_label(tgt, label, ior)
	target_info_t		*tgt;
	struct disklabel	*label;
	io_req_t		ior;	
{
	/* here look for an Omron label */
	register scsi_omron_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op    = IO_READ | IO_INTERNAL;
	ior->io_error = 0;

	/*
	 * If the controller supports scatter/gather set the io_op flag
	 */
	if (scsi_softc[tgt->masterno]->supports_sgio)
		ior->io_op |= IO_SGLIST;

	tgt->ior = ior;
	scdisk_read( tgt, OMRON_LABEL_BYTE_OFFSET/tgt->block_size, ior);
	iowait(ior);

	if (ior->io_op & IO_ERROR)
		return FALSE;

	data = (char *)ior->io_data;
	part =
	    (scsi_omron_label_t*)&data[OMRON_LABEL_BYTE_OFFSET%tgt->block_size];
	if (part->magic == OMRON_LABEL_MAGIC) {
		if (scsi_debug)
		printf("{Using OMRON label}");
		for (i = 0; i < 8; i++) {
			label->d_partitions[i].p_size =
				part->partitions[i].n_sectors;
			label->d_partitions[i].p_offset =
				part->partitions[i].offset;
		}
		bcopy(part->packname, label->d_packname, 16);
		label->d_ncylinders = part->ncyl;
		label->d_acylinders = part->acyl;
		label->d_ntracks = part->nhead;
		label->d_nsectors = part->nsect;
		/* Many disks have this wrong, therefore.. */
#if 0
		label->d_secperunit = part->maxblk;
#else
		label->d_secperunit = label->d_ncylinders *
				      label->d_ntracks	  *
				      label->d_nsectors;
#endif
		return TRUE;
	}
	return FALSE;
}

/*
 * Find and convert from a Intel BIOS label to BSD
 */
boolean_t
rz_bios_label(tgt, label, ior)
	target_info_t		*tgt;
	struct disklabel	*label;
	io_req_t		ior;	
{
	/* here look for a BIOS label */
	register scsi_bios_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op    = IO_READ | IO_INTERNAL;
	ior->io_error = 0;

	/*
	 * If the controller supports scatter/gather set the io_op flag
	 */
	if (scsi_softc[tgt->masterno]->supports_sgio)
		ior->io_op |= IO_SGLIST;

	tgt->ior = ior;
	scdisk_read( tgt, BIOS_LABEL_BYTE_OFFSET/tgt->block_size, ior);
	iowait(ior);

	if (ior->io_op & IO_ERROR)
		return FALSE;

	data = (char *)ior->io_data;
	part =
	    (scsi_bios_label_t*)&data[BIOS_LABEL_BYTE_OFFSET%tgt->block_size];
	if (part->magic == BIOS_LABEL_MAGIC) {
		struct bios_partition_info *bpart;
		if (scsi_debug)
		printf("{Using BIOS label}");
		bpart = (struct bios_partition_info *)part->partitions;
		for (i = 0; i < 4; i++) {
			label->d_partitions[i].p_size = bpart[i].n_sectors;
			label->d_partitions[i].p_offset = bpart[i].offset;
		}
#ifdef	AT386	/* this goes away real fast */
		if (!grab_bob_label(tgt, label, ior, bpart))
			return FALSE;	/* take default setup of "a" and "c" */
#else
		label->d_npartitions = 4;
#endif
		label->d_bbsize = BIOS_BOOT0_SIZE;
		return TRUE;
	}
	return FALSE;
}

#ifdef        iPSC386
/* 
 * Code lives in i386ipsc/disk_label.c for now. Might someday move to here 
 * rz_ipsc_label will always return true as it is hard coded.
 *
*/
extern        boolean_t rz_ipsc_label();
#define       VENDOR_LABEL   rz_ipsc_label
#endif        iPSC386

/*
 * Try all of the above
 */
boolean_t rz_vendor_label(tgt, label, ior)
	target_info_t		*tgt;
	struct disklabel	*label;
	io_req_t		ior;	
{
	/* If for any reason there might be problems someday.. */
#ifdef	VENDOR_LABEL
	if (VENDOR_LABEL( tgt, label, ior)) return TRUE;
	if (ior->io_op & IO_ERROR) return FALSE;
#endif	/*VENDOR_LABEL*/

#ifndef	PARAGON860
	if (rz_dec_label( tgt, label, ior)) return TRUE;
	if (ior->io_op & IO_ERROR) return FALSE;

	if (rz_bios_label( tgt, label, ior)) return TRUE;
	if (ior->io_op & IO_ERROR) return FALSE;

	if (rz_omron_label( tgt, label, ior)) return TRUE;
#endif	PARAGON860

	return FALSE;
}
