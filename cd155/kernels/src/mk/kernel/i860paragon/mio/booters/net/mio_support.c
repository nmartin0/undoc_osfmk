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
 *  Support routines for the MIO card
 */
#include <sys/types.h>
#include "./mio.h"

miocopy(src, dst, count)
	char*	src;
	char*	dst;
	int	count;
{
	register short	*s_src;
	register short	*s_dst;
	register int	i = count;
		 int	res;

	/*
	 *  XXX Check for both address being odd and adjust for word transfer.
	 */
	if (((long)dst & 1) || ((long)src & 1)) {
		printf("mioecopy miss!!! src = %x dst = %x\n", src, dst);
		while(i--)
			*dst++ = *src++;
	} else {
		res = (i & 1);
		i /= 2;
		s_src = (short*)src;
		s_dst = (short*)dst;

		while(i--)
			*s_dst++ = *s_src++;
		if (res)
			*((char*)s_dst) = *((char*)s_src);
	}

	return (count);
}

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
	i /= 2;
	while(i--)
		*s_dst++ = 0;
	if (res)
		*((char*)s_dst) = 0;

	return (count);
}

/*
 *  Spin loop routine with microsecond granularity.  For realatively long
 *  times (i.e. > 5 usec) this will be pretty accurate.  It will probably
 *  take a microsecond or so for entry, setup and exit so short times could
 *  be off by quite a bit.  Caller beware!
 */
#define	INTERVAL_LO	0x70000630
#define	INTERVAL_HI	(INTERVAL_LO + 4)
#define Mhz-50
#ifdef Mhz-50
#define	TPUS		50	/* Ticks Per micro Second (@ 50Mhz) */
#else
#define	TPUS		30	/* Ticks Per micro Second (@ 30Mhz) */
#endif

delay(count)
u_long	count;
{
	extern	u_long	inl();

	register u_long	tar_hi, tar_lo;	/* target values */
		 u_long	cur_lo;		/* current value */

	/*
	 *  Get current counts
	 */
	tar_lo = cur_lo = inl(INTERVAL_LO);
	tar_hi = inl(INTERVAL_HI);

	/*
	 *  Check for carry out of low order counter when target value
	 *  is derived.  Guard against the window where high order counter
	 *  took a carry after reading tar_low but before tar_hi.
	 */
	if (((tar_lo += (count * TPUS)) < cur_lo) &&
						(tar_hi == inl(INTERVAL_HI)))
		tar_hi++;

	while ((inl(INTERVAL_HI) != tar_hi) || (inl(INTERVAL_LO) <  tar_lo))
		;
}

#define MCR			(4 << 5)	/* Modem Control Register */
#define RESET_BIT		0x08		/* OUT2 control bit      */
#define	MIO_SERIAL_ADDR		0x91400000

/* ARGSUSED */
mio_ether_reset(unit)
int	unit;
{
	outb(MIO_SERIAL_ADDR + MCR, (inb(MIO_SERIAL_ADDR + MCR) & ~RESET_BIT));
	delay(10);
	outb(MIO_SERIAL_ADDR + MCR, (inb(MIO_SERIAL_ADDR + MCR) | RESET_BIT));
}


/*
 * get_ether_addr:
 *
 *	Read FLASH EPROM to get factory set Ethernet address.
 */
/* ARGSUSED */
void
get_ether_addr(unit, addr_p)
	int	unit;
	u_char	*addr_p;
{
	u_char		*flash = ((mio_flash_t*)MIO_EEPROM_START)->
						boot_blk.bd_specific.ether_addr;
	int		i;

	for (i = 0; i < 6; i++)
		*addr_p++ = *flash++;
}

/* ARGSUSED */
void
mio_interrupt_enable(bit)
int	bit;
{
	return;
}

