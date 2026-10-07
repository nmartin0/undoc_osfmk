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
 * HISTORY
 * $Log: credential.h,v $
 * Revision 1.5  1994/11/18  20:47:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:40:18  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:59:31  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:25:52  nandy
 * ad103+tnc merged with Intel code.
 *
 *
 * Revision 1.1.2.1  1992/11/05  23:42:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:50:54  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/01/14  11:31:45  roy
 * 	Simplified credential service for now (no caching).
 *
 * Revision 2.2  91/12/08  09:43:59  rabii
 * 	Initial check in
 * 
 */

#ifndef _CREDENTIALS_H_
#define _CREDENTIALS_H_


#define IS_A_CRED 0x10

struct cr_message {
	mach_msg_header_t	head;
	char			body[300];
};

struct master_entry {
	int		cookie;
	struct mutex	entlock;
	cred_t		rcred;
	int		cmask;
	pid_t		pid;
	pid_t		pgid;
	pid_t		sid;
	task_t		task;
	struct rlimit	rlimit[RLIM_NLIMITS];
	long		blkin;
	long		blkout;
	int		ticket;
};

struct cache_entry {
	mach_port_t	cport;
	cred_t 		rcred;
	pid_t		pid;
	pid_t		pgid;
	pid_t		sid;
	int		cmask;
	task_t		task;
	struct rlimit	rlimit[RLIM_NLIMITS];
	long		blkin;
	long		blkout;
	int		ticket;
	struct proc	*p;
};

#endif /* _CREDENTIALS_H_ */
