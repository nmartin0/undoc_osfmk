/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * 
 */

/*
 * Generate definitions for Mig interfaces.  MiG can't handle random
 * C definitions or expressions any better than the assembler.
 */

#include "sys/param.h"
#include "uxkern/import_mach.h"
#include "../user/include/servers/netname_defs.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "net/if.h"
#include "kern/zalloc.h"
#include <vsocket/mi_var.h>

main()
{
	printf("#define\tMI_PKT_SIZE %d\n", MI_PKT_SIZE);
	printf("#define\tIFNAMSIZ %d\n", IFNAMSIZ);
	
	printf("#ifdef notdef\n");
	printf("#define\tMI_ALLOC %d\n", MI_ALLOC);
	printf("#define\tMI_MACH_HDR_SIZE %d\n", MI_MACH_HDR_SIZE);
	printf("#define\tMIMTU %d\n", MIMTU);
	printf("#endif\n");


	return (0);
};
