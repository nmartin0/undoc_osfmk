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
 * @OSF_COPYRIGHT@
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)termio.h       1.17  com/inc/sys,3.1,8943E 11/9/89 14:06:45 */

/*
 * COMPONENT_NAME: (sysxtty) System Extension for tty support
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 3, 9, 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * HISTORY
 * $Log: termio.h,v $
 * Revision 1.4  1994/11/18  20:41:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:12  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:36  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:13  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:13  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:14:12  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:45:55  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:55:03  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:34  gm]
 * 
 * Revision 1.3  90/06/22  20:54:20  devrcs
 * 	Restructured for SVID and POSIX compatibility.
 * 	[90/06/14  23:03:06  havens]
 * 
 * 	Completely restructured this file for SVID support.  The ioctls are
 * 	now defined in ioctl.h.  Most of the flags are defined in termios.h
 * 	Any flags which conflict with the BSD equavalents are redefined in
 * 	this file.
 * 	[90/06/13  13:02:45  havens]
 * 
 * 	Changed include file. Can't get non-sys files from here.
 * 	[90/05/31  19:36:03  nolting]
 * 
 * Revision 1.2  90/03/13  20:50:00  mbrown
 * 	moved controls to sys/ttydefaults.h
 * 	[90/02/25  14:44:45  mbrown]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/09  15:19:01  tom]
 * 
 * $EndLog$
 */

#ifndef _SYS_TERMIO_H_
#define _SYS_TERMIO_H_

#include <standards.h>

#include <sys/ioctl.h>
#include <sys/ttmap.h>

/* termios defines all the bits, baudrates, etc. */
#include <sys/termios.h>
#include <sys/ttydefaults.h>

/*
 * Ioctl control packet
 */

#define NCC 8

struct termio {
        unsigned short c_iflag;         /* input modes */
        unsigned short c_oflag;         /* output modes */
        unsigned short c_cflag;         /* control modes */
        unsigned short c_lflag;         /* line discipline modes */
        char c_line;                    /* line discipline */
        unsigned char c_cc[NCC];        /* control chars */
};

/* Unfortunately System V have and POSIX have different values for some of
 * the defines.  So we have to perform the following kluge which redefines
 * the V___ values for termio.  This means you cannot do ioctls using termios
 * and termio in the same source file.
 */
#ifdef _KERNEL
#define VVINTR	0
#define VVQUIT	1
#define	VVERASE	2
#define VVKILL	3
#define	VVEOF	4
#define VVMIN	4
#define	VVEOL	5
#define VVTIME	5
#define VVEOL2  6
#define VVSWTCH 7

#define VNOFLSH  0x8000		/*
				 * This is the system V value the BSD
				 * equalivent is 0x80000000 which won't fit
				 * in a termio structure.
				 */
#else
 
/* Redefine the these values for SVID */
#undef	VEOF
#undef	VEOL
#undef	VERASE
#undef VKILL
#undef VINTR
#undef VQUIT
#undef VMIN	
#undef VTIME	
#undef VEOL2  
#undef VSWTCH 
#undef NOFLSH

#define VINTR	0
#define VQUIT	1
#define	VERASE	2
#define VKILL	3
#define	VEOF	4
#define VMIN	4
#define	VEOL	5
#define VTIME	5
#define VEOL2  6
#define VSWTCH 7

#define NOFLSH 0x8000
#endif /* _KERNEL */

#define TIOC            _IO('t', 0)      /* Specifies ioctl group */

#define LDIOC           ('D'<<8)
#define LDOPEN          (LDIOC|0)
#define LDCLOSE         (LDIOC|1)
#define LDCHG           (LDIOC|2)
#define LDGETT          (LDIOC|8)
#define LDSETT          (LDIOC|9)

/*
 * Terminal types
 */
#define TERM_NONE       0       /* tty */
#define TERM_TEC        1       /* TEC Scope */
#define TERM_V61        2       /* DEC VT61 */
#define TERM_V10        3       /* DEC VT100 */
#define TERM_TEX        4       /* Tektronix 4023 */
#define TERM_D40        5       /* TTY Mod 40/1 */
#define TERM_H45        6       /* Hewlitt-Packard 45 */
#define TERM_D42        7       /* TTY Mod 40/2B */

/*
 * Terminal flags
 */
#define TM_NONE         0000    /* use default flags */
#define TM_SNL          0001    /* special newline flag */
#define TM_ANL          0002    /* auto newline on column 80 */
#define TM_LCF          0004    /* last col of last row special */
#define TM_CECHO        0010    /* echo terminal cursor control */
#define TM_CINVIS       0020    /* do not send esc seq to user */
#define TM_SET          0200    /* must be on to set/res flags */

/*
 * structure of ioctl arg for LDGETT and LDSETT
 */
struct  termcb  {
        char    st_flgs;        /* term flags */
        char    st_termt;       /* term type */
        char    st_crow;        /* gtty only - current row */
        char    st_ccol;        /* gtty only - current col */
        char    st_vrow;        /* variable row */
        char    st_lrow;        /* last row */
};

#endif /* _SYS_TERMIO_H_ */
