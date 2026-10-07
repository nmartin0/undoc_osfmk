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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: dumprmt.c,v $
 * Revision 1.3  1994/11/19  03:05:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  21:46:13  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 2.9  90/10/07  22:08:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:09:31  gm]
 * 
 * Revision 2.8  90/08/25  12:28:54  devrcs
 * 	insert rcsid string;
 * 	delete old sccs id string;
 * 	[90/08/09  03:57:30  nm]
 * 
 * Revision 2.7  90/08/09  14:46:06  devrcs
 * 	delete mode from rmtopen to be consistent with the
 * 	dump protocol for remote dumping;
 * 	[90/08/02  09:04:22  nm]
 * 
 * Revision 2.6  90/07/27  11:19:21  devrcs
 * 	use SERVER_PATH define in rcmd call
 * 	[90/06/22  06:38:25  nm]
 * 
 * 	integration to osc.12
 * 	[90/05/31  04:38:23  nm]
 * 
 * Revision 2.5  90/06/22  22:11:58  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:38:23  nm]
 * 
 * 	integration to osc.12
 * 	[90/05/31  04:38:23  nm]
 * 
 * Revision 2.4  90/04/14  00:12:55  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:11:52  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * Revision 2.3  90/01/02  18:41:35  gm
 * 	Fixes for first snapshot.
 * 	[90/01/02            gm]
 * 
 * Revision 2.2  89/12/26  08:57:26  gm
 * 	Current version from CMU.
 * 	[89/12/21            gm]
 * 
 * 	Change include files for SUNOS vs MACH. Also test for SO_SNDBUF
 * 	option. If not defined then don't try to do.
 * 	[89/08/31  21:38:13  ern]
 * 
 * 	Add a non-acking write routing which sees if any messages have
 * 	come in.  if so then it uses a acked write call. Also change
 * 	the ack reading stuff to ignore a noop message. Finally add a
 * 	routine which is acked indicating if any problems have happen
 * 	during unacked writes. All this adds up to back to back writes
 * 	without waiting for acks and detection of errors so they can be
 * 	handled. 16 or more percent performance improvement.
 * 	[89/08/16  13:25:53  ern]
 * 
 * 	Add edump support.
 * 	[89/06/12  22:32:40  ern]
 * 
 * $EndLog$
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: dumprmt.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:05:37 $";
#endif

/*
 * This module contains IBM CONFIDENTIAL code. -- (IBM Confidential Restricted
 * when combined with the aggregated modules for this product) OBJECT CODE ONLY
 * SOURCE MATERIALS (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1980 Regents of the University of California. All
 * rights reserved.  The Berkeley software License Agreement specifies the
 * terms and conditions for redistribution.
 */

#include	"dump.h"

#include	<sys/types.h>
#include	<sys/ioctl.h>
#include	<sys/mtio.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<pwd.h>
#include	<netdb.h>

#define	TS_CLOSED	0
#define	TS_OPEN		1

static char		rmt_getb();
static int		rmt_call();
static int		rmt_reply();
static struct mtget    *rmt_status();
static void		rmt_abort();
static void		rmt_error();
static void		rmt_gets();
static void		rmt_lostconn();
static void		rmt_perror();

static int		rmt_state = TS_CLOSED;
static int		rmt_socket_fd;

static void
rmt_lostconn()
{

	msg(MSGSTR(LOSTCON, "Lost connection to remote peer\n"));
	rmt_abort();

	/* NOTREACHED */
}

static void
rmt_error()
{
	msg(MSGSTR(RERR, "Communication error in connection to remote peer\n"));
	rmt_abort();

	/* NOTREACHED */
}

static void
rmt_perror(where)
	char	       *where;
{

#if	DUMP

	dump_perror(where);

#else	RESTORE

	restore_perror(where);

#endif	RESTORE

}

static void
rmt_abort()
{

#if	DUMP

	abort_dump();

#else	RESTORE

	abort_restore();

#endif	RESTORE

	/* NOTREACHED */
}

/* The arg to rmthost is a pointer to the pointer to the remote host name. */
/* This is done so that rcmd can change the pointer to the standard name of */
/* the remote host. */

void
rmthost(rmt_peer_name)
	char	      **rmt_peer_name;
{
	int		size;

#if	EDUMP

	rmt_socket_fd = 1;	/* use stdout */

#else	! EDUMP

	struct servent *sp;
	struct passwd  *pw;
	char	       *user_name;

	(void) signal(SIGPIPE, rmt_lostconn);

	/* get the entry of the shell/tcp service so we know what port */
	/* number it is on */

	sp = getservbyname("shell", "tcp");
	if (sp == NULL)
	{
		msg(MSGSTR(UNKSERV, "rdump: shell/tcp: unknown service\n"));
		rmt_abort();

		/* NOTREACHED */
	}

	/* get the uid name string of the current process so we can */
	/* rcmd() on the remote machine under that name */

	pw = getpwuid(getuid());
	if (pw != NULL && pw->pw_name != NULL)
	{
		user_name = pw->pw_name;
	}
	else
	{
		user_name = "root";
	}

	/* establish a connection to a rmt command on the remote machine */

	rmt_socket_fd = rcmd(rmt_peer_name, sp->s_port, user_name, user_name, SERVER_PATH, (int *) 0);
	if (rmt_socket_fd < 0)
	{
		msg(MSGSTR(NOCON, "Cannot establish connection to remote rmt command\n"));
		rmt_perror("rmthost(): rcmd()");
		rmt_abort();

		/* NOTREACHED */
	}

#endif	! EDUMP

#if	SO_SNDBUF

	/* set the socket send-buffer size to the largest multiple of the */
	/* tape block size as possible (provided it is smaller than a full */
	/* tape write size) */

	for (size = blocks_per_write * TP_BSIZE; size > TP_BSIZE; size -= TP_BSIZE)
	{
		if (setsockopt(rmt_socket_fd, SOL_SOCKET, SO_SNDBUF, (char *) &size, sizeof(int)) >= 0)
		{
			return;
		}
	}

	msg(MSGSTR(SOCSZ, "Cannot set socket send-buffer size\n"));
	rmt_perror("rmthost(): setsockopt()");
	rmt_abort();

	/* NOTREACHED */

#endif	SO_SNDBUF

}

int
rmtopen(tape_device, flags)
	char	       *tape_device;
	int		flags;
{
	char		buf[256];
	int		open_value;

	(void) sprintf(buf, "O%s\n%d\n", tape_device, flags);
	open_value = rmt_call("open()", buf);

	if (open_value >= 0)
	{
		rmt_state = TS_OPEN;
	}
	return(open_value);
}

void
rmtclose()
{
	if (rmt_state != TS_OPEN)
	{
		return;
	}
	(void) rmt_call("close()", "C\n");
	rmt_state = TS_CLOSED;
}

int
rmtread(buf, count)
	char	       *buf;
	int		count;
{
	char		line[256];
	int		n, i, cc;

	(void) sprintf(line, "R%d\n", count);
	n = rmt_call("read()", line);
	if (n < 0)
	{
		errno = n;
		return(-1);
	}
	for (i = 0; i < n; i += cc)
	{
		cc = read(rmt_socket_fd, buf + i, n - i);
		if (cc <= 0)
		{
			msg(MSGSTR(RRBN, "Cannot read buffer from socket\n"));
			rmt_perror("rmtread(): read()");
			rmt_error();

			/* NOTREACHED */
		}
	}
	return(n);
}

#if	NOTUSED

/*
 * non-blocking write 
 *
 * rmtAwrite() writes the buffer to the remote host with a op-code
 * of 'V', instead of 'W', and does not wait around for acknowledgement,
 * because there will not be any unless there is an error.
 *
 * When an error occurs on the remote end, an error acknowledgement
 * will be sent back to this end, and the next rmtAwrite call will
 * detect this when it calls ioctl(FIONREAD).  In this case, pass the
 * write request to rmtwrite(), which will get the error message when
 * it finally reaches rmt_reply(), at which point the tape error will
 * be signalled back to the master process.
 *
 * Note that rmt does not yet have the 'V', 'F', and 'X' op-codes implemented.
 * So this code here is not used.
 */

int
rmtAwrite(buf, count)
	char	       *buf;
	int		count;
{
	char		line[256];
	int		mycnt;

	if (ioctl(rmt_socket_fd, FIONREAD, (char*) &mycnt) < 0)
	{
		mycnt = 0;
	}
	if (mycnt != 0)
	{
		return(rmtwrite(buf, count));
	}

	(void) sprintf(line, "V%d\n", count);
	if (write(rmt_socket_fd, line, strlen(line)) != strlen(line))
	{
		msg(MSGSTR(RWWLN, "Cannot write command line to socket\n"));
		rmt_perror("rmtAwrite(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	if (write(rmt_socket_fd, buf, count) != count)
	{
		msg(MSGSTR(RWBN, "Cannot write buffer to socket\n"));
		rmt_perror("rmtAwrite(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	return(count);
}

/*
 * check for pending writes 
 *
 * rmtAflush() simply sends a flush op-code to the remote host and
 * waits around for the acknowledgement
 */

int
rmtAflush()
{
	char	line[256];

	(void) sprintf(line, "F0\n");
	if (write(rmt_socket_fd, line, strlen(line)) != strlen(line))
	{
		msg(MSGSTR(RWWLN, "Cannot write command line to socket\n"));
		rmt_perror("rmtAflush(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	return(rmt_reply("flush()"));
}

#if	EDUMP

/*
 * rmtsmsg- causes the passed string to be printed on the terminal
 *	 at the other end of the connection (ie: rmt's /dev/tty).
 *	 This can only be done WHEN there isn't more then one dump
 *	 process running. These are special messages.
 */

int
rmtsmsg(buf)
	char	       *buf;
{
	int		count;
	char		line[256];

	count = strlen(buf);
	(void) sprintf(line, "X%d\n", count);
	if (write(rmt_socket_fd, line, strlen(line)) != strlen(line))
	{
		msg(MSGSTR(RWWLN, "Cannot write command line to socket\n"));
		rmt_perror("rmtsmsg(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	if (write(rmt_socket_fd, buf, count) != count)
	{
		msg(MSGSTR(RWBN, "Cannot write buffer to socket\n"));
		rmt_perror("rmtsmsg(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	return(rmt_reply("message()"));
}

#endif	EDUMP

#endif	NOTUSED

int
rmtwrite(buf, count)
	char	       *buf;
	int		count;
{
	char		line[256];

	(void) sprintf(line, "W%d\n", count);
	if (write(rmt_socket_fd, line, strlen(line)) != strlen(line))
	{
		msg(MSGSTR(RWWLN, "Cannot write command line to socket\n"));
		rmt_perror("rmtwrite(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	if (write(rmt_socket_fd, buf, count) != count)
	{
		msg(MSGSTR(RWBN, "Cannot write buffer to socket\n"));
		rmt_perror("rmtwrite(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	return(rmt_reply("write()"));
}

int
rmtseek(offset, pos)
	int		offset, pos;
{
	char		line[256];

	(void) sprintf(line, "L%d\n%d\n", offset, pos);
	return(rmt_call("seek()", line));
}


static struct mtget   *
rmt_status()
{
	register int	i;
	register char  *cp;
	static struct mtget	mts;

	if (rmt_state != TS_OPEN)
	{
		return(NULL);
	}
	(void) rmt_call("status()", "S\n");
	for (i = 0, cp = (char *) &mts; i < sizeof(struct mtget); ++i)
	{
		*cp++ = rmt_getb();
	}
	return(&mts);
}

int
rmtioctl(cmd, count)
	int		cmd, count;
{
	char		buf[256];

	if (count < 0)
	{
		return(-1);
	}
	(void) sprintf(buf, "I%d\n%d\n", cmd, count);
	return(rmt_call("ioctl()", buf));
}

static int
rmt_call(cmd, buf)
	char	       *cmd, *buf;
{
	int		len_to_write;

	len_to_write = strlen(buf);
	if (write(rmt_socket_fd, buf, len_to_write) != len_to_write)
	{
		msg(MSGSTR(RWBN, "Cannot write buffer to socket\n"));
		rmt_perror("rmt_call(): write()");
		rmt_error();

		/* NOTREACHED */
	}
	return(rmt_reply(cmd));
}

static int
rmt_reply(cmd)
	char	       *cmd;
{
	char		code[256], emsg[BUFSIZ];

	/* ignore attention records */

	for(; ; )
	{
		rmt_gets(code, sizeof(code));
		if (code[0] != 'B')
		{
			break;
		}
	}

	if (code[0] == 'E' || code[0] == 'F')
	{
		rmt_gets(emsg, sizeof(emsg));
		msg("%s: %s: %s\n", cmd, emsg, code + 1);
		if (code[0] == 'F')
		{
			rmt_state = TS_CLOSED;
		}
		return(-1);
	}

	if (code[0] != 'A')
	{
		msg(MSGSTR(PROTO, "rmt_reply(): Protocol to remote tape server botched (code %s)\n"), code);
		rmt_error();

		/* NOTREACHED */
	}
	return(atoi(&code[1]));
}

static char
rmt_getb()
{
	char		c;

	if (read(rmt_socket_fd, &c, 1) != 1)
	{
		msg(MSGSTR(RRSCN, "Cannot read single character from socket\n"));
		rmt_perror("rmt_getb(): read()");
		rmt_error();

		/* NOTREACHED */
	}
	return(c);
}

static void
rmt_gets(cp, len)
	char	       *cp;
	int		len;
{
	while (len > 1)
	{
		*cp = rmt_getb();
		if (*cp == '\n')
		{
			*(cp + 1) = '\0';
			return;
		}
		++cp;
		--len;
	}
	msg(MSGSTR(PROTO1, "rmt_gets(): Protocol to remote tape server botched\n"));
	rmt_error();

	/* NOTREACHED */
}
