/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: x24c02.c,v $
 * Revision 1.2.8.1  1995/06/11  18:33:58  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:44:08  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	x24c02.c
 *	Author:	Jim Chorn (modified by Jerrie Coffman for the microkernel)
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	11/94
 *
 *	Xicor X24C02 Serial E**2 Prom (256x8 bits)
 *
 *	The interfaces are described below
 */


#include <sdb.h>
#if	NSDB > 0

#include <sys/types.h>

#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/x24c02.h>

extern void		outb();
extern unsigned char	inb();

#define X24C02_CHIP_ADDRESS	SDB_SEEPROM

#ifndef X24C02_CHIP_ADDRESS
#define	X24C02_CHIP_ADDRESS	0x80000050	/* Default */
#endif
#define X24C02_CHIP	((volatile x24c02_reg_t *)X24C02_CHIP_ADDRESS)

/*
 * Functions provided:
 *
 * Random Sequential Read Operation:
 * int x24c02_read (u_char *buf, int count, u_char word_address, u_short slave);
 *
 * Multiple Page Write Operation:
 * int x24c02_write(u_char *buf, int count, u_char word_address, u_short slave);
 *
 *	Where:	<buf> is the place in memory that data is to be taken from
 *		(x24c02_write) or placed to (x24c02_read).
 *		<count> is the number of bytes to read/write to/from buf.
 *		<word_address> is the address inside the prom to read/write.
 *		<slave> is the x24c02 slave address bits A0-A2 (or bank address)
 *
 *	Return Values:
 *		x24c02_read returns number of bytes read.
 *		x24c02_write returns number of bytes written.
 *
 * Required Support Routines:
 *	delay()	- A micro second granularity delay routine.
 *
 * Note: No single byte read/write operation is preformed because the multi-byte
 *	 operations are just as efficient in single byte mode as in multi byte
 *	 mode.
 *
 * Static Internal Procedures (PRIVATE):
 *	start_bit(), stop_bit(), send_ack(), chip_nack(),
 *	clock_bits_in(), clock_bits_out(), clock_bit()
 */

/*
 * clock_bit - clocks a data bit out.
 *
 * Assumes: X24C02_SCL is low on entrance.
 * Leaves X24C02_SCL high on exit.
 * Returns: nothing.
 */
static void
clock_bit( volatile x24c02_reg_t *reg, x24c02_reg_t val )
{ 
	outb(reg, (inb(reg) & ~(X24C02_SDA|X24C02_SCL)) | val | X24C02_DIR);
	delay(t_DH);	
	/* Clock the bit out */ 
	outb(reg, inb(reg) | X24C02_SCL);
	delay(t_HIGH);	
	/*
	 * Note: Do not transition DIR on and SDA off at the same time
	 * because this appears not to work. This is avoided by not
	 * turning off DIR on every bit going out.
	 */
}

/*
 * start_bit - issue a start bit to the x24c02
 *
 * Issue a start bit. A start bit is a high to low transition of the data bit
 * when the clock is high.
 *
 * Assumes: X24C02_SCL is high or tri stated on entrance.
 * Leaves X24C02_SCL low on exit.
 * Returns: nothing.
 */
static void
start_bit( volatile x24c02_reg_t *reg, x24c02_reg_t val )
{ 
	/* Gaurrantee that there is time between stop bits and start bits */
	delay(t_SU_STA); 
	/* Ensure SDA is high, clock high and driving out */ 
	outb(reg, val|X24C02_SDA|X24C02_SCL|X24C02_DIR); 
	delay(t_SU_STA); 
	/* Transition SDA low during a high SCL */ 
	outb(reg, inb(reg) & ~X24C02_SDA); 
	delay(t_HD_STA); 
	/* Transition SCL low */ 
	outb(reg, inb(reg) & ~X24C02_SCL); 
}

/*
 * stop_bit - issue a stop bit to the x24c02
 *
 * Issue a stop bit. A stop bit is a low to high transition of the data bit
 * when the clock is high.
 *
 * Assumes: X24C02_SCL is high on entrances and leaves it high on exit.
 * Also Leaves direction bit off (eg. in tri-stated condition).
 * Returns: nothing.
 */
static void
stop_bit( volatile x24c02_reg_t *reg )
{ 
	/* Transition X24C02_SCL low */ 
	/* Ensure always driving */
	outb(reg, (inb(reg) & ~X24C02_SCL) | X24C02_DIR);
	delay(t_DH); 
	/* Transition X24C02_SDA low */ 
	outb(reg, inb(reg) & ~X24C02_SDA); 
	delay(t_SU_STO); 
	/* Transition X24C02_SCL high */ 
	outb(reg, inb(reg) | X24C02_SCL); 
	delay(t_HIGH); 
	/* Transition X24C02_SDA high during SCL high */ 
	outb(reg, inb(reg) | X24C02_SDA); 
	delay(t_BUF); 
	/* Transition X24C02_DIR low during SCL high to tri-state */ 
	outb(reg, inb(reg) & ~X24C02_DIR); 
}

/*
 * clock_bits_out - clock data bits out to the x24c02
 *
 * Clock all 8 bits 'on to' the serial interface.
 *
 * The data byte is clocked out most-significant bit first.
 *
 * Assumes: X24C02_SCL is low on entrance and X24C02_DIR is high.
 * Always leaves X24C02_SCL in a high state.
 * Returns: nothing.
 */
static void
clock_bits_out( volatile x24c02_reg_t *reg, u_char bits )
{ 
	short	my_count = 8; 

	while (my_count-- > 0) { 
		/* Set X24C02_SDA to value for data bit */ 
		if (bits & X24C02_DMASK)
			clock_bit(reg, X24C02_SDA);
		else
			clock_bit(reg, 0);
		/* Shift to next bit to be output'd */ 
		bits <<= 1; 
		if ( my_count > 0 )
			outb(reg, inb(reg) & ~X24C02_SCL); 
		else
			/*
			 * Ensure clock, data and drivers are off when done
			 * Avoids fighting pulses on X24C02_SDA during ACK.
			 */
			outb(reg,
			  inb(reg) & ~(X24C02_SCL|X24C02_DIR)); /* Expect ACK */
		delay(t_DH);	
	} 
}

/*
 * clock_bits_in - clock data bits in from the x24c02
 *
 * Clock all 8 bits 'out of' the serial interface.
 *
 * The data byte is clocked in most-significant bit first.
 *
 * Assumes: X24C02_SCL is low on entrance and X24C02_DIR is high.
 * Always leaves X24C02_SCL in a high state.
 * Returns: nothing.
 */
static void
clock_bits_in( volatile x24c02_reg_t *reg, u_char *bits )
{ 
	short	my_count = 8; 
	u_char	in_bits = 0; 

	while (my_count-- > 0) { 
		outb(reg, inb(reg) & ~(X24C02_DIR|X24C02_SCL)); 
		delay(t_LOW);	
		outb(reg, inb(reg) | X24C02_SCL); 
		delay(t_DH);	
		in_bits |= (inb(reg) & X24C02_SDA) ? 1 : 0;
		if (my_count > 0)	/* Skip last bit */
			in_bits <<= 1;
	}; 
	outb(reg, inb(reg) & ~(X24C02_DIR|X24C02_SCL));	/* Leave clock off */
	*bits = in_bits; 
	delay(t_SU_STO);			/* Allow settling time */
}

/*
 * send_ack - Send an acknowledge to the X24C02.
 * Returns: nothing.
 */
static void
send_ack( volatile x24c02_reg_t *reg )
{ 
	x24c02_reg_t reg_cp;
	/* Transition both clock and driving data out */ 
	reg_cp  = inb(reg);
	reg_cp &= ~(X24C02_SDA|X24C02_SCL);
	reg_cp |= X24C02_DIR;
	outb(reg, reg_cp);
	delay(t_LOW);	
	/*
	 * Note: Do not transition DIR on and SDA off at the same time
	 * because this appears not to work. This is avoided by not
	 * turning off DIR on every bit going out. And here where the
	 * Transition is avoided by the follow second write of reg.
	 */
	outb(reg, reg_cp);		/* Ensure DIR on and SDA off */
	delay(t_LOW);			/* Ensure DIR on and SDA off */
	/* Transition clock high */ 
	outb(reg, inb(reg) | X24C02_SCL); 
	delay(t_HIGH);	
	outb(reg, inb(reg) & ~X24C02_SCL);
	delay(t_LOW);	
}

/*
 * chip_nack - Determine if no acknowledge occurred on transfer.
 *
 * Recieve an acknowledge from the X24C02. Acknowledges always come during
 * the nineth clock cycle.
 * A valid acknowledge is when X24C02_SDA is low during this cycle.
 *
 * Assumes: Except for write polling mode acknowledges, it is an error
 *          if no acknowledge is returned by the device.
 *	    Reading the X24C02_DIR bit could be any thing, leave it on always.
 * Returns: Value of acknowledge bit.
 */
static x24c02_reg_t
chip_nack( volatile x24c02_reg_t *reg )
{
	x24c02_reg_t nack;

	/* Transition both clock and driving data off */
	outb(reg, inb(reg) & ~(X24C02_DIR|X24C02_SCL));
	delay(t_LOW);
	/* Transition clock high to check for Ack */
	outb(reg, inb(reg) | X24C02_SCL);
	delay(t_HIGH);
	nack = inb(reg) & X24C02_SDA;		/* Read val */
	/* SDA=L ensures no fights  SCL=L ensures no clock glitches */
	outb(reg, (inb(reg) & ~(X24C02_SDA|X24C02_SCL))|X24C02_DIR);
	return nack;
}

/*
 * x24c02_read - Preform a random (sequential) read of an X24C02 memory address.
 * Assumes: Reader has read the chip spec.
 * Returns: Number of bytes read.
 */
int
x24c02_read( u_char *buf, int count, u_char word_address, u_short slave )
{
	x24c02_reg_t	reg = SET_A0TOA2(slave); /* slave address setup */
	int		return_count = count;

	if (count <= 0)
		return 0;
	/*
	 * dummy write cycle to start things off.
	 */
	start_bit(X24C02_CHIP, reg);
	clock_bits_out(X24C02_CHIP, X24C02_SLAVE_ADDRESS(slave));
	if (chip_nack(X24C02_CHIP)) {
		printf("X24C02 Error: No ack on read slave address\n");
		stop_bit(X24C02_CHIP);
		return return_count - count;
	}
	clock_bits_out(X24C02_CHIP, word_address);
	if (chip_nack(X24C02_CHIP)) {
		printf("X24C02 Error: No ack on read word_address\n");
		stop_bit(X24C02_CHIP);
		return return_count - count;
	}
	/*
	 * Do sequential read operation until all data has been read.
	 */
	start_bit(X24C02_CHIP, reg);
	clock_bits_out(X24C02_CHIP, X24C02_SLAVE_ADDRESS(slave)|X24C02_RW_BIT);
	if (chip_nack(X24C02_CHIP)) {
		printf("X24C02 Error: No ack on second read slave address\n");
		stop_bit(X24C02_CHIP);
		return return_count - count;
	}
	while (count > 0) {		/* Chip is responding so scan data in */
		clock_bits_in(X24C02_CHIP, buf);
		if (count > 1)		/* Omit 9th clock cycle acknowledge */
			send_ack(X24C02_CHIP);	/* Send ACK to chip */
		++buf;
		--count;
	}
	stop_bit(X24C02_CHIP);
	return return_count - count;
}

/*
 * x24c02_write - Preform multiple page writes to X24C02 memory.
 * Returns: Number of bytes written.
 */
int
x24c02_write(u_char *buf, int count, u_char word_address, u_short slave)
{
	x24c02_reg_t	reg = SET_A0TOA2(slave); /* slave address setup */
	u_char		index;
	int		timer = TIMEOUT;
	int		return_count = count;

	reg |= X24C02_WEN;			/* mode = write enable */
	while (count > 0 && timer > 0) {
		start_bit(X24C02_CHIP, reg);
		clock_bits_out(X24C02_CHIP, X24C02_SLAVE_ADDRESS(slave));
		if (chip_nack(X24C02_CHIP)) {
			printf("X24C02 Error: no ack write slave address\n");
			stop_bit(X24C02_CHIP);
			return return_count - count;
		}
		clock_bits_out(X24C02_CHIP, word_address);
		if (chip_nack(X24C02_CHIP)) {
			printf("X24C02 Error: no ack on write word_address\n");
			stop_bit(X24C02_CHIP);
			return return_count - count;
		}
		/*
		 * Max page write is 4 avoid wrap by masking off word_address
		 */
		index = word_address & X24C02_MAX_PAGE_MASK;
		for ( ; index < X24C02_MAX_PAGE_WRITE && count > 0;
		    ++index, --count) {
			clock_bits_out(X24C02_CHIP, *buf);
			if (chip_nack(X24C02_CHIP)) {
				++index; --count;
				printf("X24C02 Warning: no ack data\n");
				break; /* Try write cycle polling */
			}
			++buf;
			++word_address;
		}
		stop_bit(X24C02_CHIP);
		/*
		 * Enter Write Ack Polling because of 5ms typical write cycles
		 */
		while (1) {
			start_bit(X24C02_CHIP, reg);
			clock_bits_out(X24C02_CHIP,X24C02_SLAVE_ADDRESS(slave));
			if (!chip_nack(X24C02_CHIP)) {
				stop_bit(X24C02_CHIP);
				break;		/* saw ack so continue writes */
			}
			stop_bit(X24C02_CHIP);
			if (timer-- <= 0)
				break;		/* timeout stop and fall thru */
		}
	}
	if (timer <= 0)
		printf("X24C02 Error: Write Ack timeout occurred\n");
	reg &= ~X24C02_WEN;			/* mode = write disable */
	return return_count - count;
}

#endif	NSDB > 0
