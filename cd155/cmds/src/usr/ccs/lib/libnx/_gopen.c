/*
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_gopen.c,v 1.8.2.1 1995/06/11 18:42:45 kat Exp $
 *
 * Perform a global open on a pathname and set the PFS I/O mode.
 *
 *      HISTORY:
 *      $Log: _gopen.c,v $
 * Revision 1.8.2.1  1995/06/11  18:42:45  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.8  1995/04/04  18:55:07  rlg
 * The gopen step (for the M_UNIX, M_RECORD, and M_ASYNC I/O modes) of
 * duplicating the file server's file table was redesigned.  This change
 * results in only one "many to one" messages (sent from N-1 compute nodes
 * to logical node 0 in the application's partition) and one set of "one to
 * many" messages (sent from each file server to those N-1 compute nodes).
 * The old code required each of the compute nodes in the application's
 * partition (minus one) to execute a sequence of four RPCs to the file
 * servers containing the header file and each stripe file.  This resulted
 * in four "many to one" messages sent to each of those file servers.
 *
 *  Reviewer:  Balaji Narasimhan, Stan Smith
 *  Risk:      Medium (number of modules changed, scope of redesign)
 *  PTS#       9637
 *  Testing:   pfs and fileio EATs, five Eval gopen tests,
 *             rw and iomode integration tests
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnx/_gopen.c
 *             cmds_libs/src/usr/ccs/lib/libnx/pfs_iomode.h
 *             server/svr/src/svr/emulator/pfs_user_side.c
 *             server/svr/src/svr/server/conf/syscalls.master
 *             server/svr/src/svr/server/uxkern/fsvr.defs
 *             server/svr/src/svr/server/uxkern/fsvr_server_side.c
 *             server/svr/src/svr/server/uxkern/pfs2.defs
 *             server/svr/src/svr/server/uxkern/pfs2_server_side.c
 *
 * Revision 1.6  1994/06/13  15:41:04  rlg
 * Added the M_ASYNC I/O mode for shared files.  This mode is characterized by:
 *     o	each node has a unique file pointer,
 *     o	nodes are not synchronized
 *     o	file access is unrestricted
 *     o	standard UNIX file sharing semantics requiring atomicity of I/O
 *      	are not preserved.
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
 * Revision 1.5  1994/06/03  15:16:16  rlg
 * Merged the changes from revisions 1.2.4.2, 1.2.4.4, and 1.2.4.5 on the
 * R1.2 branch into the mainline
 *
 * Revision 1.2.4.5  1994/06/03  15:06:07  rlg
 * Initialized a pointer to NULL so an error path would not attempt to free a
 * bogus address.
 *
 * Revision 1.2.4.4  1994/06/03  14:51:36  rlg
 * This update fixes two errors introduced in previous bug fixes.  The fix
 * for 6237 used the fstat() function where festat() should have been used.
 * The fix for 6244 had an error path where the file was closed twice.  Both
 * problems were resolved with this update.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #:  9180
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  libnx/_gopen.c
 *
 * Revision 1.4  1994/03/29  21:38:26  bks
 * Changed fourth argument from type int to type mode_t to match nx.h, and
 * specification for _gopen().
 *
 *  Reviewer: Cris Derr
 *  Risk: L
 *  Benefit or PTS #: 8766
 *  Testing: Compiled libnx, showfs, ls, tar, mount
 *  Module(s): gopen()
 *
 * Revision 1.2.4.2  1994/03/21  18:38:10  rlg
 * Changed _gopen() to first determine if the file exist and, if it doesn't,
 * if it should be deleted on an error condition.  The error processing code
 * was also changed to delete the file if the  delete_on_error  flag is set.
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #: 6244
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  libnx/_gopen.c
 *
 * Revision 1.3  1994/03/01  22:38:18  dbm
 * Mainline commit of fixes for bug #6237.
 *
 * Revision 1.2.4.1  1994/03/01  22:10:46  dbm
 * Fixed gopen() to disallow opening of device special files.
 *  Reviewer: Bob Godley
 *  Risk:L
 *  Benefit or PTS #:6237
 *  Testing: Specific test case, ran bigesize EAT.
 *  Module(s):
 * 	cmds_libs/usr/ccs/lib/libnx/_gopen.c
 *
 * Revision 1.2  1993/06/23  23:18:35  dbm
 * Modified to work correctly with 1 node.
 *
 * Revision 1.1  1993/02/12  17:32:22  dbm
 * Initial revision
 *
 */


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syscall.h>
#include <unistd.h>

#include "nx.h"
#include "pfs_iomode.h"

/*
 * _gopen.c
 *
 * Description:
 *		This function is used to open a shared file and 
 *		set the PFS I/O mode of the file. At the same time, 
 *		a global synchronizing operation is performed.
 *		This function returns the file descriptor handle
 *		to the open file on success.  On failure, a negative
 *		value is returned and errno is set.
 *	
 * Parameters:	
 *		path	Pathname of the file to open.
 *
 *		oflag	Access flag.
 *	
 *		iomode	An integer from M_UNIX through M_ASYNC that sets
 *			the PFS I/O mode of the designated file:
 *
 *			M_UNIX		Individual file pointer.
 *
 *			M_LOG		Common file pointer.
 *
 *			M_SYNC		Synchronized, common file pointer,
 *					variabgle length requests.
 *
 *			M_RECORD	Synchronized, common file pointer,
 *					fixed length requests.
 *
 *			M_GLOBAL	Synchronized, common file pointer,
 *					synchronized requests.
 *
 *			M_ASYNC		Individual file pointer, no token
 *					synchronization on reads *or* writes.
 *
 *		mode	Permissions of the file, (used when creating
 *			the file).
 *
 * Returns:
 *		The file descriptor handle to the file that was
 *		opened.
 */


int	nnodes;
int	node;
int     ptype;

int
_gopen(path, oflag, iomode, mode)
char	*path;
int	oflag;
int	iomode;
mode_t	mode;
{
    int                 error;
    int                 fdes;
    long                fdte_size;
    struct estat        estat_buf;
    pfs_gopen_info_t    local_gopen_info;
    pfs_gopen_info_t    node0_gopen_info;

    int                 close_on_error  = 0;
    int                 delete_on_error = 0;
    int                 status = 0;
    unsigned char       *fdte_info = NULL;

    int                 *tmp_ptr;
    unsigned int        tmp_1;



    node    = mynode();
    nnodes  = numnodes();
    ptype   = myptype();

    /*
     * Copy the input parameters into the local_gopen_info structure.
     */
    strncpy(local_gopen_info.file_name, path, MAXPATHLEN);
    local_gopen_info.access = oflag;
    local_gopen_info.mode = mode;
    local_gopen_info.iomode = iomode;
    local_gopen_info.fdt_info_length = 0;
    local_gopen_info.errno = 0;
    local_gopen_info.unixpid = getpid();
    local_gopen_info.gopen_reply_port = -1;    /*  MACH_PORT_NULL;  */

    if (node == 0) {
        /*
         * First, determine if file should be deleted on an
         * error:
         */
        if ((oflag & O_CREAT) && !(oflag & O_EXCL)) {
            if (access(path, F_OK)) {
                delete_on_error = 1;
            }
            errno = 0;
        }

        if ((fdes = open( path, oflag, mode)) < 0) {
            if (nnodes > 1) {
                /*
                 * Need to propagate this error along to the 
                 * other nodes.
                 */
                local_gopen_info.errno = errno;
            } else {
                return -1;
            }

        } else {

            close_on_error = 1;

            /*
             * Perform a stat to ensure that the gopen
             * is being performed on a normal file.
             */
            if ((error = festat(fdes, &estat_buf)) < 0 ) {
                local_gopen_info.errno = errno;
                goto FILE_ERROR;
            }

            /*
             * Check for regular file:
             */
            if (!S_ISREG(estat_buf.st_mode)) {
                local_gopen_info.errno = EINVAL;
                goto FILE_ERROR;
            }

            /*
             * Get the size of the local fdte structure.
             */
            if (syscall(SYS_pfs_get_fdte_size,
                        fdes, 
                        &local_gopen_info.fdt_info_length) < 0) {
                local_gopen_info.errno = errno;
                goto FILE_ERROR;
            }

            /*
             * Allocate the buffer to hold the fdte structure info:
             */
	    fdte_size = local_gopen_info.fdt_info_length;
            fdte_info = (unsigned char *)malloc(fdte_size);
            if (fdte_info == NULL) {
                local_gopen_info.errno = ENOMEM;
                goto FILE_ERROR;
            }

            /*
             * Get the local fdte information from the emulation
             * library for distribution to the other nodes.  Also,
	     * create a reply port (needed for processing of the
	     * M_UNIX, M_RECORD, and M_ASYNC I/O modes).
             */
            if (syscall(SYS_pfs_get_fdte_info,
			fdes, fdte_info, nnodes,
			&local_gopen_info.gopen_reply_port) < 0) {
                local_gopen_info.errno = errno;
		goto FILE_ERROR;
	    }
	}

FILE_ERROR:
        if (local_gopen_info.errno) {
            if (close_on_error) {
                error = close(fdes);
                close_on_error  = 0;
            }
            if (delete_on_error) {
                error = unlink(path);
                delete_on_error  = 0;
            }
        }
    }

    if (nnodes > 1) {
        /*
         * Compare the gopen parameter information with all of the 
         * nodes in the application: 
         */
        local_gopen_info.unixpid = getpid();
	if ((status = exchange(&local_gopen_info,
			      &node0_gopen_info)) != 0 ) {
	    if (status > 0) {
		/*
		 * At least on node is reporting an error, so set
		 * errno in all nodes to the same value.  (A negative
		 * value of  status  indicates an error reported by
		 * the _gdhigh() function, so errno is already set.)
		 */
		errno = status;
	    }
	    goto ERROR_EXIT;
	}

        /*
         * Make sure that node 0 opened the file correctly:
         */
        if (node0_gopen_info.errno) {
            errno = node0_gopen_info.errno;
            goto ERROR_EXIT;
        }
    }

    /*
     * Make sure valid iomode:
     */
    if ((iomode < M_UNIX) || (iomode > M_ASYNC)) {
        errno = EINVAL;
        goto ERROR_EXIT;
    }

    /* 
     * Send the fdte information from node 0 to the other nodes:
     */
    if (nnodes > 1) {
        if (node == 0) {

	    /*
	     * Broadcast the fdte_info structure to all the other
	     * nodes in the application.
	     */
	    if (_csend(PFS_GOPEN_MSG_TYPE,
		       fdte_info, fdte_size, -1, ptype) < 0 ) {
		goto ERROR_EXIT;
	    }

        } else {
            /*
	     * Node 1..N:   Receive the fdte information from
	     *              node 0 and send it down to the
	     *              emulation library to establish the
	     *              file table entry.
	     */
	    fdte_size = node0_gopen_info.fdt_info_length;
	    fdte_info = (unsigned char *)malloc(fdte_size);
	    if (fdte_info == NULL) {
	        errno = ENOMEM;
		goto ERROR_EXIT;
	    }

            if (_crecv(PFS_GOPEN_MSG_TYPE,
		       fdte_info, fdte_size) < 0 )   {
	        goto ERROR_EXIT;
	    }

	    /*
	     * Pass fdte_info to the emulation library.
	     */
	    if (syscall(SYS_pfs_put_fdte_info,
			fdte_info, node0_gopen_info.fdt_info_length,
			iomode, &fdes) < 0) {
	        goto ERROR_EXIT;
	    }
	}

	if ((iomode == M_UNIX)   ||
	    (iomode == M_RECORD) ||
	    (iomode == M_ASYNC)   ) {
	    /*
	     * These I/O modes require that each compute node
	     * maintains its own file pointer.  The implementation
	     * is also optimized so that each node maintains its
	     * own set of file ports for the file (and each of its
	     * stripe file).  Call the emulator to duplicate the
	     * fdte information and file ports:
	     */
	    if (syscall(SYS_pfs_dup_fdte_info,
			fdes, node, nnodes,
			node0_gopen_info.unixpid,
			node0_gopen_info.gopen_reply_port) < 0) {
	        goto ERROR_EXIT;
	    }
	} 
    }

    /*
     * Now that the local fdt entries have been established, it's time
     * to set the iomode of the file.
     */
    gsync();
    if (_pfs_setiomode(fdes, iomode, 
		       (nnodes > 1 ? node0_gopen_info.fdt_info_length : 
			local_gopen_info.fdt_info_length)) < 0) {
        goto ERROR_EXIT;
    }

    free(fdte_info);

    return fdes;


ERROR_EXIT:
    if (fdte_info != NULL) {
        free(fdte_info);
    }
    if (close_on_error) {
        error = close(fdes);
    }
    if (delete_on_error) {
        error = unlink(path);
    }
    return -1;
}



/*
 * exchange
 *
 * Description:
 *              This is a local function that is used to exchange the iomode
 *              information using nx message passing. 
 *
 * Parameters:  
 *              local_gopen_info,       local gopen information.
 *              node0_gopen_info:       gopen information from node 0.
 *
 * Returns:
 *              Status of the operation.
 *              This function will return a 0 on success, on failure a -1
 *              will be returned and errno will be set to the error code.
 */
static int 
exchange(local_gopen_info, node0_gopen_info)
pfs_gopen_info_t        *local_gopen_info;      /* Local iomode info  */
pfs_gopen_info_t        *node0_gopen_info;      /* Global iomode info */
{
    long    cmp_status = 0;
    long    temp;


    errno = 0;
    /*
     * Node 0 will broadcase its gopen parameters to all
     * other nodes in the partition:
     */

    if (node == 0) {

        if (_csend(PFS_GOPEN_MSG_TYPE,
		   local_gopen_info,
		   sizeof(pfs_gopen_info_t),
		   -1, _myptype()) < 0 ) {
	    return -1;
        }

	strcpy(node0_gopen_info->file_name,
	       local_gopen_info->file_name);
	node0_gopen_info->iomode  = local_gopen_info->iomode;
	node0_gopen_info->access  = local_gopen_info->access;
	node0_gopen_info->mode    = local_gopen_info->mode;
	node0_gopen_info->fdt_info_length =
                                    local_gopen_info->fdt_info_length;
	node0_gopen_info->errno   = local_gopen_info->errno;
	node0_gopen_info->unixpid = local_gopen_info->unixpid;
	node0_gopen_info->gopen_reply_port =
                                    local_gopen_info->gopen_reply_port;

    } else {

        /*
	 * Compare this node's parameters against node 0's parameters
	 */
        if (_crecv(PFS_GOPEN_MSG_TYPE,
		   node0_gopen_info,
		   sizeof(pfs_gopen_info_t)) < 0 ) {
	    return -1;
	}

        if ((strcmp(local_gopen_info->file_name,
		    node0_gopen_info->file_name))                  || 
	    (local_gopen_info->access != node0_gopen_info->access) ||
	    (local_gopen_info->iomode != node0_gopen_info->iomode)) {

	    cmp_status = EMIXIO;

	}

    }

    /*
     * get the results of the global compare operation
     */
    if (_gihigh(&cmp_status,
		(long)1,
		&temp) < 0) {
        return -1;
    }

    return cmp_status;
}
