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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)ns_curses.h       1.7  com/lib/curses,3.1,8943 10/16/89 23:33:24 */
#ifndef _H_NS_CURSES
#define _H_NS_CURSES

/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   ns_curses.h
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*
 * NAME:        ns_curses.h
 *
 * FUNCTION:
 *
 *      Defines and includes required for curses to work in a non-
 *      standard environment.
 */

#include	"RecStruct.h"
#include	"VTio.h"
#include	"VTioctl.h"

/*	Make sure that curshdr.h is included before workst.h is		*/
/*	brought in so that screen can be defined properly.		*/

#ifndef		INFINITY
#include        <curshdr.h>
#endif

#include	"workst.h"

#define		SP		((struct screen *)&(WS->w_screen))

#define		cur_term	WS->w_cur_term

#define		outputspeed(x)	B9600

#endif /* _H_NS_CURSES */
