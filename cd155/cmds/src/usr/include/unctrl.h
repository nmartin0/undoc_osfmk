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
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)unctrl.h	1.4  com/inc,3.1,8943 3/4/89 15:32:17 */
/* unctrl.h	5.1 - 86/12/09 - 06:06:38 */
/*
 * unctrl.h
 *
*/
#ifndef _UNCTRL_H_
#define _UNCTRL_H_

#ifndef unctrl
extern char	*_unctrl[];

# define	unctrl(ch)	(_unctrl[(unsigned) ch])
#endif

#endif /* _UNCTRL_H_ */
