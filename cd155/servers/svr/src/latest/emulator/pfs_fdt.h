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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 */

/*
 * HISTORY
 * $Log: pfs_fdt.h,v $
 * Revision 1.16  1994/11/18  20:24:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/08/31  22:46:07  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.14.2.1  1994/08/19  22:50:42  dbm
 * Added support for a new bootmagic, PFS_ASYNC_DFLT, this allows setting
 * the default PFS I/O mode to M_ASYNC.
 *
 *  Reviewer:Bob Godley
 *  Risk:M
 *  Benefit or PTS #:10569
 *  Testing: Specific test cases. PFS EATS (With and without bootmagic set)
 *  Module(s):
 *
 *     (server)
 *         uxkern/boot_config.c
 *         uxkern/fsvr_server_side.c
 *         uxkern/fsvr.defs
 *     (emulator)
 *         emul_init.c
 *         fsvr_user_side.c
 *         pfs2_user_side.c
 *         pfs_iomode.c
 *         pfs_tokenmgt.c
 *         pfs_iomode.h
 *         pfs_fdt.h
 *     (libnx)
 *         _pfs_setio.c
 *         _setiomode.c
 *
 * Revision 1.14  1994/06/14  17:41:42  brad
 * Added R1.3 support for getting, setting, and temporarily mapping
 * per-file PFS stripe attributes via new F_GETSATTR and F_SETSATTR
 * fcntl() requests.  Also added an F_GETFULLSATTR request for use by ls
 * in getting full (including stripe file pathnames) stripe attributes.
 * Also modified default attributes given a file when it is created: the
 * start stripe directory is now a random member of the stripe group,
 * rather than always the first member, so small files consume disk space
 * evenly across the stripe group.
 *
 *  Reviewer: rlg (in progress)
 *  Risk: Med
 *  Benefit or PTS #: 7593, 9686
 *  Testing: Developer tests, PFS EATs on 64 nodes
 *  Module(s): server/{sys/fcntl.h,pfs/pfs.h}
 *             emulator/{pfs_fdt.h,fsvr_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.13  1994/03/14  01:44:40  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 *  Module(s):
 *
 * Revision 1.12  1994/03/04  22:25:39  dbm
 * Mainline merge for bug 6919, (1.2 rev 1.8.4.3)
 *
 * Revision 1.11  1994/02/04  19:46:40  brad
 * Modified extended math support so that: 1) Emath routines set a new
 * error parameter instead of relying on a return value of -1 on overflow.
 * The latter method did not handle valid return values of -1 (this caused
 * eseek with resulting offset of -1 to return EQESIZE instead of EINVAL,
 * for example).  2) The emath code can be reused by libesize.a and libnx.a,
 * instead of having multiple copies of the same code in different places.
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #:
 *  Testing: Ran PFS EATs, ran emath tests.
 *  Module(s): fsvr_user_side.c pfs2_user_side.c pfs_emath.c pfs_fdt.h
 *             pfs_iomode.c pfs_tokenmgt.c pfs_user_side.c
 *
 * Revision 1.10  1994/01/27  22:26:26  brad
 * Merge of revision 1.8.4.2 from the R1.2 branch.
 *
 * Revision 1.8.4.3  1994/03/04  21:48:38  dbm
 * Added revoke_port field to I/O mode info to allow M_RECORD to have its
 * own revoke ports.
 *  Reviewer: Brad Rullman
 *  Risk: M
 *  Benefit or PTS #: 6919
 *  Testing: PFS Eats, overlapping PFS Sats.
 *  Module(s):
 * 	pfs_fdt.h
 *
 * Revision 1.8.4.2  1994/01/27  01:40:27  brad
 * Added a workaround for PTS #7082 ... when the -plk NX switch is used,
 * vm_copy() does not preserve the 'wired' state of the VM pages in the
 * user's buffer.  Workaround by not using vm_copy() when -plk used,
 * resulting in possible PFS performance degradation at higher bandwidths.
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 7082
 *  Testing: Verified workaround with test from PTS report, ran PFS EATs
 *     on 64 nodes, ran PFS SAT on 64 nodes.
 *  Module(s): emulator/{pfs2_user_side.c,pfs_msgutil.c,i860/emul_machdep.c}
 *
 * Revision 1.8.4.1  1994/01/09  00:19:43  brad
 * Fixed bug found by lint (wrong number pf params to pfs_free() in
 * pfs2_user_side.c); also fixed lint warnings in PFS-related code.
 *
 * Revision 1.9  1994/01/05  17:08:53  brad
 * Fixed lint warnings in PFS-related code.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Some PFS source now passes lint
 *  Testing: Ran PFS EATs
 *  Module(s): emulator/emul_callback.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_fdt.h
 *             emulator/pfs_iomode.c
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.8  1993/08/04  01:20:19  dbm
 * Changed IOMODE_MAXPORT from 5 to 20 to allow for more spanning tree
 * ports, (found in PFS Eats).
 *
 * Revision 1.7  1993/07/16  03:01:53  dbm
 * Added token optimization functionality.
 *
 * Revision 1.6  1993/06/16  20:31:28  dbm
 * Removed iomode from pfs_iomode structure since it is now in the fdte
 * structure.
 *
 * Revision 1.5  1993/06/06  01:02:20  brad
 * Moved definition of STATPFS_BUFSZ constant out of macro definitions.
 *
 * Revision 1.4  1993/05/12  00:09:49  brad
 * Moved debug macros from pfs2_user_side.c.
 *
 * Revision 1.3  1993/04/03  03:18:25  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.10  1993/03/11  00:36:29  dbm
 * Added definition of STATPFS_BUFSZ, was in pfs2_user_side.c.  This allows
 * more than one module to use it.
 *
 * Revision 1.1.2.1.2.9  1993/03/10  06:29:28  brad
 * Added macros: GREATER, LESS, EQUAL, EMIN, EMAX, ETOOBIG, ...
 *
 * Revision 1.1.2.1.2.8  1993/02/23  04:48:59  brad
 * Removed stripefile length from the stripefile descriptor, since it's unused.
 *
 * Revision 1.1.2.1.2.7  1993/02/04  00:35:10  brad
 * Added extended offset support to SFOFF_TO_POFF macro.
 *
 * Revision 1.1.2.1.2.6  1993/01/20  20:51:40  brad
 * Added support for extended PFS file offsets and sizes in the stripefile
 * macros.
 *
 * Revision 1.1.2.1.2.5  1993/01/05  22:41:03  brad
 * Added SSIZ macro.
 *
 * Revision 1.1.2.1.2.4  1992/12/21  21:47:27  brad
 * Added macros for manipulating PFS and stripefile offsets.
 *
 * Revision 1.1.2.1.2.3  1992/12/12  01:48:28  brad
 * Update of latest PFS functionality in preparation for a merge with the
 * NX branch.
 *
 * Revision 1.1.2.1.2.2  1992/12/03  00:14:34  dbm
 * Updated for pfs i/o mode information in the fdte entry.
 *
 * Revision 1.2  1992/11/30  22:09:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:01:15  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/10  15:49:03  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:16:06  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.1  1992/10/22  15:01:11  dbm
 * New file for PFS functionality.
 *
 * Revision 1.1  1992/07/31  12:02:49  brad
 * Initial revision
 *
 *
 */
#ifndef	_PFS_FDT_H_
#define	_PFS_FDT_H_

#include "sys/estat.h"

/*
 * Initial size of the buffer used to contain stripe attributes.
 * Must be >= sizeof(struct statpfs).
 */
#define STATPFS_BUFSZ 8192

/*
 * stripe_fd_t: stripefile descriptor.
 *
 * Describes an open stripefile, which contains stripe units of an open 
 * PFS file.
 */
typedef struct stripe_fd {
	mach_port_t	s_fp;			/* port to open stripefile */
	off_t		s_offset;		/* stripefile seek pointer */
} stripe_fd_t;

/*
 * pfs_fd_t: PFS striped file descriptor.
 * 
 * If an open file is a PFS (striped) file, the file descriptor table (fdt)
 * entry for that file references the PFS header file, and the pfs_fd
 * pointer in the fdt entry points to the pfs_fd_t structure defined
 * below.  This structure contains fields describing the stripes of the PFS
 * file.  The p_stripe_fdt field is a placeholder for the variable-length 
 * table of stripefile descriptors.
 *
 * FILE DESCRIPTOR TABLE LOCKING NOTES: 
 *	lock:		Lock in fdte struct to protect the fields of the file
 *			descriptor table entry.
 *	io_lock:	Atomicity lock.  Used by Mapped Files support to
 *			protect file data (which is cached in client memory
 *			and thus not protected by server vnode locking).
 * 	p_io_lock:	PFS atomicity lock.  Used to protect PFS file data
 *			across multiple stripefile operations (each of which,
 *			individually, *is* protected by server vnode locking).
 *			This lock also protects the fields of the stripe_fd.
 */
typedef struct pfs_fd {
	spin_lock_t	p_io_lock;		/* atomicity lock */
	int		p_use_token;		/* Asynchronous default flag */
	esize_t		p_offset;		/* PFS file seek pointer */
	esize_t		p_length;		/* PFS file length */
	size_t		p_stripe_unit_size;	/* size of each stripe unit */
	uint_t		p_stripe_factor;	/* # stripe units per stripe */
	uint_t		p_start_stripedir;	/* index of start stripedir */
	stripe_fd_t	p_stripe_fdt[1];	/* stripefile descr. table */
} pfs_fd_t;

#define fdte_pfsio_lock(pfs_fd)		spin_lock(&(pfs_fd->p_io_lock))
#define fdte_pfsio_unlock(pfs_fd)	spin_unlock(&(pfs_fd->p_io_lock))
#define fdte_pfsio_lock_init(pfs_fd)	spin_lock_init(&(pfs_fd->p_io_lock))

/*
 * PFS I/O Mode Data Structures:
 */
#define PFS_TOKEN_MAGIC	0x55555555
/*
 * Client token state information:
 */
#define PFS_TOKEN_RELEASED 0	/* Indicates client does not have the token */
#define PFS_TOKEN_ACQUIRED 1	/* Indicates client has the token */
#define PFS_TOKEN_RIP 	   2	/* Indicates client has been sent revoke */
#define PFS_TOKEN_ACQUIRING 3	/* Indicates client waiting to acquire the
				 * token.
				 */
/*
 * Client token info structure.
 */
typedef struct {
	int		token_state;		/* Token state for each
						 * of the clients, i.e. 
						 * obtained or not
						 */
	mach_port_t	revoke_ports;		/* Revoke ports to the 
						 * Client's token. 
						 */
}clnt_tok_info;

/*
 * pfs_token_mgr_t:	PFS I/O mode information for the PFS token manager.
 *			This structure is used by node 0 for managing the
 *			shared token, most of the fields in this structure
 *			are basically duplications of the standard token
 *			management information.
 */
typedef struct {
	mach_port_t	server_token;		/* Server reference to token */
	mach_port_t	client_token;		/* Client token reference */
	int		flags;			/* Real token flags. */
	esize_t		offset;			/* Real offset in the file. */
	esize_t		length;			/* Real file length. */
        off_t           min_offset;             /* range for cleaning */
        off_t           max_offset;
	int		accessed:1,		/* file been accessed? */
			modified:1,		/* file been modified? */
			must_release:1,		/* must release the token */
			can_revoke:1;		/* can revoke the token */ 

	int		tmgr_token_state;	/* State of the token manager.
						 * token. 
						 */
	int		token_cnt;		/* Number of clients using the
						 * token. 
						 */

	clnt_tok_info	clnt_tok[1];		/* 
						 * Client token info:
						 */
}pfs_token_mgr_info;
	

/*
 * pfs_iomode_t: PFS I/O mode descriptor referenced by a file descriptor
 *               table entry.
 */
#define IOMODE_MAXPORT   20
typedef struct {
	mach_port_t	tmgr_id_port;		/* token manager id port */
	mach_port_t	tmgr_req_port;		/* token manager request port.*/
	mach_port_t	revoke_port;		/* token mgmt revoke port. */
	int		fdte_ref;		/* Node 0 fdte reference   */
	int		number_of_nodes;	/* number of nodes.        */
	int		my_node_number;		/* my logical node number. */
	pfs_token_mgr_info *token_mgr_info;	/* Token manager info */
	mach_port_t	syncin_port;		/* port to rcv sync msgs.  */
	mach_port_t	syncout_port;		/* port to send sync msgs. */
	mach_port_t	datain_port;		/* Port to rcv data on.    */
	mach_port_t	dataout_ports[IOMODE_MAXPORT]; /* Ports to send data */
} pfs_iomode_t;

/*
 * ptuple_t: Definition of the ordered tuple that represents a logical PFS 
 *	     file offset.
 */
typedef struct pfs_offset_tuple {
	uint_t	snum;		/* stripe in which the PFS offset lies      */
	uint_t	sfile;		/* stripefile in which the PFS offset lies  */
	off_t	sunitoff;	/* offset into the stripe unit in which ... */
} ptuple_t;


/*
 * External declarations.
 */
extern esize_t	__eadd();
extern esize_t	__eadd1();
extern long	__ecmp();
extern long	__ediv();
extern long	__emod();
extern esize_t	__emul();
extern esize_t	__esub();
extern esize_t	__esub1();
extern long	__etos();
extern esize_t	__stoe();
extern void	div();


/*
 * Macros for manipulating PFS stripefiles.
 *
 * Legend:
 *	S  => stripe
 *	SU => stripe unit
 *	SF => stripe file
 *	P  => PFS file
 *	D  => data
 */

/*
 * SSIZ:
 *	Size of one stripe.
 */
#define SSIZ(sunitsz, sfactor) ((sunitsz) * (sfactor))

/*
 * SFOFF_TO_SUOFF:
 *	Offset into stripe unit that given stripefile offset lies in.
 */
#define SFOFF_TO_SUOFF(off, sunitsz)	((off) % (sunitsz))

/*
 * POFF_TO_SUOFF:
 *	Offset into stripe unit that the given logical PFS offset lies in.
 */
#define POFF_TO_SUOFF(poff, sunitsz)	( \
	__emod((poff), (long)(sunitsz), &dont_care) \
)

/*
 * POFF_TO_SUNUM:
 *	Stripe unit number corresponding to the given logical PFS offset.
 */
#define POFF_TO_SUNUM(poff, sunitsz)	( \
	__ediv((poff), (long)(sunitsz), &dont_care) \
)

/*
 * POFF_TO_SNUM:
 *	Stripe number corresponding to the given PFS offset.
 */
#define POFF_TO_SNUM(poff, sunitsz, sfactor) ( \
	POFF_TO_SUNUM((poff), (sunitsz)) / (sfactor) \
)

/*
 * POFF_TO_SFNUM:
 *	Stripefile number corresponding to the given PFS offset.
 */
#define POFF_TO_SFNUM(poff, sunitsz, sfactor) ( \
	POFF_TO_SUNUM((poff), (sunitsz)) % (sfactor) \
)

/*
 * PTUPLE_INIT:
 *	Initialize a PFS offset ordered tuple.
 */
#define PTUPLE_INIT(p, poff, sunitsz, sfactor) \
	p.snum     = POFF_TO_SNUM((poff), (sunitsz), (sfactor)); \
	p.sfile    = POFF_TO_SFNUM((poff), (sunitsz), (sfactor)); \
	p.sunitoff = POFF_TO_SUOFF((poff), (sunitsz));

/*
 * PTUPLE_TO_SFOFF:
 *	Given the number of a stripefile, the value of this macro is the
 *	stripefile offset that corresponds to the logical PFS offset defined
 *	by the given <p_snum, p_sfile, p_sunitoff> tuple.
 *
 * Arguments:
 *	p_snum    : number of the stripe in which the given PFS offset lies
 *	p_sfile   : number of the stripefile in which the given PFS offset lies
 *	p_sunitoff: offset into the stripe unit in which the PFS offset lies
 *	sfile     : number of the stripefile that we want an offset for
 *	sunitsz   : stripe unit size
 *
 */
#define PTUPLE_TO_SFOFF(p_snum, p_sfile, p_sunitoff, sfile, sunitsz) ( \
	(p_snum) * (sunitsz) + \
	(((sfile) < (p_sfile)) ? (sunitsz) : \
 	                         (((sfile) > (p_sfile)) ? 0 : p_sunitoff)) \
)

/*
 * POFF_TO_SFOFF:
 *	Given the number of a stripefile, the value of this macro is the
 *	stripefile offset that corresponds to the given logical PFS offset.
 *
 * Arguments:
 *	poff      : logical PFS offset
 *	sfile     : number of the stripefile that we want an offset for
 *	sunitsz   : stripe unit size
 *	sfactor   : stripe factor
 */
#define POFF_TO_SFOFF(poff, sfile, sunitsz, sfactor) ( \
	PTUPLE_TO_SFOFF(POFF_TO_SNUM((poff), (sunitsz), (sfactor)), \
			POFF_TO_SFNUM((poff), (sunitsz), (sfactor)), \
			POFF_TO_SUOFF((poff), (sunitsz)), \
			sfile, \
			sunitsz) \
)

/*
 * PTUPLE_TO_DOFF:
 *	Given the number of a stripefile, the value of this macro is the data
 *	offset into a user buffer (i.e. the offset from the pfs_offset) that
 *	corresponds to the logical PFS offset defined by the given
 *	<p_snum, p_sfile, p_sunitoff> tuple.
 *
 * Arguments:
 *	p_snum    : number of the stripe in which the given PFS offset lies
 *	p_sfile   : number of the stripefile in which the given PFS offset lies
 *	p_sunitoff: offset into the stripe unit in which the PFS offset lies
 *	sfile     : number of the stripefile that we want an offset for
 *	sunitsz   : stripe unit size
 *
 */
#define PTUPLE_TO_DOFF(p_snum, p_sfile, p_sunitoff, sfile, sunitsz, sfactor) (\
	((sfile) < (p_sfile)) ? \
		(((sfactor) + (sfile) - (p_sfile) - 1)) * (sunitsz) + \
			((sunitsz) - (p_sunitoff)) : \
		(((sfile) > (p_sfile)) ? \
			(((sfile) - (p_sfile) - 1) * (sunitsz)) + \
		 		 ((sunitsz) - (p_sunitoff)) : \
			0) \
)

/*
 * SFOFF_TO_SUNUM:
 *	Stripe unit number corresponding to the given stripefile offset and
 *	stripefile.
 */
#define SFOFF_TO_SUNUM(sfoff, sfile, sunitsz, sfactor) ( \
	((sfoff) / (sunitsz)) * (sfactor) + (sfile) \
)

/*
 * SFOFF_TO_POFF:
 *	The value of this macro is the (extended) logical PFS file offset
 *	corresponding to the given stripefile offset that lies in the given
 *	stripe unit.
 *
 * Arguments:
 *	sfoff     : stripefile offset
 *	sunit     : (extended) number of stripe unit containing the stipefile
 *		    offset
 *	sunitsz   : stripe unit size
 */
#define SFOFF_TO_POFF(sfoff, sunit, sunitsz) ( \
	(__eadd1(__emul((sunit), (long)(sunitsz), &dont_care), \
		 (long)SFOFF_TO_SUOFF((sfoff), (sunitsz)), \
		 &dont_care)) \
)


/*
 * Macros for manipulating extended offsets and sizes.
 */

/*
 * LESS:
 *	Returns TRUE if extended number e1 is less than extended number e2.
 *
 */
#define LESS(e1, e2) (__ecmp((e1), (e2)) < 0)

/*
 * GREATER:
 *	Returns TRUE if extended number e1 is greater than extended number e2.
 *
 */
#define GREATER(e1, e2) (__ecmp((e1), (e2)) > 0)

/*
 * EQUAL:
 *	Returns TRUE if extended number e1 is equal to extended number e2.
 *
 */
#define EQUAL(e1, e2) (__ecmp((e1), (e2)) == 0)

/*
 * EMIN:
 *	The extended minimum of the two given extended numbers.
 */
#define EMIN(e1, e2) ((LESS((e1), (e2))) ? (e1) : (e2))

/*
 * EMAX:
 *	The extended maximum of the two given extended numbers.
 */
#define EMAX(e1, e2) ((GREATER((e1), (e2))) ? (e1) : (e2))

/*
 * MAX_PFS_SIZE:
 *	A valid PFS file size or offset is limited by the largest possible PFS
 *	file that can be created given the stripe factor and maximum size of a
 *	stripefile.
 *
 *	The largest size of a stripefile is NOT simply the OSF/1 limit of
 *	2GB-1, because only full stripe units are written into stripefiles.
 *	So in general, each stripefile can be up to 
 *	((2GB-1) - ((2GB-1) mod sunitsize)) in size.  The exception to this
 *	rule is stripefile 0, which, if it contains the last stripe unit of the
 *	file, and if the last stripe unit is a partial that fits into the
 *	((2GB-1) mod sunitsize) space, can be bigger.  In other words, the 
 *	maximum size of a PFS file is:
 *
 *		(((2GB-1) - EOF_partial) * sfactor) + EOF_partial
 *
 *	where
 *
 *		EOF_partial = ((2GB-1) mod sunitsize))
 */
#define MAX_PFS_SIZE(sunitsz, sfactor) ( \
	__eadd1( \
		__emul( \
		       __esub1(ex_int_max, (long)(INT_MAX % (sunitsz)), \
			       &dont_care), \
		       (long)(sfactor), \
		       &dont_care), \
		(long)(INT_MAX % (sunitsz)), \
		&dont_care) \
)

/*
 * ETOOBIG:
 * 	Sanity check a PFS file offset or size.  Returns TRUE if the given
 *	PFS offset might cause an overflow when computing stripe unit numbers
 *	or stripefile offsets.
 */
#define ETOOBIG(poff, sunitsz, sfactor) ( \
	GREATER((poff), MAX_PFS_SIZE((sunitsz), (sfactor))) \
)


/* #define	DEBUG_PFS */

#ifdef	DEBUG_PFS
/*
 * Macros for PFS debugging.  The following debug levels are valid:
 *
 *	0 => no debug
 *	1 => a function call trace is printed to stderr
 *	2 => a function call trace is printed to the console
 *	3 => detailed debug output is printed to stderr
 *	4 => detailed debug output is printed to the console
 */
#define PFS_TRACE(s) \
	if (pfs_debug_flag >= 1) e_printf s
#define PFS_DEBUG(s) \
	if (pfs_debug_flag >= 3) e_printf s
#else
#define PFS_TRACE(s)   
#define PFS_DEBUG(s)   
#endif	DEBUG_PFS

#endif	_PFS_FDT_H_
