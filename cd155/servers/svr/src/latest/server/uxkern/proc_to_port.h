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
/* #$# */
/*
 * HISTORY
 * $Log: proc_to_port.h,v $
 * Revision 1.5  1994/11/18  20:49:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:43:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:05:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:31:58  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:55:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:54:50  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1993/01/07  11:15:48  condict
 * 	pport_to_proc_wakeup() now returns a value.
 * 	[1992/09/24  19:57:24  emcmanus]
 *
 * Revision 2.3  91/10/17  10:07:37  sjs
 * fixed RCS comments
 * 
 * Revision 2.2  91/10/14  13:24:32  sjs
 * Check in.
 * 
 * Revision 3.0  91/09/27  11:54:45  emcmanus
 * 	Installed for profiling.
 * 
 * Revision 1.0  91/05/24  11:36:13  adn
 * 	Created.
 * 	[91/04/01            adn]
 * 
 * $EndLog$
 */

/*****************************************************************/
/*
 * Profiling_port ->process and process->profiling_port routines.
 */
/*****************************************************************/

#ifndef	PROC_TO_PORT_H
#define	PROC_TO_PORT_H

extern void	   pport_to_proc_init();

extern mach_port_t pport_to_proc_enter();	/* struct proc *  */

extern struct proc *
		   pport_to_proc_lookup();	/* mach_port_t */

extern void	   pport_to_proc_remove();	/* mach_port_t */

extern void	   pport_to_proc_sleep();	/* mach_port_t */

extern int	   pport_to_proc_wakeup();	/* mach_port_t */

#endif	PROC_TO_PORT_H
/* #$# */
