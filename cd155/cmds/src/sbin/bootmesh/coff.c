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
 
#include 	<sys/types.h>
#include 	"bootmesh.h"
#include 	"coff.h"
#include	"nic.h"

/*
 * Download the microkernel.
 * 	Handle the complexities of transferring a runnable executable,
 * 	by emulating the work of translating a coff-format file into
 * 	a memory image.
 */

typedef	int (*PFI)();


#define NOPAGI_TEXT_OFFSET	\
		((ldf_hdr.coff.mainhdr.f_nscns * sizeof(struct scnhdr)) + \
	 	sizeof(struct filehdr) + ldf_hdr.coff.mainhdr.f_opthdr)+0x10

#define NOPAGI_DATA_OFFSET	\
		(NOPAGI_TEXT_OFFSET + ldf_hdr.coff.opthdr.tsize)
#define PAGI_TEXT_OFFSET	0
#define PAGI_DATA_OFFSET	\
		(NOPAGI_TEXT_OFFSET+ldf_hdr.coff.opthdr.tsize) - \
		(ldf_hdr.coff.opthdr.data_start - vm_data_start)


#define	MAX_NUM_SECTIONS 	3
struct scnhdr	sh[MAX_NUM_SECTIONS];	 /* Section hdrs for TEXT, DATA & BSS */

u_long		vm_data_start, vm_data_end, vm_text_start, vm_text_end,
		vm_bss_start, vm_end;
u_long		section_size, section_offset, xfer_offset, coff_start,sym_start;
static int		num_sections, section_num, load_state = LD_TEXT_DATA;

u_char		*load_ptr;
ldf_hdr_t	ldf_hdr;
int		(*start_addr)();

download_kernel(nodes, buf, len, first_pkt)
	nodetype	*nodes;
	u_char		*buf;
	int		len;
	int		first_pkt;
{
	int tlen;
	ldf_hdr_t *ldf_hdr_p;

#ifdef DEBUG
DBG2("download_kernel: first_pkt = %d\n",first_pkt);
#endif DEBUG
        if (load_state == LD_FINISHED)
		return (0);

	if (first_pkt) {
		load_state = LD_TEXT_DATA;
#ifdef todo
/* assure that first_pkt is never smaller than coff headers  */
#endif todo
	        /*
		 *  Get the coff and optional header
		 */
		bcopy(buf, &ldf_hdr, sizeof(ldf_hdr));
#ifdef DEBUG
DBG2("download_kernel: opthdr = %d\n",ldf_hdr.coff.mainhdr.f_opthdr);
#endif DEBUG

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
		load_ptr	 = (u_char *)
			intel_trunc_page(ldf_hdr.coff.opthdr.text_start);
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

#ifdef DEBUG
DBG("download_kernel: tsize %d(0x%x) + dsize %d(0x%x) + bsize %d(0x%x) = %d(0x%x) \n",
	ldf_hdr.coff.opthdr.tsize, ldf_hdr.coff.opthdr.tsize,
	ldf_hdr.coff.opthdr.dsize, ldf_hdr.coff.opthdr.dsize,
	ldf_hdr.coff.opthdr.bsize, ldf_hdr.coff.opthdr.bsize,
	ldf_hdr.coff.opthdr.tsize + ldf_hdr.coff.opthdr.dsize +
	ldf_hdr.coff.opthdr.bsize,
	ldf_hdr.coff.opthdr.tsize + ldf_hdr.coff.opthdr.dsize +
	ldf_hdr.coff.opthdr.bsize);
#endif DEBUG

		/*
		 *  Do sanity check
		 */
		if ((vm_text_end > vm_data_start) ||
		    (vm_data_end > vm_bss_start)) {
			fprintf(stderr,"\ttext: %x   data: %x   bss: %x\n",
				vm_text_start, vm_data_start, vm_bss_start);
			ERR("COFF sections overlap - start address\n");
		}

		section_num = 0;

#ifdef DEBUG
DBG2("download_kernel: section name:%5s\n", sh[section_num].s_name);
DBG2("download_kernel: sh[%d].s_scnptr:%d\n",
	section_num, sh[section_num].s_scnptr);
#endif DEBUG

		section_offset = PAGI_TEXT_OFFSET;
		section_size = sh[section_num].s_scnptr +
			ldf_hdr.coff.opthdr.tsize;
		xfer_offset = 0;

#ifdef DEBUG
DBG2("download_kernel: section_size:%d\n", section_size);
DBG2("download_kernel: load_ptr=%x xfer_offset:%x\n",load_ptr,xfer_offset);
#endif DEBUG

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

		/*
 		 * download selected buffer contents
		 */
		if (nic_buf_send(nodes, buf, tlen, load_ptr) < 0)
			return(-1);

	
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

#ifdef DEBUG
DBG2("download_kernel: sh[%d].s_scnptr:%x\n",
	section_num, sh[section_num].s_scnptr);
DBG2("download_kernel: section name:%5s\n", sh[section_num].s_name);
DBG2("download_kernel: section_size:%d\n", section_size);
DBG2("download_kernel: load_ptr=%x xfer_offset:%x\n", load_ptr, xfer_offset);
#endif DEBUG

			if (sh[section_num].s_flags & STYP_BSS) {
				/*
				 * clear selected memory contents
				 */
				if (boot_kernel != boot_sunmos_kernel){
					if (nic_zero_send(nodes, section_size,
						load_ptr) < 0)
					    return(-1);
				}

				load_state = LD_SYMS;

				/*
				 *  Set addr to start loading symbols
				 */
				load_ptr = (u_char *)vm_end;

				/*
				 *  Put a copy of the coff header before symbols
				 */
#ifdef DEBUG
DBG("download_kernel: loading coff header to addr:%x\n", load_ptr);
#endif DEBUG

				section_offset = ldf_hdr.coff.mainhdr.f_symptr;
				section_size = 0xffffffff;/* Copy until EOF!! */

				/* 
				 *  Patch up the header to be a memory pointer
				 *  instead of a file pointer.
				 */
				ldf_hdr.coff.mainhdr.f_symptr =
				     ((long)load_ptr + sizeof(ldf_hdr));

				/*
 				 * download header and record location
				 */
				if (nic_buf_send(nodes, &ldf_hdr,
					 sizeof(ldf_hdr), load_ptr) < 0)
						return(-1);

				coff_start = (u_long)load_ptr;
				load_ptr += sizeof(ldf_hdr);
				sym_start = (u_long)load_ptr;

#ifdef DEBUG
DBG("download_kernel: loading symbols offset:%x load_ptr:%x\n",
    	section_offset, load_ptr);
#endif DEBUG
			}
		}
	}
	return (0);
}
