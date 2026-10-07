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
 

#include	<mach/mach_types.h>

/*
 * NIC Memory Loader commands
 */

#define NIC_BOOT_RESTART	0x40
#define NIC_BOOT_LOAD_BLK	0x41
#define NIC_BOOT_CLEAR_BLK	0x42
#define NIC_BOOT_GO_ADDR	0x43


/*
 * Packet format expected by NIC Memory Loader
 */

#define	MAX_GOTO_ARGS		12
#define	MAX_GOTO_ARG_SZ		(MAX_GOTO_ARGS * sizeof(int))
#define BOOTBUF_HDR_SZ 		8
#define MAX_BOOT_PACKET		1024		

struct bootbuf {
	int 	data_bytes;		/* nbytes transferred to memory */
	double	*data_addr;		/* destination in node's memory */
	char	data[MAX_BOOT_PACKET];
};
typedef struct bootbuf bootbuf_t;


/*
 * Arguments for boot packet protocol interface;
 * make these simple globals until we use them
 * for something interesting 
 */

#define NIC_NO_BROADCAST	0
#define NIC_BROADCAST		1
#define HOST_PORT		(mach_port_t)0
long 	sequence;
#define bump_seq(seq)		((seq)++)


