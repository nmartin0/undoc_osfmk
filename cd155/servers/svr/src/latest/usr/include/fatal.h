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
 * HISTORY
 * $Log: fatal.h,v $
 * Revision 1.3  1994/11/18  21:09:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:19  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:38  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:16  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:35:20  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:27:17  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:45:12  gm]
 * 
 * Revision 1.2  90/03/13  21:22:09  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:09:45  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)fatal.h	1.4  com/inc,3.1,8943 3/4/89 15:15:48 */
/* fatal.h	5.1 - 86/12/09 - 06:04:27 */
#ifndef _FATAL_H_

#define _FATAL_H_        1
#include <setjmp.h>

extern	int	Fflags;
extern	char	*Ffile;
extern	int	Fvalue;
extern	int	(*Ffunc)();
extern  jmp_buf Fjmp;

# define FTLMSG		0100000
# define FTLCLN		 040000
# define FTLFUNC	 020000
# define FTLACT		    077
# define FTLJMP		     02
# define FTLEXIT	     01
# define FTLRET		      0

# define FSAVE(val)	SAVE(Fflags,old_Fflags); Fflags = val;
# define FRSTR()	RSTR(Fflags,old_Fflags);

#endif /* _FATAL_H_ */
