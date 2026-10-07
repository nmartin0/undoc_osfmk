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
static char rcsid[] = "@(#)$RCSfile: fmterr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:15 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: Fmterr
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
/* fmterr.c 1.5 com/cmd/sccs/lib/comobj,3.1,9021 9/15/89 14:19:00"; */

# include	"defines.h"

Fmterr(pkt, cfile, line)
char *cfile;
register struct packet *pkt;
{
	fclose(pkt->p_iop);
	fprintf(stderr, "Fmterr: file=%s, line=%d\n", cfile, line);
	sprintf(Error,MSGCO(FRMTERR, 
           "There is a format error at line %u.(co4)\n"),pkt->p_slnno); 
	fatal(Error);
}
