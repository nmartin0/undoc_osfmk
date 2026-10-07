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
 
#include <sys/types.h>
#include "mio.h"
#include "tftp.h"

extern	int	end;
extern	int	ether_err;
extern	int	nbyte;

extern		u_char	eth_self[];
extern		char	*boot_string[ BOOT_STRS ];
extern		u_short	auto_boot_lck;

mio_flash_t	*flash_p = (mio_flash_t*)MIO_EEPROM_START;

#define	GP_FLASH_START	0x60700000

struct	tftp_data {
	u_short	op;		/* type */
	u_short	blknum;		/* block number */
	u_char	data[512];	/* of data */
} flash_rcv_buf;

struct	block_info {
	u_char	*start;
	u_char	*end;
};

char	*flash_menu = "\
\n\
1) Load MIO Flash BOOT file over TFTP\n\
2) Load MIO Flash Diagnostics file over TFTP\n\
3) Load GP  Flash file over TFTP\n\
4) Program MIO BOOT/Diagnostics code into Flash\n\
5) Program GP Flash\n\
6) Change Ethernet address\n\
7) Change boot strings\n\
8) %s auto boot\n\
9) Dump GP Flash\n\
q) Return to main menu\n\
\n\
-> ";

prog_flash()
{
	extern	dump_mem();

	u_char		input[128];
	u_char		ip_addr[4];
	u_char		*p;
	u_char		*dst;
	u_char		val;
	int		len;
	int		file_len;
	int		i;
	diag_1_t	*diag = (diag_1_t*)&end;
	char		*type;


	while (1) {
		printf(flash_menu, auto_boot_lck ? "Enable" : "Disable");
		gets(input, sizeof(input));

		switch (input[0]) {
			case '1':
			case '2':
			case '3':
				if (input[0] == '1') {
					type = "BOOT";
					len  = sizeof(diag->tftp);
					dst  = diag->tftp;
				} else  if (input[0] == '2') {
					type = "MIO Diagnostics";
					len  = sizeof(diag->mio_diag);
					dst  = diag->mio_diag;
				} else {
					type = "GP Flash";
					len  = sizeof(diag->tftp);
					dst  = diag->tftp;
				}

				/*
				 *  Save what is currently in parameter block 2.
				 */
				if (input[0] != '3')
					bcopy (&flash_p->main, diag,
							sizeof(diag_1_t));
				printf("Enter server IP address for %s file: ",
									type);
				gets(input, sizeof(input));

				p = input;
				for (i = 0; i < 4; i++) {
					val = 0;
					while (*p >= '0' && *p <= '9') {
						val = val*10 + *p - '0';
						p++;
					}
					p++;
					ip_addr[i] = val;
				}

				printf("Enter %s filename: ", type);
				gets(input, sizeof(input));

				if ((file_len = get_flash_file(input, ip_addr,
								dst)) > len)
	printf("WARNING: file larger than reserved % area in Flash", type);
				break;

			case '4':
				p = (u_char*)MIO_EEPROM_START;
				if (flash_16(diag, p, sizeof(diag_1_t)) < 0)
					printf("Flash programming error\n");
				else
					printf("Flash programming successful\n");
				break;

			case '5':
				p = (u_char*)GP_FLASH_START;
				printf("Enter starting offset into GP Flash: ");
				gets(input, sizeof(input));
				p += strtol(input, 0, 0);
				printf("Verify - program 0x%x for %d bytes?: ",
							p, file_len);
				gets(input, sizeof(input));
				if (input[0] != 'y')
					break;
				if (flash_8(diag, p, file_len) < 0)
					printf("Flash programming error\n");
				else
					printf("Flash programming successful\n");
				break;
			case '6':
				set_ether();
				break;
			case '7':
				set_boot_string();
				break;
			case '8':
				toggle_boot_lck();
				break;
			case '9':
				dump_gp_flash();
				break;
			case 'q':
				return;
		}
	}
}

get_flash_file(filename, ip_addr, dst)
	u_char	*filename;
	u_char	*ip_addr;
	u_char	*dst;
{
	int	n;
	int	d;
	int	retry;

	if (setup_mioe() == 0) {
		printf("MIO Ethernet setup failed!\n");
		return (0);
	}

	for (retry = 0; retry < MAX_RETRY; retry++) {

		/*
		 * Request the file
		 */
		mk_tftp_req(ip_addr, filename, (u_short)(UDP_GENERAL + retry));

		/*
		 * Receive packets and load them
		 */
		for (d = ether_err = 0; 1; dst += n){
			n = tftp_get(&flash_rcv_buf);

			if (n <= 0)
				break;

			bcopy(flash_rcv_buf.data, dst, n);

			/*
			 * Print heart beat
			 */
			if (d % 50 == 0) {
				printf("%c\r%c", 0xFF, "|/-\\"[(d/50)%4]);
			}
			d++;
		}

		if (n == 0) {
			printf ("Loaded %d bytes\n", nbyte);
			return (nbyte);
		}
		else if (ether_err) {
			printf("Download failed with error\n");
			return (0);
		}
	}
	printf("Download failed, max retries\n");
	return (0);
}

/*
 *  Set Ethernet address.
 */
set_ether()
{
	struct boot_blk	*boot = (struct boot_blk*)&end;
	u_char		buf[32];
	u_char		ether[6];
	u_char		*p1, *p2;
	int		i;

	printf("Enter Ethernet address as colon separated hex digits: ");
	gets(buf, sizeof(buf));

	for (p1 = buf, i = 0; i < 6; i++) {
		ether[i] = (u_char)strtol(p1, &p2, 16);
		if (p1 == p2) {
			printf("Invalid address format.\n");
			return;
		}
		p1 = ++p2;  /* skip delimiter pointed to by p2 */
	}

	/*
	 *  Save what is currently in the block before we erase it.
	 */
	bcopy (&flash_p->boot_blk, boot, sizeof(flash_p->boot_blk));
	bcopy (ether, boot->bd_specific.ether_addr, 6);
	if (flash_16(boot, &flash_p->boot_blk, sizeof(flash_p->boot_blk)) < 0)
		printf("Flash programming error\n");
}
	
/*
 *  Set boot string.
 */
set_boot_string()
{
	struct param_2	*param = (struct param_2*)&end;
	u_char		buf[ 16 ];
	int		index;

	/*
	 *  Save what is currently in parameter block 2.
	 */
	bcopy (&flash_p->param_2, param, sizeof(flash_p->param_2));

	/*
	 *  Find out which string they want to replace then get it and
	 *  place it in the memory based copy.
	 */
	printf("Which string index (0 - %d): ", BOOT_STRS - 1);
	gets(buf, sizeof(buf));
	index = strtol(buf, 0, 0);

	printf("Enter the boot string for index %d:\n", index);
	gets(&param->setup.boot_string[ index ][ 0 ], BOOT_STR_SIZE);

	/*
	 *  Program the PROM.
	 */
	if (flash_16(param, &flash_p->param_2, sizeof(flash_p->param_2)) < 0)
		printf("Flash programming error\n");
}

/*
 *  Toggle auto boot lock flag.
 */
toggle_boot_lck()
{
	struct boot_blk	*boot = (struct boot_blk*)&end;

	/*
	 *  Save what is currently in the block before we erase it.
	 */
	bcopy (&flash_p->boot_blk, boot, sizeof(flash_p->boot_blk));
	auto_boot_lck = !auto_boot_lck;
	boot->bd_specific.auto_boot_lck = auto_boot_lck;
	if (flash_16(boot, &flash_p->boot_blk, sizeof(flash_p->boot_blk)) < 0)
		printf("Flash programming error\n");
}

/*
 *  Read Ethernet and boot string from flash.
 */
read_flash()
{
	u_char		*e   = &flash_p->boot_blk.bd_specific.ether_addr[0];
	u_short		*lck = &flash_p->boot_blk.bd_specific.auto_boot_lck;
	char		*s;
	int		i, j;
	
	/*
	 *  Read Ethernet address.
	 */
	for (i = 0; i < 6; i++)
		eth_self[ i ] = e[ i ];

	/*
	 *  Validate it (as best we can).
	 */
	if ((e[ 0 ] != 0) || (e[ 1 ] != 0xaa) || (e[ 2 ] != 0))
		printf ("Invalid Flash Ethernet address (%x:%x:%x:%x:%x:%x)\n",
							BURST_ETHER(e));
	
	/*
	 *  Validate boot string (as best we can).
	 */
	for (i = 0; i < BOOT_STRS; i++) {
		s = &flash_p->param_2.setup.boot_string[ i ][ 0 ];
		for (j = 0; j < BOOT_STR_SIZE; j++) {
			if (s[ j ] == 0) {
				boot_string[ i ] = s;
				break;
			} else if (s[j] & 0x80) {
				boot_string[i] = "<valid boot string not set>";
				break;
			}
		}
	}

	/*
	 *  Read auto boot lock
	 */
	auto_boot_lck = *lck;
}

struct	block_info	blk_16_info [] = {
	{ (u_char*)MIO_EEPROM_START,
				(u_char*)(MIO_EEPROM_START + 0x37fff) },
	{ (u_char*)(MIO_EEPROM_START + 0x38000),
				(u_char*)(MIO_EEPROM_START + 0x39fff) },
	{ (u_char*)(MIO_EEPROM_START + 0x3a000),
				(u_char*)(MIO_EEPROM_START + 0x3bfff) },
	{ (u_char*)(MIO_EEPROM_START + 0x3c000),
				(u_char*)(MIO_EEPROM_START + 0x3ffff) },
	{ (u_char*)0,          (u_char*)0                             },
};

flash_16(ram_p, flash_p, len)
	u_short	*ram_p;
	u_short	*flash_p;
	int	len;		/* number of bytes */
{
	struct block_info	*p;
	int			i;
	int			result = 0;

	printf("Flash program 0x%x for %d bytes\n", flash_p, len);

	/*
	 *  Make sure data is contained within a valid block boundary.
	 */
	for (p = blk_16_info; p->start; p++)
		if ((flash_p >= (u_short*)p->start) &&
		    ((((u_char*)flash_p) + (len - 1)) <= p->end))
			break;

	if (p->start == 0) {
		printf("Flash address/len 0x%x/%d out of range\n", flash_p,len);
		return (-1);
	}
	
	/*
	 *  Erase the block in question
	 *  N.B. flash_p is pointing to the status register (first byte in blk).
	 */
	outw(flash_p, 0x2020);			/* setup erase */
	outw(flash_p, 0xd0d0);			/* erase confirm */
	outw(blk_16_info[0].start, 0x7070);	/* read status register */

	/*
	 *  This should be done within 3 seconds.  (We'll give'em 4.)
	 */
	for (i = 4000; i && ((inw(flash_p) & 0xf1f1) != 0x8080); i++)
		delay(1000);

	if (i == 0) {
		printf("Erase failure - status = 0x%x\n", (u_long)*flash_p);
		result = -2;
		goto out;
	}

	printf("Erase complete\n");

	/*
	 *  Program the flash
	 */
	len += (len & 1);	/* make length multiple of 2 */

	for (len /= 2; len--; ++flash_p, ++ram_p) {
		outw(flash_p, 0x4040);		  /* Setup program */
		outw(flash_p, *ram_p);		  /* program data */
		outw(blk_16_info[0].start, 0x7070); /* Address status register*/

		/*
		 *  This should be done witin 15 usecs.  (We'll give'em 20.)
		 */
		for (i = 20; i && ((inw(flash_p) & 0xf1f1) != 0x8080); i++)
			delay(1);

		if (i == 0) {
			printf("Program failure - status = 0x%x\n",
							(u_long)*flash_p);
			result = -3;
			goto out;
		}
		
		/*
		 *  Validate data.
		 */
		outw(flash_p, 0xffff);			/* Read array command */
		if (inw(flash_p) != *ram_p) {
			printf("Data did not compare (%x %x)\n",
							*flash_p, *ram_p);
			result = -4;
			goto out;
		}
	}

	printf("Programming complete\n");

out:
	/*
	 *  Clear the status register and put Flash into Read Array mode.
	 */
	outw(blk_16_info[0].start, 0x5050);	/* clear status register */
	outw(--flash_p, 0xffff);		/* Read array command */

	return (result);
}

struct	block_info	blk_8_info [] = {
	{ (u_char*)GP_FLASH_START,
				(u_char*)(GP_FLASH_START + 0x1bfff) },
	{ (u_char*)(GP_FLASH_START + 0x1c000),
				(u_char*)(GP_FLASH_START + 0x1cfff) },
	{ (u_char*)(GP_FLASH_START + 0x1d000),
				(u_char*)(GP_FLASH_START + 0x1dfff) },
	{ (u_char*)(GP_FLASH_START + 0x1e000),
				(u_char*)(GP_FLASH_START + 0x1ffff) },
	{ (u_char*)0,          (u_char*)0                           },
};

flash_8(ram_p, flash_p, len)
	u_char	*ram_p;
	u_char	*flash_p;
	int	len;		/* number of bytes */
{
	struct block_info	*p;
	u_char			*cnvrt_p;
	u_char			*status_p = (u_char*)gp_flash_convert(blk_8_info[0].start);
	int			i;
	int			result = 0;

	printf("Flash program 0x%x for %d bytes\n", flash_p, len);

	/*
	 *  Make sure data is contained within a valid block boundary.
	 */
	for (p = blk_8_info; p->start; p++)
		if ((flash_p >= p->start) && ((flash_p + (len - 1)) <= p->end))
			break;

	if (p->start == 0) {
		printf("Flash address/len 0x%x/%d out of range\n", flash_p,len);
		return (-1);
	}
	
	/*
	 *  Convert Flash pointers.
	 */
	status_p = (u_char*)gp_flash_convert(blk_8_info[0].start);
	cnvrt_p  = (u_char*)gp_flash_convert(flash_p);

	/*
	 *  Erase the block in question.
	 */
	outb(cnvrt_p, 0x20);			/* setup erase */
	outb(cnvrt_p, 0xd0);			/* erase confirm */
	outb(status_p, 0x70);			/* read status reg */

	/*
	 *  This should be done within 3 seconds.  (We'll give'em 4.)
	 */
	for (i=4000; i && ((inb(cnvrt_p) & 0xf1) != 0x80);i++)
		delay(1000);

	if (i == 0) {
		printf("Erase failure - status = 0x%x\n", (u_long)*flash_p);
		result = -2;
		goto out;
	}

	printf("Erase complete\n");

	/*
	 *  Program the flash
	 */
	for (; len--; flash_p++, ram_p++) {
		cnvrt_p = (u_char*)gp_flash_convert(flash_p);	/* convert */
		outb(cnvrt_p, 0x40);			/* Setup program */
		outb(cnvrt_p, *ram_p);			/* program data */

		/*
		 * address status register
		 */
		outb(status_p, 0x70);

		/*
		 *  This should be done witin 15 usecs.  (We'll give'em 20.)
		 */
		for (i = 20; i && ((inb(cnvrt_p) & 0xf1) != 0x80); i++)
			delay(1);

		if (i == 0) {
			printf("Program failure - status = 0x%x\n",
							(u_long)*flash_p);
			result = -3;
			goto out;
		}
		
		/*
		 *  Validate data.
		 */
		outb(cnvrt_p, 0xff);			/* Read array command */
		if (inb(cnvrt_p) != *ram_p) {
			printf("Data did not compare (%x %x)\n",
							*flash_p, *ram_p);
			result = -4;
			goto out;
		}
	}

	printf("Programming complete\n");

out:
	/*
	 *  Clear the status register and put Flash into Read Array mode.
	 */
	outb(status_p, 0x50);		/* clear status register */
	outb(--cnvrt_p, 0xff);		/* Read array command */

	return (result);
}

dump_gp_flash()
{
	u_char	buf[16];
	u_char	flash[16];
	u_char	*len_p;
	u_char	*start;
	int	len;
	int	i;
	
	while (1) {
		printf("Enter GP Flash offset (0 - 0x1ffff) and len ('q' to quit): ");
		gets(buf, 80);
	 
		if (buf[0] == 'q')
			return;

		start = (u_char*) strtol(buf, &len_p, 0);
		len   = (int)     strtol(len_p, 0, 0);

		if (start > (u_char*)0x1ffff) {
			printf("Offset %x invalid - must be <= 0x1ffff\n",
									start);
			continue;
		}
		
		start += GP_FLASH_START;

		if (len == 0)
			len = 256;

		/*
		 *  Get ourselfs 16 byte aligned.
		 */
		printf("\n");
		if ((u_long)start & 0xf) {
			int	spare = (u_long)start & 0x0f;

			for (i = 0; i < spare; i++)
				flash[i] = inb(gp_flash_convert(start + i));
			dump(flash, spare, start);
			len   -= spare;
			start += spare;
		}

		while (len > 0) {
			for (i = 0; i < 16; i++)
				flash[i] = inb(gp_flash_convert(start + i));

			dump(flash, (len < 16 ? len : 16),  start);
			len   -= 16;
			start += 16;
		}
		printf("\n");
	}
}

/*
 *  Converts addresses to GP Flash data addresses
 */
gp_flash_convert(addr)
	u_long addr;
{
	addr &= 0xfff1ffffL;         /* clear A17-A19 */
	addr |= ((0x7 & addr) << 17);/* displace A0-A2 up to A17-A19 */
	addr &= 0xfffffff8L;        /* A0-A2 (Flash is connected to N11 D0-D7 */
	return (addr);
}
