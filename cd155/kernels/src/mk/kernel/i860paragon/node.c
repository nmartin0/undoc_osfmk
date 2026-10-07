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

#include <kern/assert.h>
#include <kern/lock.h>
#include <mach/machine/vm_types.h>
#include <i860paragon/lbus.h>
#include <i860paragon/dram.h>
#include <i860paragon/expansion.h>


extern void	outl(), splon();
extern int	inl(), sploff();
extern int	paragon_mp3;

/*
 *	REALLY set the node control register
 */
#define	node_control_register_write(bits)	outl(LB_NODE_CONTROL, (bits))

/*
 *	Private copy of the node control register.
 * 	Needed for GP node where this register is write-only.
 */
static unsigned long	node_control_register_shadow;

decl_simple_lock_data(, ncr_lock);

/*
 *	Initialize the node control register and its shadow.
 */
void
node_control_register_init()
{
	node_control_register_shadow = 0
		/*LB_NODE_CTRL_CPU0_LED0	/* cpu0, ylo (panel bit 6) */
		/*LB_NODE_CTRL_CPU0_LED1	/* cpu0, grn (panel bit 5) */
		/*LB_NODE_CTRL_CPU1_LED0	/* cpu1, ylo (panel bit 4) */
		/*LB_NODE_CTRL_CPU1_LED1	/* cpu1, grn (panel bit 3) */
		/*LB_NODE_CTRL_RED_LED		/* node, red (panel bit 2) */
		/*LB_NODE_CTRL_CPU1_RESET	/* cpu1 reset! */
		/*LB_NODE_CTRL_CPU2_RESET	/* cpu2 reset! */
		/*LB_NODE_CTRL_CPU0_IDLE	/* cpu0 idle! (panel bit 1) */
		/*LB_NODE_CTRL_CPU1_IDLE	/* cpu1 idle! (panel bit 0) */
		/*LB_NODE_CTRL_BOOT		/* boot bit */
		| LB_NODE_CTRL_DRAMENABLE	/* enable access to DRAM */
		/*LB_NODE_CTRL_REF_DEF		/* memory refresh defeat */
		/*LB_NODE_CTRL_KEN		/* cache-enable! */
		/*LB_NODE_CTRL_PARK		/* Don't arbitrate the bus */
		| LB_NODE_CTRL_INTOUT		/* interrupt from node */
		| LB_NODE_CTRL_ECCSEL		/* write ECC bits dram write */
		/*LB_NODE_CTRL_EXPSCANIN	/* expansion SCAN input */
		/*LB_NODE_CTRL_EXPSCANMD	/* expansion SCAN mode */
		/*LB_NODE_CTRL_EXPSCANCK	/* expansion SCAN clock */
		/*LB_NODE_CTRL_LEDBIT4		/* (panel bit 7?) */
		| LB_NODE_CTRL_NISTRESET	/* controlled NIST reset */
		/*LB_NODE_CTRL_CPUTURBO		/* enable 0-cycle wr latency */
		/*LB_NODE_CTRL_EXPTURBO		/* enable 0-cycle wr latency */
		/*LB_NODE_CTRL_DIFFOUT		/* bit-serial output (rs422) */
		;

	/*
	 *	assert upper byte as 0xff to help detect
	 *	the presence of an RPM card.
	 */
	node_control_register_shadow |= (0xff << 24);
	node_control_register_write(node_control_register_shadow);

	simple_lock_init(&ncr_lock);

	return;
}


/*
 *	Return the shadow copy of the node control register
 */
unsigned long
node_control_register_read(void)
{
	return (node_control_register_shadow);
}


/*
 *	Called at sploff, holding ncr_lock
 */
void
ncr_set_locked(unsigned long bits)
{
	node_control_register_shadow |= bits;
	node_control_register_write(node_control_register_shadow);
}

/*
 *	Called at sploff, holding ncr_lock
 */
void
ncr_clear_locked(unsigned long bits)
{
	node_control_register_shadow &= ~bits;
	node_control_register_write(node_control_register_shadow);
}

/*
 *	Set bits in the node control register
 */
void
node_control_register_set(unsigned long bits)
{
	int s;

	NCR_ENTER(s);
	ncr_set_locked(bits);
	NCR_EXIT(s);

	return;
}

/*
 *	Clear bits in the node control register
 */
void
node_control_register_clear(unsigned long bits)
{
	int	s;

	NCR_ENTER(s);
	ncr_clear_locked(bits);
	NCR_EXIT(s);

	return;
}

/*
 *	Is an expansion card present?
 */
int
node_status_register_expansion_present()
{
	int	s;

	s = node_status_register_read();
	return ((s & LB_NODE_STAT_EXPPRESENT) == 0);
}

/*
 *	What type of expansion card is present?
 *
 *	MIO is default (ID byte is unreliable)
 *
 *	see expansion.h for a breakdown of bits.
 */
int	_expansion_id = -1;

int
expansion_id()
{
	int	s;
	
	if (_expansion_id != -1)
		return (_expansion_id);

	if (!node_status_register_expansion_present()) {
		_expansion_id = 0;
		return (_expansion_id);
	}

	if ((s = inb(EXP_ID_ADDR)) == 0xff) {
		_expansion_id = EXP_ID_MIO;
		return (_expansion_id);
	}
	
	s &= EXP_ID_MASK;
	switch (s)  {
	case EXP_ID_MIO:
	case EXP_ID_HIPPI:
	case EXP_ID_MEMX16:
	case EXP_ID_MEMX32:
	case EXP_ID_MEMX64:
	case EXP_ID_MEMX128:
	case EXP_ID_SCSI:
		_expansion_id = s;
		return (s);
	}

	_expansion_id = EXP_ID_MIO;
	return (_expansion_id);
}

/*
 *	Return the base address of memory.
 */
vm_offset_t
node_memory_base()
{
	return ((vm_offset_t) (DRAM_BASE
		|DRAM_SNOOP_CPU0
		|DRAM_SNOOP_CPU1
		|DRAM_HIGH_ALIAS));	/* stay within 128MB of the top */
}


/*
 *	Return this node's memory size.
 */
vm_size_t
node_memory_size()
{
	int	s = node_status_register_read();
	vm_size_t	bytes;

	if (paragon_mp3)
		bytes = MP_FRUID_BYTES(MP_NODE_STATUS_FRUID(s));
	else
		bytes = GP_NODE_STATUS_BYTES(s);

	return (bytes);
}

/*
 *	Determine if this is an MP3 nodeboard. Return a 1 of so, else
 * 	return a zero (0).
 */

int
node_type()
{
	int	nsr;

	nsr = node_status_register_read();


	/*
	 * All MP nodes drive LB_NODE_STAT_ISMPNODE to 1
	 * so if it is is 0, then we're a GP.
	 */
	if (!(nsr & LB_NODE_STAT_ISMPNODE))
		return (0);	/* GP */

	/*
	 * As some GP nodes drive NODE_STAT_ISMPNODE to 1, we have
	 * a backup to test...  The top byte of the NSR on MP nodes
	 * will always be non zero. (bits [30:31] are pulled high).
	 * On the GP, these bits are the contents of the scan register.
	 */

	outb(LB_DIAG_OUT, 0);	/* Write a zero byte to the scan register */

	while ( (node_status_register_read() & 0xFF000000) != 0 )
	{
		/* More evidence we have an MP */

		if ( inb(LB_DIAG_IN) == 0 )
		{
			/*
			 * Just to make sure that scan has not
			 * snuck in and changed it
			 */

			return (1);	/* MP */
		} else
		{
			/*
			 * Write it again, since scan stepped on it
			 */
			outb(LB_DIAG_OUT, 0);
		}
	}
	return (0);	/* GP */
}

/*
 * cpu_start(cpunum)
 * Start cpu 'cpunum' by releasing it from RESET.
 */

void
cpu_start(int cpunum)
{
        assert(cpunum == 1 || cpunum < NCPUS);

	if (cpunum == 1)
	{
		extern int boot_msg_proc;

		node_control_register_set(LB_NODE_CTRL_CPU1_RESET);
		printf("Starting CPU 1 as %s.\n",
			boot_msg_proc ? "a Message CoProcessor" :
			"an Application Processor");
	}
	else if ((cpunum == 2) && paragon_mp3)
	{
		node_control_register_set(LB_NODE_CTRL_CPU2_RESET);
		printf("Starting CPU 2 as an Application Processor.\n");
	}
	return;
}
