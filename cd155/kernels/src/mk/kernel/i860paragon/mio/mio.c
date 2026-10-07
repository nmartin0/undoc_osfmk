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
 * Copyright 1992 by Intel Corporation,
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
 * $Log: mio.c,v $
 * Revision 0.15  1994/11/18  20:45:11  mtm
 * Copyright additions/changes
 *
 * Revision 0.14  1994/07/12  19:20:40  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 0.13  1994/06/15  19:45:57  lenb
 *         cleanup baton debugging code
 *
 * Revision 0.12  1994/03/19  01:14:59  lenb
 * Benefit: ASMP kernel baton_enter/baton_exit
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 0.11  1994/01/28  17:31:40  steved
 *  Reviewer: andyp, leb, sean.
 *  Risk: Low. Only affects DOT's
 *  Benefit or PTS #: Restores the ability to !halt(1) from the debugger
 * 		   and get the monitor prompt. Makes rebooting DOT's
 * 	  	   possible.
 *  Testing: Did it.
 *  Module(s): i860paragon/mio/mio.c
 *
 * Revision 0.10  1993/06/30  22:39:43  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.9  1993/06/09  01:35:48  terry
 * source sync with OSF
 *
 * Revision 0.8  1993/06/05  00:44:56  arlin
 * changed hippi_bd_present() to expansion_id()
 *
 * Revision 0.7  1993/05/27  22:12:18  arlin
 * Added Hippi Driver Support, ARD
 *
 * Revision 0.6  1993/04/27  21:08:27  dleslie
 * Copy of R1.0 onto main trunk
 *
 * Revision 0.5.6.1  1993/04/22  18:31:41  dleslie
 * First R1_0 release
 *
 * Revision 0.6  1993/04/05  01:32:18  andyp
 * Cleaned up a comment or two.
 *
 * Revision 0.5  1993/01/07  18:35:18  andyp
 * Added a boolean that will help the hardware guys look for
 * the cause of spurious interrupts.
 *
 * Revision 0.4  1992/09/11  21:00:10  andyp
 * Added a missing "goto" for one of the long copy cases.
 *
 * Revision 0.3  1992/08/27  17:17:13  andyp
 * Run-time configuration of the MIO LED lightshow.
 *
 * Revision 0.2  92/08/25  09:46:43  andyp
 * Added code to collect statistics on copy sizes and frequency.
 * The data gathered was then used to determine which kinds of
 * routines to implement.  This revision requires the new file
 * i860paragon/mio/miocopy.s.
 * 
 * Revision 0.1  92/07/09  16:43:52  andyp
 * Initial checkin of Paragon files.
 * 
 *
 */

#include <mio.h>
#if	NMIO > 0
#include <asc.h>
#include <mioe.h>
#include <mach_kdb.h>

#include <sys/types.h>
#include <vm/vm_kern.h>
#include <chips/busses.h>
#include <i860/thread.h>
#include <i860/coff.h>
#include <i860/syms.h>
#include <i860/exec.h>
#include <i860paragon/spl.h>
#include <i860paragon/expansion.h>
#include <i860paragon/mio/miodev.h>
#include <i860paragon/mio/mio.h>
#include <i860paragon/mio/flash.h>
#include <i860paragon/dram.h>
#include <i860paragon/baton.h>


extern void	outw();
extern unsigned short	inw();

caddr_t	mio_std[NMIODEVS] = {
	(caddr_t) MIO_BASE_SRAM,
	(caddr_t) MIO_TOPOF_EEPROM
 };
struct bus_device	*mio_dinfo[NMIODEVS];
struct bus_ctlr		*mio_minfo[NMIODEVS];
int	mioprobe(), mioslave(), mioattach(), miointr();

struct bus_driver miodriver = {
	mioprobe,		/* is the mio card attached? */
	mioslave,		/* any devices on the card? */
	mioattach,		/* setup driver after the probe */
	0,			/* start transfer */
	mio_std,		/* device csr addresses */
	"mio",			/* name of the device */
	mio_dinfo,		/* backpointers to init structs */
	"mio",			/* name of a controller */
	mio_minfo,		/* backpointers to init structs */
	0			/* flags */
};

vm_offset_t	mio_map_memory();
static boolean_t	mio_probed_okay;

extern int	interrupt_expansion_install();
static		(*previous_handler)();
static		(*previous_reboot)();
/*static*/ unsigned long	mio_interrupt_enable_shadow;

extern int	show_spurious_interrupts;
int		mio_spurious_interrupts;


/*
 *	Return non-zero if an MIO card is present.
 */
mioprobe(vaddr, ctlr)
	caddr_t		vaddr;
	struct bus_ctlr	*ctlr;
{
        extern int expansion_id();

         /* check for a MIO expansion board type */
	if (expansion_id() != EXP_ID_MIO)
		return 0;

	mio_probed_okay = TRUE;
	mio_interrupt_init();
	mio_reboot_init();
	mio_led_off();

	return 1;
}


/*
 *	Probe for any slave devices.  In this case, the
 *	slaves are the 512K SRAM on the MIO card and the
 *	256K eeprom.
 */
mioslave(device, vaddr)
	struct bus_device	*device;
	caddr_t			vaddr;
{

	switch(device->unit) {
	case  0: return mio_probe_sram(device);
	case  1: return mio_probe_eeprom(device);
		break;
	}

	printf("mioslave: bad unit number (unit==%d)\n", device->unit);
	return 0;

}


/*
 *	Map in the memory on the MIO card.
 */
mioattach(device)
	struct bus_device	*device;
{
	vm_offset_t	vaddr;
	char		*flavor;

	switch (device->unit) {
	case MIO_DEV_SRAM:
		flavor = "sram";
		break;
	case MIO_DEV_FLASH:
		flavor = "eeprom";
		break;
	default:
		flavor = "memory from Mars";
		break;
	}

	vaddr = mio_map_memory(device->phys_address, device->sysdep);
	if (vaddr == 0) {
		return 0;
	}
	device->address = (caddr_t) vaddr;

	mio_dinfo[device->unit] = device;

	printf(" virt 0x%x, phys 0x%x, size %uKB (%s)",
		device->address,
		device->phys_address,
		((int) device->sysdep / 1024),
		flavor);

	return 0;
}


/*
 *	Setup for taking an interrupt from the MIO card.
 */
mio_interrupt_init()
{
	/*
	 *	The shadow and the hardware are 0 at reset.
	 *	However, the com driver (acting as console)
	 *	has already been probed and has already set
	 *	bits in the shadow register.
	 */
	/*mio_interrupt_enable_shadow = 0;*/

	/*
	 *	Install a handler for interrupts from
	 *	an expansion card.
	 */
	if (interrupt_expansion_install(miointr, &previous_handler))
		panic("mio_interrupt_init");

	return 0;

}


/*
 *	Install a function pointer to reload and run
 *	from expansion ROM at reboot time.
 */
mio_reboot_init()
{
	static void	mio_reboot_from_flash();

	if (reboot_expansion_install(mio_reboot_from_flash, &previous_reboot))
		panic("mio_reboot_init");

	return 0;
}


/*
 *	Set bits in the interrupt enable register.
 */
mio_interrupt_enable(bits)
	int	bits;
{
	mio_interrupt_enable_shadow |= bits;
	outw(MIO_INTERRUPT_CONTROL, mio_interrupt_enable_shadow);

	return bits;
}


/*
 *	Clear bits in the interrupt enable register.
 */
mio_interrupt_disable(bits)
	int	bits;
{
	mio_interrupt_enable_shadow &= ~bits;
	outw(MIO_INTERRUPT_CONTROL, mio_interrupt_enable_shadow);

	return bits;
}


/*
 *	Return the shadow of the interrupt enable register.
 */
unsigned long mio_interrupt_register_shadow()
{
	return mio_interrupt_enable_shadow;
}


/*
 *	Dispatch an interrupt from the MIO card.
 */
miointr(regs)
	register struct i860_saved_state *regs;
{
	unsigned short	status, enable;
	int	s = -1;

	status = inw(MIO_INTERRUPT_STATUS);
	enable = mio_interrupt_enable_shadow;

	if ((status & MIO_INTPENDING_SERIAL)
		&& (enable & MIO_INTENABLE_SERIAL)) {
		s = splmio_noi();
		baton_enter();
		com_intr(0);
		baton_exit();
		return s;
	}
#if	NASC > 0
	if ((~status & MIO_INTPENDING_SCSI)		/* active low */
		&& (enable & MIO_INTENABLE_SCSI)) {
		s = splmio_noi();
		baton_enter();
		asc_intr(0);
		baton_exit();
		return s;
	}
#endif
#if	NMIOE > 0
	if ((status & MIO_INTPENDING_ETHER)
		&& (enable & MIO_INTENABLE_ETHER)) {
		s = splmio_noi();
		baton_enter();
		mioe_intr(0);
		baton_exit();
		return s;
	}
#endif
#if	0
	if ((status & MIO_INTPENDING_PARITY)
		&& (enable & MIO_INTENABLE_PARITY)) {
		mio_parity_error_interrupt(status);
		panic("miointr: mio parity error");
		return s;
	}
#endif	0

	mio_spurious_interrupts++;
	{
		extern int	la_trigger_spurious;	/* XXX */

		if (la_trigger_spurious)		/* XXX */
			inl(0x61300000);		/* XXX */

	}
	if (show_spurious_interrupts) {
		printf("miointr: spurious interrupt: status=0x%04x, enable=0x%04x\n",
			status, enable);
	}

	return s;
}


/*
 *	Map in a chunk of MIO memory
 */
vm_offset_t mio_map_memory(paddr, len)
	vm_offset_t		paddr;
	vm_size_t		len;
{
	vm_offset_t		vaddr, v;
	extern vm_offset_t	pmap_map_bd();

	if (!mio_probed_okay)
		return 0;

	paddr = (vm_offset_t) trunc_page(paddr);
	len = (vm_size_t) round_page(len);

	/*
	 *	No, this isn't really pageable...this call
	 *	simply carves out a chunk of address space that
	 *	will very shortly be filled by physical
	 *	memory on the MIO card.
	 */
	if (kmem_alloc_pageable(kernel_map, &vaddr, len) != KERN_SUCCESS) {
		return (vm_offset_t) 0;
	}

	/*
	 *	Enter the physical pages into the kernel map.
	 *	pmap_map_bd() marks the pages as referenced,
	 *	modified, and cache-disable.
	 */
	(void) pmap_map_bd(vaddr, paddr, paddr + len, VM_PROT_WRITE);
	for (v = vaddr; v < v + len; v += PAGE_SIZE) {
		pmap_change_wiring(kernel_pmap, v, TRUE);
	}

	return vaddr;
}


/*
 *	Probe and autosize the static ram on the MIO card.
 *
 *	XXX doesn't autosize yet.
 *	XXX doesn't probe either.
 */
static int mio_probe_sram(device)
	struct bus_device	*device;
{
	return 1;
}


/*
 *	Probe and autosize the onboard Flash(tm) EEPROM.
 *
 *	XXX doesn't autosize yet.
 *	XXX doesn't probe either.
 */
static int mio_probe_eeprom(device)
	struct bus_device	*device;
{
	return 1;
}


/*
 *	Shut down the software and as much of the CPU as possible;
 *	copyout the rom from the MIO card and call it.
 *
 *	XXX The danger is that the current kernel stack that we're
 *	XXX running on will be occupied by the flash.
 *
 *	XXX CAN ONLY ACCESS FLASH VIA 16-bit OR 8-bit LOADS
 */
static void mio_reboot_from_flash()
{
	char	*src, *dst, *flash;
	char	*bss, *entry;
	struct filehdr	fh;
	struct scnhdr	sh, *scn;
	struct aouthdr	ah;
	int	i, offset;

	flash = (char *) (mio_dinfo[MIO_DEV_FLASH]->address);

	miocopy((char *) flash, &fh, sizeof(fh));
	if (fh.f_magic != I860MAGIC)
		return;

	miocopy((char *) flash + sizeof(fh), &ah, sizeof(ah));

	/*
	 *	Copy out each section.
	 */
	offset = sizeof(fh) + fh.f_opthdr;
	scn = (struct scnhdr *) (flash + offset);
	for (i = 0; i < fh.f_nscns; i++, scn++) {
		miocopy(scn, &sh, sizeof(sh));
		if (sh.s_flags & (STYP_TEXT|STYP_DATA)) {
			src = flash + sh.s_scnptr;
			dst = (char *) sh.s_vaddr;
			dst = (char *)((long)dst & ~(0x0F << 24) |
				 DRAM_HIGH_ALIAS);
			miocopy(src, dst, sh.s_size);
		} else if (sh.s_flags & STYP_BSS) {
			dst = (char *) sh.s_vaddr;
			dst = (char *)((long)dst & ~(0x0F << 24) |
				 DRAM_HIGH_ALIAS);
			bzero(dst, sh.s_size);
		} else {
			/* we don't get it... */
		}
	}

	/*
	 *	Flush the caches, disable address translation,
	 *	set the stack pointer to 0xc000ffc0, and
	 *	call the MIO bootstrap with a single parameter of -1.
	 */
	mio_ether_reset(0);
	flush();
	printf("flushed and rebooting...\n");
	outl(0xc0000ffc, node_control_register_read());
	i860_reboot(ah.entry, 0xc0000ff0, 0);
}


#define	MIOCOPYSTATS	1
#if	MIOCOPYSTATS
int	mio_collect_copystats;		/* enable at run-time */
unsigned long	mio_copystat_calls;
unsigned long	mio_copystat_total;
unsigned long	mio_copystat_large;
unsigned long	mio_copystat_copyins;
unsigned long	mio_copystat_quad_copyin_hits;
unsigned long	mio_copystat_quad_copyout_hits;
unsigned long	mio_copystat_long_copyin_hits;
unsigned long	mio_copystat_long_copyout_hits;
unsigned long	mio_copystat_byte_copy_hits;
unsigned long	mio_copystat_word_copy_hits;
unsigned long	mio_copystat_no_hits;
unsigned long	mio_copystat_copyouts;
unsigned long	mio_copystat_moves;
unsigned long	mio_copystat_bucket[32];
unsigned long	mio_copystat_null;
unsigned long	mio_copystat_unaligned_src;
unsigned long	mio_copystat_unaligned_dst;
unsigned long	mio_copystat_unaligned_cnt;

#if	MACH_KDB
void db_show_mio_copystats()
{
	int	i;

	if (!mio_collect_copystats)
		db_printf("miocopy() stats are disabled.\n");
	db_printf("%8u total calls\n", mio_copystat_calls);
	db_printf("%8u total bytes\n", mio_copystat_total);
	db_printf("%8u total copyins (mio->dram)\n", mio_copystat_copyins);
	db_printf("%8u total copyouts (dram->mio)\n", mio_copystat_copyouts);
	db_printf("%8u total moves (mio->mio)\n", mio_copystat_moves);
	db_printf("%8u total byte copy hits\n", mio_copystat_byte_copy_hits);
	db_printf("%8u total word copy hits\n", mio_copystat_word_copy_hits);
	db_printf("%8u total long copyin hits\n", mio_copystat_long_copyin_hits);
	db_printf("%8u total long copyout hits\n", mio_copystat_long_copyout_hits);
	db_printf("%8u total quad copyin hits\n", mio_copystat_quad_copyin_hits);
	db_printf("%8u total quad copyout hits\n", mio_copystat_quad_copyout_hits);
	db_printf("%8u big copies (> %dK)\n", mio_copystat_large, 1<<16);
	db_printf("%8u zero-length copies\n", mio_copystat_null);
	db_printf("%8u src misaligned\n", mio_copystat_unaligned_src);
	db_printf("%8u dst misaligned\n", mio_copystat_unaligned_dst);
	db_printf("%8u cnt misaligned\n", mio_copystat_unaligned_cnt);
	for (i = 16; i >= 0; i--) {
		db_printf("%8u copies >= %6u\n", mio_copystat_bucket[i],
			1 << i);
	}
}
#endif


static int ilog2(i)
unsigned long	i;
{
	int	b = -1;

	if (!i) return -1;
	if (i & 0xffff0000) b += 16; else i <<= 16;
	if (i & 0xff000000) b +=  8; else i <<=  8;
	if (i & 0xf0000000) b +=  4; else i <<=  4;
	if (i & 0xc0000000) b +=  2; else i <<=  2;
	if (i & 0x80000000) b +=  1; else i <<=  1;

	return b + 1;
}
#endif	MIOCOPYSTATS

int	mio_copy_light_show = 0;

/*
 *	copy to/from MIO static ram, count is number of bytes
 *
 */
miocopy(src, dst, cnt)
	char*	src;
	char*	dst;
	int	cnt;
{
	char	*sram = (mio_dinfo[MIO_DEV_SRAM]->address);	/* XXX */
	int	copyin, copyout, move;

	copyin = (src >= sram) && (src < (sram + (512 * 1024)));
	copyout = (dst >= sram) && (dst < (sram + (512 * 1024)));
	if ((move = copyin && copyout)) {
		copyin = 0;
		copyout = 0;
	}
#if	MIOCOPYSTATS
	if (mio_collect_copystats) {
		mio_copystat_calls++;
		mio_copystat_total += cnt;
		if (copyin)
			mio_copystat_copyins++;		/* mio->dram*/
		if (copyout)
			mio_copystat_copyouts++;	/* dram->mio */
		if (move)
			mio_copystat_moves++;		/* mio->mio */
		{
			int	b;

			b = ilog2(cnt);
			if (b > -1)
				mio_copystat_bucket[b]++;
			else
				mio_copystat_null++;
			if (b > 16)
				mio_copystat_large++;
		}
		if ((long) src & 1)
			mio_copystat_unaligned_src++;
		if ((long) dst & 1)
			mio_copystat_unaligned_dst++;
		if (cnt & 1)
			mio_copystat_unaligned_cnt++;
	}
#endif	MIOCOPYSTATS

	if (mio_copy_light_show)
		mio_led_on();
	if (cnt == 6) {
		/*
		 *	ethernet address traffic to/from the card.
		 */
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];
		dst[4] = src[4];
		dst[5] = src[5];
		goto copydone;
	}

	/*
	 *	*must* check for the misaligned cases first
	 */
	if (((int) src & 1) || ((int) dst & 1)) {
		/* misaligned case */
#if	MIOCOPYSTATS
		if (mio_collect_copystats)
			mio_copystat_byte_copy_hits++;
#endif	MIOCOPYSTATS
		mio_byte_copy(src, dst, cnt);
		goto copydone;
	}
	if (cnt & 1) {
		/* non-multiple case */
#if	MIOCOPYSTATS
		if (mio_collect_copystats)
			mio_copystat_byte_copy_hits++;
#endif	MIOCOPYSTATS
		mio_byte_copy(src, dst, cnt);
		goto copydone;
	}

	if (copyin) {
		/*
		 *	can we do 16-bit loads and 128-bit stores?
		 *	(src is known to be 2-byte aligned)
		 */
		if (((cnt & 0xf) == 0) && (((int) dst & 0xf) == 0)) {
#if	MIOCOPYSTATS
			if (mio_collect_copystats)
				mio_copystat_quad_copyin_hits++;
#endif	MIOCOPYSTATS
			mio_quad_copyin(src, dst, cnt >> 4);
			goto copydone;
		}
		/*
		 *	okay, try 16-bit loads and 32-bit stores...
		 */
		if (((cnt & 0x3) == 0) && (((int) dst & 0x3) == 0)) {
#if	MIOCOPYSTATS
			if (mio_collect_copystats)
				mio_copystat_long_copyin_hits++;
#endif	MIOCOPYSTATS
			mio_long_copyin(src, dst, cnt >> 2);
			goto copydone;
		}
	} else if (copyout) {
		/*
		 *	can we do 128-bit loads and 16-bit stores?
		 *	(dst is known to be 2-byte aligned)
		 */
		if (((cnt & 0xf) == 0) && (((int) src & 0xf) == 0)) {
#if	MIOCOPYSTATS
			if (mio_collect_copystats)
				mio_copystat_quad_copyout_hits++;
#endif	MIOCOPYSTATS
			mio_quad_copyout(src, dst, cnt >> 4);
			goto copydone;
		}
		/*
		 *	okay, try 32-bit loads and 16-bit stores...
		 */
		if (((cnt & 0x3) == 0) && (((int) src & 0x3) == 0)) {
#if	MIOCOPYSTATS
			if (mio_collect_copystats)
				mio_copystat_long_copyout_hits++;
#endif	MIOCOPYSTATS
			mio_long_copyout(src, dst, cnt >> 2);
			goto copydone;
		}
	}

	/*
	 *	can we do 16-bit loads and 16-bit stores?
	 */
	if ((cnt & 1) == 0) {
#if	MIOCOPYSTATS
		if (mio_collect_copystats)
			mio_copystat_word_copy_hits++;
#endif	MIOCOPYSTATS
		/*mio_word_copy((short *) src, (short *) dst, cnt >> 1);*/
		scopy((short *) src, (short *) dst, cnt >> 1);
		goto copydone;
	}

	/*
	 *	give up -- just copy the bytes
	 */
#if	MIOCOPYSTATS
	if (mio_collect_copystats) {
		mio_copystat_byte_copy_hits++;
		mio_copystat_no_hits++;
	}
#endif	MIOCOPYSTATS
	mio_byte_copy(src, dst, cnt);

copydone:
	if (mio_copy_light_show)
		mio_led_off();
	return (cnt);
}


#if	0
mio_word_copy(src, dst, cnt)
register short	*src, *dst;
register int	cnt;
{
	register short	t0, t1, t2, t3;

	while (cnt >= 4) {
		t0 = src[0];
		t1 = src[1];
		t2 = src[2];
		t3 = src[3];
		src += 4;
		dst[0] = t0;
		dst[1] = t1;
		dst[2] = t2;
		dst[3] = t3;
		dst += 4;
		cnt -= 4;
	}
	while (cnt-- > 0)
		*dst++ = *src++;
}


mio_byte_copy(src, dst, cnt)
register char	*src, *dst;
register int	cnt;
{
	register char	t0, t1, t2, t3;

	while (cnt >= 4) {
		t0 = src[0];
		t1 = src[1];
		t2 = src[2];
		t3 = src[3];
		src += 4;
		dst[0] = t0;
		dst[1] = t1;
		dst[2] = t2;
		dst[3] = t3;
		dst += 4;
		cnt -= 4;
	}
	while (cnt-- > 0)
		*dst++ = *src++;
}
#endif	0


/*
 *	16-bit zero, count is number of bytes
 *
 *	XXX could be made much faster in the future.
 */
miozero(dst, count)
	char*	dst;
	int	count;
{
	register short	*s_dst;
	register int	i = count;
		 int	res;

	if ((long)dst & 1) {
			*dst++ = 0;
			i--;
	}

	s_dst = (short*)dst;
	res = (i & 1);
	i >>= 1;
	while(i--)
		*s_dst++ = 0;
	if (res)
		*((char*)s_dst) = 0;

	return (count);
}


/*
 *	On the MIO card, iOUT2 from the i82510 is connected
 *	to the i82586.  To reset the ethernet chip, drop
 *	iOUT2, wait 10us, raise iOUT2.
 */
mio_ether_reset(unit)
int	unit;
{
	int	s;

	com_drop_out2(unit);
	s = sploff();
	delay(10);
	splon(s);
	com_raise_out2(unit);
}


/*
 * get_ether_addr:
 *
 *	Read FLASH EPROM to get factory set Ethernet address.
 */
/* ARGSUSED */
void get_ether_addr(unit, addr_p)
	int	unit;
	u_char	*addr_p;
{
	mio_flash_t	flash;
	u_char		*ether;
	int		i;

	flash = (mio_flash_t) (mio_dinfo[MIO_DEV_FLASH]->address);
	ether = &flash->boot_blk.bd_specific.ether_addr[0];

	for (i = 0; i < 6; i++)
		*addr_p++ = *ether++;
}


int	mio_led_state = 0;

/*
 *	turn on the led on the card
 */
mio_led_on()
{
	outb(MIO_LED, (mio_led_state = 0));
}


/*
 *	turn off the led on the card
 */
mio_led_off()
{
	outb(MIO_LED, (mio_led_state = 2));
}


/*
 *	toggle the state of the led on the card
 */
mio_led_not()
{
	outb(MIO_LED, (mio_led_state = 2 - mio_led_state));
}


/*
 *	return the state of the led on the card
 *
 *	(1 == led is on, 0 == led is off)
 */
mio_led_get()
{
	return !mio_led_state;
}


#endif	NMIO > 0
