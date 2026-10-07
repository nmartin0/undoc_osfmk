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
#ifndef lint
#ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: flip.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:56:08 $";
#endif
#endif
/* static char sccsid[] = "@(#)flip.c	1.4  com/cmd/prog/cflow,3.1,9021 3/19/90 09:15:31"; */

/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: main
 *
 * ORIGINS: 00 03 10 27 32
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <stdio.h>
#include <locale.h>
#include <ctype.h>

main()
	{
	char line[BUFSIZ], *pl, *gets();

	setlocale(LC_ALL, "");
	while (pl = gets(line))
		{
		while (*pl != ':')
			++pl;
		*pl++ = '\0';
		while (isspace(*pl))
			++pl;
		printf("%s : %s\n", pl, line);
		}
	}
