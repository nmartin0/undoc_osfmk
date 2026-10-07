/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * mach_msg_stub routines designed to support NORMA2 IPC testing.
 *
 * $Id: fake.c,v 1.3 1994/11/18 20:58:28 mtm Exp $
 */


/*
 * temp until we configure out the 'Mach IPC' modules from mcmsg.
 */

netipc_recv_intr()
{
	panic("netipc_recv_intr()?");
}

netipc_send_intr()
{
	panic("netipc_recv_intr()?");
}

