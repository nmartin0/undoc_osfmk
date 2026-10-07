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
 *
 * HISTORY
 * $Log: pfs_iomode.h,v $
 * Revision 1.9  1994/11/18  20:24:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/31  22:46:13  mtm
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
 * Revision 1.7.2.1  1994/08/19  22:49:48  dbm
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
 * Revision 1.7  1994/06/13  15:19:21  rlg
 * Added the M_ASYNC I/O mode for shared files.  This mode is characterized by:
 *     o	each node has a unique file pointer,
 *     o	nodes are not synchronized
 *     o	file access is unrestricted
 *     o	standard UNIX file sharing semantics requiring atomicity of I/O
 * 	are not preserved.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  medium
 *  Benefit or PTS #:  7480
 *  Testing:  I/O mode unit test; 132 Eval I/O tests; rw performance test;
 *  Module(s):  emulator/fsvr_user_side.c		libnx/_gopen.c
 * 		      pfs2_user_side.c		      _pfs_setio.c
 * 		      pfs_iomode.c		      _setiomode.c
 * 		      pfs_iomode.h		      gopen.c
 * 		      pfs_tokenmgt.c		      gopen_.c
 * 		      pfs_user_side.c		      pfs_iomode.h
 * 						      setiomode.c
 *
 * Revision 1.6  1994/02/16  00:53:34  dbm
 * Merge of 1.2 branch, revision 1.5.2.1
 *
 * Revision 1.5.2.1  1994/02/16  00:05:39  dbm
 * Added definition for PFS_OP_LSIZE.
 *
 *  Reviewer:None.
 *  Risk:L
 *  Benefit or PTS #:6337
 *  Testing: Ran specific test case, Ran PFS Eats several times.
 *  Module(s):
 * 	pfs_iomode.h
 *
 * Revision 1.5  1993/09/08  20:58:59  dbm
 * Added PFS_TOKENMGT macro to dissable M_RECORD mode for UFS files.
 *
 * Revision 1.4  1993/08/04  01:19:22  dbm
 * Changed IOMODE_MAXPORT from 5 to 20 to allow for more spanning tree ports.
 *
 * Revision 1.3  1993/07/16  03:02:16  dbm
 * Added token optimization functionality.
 *
 * Revision 1.2  1993/04/03  03:18:39  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.4.4  1993/02/12  17:20:41  dbm
 * Added M_GLOBAL functionality.
 *
 * Revision 1.1.4.3  1993/01/20  20:47:50  brad
 * Removed spurious RCS keywords from Log.
 *
 * Revision 1.1.4.2  1993/01/14  20:36:58  dbm
 * Added definitions to support gopen().
 *
 * Revision 1.1.4.1  1993/01/11  17:30:57  dbm
 * Added changes to support PFS files with I/O modes.
 *
 * Revision 1.1  1992/11/25  02:41:08  dbm
 * Added for supporting mapped files with PFS I/O modes.
 *
 * Revision 1.3  1992/08/04  16:42:19  dbm
 * Added RCS Log instead of Header keyward.
 *
 */

/*
 * Definitions for the setiomode function.
 *
 */
#ifndef _EMULATOR
#include <sys/param.h>
#define  mach_port_t int
#define  PFS_GOPEN_MSG_TYPE	20000000
#endif

#define PFS_MACH_PORT_BASE	40000
#define IOMODE_MAXPORT		20
#define PFS_MAXSPAN		20
/*
 * iomode definitions, (as per user nx.h/fnx.h definitions):
 */
#define 	M_UNIX		0
#define		M_LOG		1
#define		M_SYNC		2
#define		M_RECORD	3
#define		M_GLOBAL	4
#define		M_ASYNC		5


#define PFS_TOKENMGT(x)		((x->pfs_iomode == M_RECORD)  && (x->pfs_fd))

/*
 * Mach port communicatons mechanism:
 */
#define		IOMODE_COM_NEIGHBOR	0 /* Neighbor communications.      */
#define		IOMODE_COM_SPANNING	1 /* Spanning tree communicatoins. */

/*
 * Global Operation(gop) types:
 */
#define	PFS_OP_CLOSE	0
#define PFS_OP_READ	1
#define PFS_OP_READV	2
#define PFS_OP_WRITE	3
#define PFS_OP_WRITEV	4
#define PFS_OP_LSEEK	5
#define PFS_OP_TOKEN	6
#define PFS_OP_RESETIO	7
#define PFS_OP_ISEOF	7
#define PFS_OP_DUP	8
#define PFS_OP_GETINFO	9
#define PFS_OP_OFFSET	10
#define PFS_OP_TOKMGR	11
#define PFS_OP_LSIZE	12


/*
 * Global Operation(gop) states:
 */
#define	IOOP_START	0
#define IOOP_RCV	1
#define IOOP_END	2

/*
 * pfs_gopen_info_t:		This data structure is used by
 *				the gopen() function to exchange
 *				file open information between the
 *				processes in an application.
 */
typedef struct {
	int	iomode;
	char	file_name[MAXPATHLEN];
	int	access;
	int	mode;
	int 	fdt_info_length;
	int	errno;
	pid_t	unixpid;
} pfs_gopen_info_t;


#ifdef _EMULATOR
/*
 * pfs_gopen_fdte_info_t	This data structure is used by
 *				the pfs_get_fdte_info and 
 *				pfs_put_fdte_info system calls
 *				in order to exchange information
 *				on a file being opened using the
 *				gopen function.
 */
typedef struct {
	fdt_entry_t	fdte_info;
	pfs_fd_t	pfs_info;
} pfs_gopen_fdte_info_t;
#endif

/*
 * pfs_setio_info_t:		This data structure is used by
 *				the get_local_iomode_info() function
 *				to return information about an 
 *				open file.
 */
typedef struct {
	int		iomode;
	dev_t		device_id;
	ino_t		inode_id;
	int		node_id;
	esize_t		offset;
}pfs_setio_info_t;
		
/*
 * pfs_comm_info_t:		This data structure is used by the
 *				set_local_iomode_info() and set_global_
 *				iomode_info() system calls to pass 
 *				emulation library communications resource
 *				information between the user library and
 *				the emulation library.
 */
typedef struct {
	pid_t		unix_pid;
	mach_port_t	syncin_port;
	mach_port_t	syncout_port;
	mach_port_t	datain_port;
	mach_port_t	dataout_ports[IOMODE_MAXPORT];
} pfs_comm_info_t;

/*
 * pfs_fp_info_t:		This data structure is used by the 
 *				set_local_iomode_info() and set_global_
 *				iomode_info() system calls to pass 
 *				emulation library file pointer information
 *				between the user library and the 
 *				emulation library.
 */
typedef struct {
	mach_port_t	tmgr_id_port;	/* Token Manager id Port. (Node 0) 
					 * This port is used to identify
					 * the client token for revoke
					 * purposes. 
					 */
	mach_port_t	revoke_port;	/* Revoke Port, (One Per Node) */
	int		fdte_ref;	/* Node 0's fdte reference */
	mach_port_t	mem_obj;	/* Memory object. */
	mach_port_t	fp_ports[1];	/* File Port(s)	*/
}pfs_fp_info_t;
 
/*
 * iomode_info structure:	This structure is used to exchange 
 *				information between the user library
 *				and the emulation library. This same
 *				structure is used to pass information
 *				between emulators.
 */
struct iomode_info {
	mach_port_t	fp;		/* Mach port id of the file. */
	dev_t		dev;		/* Device file on.	     */
	ino_t		inode;		/* File inode.		     */
	esize_t		offset;		/* File offset. 	     */
	int		iomode;		/* Iomode.		     */
	int		ptype_pid;	/* Unix pids of all tasks.   */
 	mach_port_t	in_port[IOMODE_MAXPORT];/* Communications port(s).   */
 	mach_port_t	out_port[IOMODE_MAXPORT];/* Communications port(s).   */
#ifdef PFS
	/* We will need all information about the stripe files
	 *   to pass along the the other emulation libraries.
	 */
#endif
};

/*
 * Iomode message structure, sent via Mach IPC:
 */
typedef struct {
	int	op_type;	/* Operation type. */
	int	error;		/* Error. */
	esize_t	offset;		/* Extended offset. */
	esize_t	length;		/* Extended length. */
	esize_t new_offset;	/* Extended new offset. */
	int	modified;	/* Indicates if file length modified. */
	int	accessed;	/* Indicates if file has been accessed. */
} iomode_msg;

/*
 * Global message structure, sent via Mach IPC:
 */
typedef struct {
	int	op_type;	/* Operation type. */
	int	count;		/* Count. */
	int	retval;		/* Return value from I/O operation. */
	int	error;		/* Error value from I/O operation. */
} global_hdr;

/*
 * Token manager message, sent via Mach IPC:
 */
#define TMGR_OP_REVOKE	1000
#define TMGR_OP_ACQUIRE 1001
#define TMGR_OP_RELEASE	1002

typedef struct {
	int	op_type;	/* Operation type */
	int	node;		/* Logical node # */
	int	flags;		/* Flags (for acquire). */
	esize_t	offset; 	/* Current offset (for release) */
	esize_t length; 	/* Current length (for release) */
	int	modified;	/* Modified flag  (for release) */
	int	accessed;	/* Accessed flag  (for release) */
}tmgr_req_msg;
	
typedef struct {
	int	error;		/* Error */
	esize_t	offset; 	/* Current offset */
	esize_t length; 	/* Current length */
}tmgr_rply_msg;


/*
 * Global operation(gop) parameter types:
 */
typedef struct {		/* Syncoff iomode gop parameter structure. */
	int	op_type;
	int	count;
	esize_t	offset;
}SYNC_DATA;

typedef struct {		/* Syncoff iomode gop parameter structure. */
	esize_t	offset;
	esize_t	length;
}MAX_DATA;

typedef struct {		/* Reset iomode gop parameter structure. */
	esize_t	offset;
	esize_t	length;
} RESETIO_DATA;

typedef struct {		/* Close iomode gop parameter structure. */
	esize_t	offset;
	esize_t	length;
	int	modified;
	int	accessed;
} CLOSE_DATA;

typedef struct {		/* Lseek iomode gop parameter structure. */
	int	sbase;
	esize_t	offset;
} LSEEK_DATA;

typedef struct {		/* Iseof iomode gop parameter structure. */
	esize_t	offset;
	esize_t	length;
	int	*result;
} ISEOF_DATA;

