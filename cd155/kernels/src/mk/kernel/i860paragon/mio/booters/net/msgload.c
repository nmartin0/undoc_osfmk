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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *  Copyright 1991 Intel Corporation.
 *
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mio/booters/net/msgload.c,v 0.5 1994/11/18 20:45:49 mtm Exp $
 *
 * $Log: msgload.c,v $
 * Revision 0.5  1994/11/18  20:45:49  mtm
 * Copyright additions/changes
 *
 * Revision 0.4  1993/06/30  22:40:26  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.3  1993/04/27  20:32:06  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 0.2.6.2  1993/04/22  18:31:50  dleslie
 * First R1_0 release
 *
 * Revision 0.2  1992/07/17  17:42:44  rkl
 * Added support for boot magic and did general cleanup.
 *
 * Revision 1.4  92/03/05  12:12:43  shala
 * Remove rcsid.
 * 
 * Revision 1.3  92/03/05  09:38:00  regnier
 * Add Delta support.
 * 
 * Revision 1.2  92/03/03  09:36:18  shala
 * Add Header and Log markers.
 * 
 *
 *
 */

#include <sys/types.h>
#include "coff.h"

typedef	int (*PFI)();

#define LD_TEXT_DATA 1
#define LD_SYMS 2
#define LD_FINISHED 3

#define INTEL_PGBYTES	4096	/* bytes per page */
#define intel_trunc_page(x)	(((u_int)(x)) & ~(INTEL_PGBYTES-1))
#define intel_round_page(x)	((((u_int)(x)) + INTEL_PGBYTES - 1) & \
						~(INTEL_PGBYTES-1))

#define NOPAGI_TEXT_OFFSET	\
		((ldf_hdr.coff.mainhdr.f_nscns * sizeof(struct scnhdr)) + \
	 	sizeof(struct filehdr) + ldf_hdr.coff.mainhdr.f_opthdr)+0x10

#define NOPAGI_DATA_OFFSET	(NOPAGI_TEXT_OFFSET + ldf_hdr.coff.opthdr.tsize)
#define PAGI_TEXT_OFFSET	0
#define PAGI_DATA_OFFSET	\
			(NOPAGI_TEXT_OFFSET+ldf_hdr.coff.opthdr.tsize) - \
			(ldf_hdr.coff.opthdr.data_start - vm_data_start)


#define MAX_NUM_SECTIONS 3
struct scnhdr	sh[MAX_NUM_SECTIONS];	 /* Section hdrs for TEXT, DATA & BSS */

u_long		vm_data_start, vm_data_end, vm_text_start, vm_text_end,
		vm_bss_start, vm_end;
u_long		section_size, section_offset, xfer_offset, coff_start,sym_start;
int		num_sections, section_num, load_state = LD_TEXT_DATA;

u_char		*load_ptr;
ldf_hdr_t	ldf_hdr;
extern		int	(*start_addr)();

load_data(buf, len)
	u_char	*buf;
	int	len;
{
	extern	int	first_pkt;

	int tlen;
	ldf_hdr_t *ldf_hdr_p;

        if (load_state == LD_FINISHED)
		return (0);

	if (first_pkt) {
	        /*
		 *  Get the coff and optional header
		 */
		bcopy(buf, &ldf_hdr, sizeof(ldf_hdr));

		/*
		 *  Get the section headers
		 */
		num_sections = ldf_hdr.coff.mainhdr.f_nscns;
		if (num_sections > MAX_NUM_SECTIONS)
			num_sections = MAX_NUM_SECTIONS;

		bcopy(buf + (sizeof(ldf_hdr.coff.mainhdr) + 
			    ldf_hdr.coff.mainhdr.f_opthdr),
				&sh[0], sizeof(struct scnhdr)*num_sections);

		/*
		 *  Calculate start end addresses
		 */
		load_ptr =  (u_char *)intel_trunc_page(ldf_hdr.coff.opthdr.text_start);
		vm_text_start	= (u_long)load_ptr;
		vm_text_end	= ldf_hdr.coff.opthdr.text_start +
					      ldf_hdr.coff.opthdr.tsize;
		vm_data_start	= ldf_hdr.coff.opthdr.data_start;
		vm_data_end	= ldf_hdr.coff.opthdr.data_start +
					      ldf_hdr.coff.opthdr.dsize;
		vm_bss_start	= ldf_hdr.coff.opthdr.data_start +
						ldf_hdr.coff.opthdr.dsize;
		vm_end = intel_round_page(ldf_hdr.coff.opthdr.data_start + 
					 ldf_hdr.coff.opthdr.dsize +
					 ldf_hdr.coff.opthdr.bsize);

		printf("\n%d + %d + %d = %d\n", ldf_hdr.coff.opthdr.tsize,
			ldf_hdr.coff.opthdr.dsize, ldf_hdr.coff.opthdr.bsize,
					ldf_hdr.coff.opthdr.tsize +
					ldf_hdr.coff.opthdr.dsize +
					ldf_hdr.coff.opthdr.bsize);

		/*
		 *  Do sanity check
		 */
		if ((vm_text_end > vm_data_start) ||
		    (vm_data_end > vm_bss_start)) {
			printf("COFF sections overlap - start address:\n");
			printf("   text: %x   data: %x   bss: %x\n",
				vm_text_start, vm_data_start, vm_bss_start);
			return (-1);
		}

		section_num = 0;

#ifdef VERBOSE
	printf("sh[%d].s_scnptr:%d\n", section_num, sh[section_num].s_scnptr);
	printf("section name:%5s\n", sh[section_num].s_name);
#endif

		section_offset = PAGI_TEXT_OFFSET;
		section_size = sh[section_num].s_scnptr+ldf_hdr.coff.opthdr.tsize;
		xfer_offset = 0;

#ifdef VERBOSE
	printf("section_size:%d\n", section_size);
        printf("load_ptr=%x xfer_offset:%x\n", load_ptr, xfer_offset);
#endif VERBOSE

		start_addr = (PFI)ldf_hdr.coff.opthdr.entry;

		first_pkt = 0;
		if (len == 0)
			return (0);
	}

	/*
	 *  Move the bytes into memory
	 */
        while(len > 0) {
		/*
		 *
		 * section_offset:	The offset into the executable of the
		 *			section we are interested in.
		 *
		 * section_size:	Number to byte left in the current
		 *			section to copy.
		 *
		 * xfer_offset:		The offset of where we are currently
		 *			in the executable.
		 */
		if (section_offset > xfer_offset) {
			if ((xfer_offset + len) > section_offset) {
				len -= (section_offset - xfer_offset);
				buf += (section_offset - xfer_offset);
				xfer_offset += (section_offset - xfer_offset);
			} else {
				xfer_offset += len;
				len = 0;
				continue;
			}
		}

		tlen = len;
		if (len > section_size) {
			tlen = section_size;
		}

		bcopy(buf, load_ptr, tlen);
		load_ptr += tlen;

		if (load_state == LD_SYMS) {
			vm_end = (u_long)load_ptr;
		}

		buf += tlen;
		len -= tlen;

		section_size -= tlen;
		xfer_offset  += tlen;

		if (load_state == LD_SYMS)
			continue;

		if (section_size == 0) {
			section_num++;

			if (section_num > num_sections) {
				xfer_offset += len;
				len = 0;
				continue;
			}

			load_ptr = (u_char *)sh[section_num].s_vaddr;
			section_offset = sh[section_num].s_scnptr;
			section_size = sh[section_num].s_size;

#ifdef VERBOSE
	printf("sh[%d].s_scnptr:%x\n", section_num, sh[section_num].s_scnptr);
	printf("section name:%5s\n", sh[section_num].s_name);
	printf("section_size:%d\n", section_size);
	printf("load_ptr=%x xfer_offset:%x\n", load_ptr, xfer_offset);
#endif

			if (sh[section_num].s_flags & STYP_BSS) {
				bzero(load_ptr, section_size);
				load_state = LD_SYMS;

				/*
				 *  Set addr to start loading symbols
				 */
				load_ptr = (u_char *)vm_end;

				/*
				 *  Put a copy of the coff header before symbols
				 */
#ifdef VERBOSE
	printf("coff hdr addr:%x\n", load_ptr);
#endif
				ldf_hdr_p = (ldf_hdr_t *)load_ptr;
				bcopy(&ldf_hdr, load_ptr, sizeof(ldf_hdr));

				coff_start = (u_long)load_ptr;
				load_ptr += sizeof(ldf_hdr);
				sym_start = (u_long)load_ptr;

				section_offset = ldf_hdr.coff.mainhdr.f_symptr;
				section_size = 0xffffffff;/* Copy until EOF!! */

				/* 
				 *  Patch up the header to be a memory pointer
				 *  instead of a file pointer.
				 */
				ldf_hdr_p->coff.mainhdr.f_symptr = (long)load_ptr;
#ifdef VERBOSE
	printf("==>Loading Symbols<== offset:%x load_ptr:%x\n",
			section_offset, load_ptr);
#endif		    
			}
		}
	}
	return (0);
}
