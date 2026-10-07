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
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: getline.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:19 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: getline
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* getline.c 1.4 com/cmd/sccs/lib/comobj,3.1,9021 9/15/89 14:00:09"; */

# include	"defines.h"

/*
	Routine to read a line into the packet.  The main reason for
	it is to make sure that pkt->p_wrttn gets turned off,
	and to increment pkt->p_slnno.
*/

char *
getline(pkt)
register struct packet *pkt;
{
	char *n, *fgets();
	register char *p;

	if(pkt->p_wrttn==0)
		putline(pkt,(char *) 0);
	if ((n = fgets(pkt->p_line,sizeof(pkt->p_line),pkt->p_iop)) != NULL) {
		pkt->p_slnno++;
		pkt->p_wrttn = 0;
		for (p = pkt->p_line; *p; )
			pkt->p_chash += *p++;
	}
	else {
		if (!pkt->p_chkeof)
			fatal(MSGCO(PRMTREOF,
			    "\nThe end of the file was premature.(co5)\n"));
		if (pkt->do_chksum && (pkt->p_chash ^ pkt->p_ihash)&0xFFFF)
			fatal(MSGCO(CORRUPT,"\nThe file is damaged.(co6)\n"));
		if (pkt->p_reopen) {
			fseek(pkt->p_iop,0L,0);
			pkt->p_reopen = 0;
			pkt->p_slnno = 0;
			pkt->p_ihash = 0;
			pkt->p_chash = 0;
			pkt->p_nhash = 0;
			pkt->p_keep = 0;
			pkt->do_chksum = 0;
		}
		else {
			fclose(pkt->p_iop);
			pkt->p_iop = 0;
		}
	}
	return(n);
}
