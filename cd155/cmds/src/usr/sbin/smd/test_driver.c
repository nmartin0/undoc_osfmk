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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/test_driver.c,v 1.2 1994/11/19 03:16:50 mtm Exp $
 *
 * HISTORY
 * $Log: test_driver.c,v $
 * Revision 1.2  1994/11/19  03:16:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/05  23:30:27  rkl
 * Initial revision
 *
 *
 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#ifdef INET_SOCKET
#include <netinet/in.h>
#include <netdb.h>
#else
#include <sys/un.h>
#endif
#include <sys/errno.h>
#include <stdio.h>
#include <sys/signal.h>
#include "defs.h"



char	*menu = "\
\n\
r) generate a user request\n\
s) generate a scheduler indication\n\
o) open a connection\n\
c) close connection\n\
q) quit\n\
\n\
";

fd_set  readmask;               /* read select mask */

sig_catch()
{
	printf("Caught signal\n");
}
main()
{
	int			fd, i;
#ifdef INET_SOCKET
	struct	sockaddr_in	addr;
	struct	hostent		*hp;
#else
	struct sockaddr_un	addr;
#endif
	smd_req_t		req;
	allocator_ind_t		ind;
	char			buf[80];
	fd_set			tmpmask;

	signal(SIGUSR1, sig_catch);

	FD_ZERO(&readmask);
	FD_SET(0, &readmask);	/* add stdin */

	while (1) {
		printf(menu);

		bcopy(&readmask, &tmpmask, sizeof(readmask));
		if (select(64, &tmpmask, (fd_set*)0, (fd_set*)0, 0) < 0) {
			if (errno != EINTR)
                                perror("select");
			continue;
                }

		if (FD_ISSET(0, &tmpmask)) {
		gets(buf);
		switch (buf[0]) {
			case 'o':
#ifdef INET_SOCKET
				fd = socket(AF_INET, SOCK_STREAM, 0);
#else
				fd = socket(AF_UNIX, SOCK_STREAM, 0);
#endif
				if (fd < 0) {
					perror ("socket");
					break;
				}

#ifdef INET_SOCKET
				addr.sin_family      = AF_INET;
				addr.sin_port        = htons(SMD_IFACE_PORT);
				hp = gethostbyname("localhost");
				bcopy(hp->h_addr, &addr.sin_addr.s_addr, hp->h_length);
#else
				addr.sun_family = AF_UNIX;
				strcpy(addr.sun_path, SMD_IFACE_NAME);
#endif
				if (connect(fd, &addr, sizeof(addr)) < 0) {
					close (fd);
					perror ("connect");
					break;
				}
				printf("Connected FD = %d\n", fd);
				FD_SET(fd, &readmask);
				break;

			case 'c':
				close(atoi(&buf[1]));
				FD_CLR(atoi(&buf[1]), &readmask);
				break;

			case 'r':
				make_req(&req);
				write(atoi(&buf[1]), &req, sizeof(req));
				break;

			case 's':
				make_ind(&ind);
				write(atoi(&buf[1]), &ind, sizeof(ind));
				break;

			case 'q':
				return(1);
		}
	}

	for (i = 1; i < 64; i++) {
		if (FD_ISSET(i, &tmpmask)) {
			if (dump_incoming(i) < 0) {
				printf("Closing FD %d\n", i);
				close(i);
				FD_CLR(i, &readmask);
			}
		}
	}
	}
}

dump_incoming(fd)
int	fd;
{
	smd_hdr_t	hdr;
	smd_resp_t	resp;

	if (read(fd, &hdr, sizeof(smd_hdr_t)) <= 0) {
		perror("read");
		return (-1);
	}

	printf("len  = %d\n", hdr.len);
	printf("type = %d\n", hdr.type);

	while (hdr.len) {
		if (read(fd, &resp, sizeof(smd_resp_t)) <= 0) {
			perror("read");
			return (-1);
		}

		printf("pgid          = %d\n", resp.q_id.pgid);
		printf("part_id       = %d\n", resp.q_id.part_id);
		printf("acct_id       = %d\n", resp.q_id.acct_id);
		printf("uid           = %d\n", resp.q_id.uid);
		printf("req_id        = %s\n", resp.q_id.req_id);
		printf("status        = %d\n", resp.value.status);
		printf("global_time   = %d\n", resp.value.global_time);
		printf("event_time    = %d\n", resp.value.event_time);
		printf("cpu_time      = %d\n", resp.value.cpu_time);
#ifdef	not_now
		printf("part_avail    = %d\n", resp.value.part_avail);
		printf("part_utilized = %d\n", resp.value.part_utilized);
		printf("part_active   = %d\n", resp.value.part_active);
		printf("part_idle     = %d\n", resp.value.part_idle);
		printf("node_type     = %d\n", resp.value.node_type);
#endif
		printf("app_size      = %d\n", resp.value.app_size);
		printf("app_priority  = %d\n", resp.value.app_priority);
		printf("part_size     = %d\n", resp.value.part_size);

		hdr.len -= sizeof(smd_resp_t);
	}
	return (hdr.len);
}

make_req(p)
smd_req_t	*p;
{
	char	buf[80];

	p->hdr.len = sizeof(smd_req_t) - sizeof(smd_hdr_t);
	printf("Type = ");
	gets(buf);
	p->hdr.type = strtol(buf, 0, 0);

	printf("pgid = ");
	gets(buf);
	p->q_id.pgid = strtol(buf, 0, 0);

	printf("part_id = ");
	gets(buf);
	p->q_id.part_id = strtol(buf, 0, 0);

	printf("acct_id = ");
	gets(buf);
	p->q_id.acct_id = strtol(buf, 0, 0);

	printf("uid = ");
	gets(buf);
	p->q_id.uid = strtol(buf, 0, 0);

	printf("req_id = ");
	gets(buf);
	strcpy(p->q_id.req_id, buf);

	printf("flags = ");
	gets(buf);
	p->param.flags = strtol(buf, 0, 0);

	/*
	printf("sig_num = ");
	gets(buf);
	p->param.sig_num = strtol(buf, 0, 0);
	*/
	p->param.sig_num = SIGUSR1;
	printf("sig_num = %d\n", p->param.sig_num);

	/*
	printf("sig_pid = ");
	gets(buf);
	p->param.sig_pid = strtol(buf, 0, 0);
	*/
	p->param.sig_pid = getpid();
	printf("sig_pid = %d\n", p->param.sig_pid);

	printf("events = ");
	gets(buf);
	p->param.events = strtol(buf, 0, 0);

	printf("alarm_val = ");
	gets(buf);
	p->param.alarm_val = strtol(buf, 0, 0);

	printf("service = ");
	gets(buf);
	p->param.service = strtol(buf, 0, 0);
}

make_ind(p)
allocator_ind_t	*p;
{
	char	buf[80];

	p->hdr.len = sizeof(allocator_ind_t) - sizeof(smd_hdr_t);
	printf("Type = ");
	gets(buf);
	p->hdr.type = strtol(buf, 0, 0);

	printf("pgid = ");
	gets(buf);
	p->data.pgid = strtol(buf, 0, 0);

	printf("acct_id = ");
	gets(buf);
	p->data.acct_id = strtol(buf, 0, 0);

	printf("uid = ");
	gets(buf);
	p->data.uid = strtol(buf, 0, 0);

	printf("app_size = ");
	gets(buf);
	p->data.app_size = strtol(buf, 0, 0);

	printf("app_rollin = ");
	gets(buf);
	p->data.app_rollin = strtol(buf, 0, 0);

	printf("app_priority = ");
	gets(buf);
	p->data.app_priority = strtol(buf, 0, 0);

#ifdef not_now
	printf("node_type = ");
	gets(buf);
	p->data.node_type = strtol(buf, 0, 0);
#endif
	printf("part_id = ");
	gets(buf);
	p->data.part_id = strtol(buf, 0, 0);

	printf("part_size = ");
	gets(buf);
	p->data.part_size = strtol(buf, 0, 0);

	printf("rollin_quantum = ");
	gets(buf);
	p->data.rollin_quantum = strtol(buf, 0, 0);

	printf("part_rollin = ");
	gets(buf);
	p->data.part_rollin = strtol(buf, 0, 0);

	p->data.global_time = time(0);
	p->data.event_time  = time(0);
}
