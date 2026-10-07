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
 * $Log: stub.c,v $
 * Revision 1.3  1994/11/18  20:31:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  22:21:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:59  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/12/16  12:58:23  roy
 * 	Initial check-in.
 *
 * Revision 3.0  91/11/26  15:32:01  sp
 * Add new file a la integrated kernel for xenix stubs
 * 
 * Revision 1.3  90/12/06  14:02:28  devrcs
 * 	Cleanup copyright and history log comments.
 * 	[90/11/15  11:06:13  gm]
 * 
 * Revision 1.2  90/10/31  13:57:21  devrcs
 * 	Stub routines for Xenix compatibility.
 * 	[90/10/08  13:36:47  swallace]
 * 
 * $EndLog$
 */


/* The clx system call */
/* ARGUSED */
clx_sys(p, uap, retval)
	struct proc *p;
	void *uap;
	int *retval;
{
	printf("clx_sys()\n");
	return (0);
}


clx_exit()
{
}

xenix_sem_cleanup(flip)
	register struct flino *flip;
{
}
