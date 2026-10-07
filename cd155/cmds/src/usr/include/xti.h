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
 ** xti.h 1.1, last change 4/14/90
 **/

#ifndef _XTI_H
#define _XTI_H

/* Error values */
#define	TBADADDR	1
#define	TBADOPT		2
#define	TACCES		3
#define	TBADF		4
#define	TNOADDR		5
#define	TOUTSTATE	6
#define	TBADSEQ		7
#define TSYSERR		8
#define	TLOOK		9
#define	TBADDATA	10
#define	TBUFOVFLW	11
#define	TFLOW		12
#define	TNODATA		13
#define	TNODIS		14
#define	TNOUDERR	15
#define	TBADFLAG	16
#define	TNOREL		17
#define	TNOTSUPPORT	18
#define	TSTATECHNG	19
#define	TNOSTRUCTYPE	20
#define TBADNAME	21
#define	TBADQLEN	22
#define	TADDRBUSY	23

/* t_look events */
#define	T_LISTEN	0x0001
#define	T_CONNECT	0x0002
#define	T_DATA		0x0004
#define	T_EXDATA	0x0008
#define	T_DISCONNECT	0x0010
#define	T_UDERR		0x0040
#define	T_ORDREL	0x0080
#define	T_GODATA	0x0100
#define	T_GOEXDATA	0x0200
#define	T_EVENTS	0x0400
/*#define	T_ERROR	*/

/* Flag definitions */
#define	T_MORE		0x01
#define	T_EXPEDITED	0x02
#define	T_NEGOTIATE	0x04
#define	T_CHECK		0x08
#define	T_DEFAULT	0x10
#define T_SUCCESS	0x20
#define	T_FAILURE	0x40

struct t_info {
	long	addr;
	long	options;
	long	tsdu;
	long	etsdu;
	long	connect;
	long	discon;
	long	servtype;
};

/* Service types */
#define T_COTS		1	/* Connection-mode service */
#define	T_COTS_ORD	2	/* Connection service with orderly release */
#define	T_CLTS		3	/* Connectionless-mode service */

struct netbuf {
	unsigned int	maxlen;
	unsigned int	len;
	char *		buf;
};

struct t_bind {
	struct netbuf	addr;
	unsigned	qlen;
};

struct t_optmgmt {
	struct netbuf	opt;
	long		flags;
};

struct t_discon {
	struct netbuf	udata;
	int		reason;
	int		sequence;
};

struct t_call {
	struct netbuf	addr;
	struct netbuf	opt;
	struct netbuf	udata;
	int		sequence;
};

struct t_unitdata {
	struct netbuf	addr;
	struct netbuf	opt;
	struct netbuf	udata;
};

struct t_uderr {
	struct netbuf	addr;
	struct netbuf	opt;
	long		error;
};

/* t_alloc structure types */
#define	T_BIND_STR		1
#define T_OPTMGMT_STR		2
#define	T_CALL_STR		3
#define	T_DIS_STR		4
#define	T_UNITDATA_STR		5
#define	T_UDERROR_STR		6
#define	T_INFO_STR		7

/* t_alloc field identifiers */
#define	T_ADDR		0x01
#define	T_OPT		0x02
#define	T_UDATA		0x04
#define	T_ALL		0x07

/* State values */
#define	T_UNBND		1	/* unbound */
#define	T_IDLE		2	/* idle */
#define	T_OUTCON	3	/* outgoing connection pending */
#define	T_INCON		4	/* incoming connection pending */
#define	T_DATAXFER	5	/* data transfer */
#define	T_OUTREL	6	/* outgoing orderly release */
#define	T_INREL		7	/* incoming orderly release */

/* general purpose defines */
#define	T_YES		1
#define	T_NO		0
#define	T_UNUSED	(-1)
#define	T_NULL		0
#define	T_ABSREQ	0x8000

/* ISO definitions */
#define	T_CLASS0	0
#define	T_CLASS1	1
#define	T_CLASS2	2
#define	T_CLASS3	3
#define	T_CLASS4	4

/* priorities */
#define	T_PRITOP	0
#define	T_PRIHIGH	1
#define	T_PRIMID	2
#define	T_PRILOW	3
#define	T_PRIDFLT	4

/* protection levels */
#define	T_NOPROTECT		1
#define	T_PASSIVEPROTECT	2
#define	T_ACTIVEPROTECT		4

/* default value for the length of TPDU's */
#define	T_LTPDUDFLT	128

/* rate structure */
struct rate {
	long	targetvalue;
	long	minacceptvalue;
};

/* reqvalue structure */
struct reqvalue {
	struct rate	called;
	struct rate	calling;
};

/* throughput structure */
struct thrpt {
	struct reqvalue	maxthrpt;
	struct reqvalue	avgthrpt;
};

/* management structure */
struct management {
	short	dflt;		/* T_YES = default values T_NO = these values */
	int	ltpdu;		/* maximum length of TPDU */
	short	reastime;	/* reassignment time (in seconds) */
	char	class;		/* preferred class */
	char	altclass;	/* alternative class */
	char	extform;	/* extended format: T_YES or T_NO */
	char	flowctrl;	/* flow control: T_YES or T_NO */
	char	checksum;
	char	netexp;		/* network expedited data */
	char	netrecptcf;	/* receipt confirmation */
};

/* ISO connection-oriented options */
struct isoco_options {
	struct thrpt	throughput;
	struct reqvalue	transdel;	/* transit delay */
	struct rate	reserrorrate;	/* residual error rate */
	struct rate	transffailprob;	/* transfer failure problem */
	struct rate	estfailprob;	/* connection establishment failure */
	struct rate	relfailprob;	/* connection release failure */
	struct rate	estdelay;	/* connection establishment delay */
	struct rate	reldelay;	/* connection release delay */
	struct netbuf	connresil;	/* connection resilience */
	unsigned short	protection;
	short		priority;
	struct management mngmt;	/* management parameters */
	char		expd;		/* expedited data: T_YES or T_NO */
};

/* ISO connectionless options */
struct isocl_options {
	struct rate	transdel;	/* transit delay */
	struct rate	reserrorrate;	/* residual error rate */
	unsigned short	protection;
	short		priority;
};
	

/* TCP Precedence Levels */
#define	T_ROUTINE	0
#define	T_PRIORITY	1
#define	T_IMMEDIATE	2
#define	T_FLASH		3
#define	T_OVERRIDEFLASH	4
#define	T_CRITIC_ECP	5
#define	T_INETCONTROL	6
#define	T_NETCONTROL	7

/* TCP security options structure */
struct secoptions {
	short	security;
	short	compartment;
	short	handling;	/* handling restrictions */
	long	tcc;		/* transmission control code */
};

/* TCP options */
struct tcp_options {
	short		precedence;
	long		timeout;	/* abort timeout */
	long		max_seg_size;
	struct secoptions secopt;	/* security options */
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
extern	int	t_connect(int fd,struct t_call *sndcall,struct t_call *rcvcall);
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
