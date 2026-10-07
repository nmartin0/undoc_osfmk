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

 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 */

/*
 * net_boot.c
 *
 * Download and boot from files loaded over ethernet
 */
#include <sys/types.h>
#include "tftp.h"
#include "mio.h"

#define MAX_MD_SIZE	(10*1024*1024)
#define INTEL_PGBYTES	4096	/* bytes per page */
#define intel_round_page(x)	((((u_int)(x)) + INTEL_PGBYTES - 1) & \
						~(INTEL_PGBYTES-1))

extern	u_short	htons();
extern	u_short	ntohs();
extern	u_long	inl();
extern	void	outl();

extern  u_long  end;
extern	u_long	vm_end;
extern	u_long	sym_start;
extern	u_long	coff_start;
extern	int	log_level;
extern	int	log_id;

u_char	ip_self[ 4 ];
char	*boot_string[ BOOT_STRS ];
int	auto_boot_lck;
u_char	tftp_ip_addr[ 4 ];
u_short	tftp_remote_port;
u_short	tftp_local_port;
u_short	tftp_pkt;
int	nbyte;
int	tfile;
int	tbyte;
int	ether_err;
int	first_pkt;
int	software_refresh = 0;
int	hardware_refresh = 1;

#define MD_START  (char *)intel_round_page(vm_end)
char	*md_start = 0;
u_long	md_size = 0;

#define BM_START  (char *)intel_round_page(&end)
char	*bm_start = 0;

int	(*start_addr)();

struct	tftp_req {
	u_short	op;		/* request */
	char	filename[256];	/* filename and format string */
};

struct	tftp_ack {
	u_short	op;		/* type */
	u_short	blknum;		/* block number */
};

struct	tftp_data {
	u_short	op;		/* type */
	u_short	blknum;		/* block number */
	u_char	data[512];	/* of data */
} tftp_rcv_buf;

struct	tftp_data tftp_send_buf;
struct	tftp_data tftp_rcv_buf;

typedef	int (*PFI)();

char	*version = "Version 1.12";
char	*help = "\
               Network Boot Loader %s\n\
b[x] - Load and boot file using tftp : b[ index ] | [ boot string ]\n\
l[x] - Load without boot using tftp  : l[ index ] | [ boot string ]\n\
g    - Go                            : g [ starting address]\n\
B    - Set serial baud baud rate     : B [ baud rate ]\n\
d    - Dump memory                   : d\n\
p    - Program MIO Flash(tm)         : p\n\
S    - Print MIOE stats              : s\n\
r    - Toggle hardware memory refresh (currently %s)\n\
s    - Toggle software memory refresh (currently %s)\n\
?    - Print this help screen\n\
\n\
Ethernet address = %x:%x:%x:%x:%x:%x     Boot Strings =\n";
 
/* Registers (8250/16450 mode)
 */
#define BASE		0x91400000
#define BAL		(BASE + (0<<5))		/* BRGA LSB */
#define BAH		(BASE + (1<<5))		/* BRGA MSB */
#define LCR		(BASE + (3<<5))		/* Line Control Register */
#define LCR_VAL		0x03			/* N81 */

#define	NODE_CNTRL_ADDR		((u_long*)0x60000004)
#define	NODE_CNTRL_SHADOW	((u_long*)0xc0000ffc)
#define	SYSKEN_BIT		0x1000
#define REFRESH_DISABLE		0x0800

#define	INPUT_LIMIT	(BOOT_STR_SIZE + 2)
char	input [ INPUT_LIMIT ];

/*
 * Download files over ethernet
 */
main (boot_vector)
	int	boot_vector;
{
	u_long	node_cntrl_before, node_cntrl;
	char	*s;

	/*
	 *  Get a copy of the GP node control register.
	 *  This assumes that the GP ROM used an outl() to
	 *  store the shadow copy.  Otherwise, it would be
	 *  in the chip cache and we would need to do a
	 *  memory reference instruction.
	 *
	 *       node_cntrl_before = *(u_long*)NODE_CNTRL_SHADOW;
	 */
	node_cntrl_before = inl(NODE_CNTRL_SHADOW);

	/*
	 *  If KEN is not already set, call flush.  SYSKEN must NOT be
	 *  set when calling flush().
	 */
	if ( ! (node_cntrl_before & SYSKEN_BIT))
		flush();
	else
		printf("SYSKEN already set - did not flush\n");

	/*
	 *  Turn off caching.
	 */
	if (hardware_refresh) {
		node_cntrl = node_cntrl_before | SYSKEN_BIT;
		hardware_refresh = 1;
	} else {
		node_cntrl = node_cntrl_before | (SYSKEN_BIT | REFRESH_DISABLE);
		hardware_refresh = 0;
		software_refresh = 1;
	}

	outl(NODE_CNTRL_ADDR,    node_cntrl);	/* set register  */
	outl(NODE_CNTRL_SHADOW,  node_cntrl);	/* update shadow */
	printf("Node control value was %x - now %x - sp = %x\n",
		node_cntrl_before, inl(NODE_CNTRL_SHADOW), get_sp());
	printf("boot_vector=%x\n", boot_vector);

	/*
	 *  Get default Ethernet, boot strings, and auto boot lock flag
	 *  from flash.
	 */
	read_flash();

	/*
	 *  If auto_boot_lck is set comming in, keep us in interactive mode
	 */
	if (auto_boot_lck)
		boot_vector = -1;
interactive:
	/*
	 *  If interactive, go into prompt loop.
	 */
	while (boot_vector == -1) {
		printf("-> ");
		gets(input, INPUT_LIMIT);

		switch(input[0]) {

		case 'l':
		case 'b': {
			int	i, index;

			/*
			 *  Check for boot string index.
			 */
			if ((input[ 1 ] == 0) || (input[ 2 ] == 0)) {
				/*
				 *  Invalid or no index defaults to 0
				 */
				index = strtol(&input[ 1 ], 0, 10);
				if ((index < 0) || (index > (BOOT_STRS - 1)))
					index = 0;
				s = boot_string[ index ];
			} else {
				/*
				 *  Use specified boot string.
				 */
				for (i = 1; (input[ i ] == ' ') ||
					    (input[ i ] == '\t'); i++)
					;
				/*
				 *  If nothing but white space, use index 0.
				 */
				if (input[ i ] == 0)
					s = boot_string[ 0 ];
				else
					s = &input[ i ];
			}

			if (input[ 0 ] == 'b') {
				printf("Boot using: %s\n", s);
				if (load(s) == 0)
					break;
				printf("COFF header addr = 0x%x, symbol table addr/size = 0x%x/%d\n",
				    coff_start, sym_start, vm_end - sym_start);
				/*
				 *  Start the loaded program passing the
				 *  symbol table size and addr of boot magic.
				 *  For mesh booting (not needed here),
				 *  also pass start of server, size of server,
				 *  start of emulator, size of emulator.
				 */
				(*start_addr)(vm_end, md_start, md_size,
					bm_start, 0, 0, 0, 0);
			} else {
				printf("Loading %s\n", s);
				if (load(s) == 0)
					break;
				printf("Symbol table size = %d\n",
							vm_end - sym_start);
			}
			break;
		}

		case 'g':
			if (strtol(&input[ 1 ], 0, 0))
				start_addr = (PFI)strtol(&input[ 1 ], 0, 0);
			(*start_addr)(vm_end, md_start, md_size,
				bm_start, 0, 0, 0, 0);
			break;
			
		case 'd':
			dump_mem();
			break;

		case 'B': {
			int	rate;
			int	divisor;

			rate = strtol(&input[ 1 ], 0, 0);
			printf("setting baudrate to %d\n", rate);
			delay(1000);

			switch(rate) {
			case   300:
			case   600:
			case  1200:
			case  2400:
			case  4800:
			case  9600:
			case 19200:

				divisor = 115200L/rate;

				/* Set DLAB
				 */
				outb(LCR, 0x80);

				/* Set Speed
				 */
				outb(BAL,  divisor     & 0xFF);
				outb(BAH, (divisor>>8) & 0xFF);
				outb(LCR, LCR_VAL);

				break;
			default:
				printf("unsupported baud rate (%d)\n", rate);
			}
			break;
		}
		case 'p':
			set_ip_addr(boot_string[0]);  /* our IP addr default */
			prog_flash();
			read_flash();	/* set possible new boot strings */
			break;
		case 'r':
			node_cntrl = inl(NODE_CNTRL_SHADOW);
			if (hardware_refresh)
				node_cntrl |= REFRESH_DISABLE;
			else
				node_cntrl &= ~REFRESH_DISABLE;
			outl(NODE_CNTRL_ADDR,    node_cntrl);
			outl(NODE_CNTRL_SHADOW,  node_cntrl);
			hardware_refresh = !hardware_refresh;
			printf("Hardware refresh now %s\n",
					hardware_refresh ? "ON" : "OFF");
			break;
		case 's':
			software_refresh = !software_refresh;
			printf("Software refresh now %s\n",
					software_refresh ? "ON" : "OFF");
			break;
		case 'S': {
			extern mioe_print_stats();
			mioe_print_stats();
			break;
		}
		case 't':
			delay(strtol(&input[1], 0, 0));
			break;
		case '?': {
			extern	u_char	eth_self[];
			int	i;

			printf(help, version, hardware_refresh ? "ON":"OFF",
					      software_refresh ? "ON":"OFF",
					      BURST_ETHER(eth_self));
			for (i = 0; i < BOOT_STRS; i++)
				printf("%d) %s\n", i, boot_string[ i ]);
			break;
		}
		}
	}

	/*
	 *  Not interactive.  Use the boot vector.
	 */
	if ((boot_vector >= 0) && (boot_vector < BOOT_STRS))
		s = boot_string[ boot_vector ];
	else
		s = (char*) boot_vector;

	printf("Auto boot using: %s\n", s);

	/*
	 *  If the boot load fails, goto interactive mode.
	 */
	if (load(s) == 0) {
		boot_vector = -1;
		goto interactive;
	}

	printf("COFF header addr = 0x%x, symbol table addr/size = 0x%x/%d\n",
	    coff_start, sym_start, vm_end - sym_start);
	/*
	 *  Start the loaded program passing the
	 *  symbol table size.
	 */
	(*start_addr)(vm_end, md_start, md_size, bm_start, 0, 0, 0, 0);
}

#define	DELIMITER ' '
char *
extract_bootenv(name, str)
	char	*name;
	char	*str;
{
	char	*p;
	static char envstr[256];

	while (*str) {
		for (p = name; *p == *str; p++, str++)
			;

		/*
		 *  See if we have a match
		 */
		if (*str++ == '=') {
			p = envstr;
			while(*str && *str != DELIMITER)
				*p++ = *str++;
			*p++ = 0;
			return (envstr);
		}

		/*
		 *  Missed.  Get PAST the next delimiter or end of string.
		 *  NOTE: Order of evaluation is important!
		 */
		while (*str && *str++ != DELIMITER)
			;
	}
	return (0);
}

set_ip_addr(str)
	char	*str;
{
	char	*env_1, *env_2;
	int	i;

	/*
	 *  Get our IP address.
	 */
	if ((env_1 = extract_bootenv("OUR_IP_ADDR", str)) == 0) {
		printf("Boot string error - OUR_IP_ADDR not set\n");
		return (0);
	}

	for (i = 0; i < 4; i++) {
		ip_self[i] = (u_char)strtol(env_1, &env_2, 10);
		if (env_1 == env_2) {
			printf("Invalid OUR_IP_ADDR format (%s)\n", env_1);
			return (0);
		}
		env_1 = ++env_2;  /* skip delimiter pointed to by env_2 */
	}
	return (1);
}

load(str)
	char	*str;
{
	/*
	 * Initialize ethernet interface
	 */
	if (setup_mioe() == 0) {
		printf("MIO Ethernet setup failed!\n");
		return;
	}

	tbyte = 0;
	tfile = 0;

	/*
	 *  Set IP address.
	 */
	if (set_ip_addr(str) == 0)
		return;

	/*
	 * Download files
	 */
	if (load_kernel("BOOT_PARAGON_FILE", str) == 0) {
		printf("Kernel Load Failed!\n");
		return (0);
	}
	else if (load_md("BOOT_PARAGON_RAM_DISK", str) == 0) {
		printf("RAM Disk Load Failed!\n");
		return (0);
	}
	else if (load_bm("BOOT_MAGIC", str) == 0) {
		printf("Boot Magic Load Failed!\n");
		return (0);
	}
	else {
		printf("Downloaded %d files and %d bytes\n", tfile, tbyte);
		printf("Start address = %x\n", start_addr);
		return (1);
	}
}

/*
 * Download an OS kernel
 */
load_kernel(file, str)
	char	*file;
	char	*str;
{
	extern	int	load_state;
	char	*p;
	char	filename[256];
	u_char	ip_file[4];
	int	i;
	int	t;
	int	n;
	int	d;
	int	retry;

	LOG_ENTRY(1, ("load_kernel(file='%s', str='%s')\n", file, str));

	/*
	 * Get file info from boot parameter strings
	 */
	p = extract_bootenv(file, str);
	if (p == 0)
		return (0);

	/*
	 * Extract Internet address of host
	 */
	for (i = 0; i < 4; i++) {
		t = 0;
		while (*p >= '0' && *p <= '9') {
			t = t*10 + *p - '0';
			p++;
		}
		p++;
		ip_file[i] = t;
	}
	strcpy(filename, p);

	first_pkt  = 1;
	load_state = 1;
	for (retry = 0; retry < MAX_RETRY; retry++) {

		/*
		 * Request the file
		 */
		mk_tftp_req(ip_file, filename, (u_short)(UDP_GENERAL + retry));

		/*
		 * Receive packets and load them
		 */
		d = 0;
		ether_err = 0;
		for (;;) {
			n = tftp_get(&tftp_rcv_buf);

			if (n <= 0)
				break;

			if (load_data(tftp_rcv_buf.data, n)) {
				ether_err = 1;
				break;
			}

			/*
			 * Print heart beat
			 */
			if (d % 50 == 0) {
				printf("%c\r%c", 0xFF, "|/-\\"[(d/50)%4]);
			}
			d++;
		}

		if (n == 0) {

			/*
			 * Clean up
			 */
			printf ("Loaded %d bytes\n", nbyte);
			tbyte += nbyte;
			tfile++;
			return (1);
		} else if (ether_err){
			printf("Download failed with error\n");
			return (0);
		}
	}
	printf("Download failed, max retries\n");
	return (0);
}

/*
 * Download a disk image into memory
 */
load_md(file, str)
	char	*file;
	char	*str;
{
	char	*p;
	char	filename[256];
	u_char	ip_file[4];
	int	i;
	int	t;
	int	n;
	int	d;
	int	retry;
	int	zflag;

	LOG_ENTRY(2, ("load_ram_disk(file='%s', str='%s')\n", file, str));
	
	/*
	 * Get file info from boot parameter strings
	 */
	p = extract_bootenv(file, str);
	if (p == 0) {
		LOG(LOG_HIGH,("file %s not found in boot environment\n", file));
		return(1);	/* not a fatal error */
	}

	/*
	 * Extract Internet address of host
	 */
	for (i = 0; i < 4; i++) {
		t = 0;
		while (*p >= '0' && *p <= '9') {
			t = t*10 + *p - '0';
			p++;
		}
		p++;
		ip_file[i] = t;
	}
	strcpy(filename, p);

	while (*p)
		p++;
	zflag = (p[-2] == '.' && p[-1] == 'Z');

	first_pkt = 1;
	for (retry = 0; retry < MAX_RETRY; retry++) {

		/*
		 * Request the file
		 */
		mk_tftp_req(ip_file, filename, (u_short)(UDP_GENERAL + retry));

		/*
		 * Receive packets and place them in memory
		 */
		p = md_start = MD_START;
		printf("Loading ram disk at %08x\n", p);

		if (zflag) {
			p += MAX_MD_SIZE;
		}

		d = 0;
		md_size = 0;
		ether_err = 0;
		for(;;) {
			n = tftp_get(&tftp_rcv_buf);
			if ( n <= 0)
				break;

			bcopy(tftp_rcv_buf.data, (u_char*)p, n);

			p += n;
			md_size += n;

			/*
			 * Give heart beat.
			 */
			if (d % 50 == 0) {
				printf("%c\r%c", 0xFF, "|/-\\"[(d/50)%4]);
			}
			d++;
		}

		if (n == 0) {
			printf("Received %d bytes\n", nbyte);
			tbyte += nbyte;
			tfile++;

			if (zflag) {
				printf("Decompressing file\n");
				md_size = decompress(md_start + MAX_MD_SIZE,
						     md_size,
						     md_start,
						     MAX_MD_SIZE);
				printf("\nDecompressed into %d bytes\n", md_size);
			}
			return (1);
		} else if (ether_err) {
			printf("Download failed with error\n");
			return (0);
		}
	}

	printf("Download md failed, max retries\n");
	return (0);
}

/*
 * Download a boot magic image into memory
 */
load_bm(file, str)
	char	*file;
	char	*str;
{
	char	*p;
	char	filename[256];
	u_char	ip_file[4];
	int	i;
	int	t;
	int	n;
	int	d;
	int	retry;

	LOG_ENTRY(2, ("load_boot_magic(file='%s', str='%s')\n", file, str));
	
	/*
	 * Get file info from boot parameter strings;
	 * default is entire boot string, safely copied into memory
	 */
	p = extract_bootenv(file, str);
	if (p == 0) {
		LOG(LOG_HIGH,("file %s not found in boot environment\n", file));
		miocopy(str, BM_START, strlen(str)+1);
		bm_start = BM_START;
		return(1);
	}

	/*
	 * Extract Internet address of host
	 */
	for (i = 0; i < 4; i++) {
		t = 0;
		while (*p >= '0' && *p <= '9') {
			t = t*10 + *p - '0';
			p++;
		}
		p++;
		ip_file[i] = t;
	}
	strcpy(filename, p);

	first_pkt = 1;
	for (retry = 0; retry < MAX_RETRY; retry++) {

		/*
		 * Request the file
		 */
		mk_tftp_req(ip_file, filename, (u_short)(UDP_GENERAL + retry));

		/*
		 * Receive packets and place them in memory
		 */

		p = bm_start = BM_START;
		printf("Loading boot magic at %08x\n", p);

		d = 0;
		ether_err = 0;
		for(;;) {
			n = tftp_get(&tftp_rcv_buf);
			if ( n <= 0)
				break;

			bcopy(tftp_rcv_buf.data, (u_char*)p, n);

			p += n;

			/*
			 * Give heart beat.
			 */
			if (d % 50 == 0) {
				printf("%c\r%c", 0xFF, "|/-\\"[(d/50)%4]);
			}
			d++;
		}
		if (n == 0) {
			*p = 0;		/* null terminate boot magic strings */
			printf("Received %d bytes\n", nbyte);
			tbyte += nbyte;
			tfile++;
			return (1);
		} else if (ether_err) {
			printf("Download failed with error\n");
			return (0);
		}
	}

	printf("Download boot magic failed, max retries\n");
	return (0);
}
/*
 * Request a file via tftp
 */
mk_tftp_req(ip_addr, filename, port)
	u_char	*ip_addr;
	char	*filename;
	u_short	port;
{
	struct tftp_req	*req = (struct tftp_req*) &tftp_send_buf;
	int	i;

	LOG_ENTRY(3,("mk_tftp_req(ip_addr=%d.%d.%d.%d, filename='%s',port=%d)\n",
					BURST_IP(ip_addr), filename, port));
	/*
	 * Format the request packet
	 */
	for (i = 0; *filename; i++)	/* File name */
		req->filename[i] = *filename++;
	req->filename[i++] = 0;
	req->op = htons((u_short)TFTP_READ);

	bcopy("octet", &req->filename[i], 6);
	i += (6 + 2);			/* include 2 bytes of op code */

	/*
	 * Send the request
	 */
	sendudp(ip_addr, UDP_TFTP, port, (u_char*)req, i, LONG_TIMEOUT);
	delay(10000);

	/*
	 * Set up to receive packets
	 */
	nbyte = 0;
	tftp_pkt = 1;
	tftp_local_port = port;
	bcopy(ip_addr, tftp_ip_addr, 4);
}

/*
 * Receive one packet
 */
tftp_get(buf)
	struct tftp_data *buf;
{
	struct	tftp_ack *ack = (struct tftp_ack*) &tftp_send_buf;
	int	n;
	int	tmo;
	int	done;

	LOG_ENTRY(4, ("tftp_get(buf=%x)\n", buf));

	/*
	 * Check for last time through
	 * (because last packet was short)
	 */
	if (tftp_pkt == 0) {
		LOG(LOG_LOW, ("Last packet received\n"));
		return 0;
	}

	/*
	 * Receive packets until we get the right sequence number
	 */
	done = 0;
	while (!done) {
		n = recvudp(tftp_local_port,
			    &tftp_remote_port,
			    (u_char*)buf,
			    sizeof(struct tftp_data));

		if (n == -1) {
			LOG(LOG_HIGH, ("recvudp() returned error\n"));
			return -1;
		}

		/*
		 *  Put things into host byte order.
		 */
		buf->op     = ntohs(buf->op);
		buf->blknum = ntohs(buf->blknum);

		LOG(LOG_LOW, ("receive packet: op=%d blk=%d len=%d\n",
			buf->op, buf->blknum, n));

		if (buf->op == TFTP_ERROR) {
			ether_err = 1;
			return -1;
		}

		/*
		 *  See if we missed a block.
		 */
		if (buf->blknum > tftp_pkt) {
			LOG(LOG_HIGH,("blk out of sequence: got %d wanted %d\n",
							buf->blknum, tftp_pkt));
			return -1;
		}

		ack->op = htons(TFTP_ACK);

		/*
		 *  Check for retransmission of block by server
		 */
		tmo = SHORT_TIMEOUT;
                if (buf->blknum < tftp_pkt) {
			LOG(LOG_HIGH, ("blk %d retransmitted by server\n",
								buf->blknum));
                        ack->blknum = htons(buf->blknum);
		} else {
			/*
			 *  Ah, this one is just right.
			 */
                        ack->blknum = htons(tftp_pkt);

			if (n == TFTP_MAXMSG) {
				tftp_pkt++;
			} else {	/* Last packet */
				tftp_pkt = 0;
				tmo = 0;
			}
			done = 1;
		}

		sendudp(tftp_ip_addr,
			tftp_remote_port,
			tftp_local_port,
			(u_char*)ack,
			4,
			tmo);
	}

	/*
	 * Return length of data
	 */
	nbyte += n - 4;
	return (n - 4);
}
