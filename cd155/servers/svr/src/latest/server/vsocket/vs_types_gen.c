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
 * $Log: vs_types_gen.c,v $
 * Revision 1.7  1995/02/11  00:08:22  stans
 *  Prevent multiple includes on vsocket/vs_types_gen.h by providing a wrapper
 *  #ifndef.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.6  1995/02/01  23:37:46  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:52:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:42:17  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.3  1993/07/14  18:49:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:15:10  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/05/18  03:21:22  cfj
 * Complete MI Driver merge.
 *
 * Revision 3.4  93/08/26  11:12:36  mjl
 * [LCCbug #0374] Add printf for VS_MAXIOCLEN constant.
 * 
 * Revision 3.3  93/05/05  22:51:40  mjl
 * Generate #defines for new MIG types used for MIv3 autoconfiguration RPCs.
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
#include "netns/ns.h"
#include "sys/un.h"
#include "vsocket/vs_types.h"
#include "vsocket/vs_chouse.h"

union allsockaddr {
	struct sockaddr 	sa;
	struct sockaddr_in	sin;
	struct sockaddr_ns	sns;
	struct sockaddr_un	sun;
};


main()
{
	printf("#ifndef\t_VSOCKET_VS_TYPES_GEN_H_\n");
	printf("#define\t_VSOCKET_VS_TYPES_GEN_H_ 1\n\n");

	printf("#define\tMAX_SERVERS\t%d\n",	MAX_SERVERS);
	printf("#define\tMAXADDRLEN\t%d\n",	MAXPATHLEN);
	printf("#define\tIFREQSIZE\t%d\n",	sizeof(union ifrequest));
	printf("#define\tIFINFOSIZE\t%d\n",	sizeof(struct iface_info));
	printf("#define\tSOCKADDRSIZE\t%d\n",	sizeof(union allsockaddr));
	printf("#define\tSOCKADDR_INSIZE\t%d\n",sizeof(struct sockaddr_in));
	printf("#define\tVS_MAXOPTLEN\t%d\n",	VS_MAXOPTLEN);
	printf("#define\tVS_MAXIOCLEN\t%d\n",	VS_MAXIOCLEN);
	printf("#define\tCHSKEYSIZE\t%d\n",	sizeof(struct chouse_key));

	printf("\n#endif\t/* _VSOCKET_VS_TYPES_GEN_H_ */\n");

	return (0);
};
