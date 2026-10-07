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
/** Copyright (c) 1989  Mentat Inc.
 ** tiuser.h 1.4, last change 1/2/90
 **/

#ifndef _TIUSER_H
#define _TIUSER_H

/* Error values */
#define	TACCES		1
#define	TBADADDR	2
#define	TBADDATA	3
#define	TBADF		4
#define	TBADFLAG	5
#define	TBADOPT		6
#define	TBADSEQ		7
#define	TBUFOVFLW	8
#define	TFLOW		9
#define	TLOOK		10
#define	TNOADDR		11
#define	TNODATA		12
#define	TNOREL		13
#define	TNOTSUPPORT	14
#define	TOUTSTATE	15
#define	TSTATECHNG	16
#define TSYSERR		17
#define	TNOUDERR	18
#define	TNODIS		19

/* t_alloc options */
#define	T_BIND		0x1
#define	T_CALL		0x2
#define T_OPTMGMT	0x4
#define	T_DIS		0x8
#define	T_UNITDATA	0x10
#define	T_UDERROR	0x20
#define	T_INFO		0x40
#define	T_ADDR		0x1000
#define	T_OPT		0x2000
#define	T_UDATA		0x4000
#define	T_ALL		0x8000

/* t_look events */
#define	T_LISTEN	1
#define	T_CONNECT	2
#define	T_DATA		3
#define	T_EXDATA	4
#define	T_DISCONNECT	5
#define	T_ORDREL	6
#define	T_ERROR		7
#define	T_UDERR		8

/* User library flags */
#define	T_MORE		0x01
#define	T_EXPEDITED	0x02
#define	T_NEGOTIATE	0x04
#define	T_CHECK		0x08
#define	T_DEFAULT	0x10
#define T_SUCCESS	0x20
#define T_FAILURE	0x40

/* Service types */
#define T_COTS		1	/* Connection-mode service */
#define	T_COTS_ORD	2	/* Connection service with orderly release */
#define	T_CLTS		3	/* Connectionless-mode service */

/* State values */
#define	T_UNBND		1	/* unbound */
#define	T_IDLE		2	/* idle */
#define	T_OUTCON	3	/* outgoing connection pending */
#define	T_INCON		4	/* incoming connection pending */
#define	T_DATAXFER	5	/* data transfer */
#define	T_OUTREL	6	/* outgoing orderly release */
#define	T_INREL		7	/* incoming orderly release */

struct netbuf {
	unsigned	maxlen;
	unsigned	len;
	char *		buf;
};

struct t_bind {
	struct netbuf	addr;
	unsigned	qlen;
};

struct t_call {
	struct netbuf	addr;
	struct netbuf	opt;
	struct netbuf	udata;
	int		sequence;
};

struct t_discon {
	struct netbuf	udata;
	int		reason;
	int		sequence;
};

struct t_info {
	long	addr;
	long	options;
	long	tsdu;
	long	etsdu;
	long	connect;
	long	discon;
	long	servtype;
};

struct t_optmgmt {
	struct netbuf	opt;
	long	flags;
};

struct t_uderr {
	struct netbuf	addr;
	struct netbuf	opt;
	long		error;
};

struct t_unitdata {
	struct netbuf	addr;
	struct netbuf	opt;
	struct netbuf	udata;
};

#ifndef	_KERNEL
extern	int	t_errno;
extern	char *	t_errlist[];
extern	int	t_nerr;

#ifdef	_NO_PROTO
extern	int	t_accept();
extern	char *	t_alloc();
extern	int	t_bind();
extern	int	t_blocking();
extern	int	t_close();
extern	int	t_connect();
extern	void	t_error();
extern	int	t_free();
extern	int	t_getinfo();
extern	int	t_getstate();
extern	int	t_listen();
extern	int	t_look();
extern	int	t_nonblocking();
extern	int	t_open();
extern	int	t_optmgmt();
extern	int	t_rcv();
extern	int	t_rcvconnect();
extern	int	t_rcvdis();
extern	int	t_rcvrel();
extern	int	t_rcvudata();
extern	int	t_rcvuderr();
extern	int	t_snd();
extern	int	t_snddis();
extern	int	t_sndrel();
extern	int	t_sndudata();
extern	int	t_sync();
extern	int	t_unbind();
#else
extern	int	t_accept(int fd, int resfd, struct t_call *call);
extern	char *	t_alloc(int fd, int struct_type, int fields);
extern	int	t_bind(int fd, struct t_bind *req, struct t_bind *ret);
extern	int	t_blocking(int fd);
extern	int	t_close(int fd);
extern	int	t_connect(int fd, struct t_call *sndcall, struct t_call *rcvcall);
extern	void	t_error(char *errmsg);
extern	int	t_free(char *ptr, int struct_type);
extern	int	t_getinfo(int fd, struct t_info *info);
extern	int	t_getstate(int fd);
extern	int	t_listen(int fd, struct t_call *call);
extern	int	t_look(int fd);
extern	int	t_nonblocking(int fd);
extern	int	t_open(char *path, int oflag, struct t_info *info);
extern	int	t_optmgmt(int fd, struct t_optmgmt *req, struct t_optmgmt *ret);
extern	int	t_rcv(int fd, char *buf, unsigned nbytes, int *flags);
extern	int	t_rcvconnect(int fd, struct t_call *call);
extern	int	t_rcvdis(int fd, struct t_discon *discon);
extern	int	t_rcvrel(int fd);
extern	int	t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags);
extern	int	t_rcvuderr(int fd, struct t_uderr *uderr);
extern	int	t_snd(int fd, char *buf, unsigned nbytes, int flags);
extern	int	t_snddis(int fd, struct t_call *call);
extern	int	t_sndrel(int fd);
extern	int	t_sndudata(int fd, struct t_unitdata *unitdata);
extern	int	t_sync(int fd);
extern	int	t_unbind(int fd);
#endif
#endif
#endif
