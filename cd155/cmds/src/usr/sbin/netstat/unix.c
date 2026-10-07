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
 * Copyright (c) 1983, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
/* Updated to some of: "@(#)unix.c	5.9 (Berkeley) 6/18/90"; */
static char sccsid[] = "%W% (Berkeley) %G%";
#endif /* not lint */

#include <cthreads.h>
#include <mach/memory_object.h>
#include <mach/port.h>

/*
 * Display protocol blocks in the unix domain.
 */
#include <sys/param.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#define _SOCKADDR_LEN		/* XXX */
#include <sys/socketvar.h>
#include <sys/mbuf.h>
#include <sys/un.h>
#include <sys/unpcb.h>
#if	MACH
#define	KERNEL_FILE
#else
#define	KERNEL
#endif
#include <sys/file.h>
#if	MACH
struct	file *file, *fileNFILE;
int	nfile;
#endif

#ifdef OSF1_ADFS
#define TABLE_UNPCB		/* include inet pcb structures */
#include <sys/table.h>
#endif

extern int	Aflag;
extern int	kmem;
extern	char *calloc();

kmem_unixpr(nfileaddr, fileaddr, unixsw)
	off_t nfileaddr, fileaddr;
	struct protosw *unixsw;
{
	register struct file *fp;
	struct file *filep;
	struct socket sock, *so = &sock;

	if (nfileaddr == 0 || fileaddr == 0) {
		printf("nfile or file not in namelist.\n");
		return;
	}
	klseek(kmem, nfileaddr, L_SET);
	if (read(kmem, (char *)&nfile, sizeof (nfile)) != sizeof (nfile)) {
		printf("nfile: bad read.\n");
		return;
	}
	klseek(kmem, fileaddr, L_SET);
	if (read(kmem, (char *)&filep, sizeof (filep)) != sizeof (filep)) {
		printf("File table address, bad read.\n");
		return;
	}
	file = (struct file *)calloc(nfile, sizeof (struct file));
	if (file == (struct file *)0) {
		printf("Out of memory (file table).\n");
		return;
	}
	klseek(kmem, (off_t)filep, L_SET);
	if (read(kmem, (char *)file, nfile * sizeof (struct file)) !=
	    nfile * sizeof (struct file)) {
		printf("File table read error.\n");
		return;
	}
	fileNFILE = file + nfile;
	for (fp = file; fp < fileNFILE; fp++) {
		if (fp->f_count == 0 || fp->f_type != DTYPE_SOCKET)
			continue;
		klseek(kmem, (off_t)fp->f_data, L_SET);
		if (read(kmem, (char *)so, sizeof (*so)) != sizeof (*so))
			continue;
		/* kludge */
		if (so->so_proto >= unixsw && so->so_proto <= unixsw + 2)
			if (so->so_pcb)
				unixdomainpr(so, fp->f_data);
	}
	free((char *)file);
}

static	char *socktype[] =
    { "#0", "stream", "dgram", "raw", "rdm", "seqpacket" };

unixdomainpr(so, soaddr)
	register struct socket *so;
	caddr_t soaddr;
{
	struct unpcb unpcb, *unp = &unpcb;
	struct mbuf mbuf, *m;
	struct sockaddr_un *sa;
	static int first = 1;

	klseek(kmem, (off_t)so->so_pcb, L_SET);
	if (read(kmem, (char *)unp, sizeof (*unp)) != sizeof (*unp))
		return;
	if (unp->unp_addr) {
		m = &mbuf;
		klseek(kmem, (off_t)unp->unp_addr, L_SET);
		if (read(kmem, (char *)m, sizeof (*m)) != sizeof (*m))
			m = (struct mbuf *)0;
		sa = (struct sockaddr_un *)(m->m_dat);
	} else
		m = (struct mbuf *)0;
	if (first) {
		printf("Active UNIX domain sockets\n");
		printf(
"%-8.8s %-6.6s %-6.6s %-6.6s %8.8s %8.8s %8.8s %8.8s Addr\n",
		    "Address", "Type", "Recv-Q", "Send-Q",
		    "Inode", "Conn", "Refs", "Nextref");
		first = 0;
	}
	printf("%8x %-6.6s %6d %6d %8x %8x %8x %8x",
	    soaddr, socktype[so->so_type], so->so_rcv.sb_cc, so->so_snd.sb_cc,
#ifdef OSF1_ADFS
	    unp->unp_vnodeport,
#else
	    unp->unp_vnode,
#endif
	    unp->unp_conn, unp->unp_refs, unp->unp_nextref);
	if (m)
		printf(" %.*s", m->m_len - sizeof(sa->sun_family),
		    sa->sun_path);
	putchar('\n');
}

void
table_unixpr()
{
	struct tbl_unpcb u;
	int index = 0, rc = 0;
	static int first = 1;

	while ((rc = table(TBL_UNPCB, index++, &u, 1, sizeof(u))) == 1) {
		if (first) {
			printf("Active UNIX domain sockets\n");
			printf(
"%-8.8s %-6.6s %-6.6s %-6.6s %8.8s %8.8s %8.8s %8.8s Addr\n",
			    "Address", "Type", "Recv-Q", "Send-Q",
			    "Inode", "Conn", "Refs", "Nextref");
			first = 0;
		}
		printf("%8x %-6.6s %6d %6d %8x %8x %8x %8x",
			u.unpcb.unp_socket, socktype[u.so_type],
			u.so_rcv.sb_cc, u.so_snd.sb_cc,
#ifdef OSF1_ADFS
	    		u.unpcb.unp_vnodeport,
#else
	    		u.unpcb.unp_vnode,
#endif
			u.unpcb.unp_conn,
			u.unpcb.unp_refs, u.unpcb.unp_nextref);
		if (u.unpcb.unp_addr)
			printf(" %.*s",
				u.unp_addr.sun_len
					- sizeof(u.unp_addr.sun_len)
					- sizeof(u.unp_addr.sun_family),
				u.unp_addr.sun_path);
		putchar('\n');
	}
	if (rc < 0)
		perror("table: TBL_UNPCB");
	if (rc > 0)
		printf("TBL_UNPCB: returned %d entries, expected 1\n", rc);
}

unixpr(nfileaddr, fileaddr, unixsw)
	off_t nfileaddr, fileaddr;
	struct protosw *unixsw;
{
	if (kmem >= 0)
		kmem_unixpr(nfileaddr, fileaddr, unixsw);
	else
		table_unixpr();
}
