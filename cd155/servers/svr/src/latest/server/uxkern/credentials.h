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
 * $Log: credentials.h,v $
 * Revision 1.4  1994/11/18  20:47:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:40:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:59:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:53:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:28  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:09  cfj
 * Bump major revision number.
 *
 * Revision 2.6  1992/09/11  09:29:55  rabii
 * 	Defined the new flag "CM_EXITING" indicating that a cred master entry
 * 	is in process of being deallocated (rabii).
 *
 * Revision 2.5  92/06/30  22:47:45  loverso
 * 	Added ruid and rgid to master and cache structures. (rabii)
 * 
 * Revision 2.4  92/04/05  17:10:58  pjg
 * 	Minor cleanup (rabii).
 * 
 * Revision 2.3  92/03/20  11:34:14  pjg
 * 	Moved fileserver_port_entry to fsvr_port.c.
 * 
 * Revision 2.2  92/03/01  18:36:13  pjg
 * 	Added caching back (rabii)
 * 
 * Revision 2.3  92/01/14  11:31:45  roy
 * 	Simplified credential service for now (no caching).
 * 
 * Revision 2.2  91/12/08  09:43:59  rabii
 * 	Initial check in
 * 
 */

#ifndef _CREDENTIALS_H_
#define _CREDENTIALS_H_


/*
 * HASHing functions 
 * CPORT_HASH used for credentials ports
 * NODE_HASH used for node number hashing
 */
#define CPORT_HASH(port) ((((port) >> 8) & 0xff) ^ ((port) & 0xff)) \
				 & (HASH_Q_NO - 1)

#define NODE_HASH(node) (node % HASH_Q_NO)

#define HASH_Q_NO	16
#define MAX_CPORT_REF	10000

/* master legal actions */
#define CM_GETRUSAGE	0x0001
#define CM_MIGRATE	0x0002
#define CM_UPDATE	0x0003
#define CM_DEALLOCATE	0x0004

/* master legal flags */
#define CM_BUSY		0x0005
#define CM_FREE		0x0006
#define CM_EXITING	0x0007

/* cache legal flags */
#define CC_FREE		0x0001
#define CC_BUSY		0x0002

/* master-cache legal return values */
#define CE_GONE		0x000a
#define CE_RETRY	0x000b

/*
 * Master structures
 */
struct master_entry {
	struct queue_entry	hlink;
	mach_port_t	cport;
	queue_head_t	serversq;
	int		nsvr;
	struct ucred	rcred;
	int		cmask;
	pid_t		pid;
	pid_t		pgid;
	pid_t		sid;
	uid_t		ruid;
	uid_t		rgid;
	task_t		task;
	unsigned int	flimcur;
	unsigned int	flimmax;
	long		blkin;
	long		blkout;
	short		flag;
};

struct master_servers {
	struct queue_entry	link;
	mach_port_t		fileserver_port;
	node_t			node;
};

/*
 * Cache structures
 */
struct cache_entry {
	struct queue_entry	hlink;
	mach_port_t	cport;
	struct ucred	*rcredp;
	int		ref;
	pid_t		pid;
	pid_t		pgid;
	pid_t		sid;
	uid_t		ruid;
	uid_t		rgid;
	int		cmask;
	task_t		task;
	unsigned int	flimcur;
	unsigned int	flimmax;
	long		blkin;
	long		blkout;
	int		ticket;
	short		flag;
	int		cport_ref;
};

#endif /* _CREDENTIALS_H_ */
