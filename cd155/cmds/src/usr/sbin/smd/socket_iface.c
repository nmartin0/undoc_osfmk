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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/socket_iface.c,v 1.4 1994/12/19 19:46:09 sdh Exp $
 *
 * HISTORY
 * $Log: socket_iface.c,v $
 * Revision 1.4  1994/12/19  19:46:09  sdh
 * Merged R1.2 fixes into R1.3.
 *
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 11511
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd, nqs
 * 	manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/smd/allocator.c
 *       	 cmds_libs/src/usr/sbin/smd/app_db.c
 *       	 cmds_libs/src/usr/sbin/smd/app_service.c
 *       	 cmds_libs/src/usr/sbin/smd/init.c
 *       	 cmds_libs/src/usr/sbin/smd/main.c
 *       	 cmds_libs/src/usr/sbin/smd/response.c
 *       	 cmds_libs/src/usr/sbin/smd/service_db.c
 *       	 cmds_libs/src/usr/sbin/smd/socket_iface.c
 *       	 cmds_libs/src/usr/sbin/smd/user_db.c
 * 	 cmds_libs/src/usr/sbin/smd/Makefile
 *
 * Revision 1.3  1994/11/19  03:16:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2.12.2  1994/05/13  16:24:52  sdh
 *  Fixed the 9307 fix. Moved the return (-1) in read_msg that was in an
 *  if to out of the if statement.
 *  Reviewer: jkearns, doyle
 *  Risk: low
 *  Benefit or PTS #: 9418
 *  Testing: EATS: nqs, macs, sched
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/smd/socket_iface.c
 *
 * Revision 1.2.12.1  1994/05/10  03:16:23  sdh
 * 	The problem was that the code had one check for both a 0 (zero)
 * 	return and a negative return and check errno if it received either.
 *  	Unfortunately, errno is undefined if the return was a zero and some
 *  	values could cause smd to go into an infinite loop. The fix was to
 *  	break out an error check that check for both an error return and
 *  	a 0 (zero) return into 2 checks. One for a negative (error) return and
 *  	one for a 0 (zero) return.
 *  Reviewer: jkearns
 *  Risk: low
 *  Benefit or PTS #: 9307
 *  Testing: EATS: nqs, macs, sched, controlc, rmcall, rmcmd
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/smd/socket_iface.c
 *
 * Revision 1.2  1993/01/19  20:12:53  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.1  1992/10/05  23:30:27  rkl
 * Initial revision
 * 
 *
 */

#include <fcntl.h>
#include <sys/socket.h>
#ifdef INET_SOCKET
#include <netinet/in.h>
#include <netdb.h>
#else
#include <sys/un.h>
#endif
#include <stdio.h>
#include <sys/errno.h>
#include "defs.h"

/*
 *  socket_init:
 *
 *	Initialize the UNIX domain socket interface.
 */
socket_init()
{
#ifdef INET_SOCKET
	struct	sockaddr_in	req;
	int			on = 1;
#else
	struct	sockaddr_un	req;
#endif

	TRACE(ENTRY, ("socket_init()\n"));

	/*
	 *  Clear the selects masks.
	 */
	FD_ZERO(&readmask);
	FD_ZERO(&writemask);
	FD_ZERO(&exceptmask);

	/*
	 *  Make sure the old socket is not around.
	 */
#ifndef INET_SOCKET
	unlink(iface_name);
#endif

	/*
	 *  Create the socket.
	 */
#ifdef INET_SOCKET
	req_fd = socket(AF_INET, SOCK_STREAM, 0);
#else
	req_fd = socket(AF_UNIX, SOCK_STREAM, 0);
#endif
	if (req_fd < 0) {
		perror("socket_init - socket()");
		exit(1);
	}
	TRACE(SOCKET, ("req_fd = %d\n", req_fd));

	/*
	 *  Bind the socket.
	 */
#ifdef INET_SOCKET
	if (setsockopt(req_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("socket_init - setsockopt()");
		exit(1);
	}

	req.sin_family      = AF_INET;
	req.sin_addr.s_addr = INADDR_ANY;
	req.sin_port        = htons(SMD_IFACE_PORT);
#else
	req.sun_family = AF_UNIX;
	strcpy(req.sun_path, iface_name);
#endif
	if (bind(req_fd, &req, sizeof(req))) {
		perror("socket_init - bind()");
		exit(1);
	}

	/*
	 *  Make sure only privelaged user can write the socket.
	 */
#ifndef INET_SOCKET
	chmod(iface_name, 0600);
#endif

	/*
	 *  Set the request FD in the select mask.
	 */
	FD_SET(req_fd, &readmask);

	/*
	 *  Listen on the created socket.
	 */
	listen(req_fd, 5);
}

/*
 *  connect_user:
 *
 *	Accept a socket connection.
 */
connect_user()
{
	int	fd;

	TRACE(ENTRY, ("connect_user()\n"));

	fd = accept(req_fd, (struct sockaddr*)0, (int*)0);
	if (fd < 0) {
		perror("connect_user - accept");
		return (-1);
	}

	return (fd);
}

/*
 *  read_msg:
 *
 *	Read a message from the interface.
 */
read_msg(user, hdr, max_size)
	smd_user_t	*user;
	smd_hdr_t	*hdr;
	int		max_size;
{
	char	*p;
	int	bytes_left, n;

	TRACE(ENTRY, ("read_msg(usr=%x, hdr=%x, max_size=%d)\n",
							user, hdr, max_size));
	/*
	 *  Read the header and make sure it is sane.
	 *
	 *  NOTE:  We assume header will arrive in one piece.
	 */
	while (1) {
		n = read(user->fd, (char*)hdr, sizeof(*hdr));

		if (n < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				TRACE(SOCKET, ("read error (%d)\n", errno));
				return (-1);
			}
		}
		if (n != sizeof(*hdr)) {
		    if (n == 0){
			TRACE(SOCKET, ("Read 0 bytes from socket %d\n", user->fd));
		    } else {
			TRACE(SOCKET, ("Failed to read the entire header\n"));
		    }
		    return (-1);
		}

		if (hdr->len > (max_size - sizeof(*hdr))) {
			TRACE(SOCKET, ("read_msg: Message too long (%d)\n",
								hdr->len));
			return (-1);
		} else {
			break;
		}
	}

	/*
	 *  Read hdr->len amount of data.  The socket interface does not
	 *  guarantee to block until the specified length is read so we
	 *  may have to make several attempts to get is all.
	 */
	TRACE(SOCKET, ("reading %d bytes from socket %d (",hdr->len,user->fd));

	p = (char*)(hdr + 1);
	bytes_left = hdr->len;
	while(bytes_left) {
		if ((n = read(user->fd, p, bytes_left)) <= 0)
			if (n < 0) {	
				if (errno == EINTR)
					continue;
				else
					return (-1);
			} else { /* n = 0 */
				return (-1);
			}
		p += n;
		bytes_left -= n;
		TRACE(SOCKET, ("+", bytes_left));
	}
	TRACE(SOCKET, (")\n", bytes_left));
	return (hdr->len);
}

/*
 *  send_msg:
 *
 *	Send message through socket.
 */
send_msg(user, buf, len)
	smd_user_t	*user;
	char		*buf;
	int		len;
{
	TRACE(ENTRY, ("send_msg(usr=%x, buf=%x, len=%d)\n", user, buf, len));
	if (write(user->fd, buf, len) < 0)
		perror ("send_msg - write");
}

/*
 *  format_sock:
 *
 *	Format a socket address structure to be sent to the allocator.
 */
format_sock_addr(addr, len)
	u_char	**addr;
	int	*len;
{
#ifdef INET_SOCKET
	static	struct	sockaddr_in	sock;
	struct	hostent			*hp;
#else
	static	struct	sockaddr_un	sock;
#endif

#ifdef INET_SOCKET
	sock.sin_family      = AF_INET;
	sock.sin_port        = htons(SMD_IFACE_PORT);
	hp = gethostbyname("localhost");
	bcopy(hp->h_addr, &sock.sin_addr.s_addr, hp->h_length);
#else
	sock.sun_family = AF_UNIX;
	strcpy(sock.sun_path, SMD_IFACE_NAME);
#endif

	*addr = (u_char*) &sock;
	*len  = sizeof(sock);
}
