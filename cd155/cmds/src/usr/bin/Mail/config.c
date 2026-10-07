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
static char rcsid[] = "@(#)$RCSfile: config.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:09 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX config.c
 * 
 * FUNCTIONS: 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	config.c     5.2 (Berkeley) 6/21/85
 */

/*
 * This file contains definitions of network data used by Mail
 * when replying.  See also:  configdefs.h and optim.c
 */

/*
 * The subterfuge with CONFIGFILE is to keep cc from seeing the
 * external defintions in configdefs.h.
 */
#define	CONFIGFILE
#include "configdefs.h"

/*
 * Set of network separator characters.
 */
char	*metanet = "!^:%@.";

/*
 * Host table of "known" hosts.  See the comment in configdefs.h;
 * not all accessible hosts need be here (fortunately).
 */
struct netmach netmach[] = {
    {	EMPTY,		EMPTYID,	AN },	/* Filled in dynamically */
    {	0,		0,		0 }
};

/*
 * Table of ordered of preferred networks.  You probably won't need
 * to fuss with this unless you add a new network character (foolishly).
 */
struct netorder netorder[] = {
	{ AN,	'@' },
	{ AN,	'%' },
	{ SN,	':' },
	{ BN,	'!' },
	{ -1,	0 }
};

/*
 * Table to convert from network separator code in address to network
 * bit map kind.  With this transformation, we can deal with more than
 * one character having the same meaning easily.
 */
struct ntypetab ntypetab[] = {
	{ '%',	AN },
	{ '@',	AN },
	{ ':',	SN },
	{ '!',	BN },
	{ '^',	BN },
	{ 0,	0 }
};

struct nkindtab nkindtab[] = {
	{ AN,	IMPLICIT },
	{ BN,	EXPLICIT },
	{ SN,	IMPLICIT },
	{ 0,	0 }
};
