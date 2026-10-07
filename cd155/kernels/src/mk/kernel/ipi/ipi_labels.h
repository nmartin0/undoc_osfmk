/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
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
 * $Log: ipi_labels.h,v $
 * Revision 1.2  1994/11/18  20:50:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:31  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_labels.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Definitions of various vendor's disk label formats.
 */

#ifndef	_IPI_LABELS_H_
#define	_IPI_LABELS_H_

/*
 * This function looks for, and converts to BSD format a vendor's label.
 * It is only called if we did not find a standard BSD label on the disk.
 */
extern boolean_t	ipi_vendor_label();


/*
 * Definition of the DEC disk label, which is located (you guessed it)
 * at the end of the 4.3 superblock.
 */

struct dec_partition_info {
	unsigned int	n_sectors;	/* how big the partition is */
	unsigned int	offset;		/* sector no. of start of part. */
};

typedef struct {
	int	magic;
#	define	DEC_LABEL_MAGIC		0x032957
	int	in_use;
	struct  dec_partition_info partitions[8];
} ipi_dec_label_t;

/*
 * Physical location on disk.
 * This is independent of the filesystem we use, although of course we'll
 * be in trouble if we screwup the 4.3 SBLOCK..
 */

#define	DEC_LABEL_BYTE_OFFSET	((2 * 8192) - sizeof(ipi_dec_label_t))

/*
 * Definitions for the primary boot information
 * This is common, because the prom knows it.
 */

typedef struct {
	int		pad[2];
	unsigned int	magic;
#	define		DEC_BOOT0_MAGIC	0x2757a
	int		mode;
	unsigned int	phys_base;
	unsigned int	virt_base;
	unsigned int	n_sectors;
	unsigned int	start_sector;
} ipi_dec_boot0_t;


/*
 * Definition of the Omron disk label,
 * which is located at sector 0. It
 * _is_ sector 0, actually.
 */
struct omron_partition_info {
	unsigned long	offset;
	unsigned long	n_sectors;
};

typedef struct {
	char		packname[128];	/* in ascii */

	char		pad[512-(128+8*8+11*2+4)];

	unsigned short	badchk;	/* checksum of bad track */
	unsigned long	maxblk;	/* # of total logical blocks */
	unsigned short	dtype;	/* disk drive type */
	unsigned short	ndisk;	/* # of disk drives */
	unsigned short	ncyl;	/* # of data cylinders */
	unsigned short	acyl;	/* # of alternate cylinders */
	unsigned short	nhead;	/* # of heads in this partition */
	unsigned short	nsect;	/* # of 512 byte sectors per track */
	unsigned short	bhead;	/* identifies proper label locations */
	unsigned short	ppart;	/* physical partition # */
	struct omron_partition_info
			partitions[8];

	unsigned short	magic;	/* identifies this label format */
#	define	OMRON_LABEL_MAGIC	0xdabe

	unsigned short	cksum;	/* xor checksum of sector */

} ipi_omron_label_t;

/*
 * Physical location on disk.
 */

#define	OMRON_LABEL_BYTE_OFFSET	0


/*
 * Definition of the i386AT disk label, which lives inside sector 0.
 * This is the info the BIOS knows about, which we use for bootstrapping.
 * It is common across all disks known to BIOS.
 */

struct bios_partition_info {

	unsigned char	bootid;	/* bootable or not */
#	define BIOS_BOOTABLE	128

	unsigned char	beghead;/* beginning head, sector, cylinder */
	unsigned char	begsect;/* begcyl is a 10-bit number. High 2 bits */
	unsigned char	begcyl;	/*     are in begsect. */

	unsigned char	systid;	/* filesystem type */
#	define	UNIXOS		99

	unsigned char	endhead;/* ending head, sector, cylinder */
	unsigned char	endsect;/* endcyl is a 10-bit number.  High 2 bits */
	unsigned char	endcyl;	/*     are in endsect. */

	unsigned long	offset;
	unsigned long	n_sectors;
};

typedef struct {
#if	0
	/* bogus - compiler alignes wrong */
	struct bios_partition_info	partitions[4];
#else
	char		partitions[4*sizeof(struct bios_partition_info)];
#endif
	unsigned short	magic;
#	define	BIOS_LABEL_MAGIC	0xaa55
} ipi_bios_label_t;

/*
 * Physical location on disk.
 */

#define	BIOS_LABEL_BYTE_OFFSET	446

/*
 * Definitions for the primary boot information
 * This _is_ block 0
 */

#define	BIOS_BOOT0_SIZE	BIOS_LABEL_BYTE_OFFSET

typedef struct {
	char			boot0[BIOS_BOOT0_SIZE];	/* boot code */
#if	0
	/* bogus - compiler alignes wrong */
	ipi_bios_label_t	label;
#else
	char			label[sizeof(ipi_bios_label_t)];
#endif
} ipi_bios_boot0_t;

#endif	_IPI_LABELS_H_
