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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 *	Unix to Blit I/O control codes
 */

#ifndef	_SYS_JIOCTL_H_
#define _SYS_JIOCTL_H_

#include <sys/ioctl.h>

#define JSMPX		TIOCUCNTL
#define JMPX		_IO(u,0)
#define JBOOT		_IO(u, 1)
#define JTERM		_IO(u, 2)
#define JTIMO		_IO(u, 4)	/* Timeouts in seconds */
#define JTIMOM		_IO(u, 6)	/* Timeouts in millisecs */
#define JZOMBOOT	_IO(u, 7)
#define JWINSIZE	TIOCGWINSZ
#define JSWINSIZE	TIOCSWINSZ

/**	Channel 0 control message format **/

struct jerqmesg
{
	char	cmd;		/* A control code above */
	char	chan;		/* Channel it refers to */
};

/*
**	Character-driven state machine information for Blit to Unix communication.
*/

#define C_SENDCHAR	1	/* Send character to layer process */
#define C_NEW		2	/* Create new layer process group */
#define C_UNBLK		3	/* Unblock layer process */
#define C_DELETE	4	/* Delete layer process group */
#define C_EXIT		5	/* Exit */
#define C_BRAINDEATH	6	/* Send terminate signal to proc. group */
#define C_SENDNCHARS	7	/* Send several characters to layer proc. */
#define C_RESHAPE	8	/* Layer has been reshaped */
#define C_JAGENT	9	/* Jagent return (What do they mean? */

/*
 * Map to new window structure
 */
#define bitsx	ws_xpixel
#define bitsy	ws_ypixel
#define bytesx	ws_col
#define bytesy	ws_row
#define jwinsize winsize

/*
**	Usual format is: [command][data]
*/

#endif	_SYS_JIOCTL_H_
