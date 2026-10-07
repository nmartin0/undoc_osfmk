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
 * nx.h
 *
 * Copyright (c) 1992 Intel Corporation
 */

#ifndef __NX_H__
#define __NX_H__

#include <sys/types.h>
#include <sys/estat.h>
#include <pfs/pfs.h>
#include <allocsys.h>

/*
 * Invalid ptype
 */
#define INVALID_PTYPE	-1

/*
 * First number in the Force type range
 */
#define FORCE_TYPE	0x40000000	/* 1073741824 decimal */

/*
 * Message passing info array
 */
extern long msginfo[8];

/*
 * Hardware clock rate
 */
#define HWHZ	10000000

#define HOST		myhost()

#ifndef	_ESIZE_T
#define	_ESIZE_T
/*
 * PFS extended size structure.
 */
struct s_size {
	long	slow;
	long	shigh;
};
typedef struct s_size esize_t;
#endif

#ifndef	_SIZE_SET
#define	_SIZE_SET
/*
 * Symbolic constants for the lsize() and esize() functions.
 */
#define SIZE_SET 0	/* set file size to MAX(cur size,"offset") */
#define SIZE_CUR 1	/* set file size to MAX(cur size,cur offset+"offset" */
#define SIZE_END 2	/* set file size to MAX(cur size,cur size+"offset" */
#endif

#ifndef _IOMODES_SET
#define _IOMODES_SET
/*
 * Symbolic constants for the setiomode() and gopen() functions.
 */
#define M_UNIX   0	/* Unshared file pointer, (default). 
			 */
#define M_LOG    1	/* Shared file pointer, accessed in first come
			 * first served order.
			 */
#define M_SYNC   2	/* Shared file pointer, variable record length, 
			 * accessed in logical node order.
			 */
#define M_RECORD 3 	/* Unshared file pointer, fixed record length,
			 * access in first come first served order, records
			 * are position dependent by logical node number.
			 */
#define M_GLOBAL 4	/* Shared file pointer, all nodes perform the 
			 * same i/o operation. 
			 */
#define M_ASYNC  5	/* Unshared file pointer, no synchronization between
			 * nodes, unrestricted file access
			 */
#endif

#ifndef	NX_ATTR_END
#define NX_ATTR_END	-1
/*
 * Symbolic constants for nx_mkpart_attr and nx_initve_attr
*/
#define	NX_ATTR_SZ	1	/* # of node for partition or application
				 */
#define	NX_ATTR_RECT	2	/* HxW for partition or application
				 */
#define	NX_ATTR_MAP	3	/* List of nodes for partition or application
				 */
#define	NX_ATTR_ANCHOR	4	/* Upper-left corner of rectangle
				 */
#define	NX_ATTR_SCHED	5	/* Partition's scheduling type
				 */
#define	NX_ATTR_RELAXED	6	/* Relaxed allocation flag
				 */
#define	NX_ATTR_EPL	7	/* Effective priority limit
				 */
#define	NX_ATTR_RQ	8	/* Rollin quantum
				 */
#define	NX_ATTR_MOD	9	/* Protection mode
				 */
#define	NX_ATTR_SEL	10	/* Node attribute selection string
				 */
#define	NX_ATTR_PRI	11	/* Priority
				 */
#define	NX_ATTR_PKT	12	/* Packet size
				 */
#define	NX_ATTR_MBF	13	/* Message buffers
				 */
#define	NX_ATTR_MEX	14	/* Memory export
				 */
#define	NX_ATTR_MEA	15	/* Memory each
				 */
#define	NX_ATTR_NOC	16	/* # of correspondents
				 */
#define	NX_ATTR_STH	17	/* Send threshold
				 */
#define	NX_ATTR_SCT	18	/* Send count
				 */
#define	NX_ATTR_GTH	19	/* Give threshold
				 */
#define	NX_ATTR_PLK	20	/* Process lock into memory
				 */
#define	NX_ATTR_ACCT	21	/* MACS Account info
				 */
#define	NX_ATTR_GUEST	22	/* Guest operating system
				 */

/* Parameters for NX_ATTR... attributes
 */
#define NX_SUNMOS	1	/* For NX_ATTR_GUEST, the guest is Sunmos */

#endif


#ifndef _NX_SCHEDTYPE_SET
#define _NX_SCHEDTYPE_SET
/*
 * Symbolic partition scheduling type constants for nx_mkpart(), 
 * nx_mkpart_rect(), and nx_mkpart_map() functions.
 */
#define NX_SPS   2 	/* Use space sharing only for partition  */
#define NX_GANG  1 	/* Use gang scheduling for partition     */
#define NX_STD   0      /* Use standard scheduling for partition */
#endif

#ifdef _NO_PROTO

extern void	cprobe();
extern void	cprobex();
extern void	crecv();
extern void	crecvx();
extern void	csend();
extern long	csendrecv();
extern double	dclock();
extern void	flick();
extern void	flushmsg();
extern void	gcol();
extern void	gcolx();
extern void	gdhigh();
extern void	gdlow();
extern void	gdprod();
extern void	gdsum();
extern void	giand();
extern void	gihigh();
extern void	gilow();
extern void	gior();
extern void	giprod();
extern void	gisum();
extern void	gland();
extern void	glor();
extern int	gopen();
extern void	gopf();
extern void	gsendx();
extern void	gshigh();
extern void	gslow();
extern void	gsprod();
extern void	gssum();
extern void	gsync();
extern void	hrecv();
extern void	hrecvx();
extern void	hsend();
extern void	hsendrecv();
extern void	hsendx();
extern long	infocount();
extern long	infonode();
extern long	infoptype();
extern long	infotype();
extern long	iprobe();
extern long	iprobex();
extern long	irecv();
extern long	irecvx();
extern long	isend();
extern long	isendrecv();
extern long	masktrap();
extern void	msgcancel();
extern long	msgdone();
extern void	msgignore();
extern long	msgmerge();
extern void	msgwait();
extern long	myhost();
extern long	mynode();
extern long	myptype();
extern long	numnodes();
extern long	nx_app_nodes();
extern long	nx_app_rect();
extern long	nx_chpart_epl();
extern long	nx_chpart_mod();
extern long	nx_chpart_name();
extern long	nx_chpart_owner();
extern long	nx_chpart_rq();
extern long	nx_chpart_sched();
extern long	nx_initve();
extern long	nx_initve_rect();
extern long	nx_initve_attr();
extern long	nx_load();
extern long	nx_loadve();
extern long	nx_mkpart();
extern long	nx_mkpart_map();
extern long	nx_mkpart_rect();
extern long	nx_mkpart_attr();
extern long	nx_nfork();
extern void	nx_perror();
extern long	nx_pri();
extern int	nx_pspart();
extern long	nx_rmpart();
extern long	nx_waitall();
extern void	setptype();

/* underscore versions */
extern long	_cprobe();
extern long	_cprobex();
extern long	_crecv();
extern long	_crecvx();
extern long	_csend();
extern long	_csendrecv();
extern double	_dclock();
extern long	_flick();
extern long	_flushmsg();
extern long	_gcol();
extern long	_gcolx();
extern long	_gdhigh();
extern long	_gdlow();
extern long	_gdprod();
extern long	_gdsum();
extern long	_giand();
extern long	_gihigh();
extern long	_gilow();
extern long	_gior();
extern long	_giprod();
extern long	_gisum();
extern long	_gland();
extern long	_glor();
extern int	_gopen();
extern long	_gopf();
extern long	_gsendx();
extern long	_gshigh();
extern long	_gslow();
extern long	_gsprod();
extern long	_gssum();
extern long	_gsync();
extern long	_hrecv();
extern long	_hrecvx();
extern long	_hsend();
extern long	_hsendrecv();
extern long	_hsendx();
extern long	_infocount();
extern long	_infonode();
extern long	_infoptype();
extern long	_infotype();
extern long	_iprobe();
extern long	_iprobex();
extern long	_irecv();
extern long	_irecvx();
extern long	_isend();
extern long	_isendrecv();
extern long	_masktrap();
extern long	_msgcancel();
extern long	_msgdone();
extern long	_msgignore();
extern long	_msgmerge();
extern long	_msgwait();
extern long	_myhost();
extern long	_mynode();
extern long	_myptype();
extern long	_numnodes();
extern long	_setptype();

/* iPSC and Touchstone DELTA Compatibility Calls */
extern long	ginv();
extern long	gray();
extern void	hwclock();
extern long	infopid();
extern void	killcube();
extern void	killproc();
extern void	led();
/*  comment out prototype for load() to avoid conflict with loader.h
 extern long	load(); */
extern unsigned long	mclock();
extern long	mypart();
extern long	mypid();
extern long	nodedim();
extern long	restrictvol();

/* underscore versions */
extern long	_ginv();
extern long	_gray();
extern void	_hwclock();
extern long	_infopid();
extern int	_killcube();
extern int	_killproc();
extern long	_led();
extern long	_load();
extern unsigned long	_mclock();
extern long	_nodedim();
extern long	_restrictvol();

#else                  /* use ANSI C required prototypes */

extern void	cprobe( long);
extern void	cprobex( long, long, long, long *);
extern void	crecv( long, void *, long);
extern void	crecvx( long, void *, long, long, long, long *);
extern void	csend( long, void *, long, long, long);
extern long	csendrecv( long, void *, long, long, long, long, void *, long);
extern double	dclock( void);
extern void	flick( void);
extern void	flushmsg( long, long, long);
extern void	gcol( void *, long, void *, long, long *);
extern void	gcolx( void *, long *, void *);
extern void	gdhigh( double *, long, double *);
extern void	gdlow( double *, long, double *);
extern void	gdprod( double *, long, double *);
extern void	gdsum( double *, long, double *);
extern void	giand( long *, long, long *);
extern void	gihigh( long *, long, long *);
extern void	gilow( long *, long, long *);
extern void	gior( long *, long, long *);
extern void	giprod( long *, long, long *);
extern void	gisum( long *, long, long *);
extern void	gland( long *, long, long *);
extern void	glor( long *, long, long *);
extern int	gopen( const char *, int, int, mode_t);
extern void	gopf(void *, long, void *, long (*function)());
extern void	gsendx( long, void *, long, long *, long);
extern void	gshigh( float *, long, float *);
extern void	gslow( float *, long, float *);
extern void	gsprod( float *, long, float *);
extern void	gssum( float *, long, float *);
extern void	gsync( void);
extern void	hrecv( long, void *, long, void (*handler)());
extern void	hrecvx( long, void *, long, long, long, void (* xhandler)(),
			long);
extern void	hsend( long, void *, long, long, long, void (*handler)());
extern void	hsendrecv( long, void *, long, long, long, long, void *, long,
			   void (*handler)());
extern void	hsendx( long, void *, long, long, long, void (*xhandler)(),
			long);
extern long	infocount( void);
extern long	infonode( void);
extern long	infoptype( void);
extern long	infotype( void);
extern long	iprobe( long);
extern long	iprobex( long, long, long, long *);
extern long	irecv( long, void *, long);
extern long	irecvx( long, void *, long, long, long, long *);
extern long	isend( long, void *, long, long, long);
extern long	isendrecv( long, void *, long, long, long, long, void *, long);
extern long	masktrap( long);
extern void	msgcancel( long);
extern long	msgdone( long);
extern void	msgignore( long);
extern long	msgmerge( long, long);
extern void	msgwait( long);
extern long	myhost( void);
extern long	mynode( void);
extern long	myptype( void);
extern long	numnodes( void);
extern long	nx_app_nodes( pid_t, nx_nodes_t *, unsigned long *);
extern long	nx_app_rect( long *, long *);
extern long	nx_chpart_epl( char *, long);
extern long	nx_chpart_mod( char *, long);
extern long	nx_chpart_name( char *, char *);
extern long	nx_chpart_owner( char *, long, long);
extern long	nx_chpart_rq( char *, long);
extern long	nx_chpart_sched( char *, long);
extern long	nx_initve( char *, long, char *, int *, char **);
extern long	nx_initve_rect( char *, long, long, long, char *, int *,
				char **);
extern long	nx_initve_attr( char *, int *, char**, ...);
extern long	nx_load( long *, long, long, long *, char *);
extern long	nx_loadve( long *, long, long, long *, char *, char **,
			   char **);
extern long	nx_mkpart( char *, long, long);
extern long	nx_mkpart_attr( char *, ...);
extern long	nx_mkpart_map( char *, long, long *, long);
extern long	nx_mkpart_rect( char *, long, long, long);
extern long	nx_nfork( long *, long, long, long *);
extern void	nx_perror( char *);
extern long	nx_pri( long, long);
extern int	nx_pspart( char *, nx_pspart_t **, unsigned long *);
extern long	nx_rmpart( char *, long, long);
extern long	nx_waitall( void);
extern void	setptype( long);

/* underscore versions */
extern long	_cprobe( long);
extern long	_cprobex( long, long, long, long *);
extern long	_crecv( long, void *, long);
extern long	_crecvx( long, void *, long, long, long, long *);
extern long	_csend( long, void *, long, long, long);
extern long	_csendrecv( long, void *, long, long, long, long, void *, long);
extern double	_dclock( void);
extern long	_flick( void);
extern long	_flushmsg( long, long, long);
extern long	_gcol( void *, long, void *, long, long *);
extern long	_gcolx( void *, long *, void *);
extern long	_gdhigh( double *, long, double *);
extern long	_gdlow( double *, long, double *);
extern long	_gdprod( double *, long, double *);
extern long	_gdsum( double *, long, double *);
extern long	_giand( long *, long, long *);
extern long	_gihigh( long *, long, long *);
extern long	_gilow( long *, long, long *);
extern long	_gior( long *, long, long *);
extern long	_giprod( long *, long, long *);
extern long	_gisum( long *, long, long *);
extern long	_gland( long *, long, long *);
extern long	_glor( long *, long, long *);
extern int	_gopen( const char *, int, int, mode_t);
extern long	_gopf(void *, long, void *, long (*function)());
extern long	_gsendx( long, void *, long, long *, long);
extern long	_gshigh( float *, long, float *);
extern long	_gslow( float *, long, float *);
extern long	_gsprod( float *, long, float *);
extern long	_gssum( float *, long, float *);
extern long	_gsync( void);
extern long	_hrecv( long, void *, long, void (*handler)());
extern long	_hrecvx( long, void *, long, long, long, void (*xhandler)(),
			long);
extern long	_hsend( long, void *, long, long, long, void (*handler)());
extern long	_hsendrecv( long, void *, long, long, long, long, void *, long,
			   void (*handler)());
extern long	_hsendx( long, void *, long, long, long, void (*xhandler)(),
			long);
extern long	_infocount( void);
extern long	_infonode( void);
extern long	_infoptype( void);
extern long	_infotype( void);
extern long	_iprobe( long);
extern long	_iprobex( long, long, long, long *);
extern long	_irecv( long, void *, long);
extern long	_irecvx( long, void *, long, long, long, long *);
extern long	_isend( long, void *, long, long, long);
extern long	_isendrecv( long, void *, long, long, long, long, void *, long);
extern long	_masktrap( long);
extern long	_msgcancel( long);
extern long	_msgdone( long);
extern long	_msgignore( long);
extern long	_msgmerge( long, long);
extern long	_msgwait( long);
extern long	_myhost( void);
extern long	_mynode( void);
extern long	_myptype( void);
extern long	_numnodes( void);
extern long	_setptype( long);

/* iPSC and Touchstone DELTA Compatibility Calls */
extern long	ginv( long);
extern long	gray( long);
extern void	hwclock( esize_t *);
extern long	infopid( void);
extern void	killcube( long, long);
extern void	killproc( long, long);
extern void	led( long);
/*  comment out prototype for load() to avoid conflict with loader.h
extern long	load( char *, long, long); */
extern unsigned long	mclock( void);
extern long	mypart( long *, long *);
extern long	mypid( void);
extern long	nodedim( void);
extern long	restrictvol( int, int, int *);

/* underscore versions */
extern long	_ginv( long);
extern long	_gray( long);
extern void	_hwclock( esize_t *);
extern long	_infopid( void);
extern int	_killcube( long, long);
extern int	_killproc( long, long);
extern long	_led( long);
extern long	_load( char *, long, long);
extern unsigned long	_mclock( void);
extern long	_nodedim( void);
extern long	_restrictvol( int, int, int *);

#endif          /* _NO_PROTO */

/*
 * PFS extern definitions.
 */
#ifndef _KERNEL
#ifdef _NO_PROTO
extern void	cread();
extern void	creadv();
extern void	cwrite();
extern void	cwritev();
extern esize_t	eadd();
extern esize_t	eadd1();
extern long	ecmp();
extern long	ediv();
extern long	emod();
extern esize_t	emul();
extern esize_t	eseek();
extern esize_t	esize();
extern long	estat();
extern esize_t	esub();
extern esize_t	esub1();
extern void	etos();
extern long	festat();
extern long	fstatpfs();
extern long	getpfsinfo();
extern long	iodone();
extern long	iomode();
extern void	iowait();
extern long	iread();
extern long	ireadv();
extern long	iseof();
extern long	iwrite();
extern long	iwritev();
extern long	lestat();
extern long	lsize();
extern void	setiomode();
extern long	statpfs();
extern esize_t	stoe();

/* underscore versions */
extern int	_cread();
extern int	_creadv();
extern int	_cwrite();
extern int	_cwritev();
extern esize_t	_eadd();
extern esize_t	_eadd1();
extern long	_ecmp();
extern long	_ediv();
extern long	_emod();
extern esize_t	_emul();
extern esize_t	_eseek();
extern esize_t	_esize();
extern long	_estat();
extern esize_t	_esub();
extern esize_t	_esub1();
extern long	_etos();
extern long	_festat();
extern long	_iodone();
extern long	_iomode();
extern int	_iowait();
extern long	_iread();
extern long	_ireadv();
extern long	_iseof();
extern long	_iwrite();
extern long	_iwritev();
extern long	_lestat();
extern long	_lsize();
extern long	_setiomode();
extern esize_t	_stoe();

#else	/* use POSIX required prototypes */
extern void	cread( int, void *, unsigned int);
extern void	creadv( int, struct iovec *, int);
extern void	cwrite( int, void *, unsigned int);
extern void	cwritev( int, struct iovec *, int);
extern esize_t	eadd( esize_t, esize_t);
extern esize_t	eadd1( esize_t, long);
extern long	ecmp( esize_t, esize_t);
extern long	ediv( esize_t, long);
extern long	emod( esize_t, long);
extern esize_t	emul( esize_t, long);
extern esize_t	eseek( int, esize_t, int);
extern esize_t	esize( int, esize_t, int);
extern long	estat( char *, struct estat *);
extern esize_t	esub( esize_t, esize_t);
extern esize_t	esub1( esize_t, long);
extern void	etos( esize_t, char *);
extern long	festat( int, struct estat *);
extern long	fstatpfs( int, struct estatfs *, struct statpfs *,
			  unsigned int);
extern long	getpfsinfo( struct pfsmntinfo **);
extern long	iodone( long);
extern long	iomode( int);
extern void	iowait( long);
extern long	iread( int, void *, unsigned int);
extern long	ireadv( int, struct iovec *, int);
extern long	iseof( int);
extern long	iwrite( int, void *, unsigned int);
extern long	iwritev( int, struct iovec *, int);
extern long	lestat( char *, struct estat *);
extern long	lsize( int, off_t, int);
extern void	setiomode( int, int);
extern long	statpfs( char *, struct estatfs *, struct statpfs *,
			 unsigned int);
extern esize_t	stoe( char *);

/* underscore versions */
extern int	_cread( int, void *, unsigned int);
extern int	_creadv( int, struct iovec *, int);
extern int	_cwrite( int, void *, unsigned int);
extern int	_cwritev( int, struct iovec *, int);
extern esize_t	_eadd( esize_t, esize_t);
extern esize_t	_eadd1( esize_t, long);
extern long	_ecmp( esize_t, esize_t);
extern long	_ediv( esize_t, long);
extern long	_emod( esize_t, long);
extern esize_t	_emul( esize_t, long);
extern esize_t	_eseek( int, esize_t, int);
extern esize_t	_esize( int, esize_t, int);
extern long	_estat( char *, struct estat *);
extern esize_t	_esub( esize_t, esize_t);
extern esize_t	_esub1( esize_t, long);
extern long	_etos( esize_t, char *);
extern long	_festat( int, struct estat *);
extern long	_iodone( long);
extern long	_iomode( int);
extern int	_iowait( long);
extern long	_iread( int, void *, unsigned int);
extern long	_ireadv( int, struct iovec *, int);
extern long	_iseof( int);
extern long	_iwrite( int, void *, unsigned int);
extern long	_iwritev( int, struct iovec *, int);
extern long	_lestat( char *, struct estat *);
extern long	_lsize( int, off_t, int);
extern long	_setiomode( int, int);
extern esize_t	_stoe( char *);

#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* __NX_H__ */
