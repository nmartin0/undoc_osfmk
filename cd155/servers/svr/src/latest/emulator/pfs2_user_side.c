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
 *
 * HISTORY
 * $Log: pfs2_user_side.c,v $
 * Revision 1.57  1995/03/13  17:36:06  stans
 *  Correct emulator exception causing buffer pointer arithmetic and subsequent
 *  access in function 'read_pfs_header()'. Also use the correct header file data
 *  read offset when reading additional header data beyond 8192.
 *
 *  Reviewer:Bob Godley
 *  Risk:medium
 *  Benefit or PTS #:12666
 *  Testing:
 *         WW10 sats
 *         Developer tests:
 *                 pathkiller
 *                 PFS i/o perf tests.
 *                 All extended file-size syscalls; lestat() and friends
 *
 * Revision 1.56  1995/03/07  23:52:07  stans
 *  PFS MAX_IONODE_REQUEST boosted from 256k to 1 Meg.
 *
 *  Reviewer:rlg
 *  Risk:low
 *  Benefit or PTS #:11397
 *  Testing:WW09 sats
 *
 * Revision 1.55  1995/03/02  18:47:08  stans
 * Vnode caching support.
 *
 *  Reviewer:jlitvin,suri,cfj
 *  Risk:medium
 *  Benefit or PTS #:8129
 *  Testing:WW07 sats
 *
 * Revision 1.54  1994/11/18  20:23:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.53  1994/08/31  22:46:02  mtm
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
 * Revision 1.51.2.2  1994/08/19  22:42:19  dbm
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
 * Revision 1.51.2.1  1994/08/10  18:53:40  rlg
 * The queue limit for the server's root vnode port was increased to
 * MACH_PORT_QLIMIT_MAX.  This change, coupled with the changes to the
 * emulator for the reply port, work around a deadlock problem with the
 * new NORMA implementation.
 *
 * Committing in R1.3 WW33 to fix PTS #10409
 *
 * Modified Files:   server/bsd/init_main.c
 *                   emulator/emul_stack.h
 *                   emulator/emul_stack_alloc.c
 *                   emulator/pfs2_user_side.c
 *
 * Revision 1.51  1994/07/01  22:01:42  rlg
 * Fixed error introduced by the M_ASYNC enhancement.  An invalid test in the
 * pfs_multi_write() routine caused both M_RECORD and M_ASYNC I/O modes to fail
 * if the file was opened with the O_APPEND flag.
 *
 *  Reviewer:  none
 *  Risk:  low
 *  Benefit or PTS #:  10060
 *  Testing:  fileio and pfs EATS; failing test case; I/O mode integration test
 *  Module(s):
 *
 * Revision 1.50  1994/06/16  02:15:32  brad
 * Very minor changes to prevent lint warnings in pfs2_user_side.c
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: get rid of lint warnings
 *  Testing: built, booted, brief developer testing
 *  Module(s): emulator/{fsvr_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.49  1994/06/14  17:42:15  brad
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
 * Revision 1.48  1994/06/14  05:51:28  brad
 * Added arguments that MiG wants to the async multi_utimes() message
 * receive calls.  The MiG-generated RPC stubs reference the timeval_2_t
 * pointers even though they aren't used in the receive stub.  This
 * cropped up now due to cfj's server modification to trap bogus pointer
 * references to page 0.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 9821
 *  Testing: Ran test in bug report, PFS EATs on 64 nodes
 *  Module(s): emulator/pfs2_user_side.c
 *
 * Revision 1.47  1994/06/13  15:27:10  rlg
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
 * Revision 1.46  1994/06/02  22:12:41  chrisp
 * e_rforkmulti_call() and e_forkfamily_call() call new routines
 * fdt_get_rights() and fdt_port_modrefs() to assemble a table of file
 * ports to be transferred to the server. Note that the first 2 entries
 * in this table are the parent's root and current directory ports.
 * Explicit installation of these ports into child tasks and the
 * release of child emulator threads has been eliminated.  Fileserver
 * RPC fsvr_file_ref() now takes an extra parameter giving the reference
 * adjustment required.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): bsd_user_side.c emul_chkpnt.c fsvr_user_side.c pfs2_user_side.c
 *
 * Revision 1.45  1994/05/09  16:32:39  jlitvin
 * Fix minor lint errors: can't take the address of a register variable!
 *
 * Revision 1.44  1994/04/26  22:42:52  brad
 * Merged revision 1.31.2.10 from the R1.2 branch.
 *
 * Revision 1.31.2.10  1994/04/26  22:30:47  brad
 * Move check for 0-length writes in pfs_multi_write *after* the token is
 * acquired, so M_SYNC mode synchronization works properly.
 *
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 9091
 *  Testing: PFS SAT and EATs on 64 nodes
 *  Module(s): emulator/pfs2_user_side.c
 *
 * Revision 1.43  1994/04/20  20:31:54  rlg
 * merge of revision 1.31.2.9 from the R1.2 branch
 *
 * Revision 1.31.2.9  1994/04/20  18:45:27  rlg
 * The PFS close function was enhanced to close the stripe files in parallel.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  medium
 *  Benefit or PTS #:  PTS # 8953
 *  Testing:  failing test case; pfs and fileio EATs
 *  Module(s):  emulator/fsve_user_side.c
 * 	     emulator/pfs2_user_side.c
 * 	     server/uxkern/pfs2.defs
 * 	     server/uxkern/pfs2_server_side.c
 *
 * Revision 1.42  1994/04/13  00:10:36  rlg
 * Merged the changes from version R1.31.2.8 on the R1.2 branch into the trunk.
 *
 * Revision 1.31.2.8  1994/04/12  22:48:26  rlg
 * Fixed problem with a misleading error message when removing a pfs file when
 * the access permissions are not set correctly in the stripe directories.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #:  PTS #8895
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  pfs2_user_side.c [pfs_multi_unlink(), multi_unlink()]
 *
 * Revision 1.41  1994/04/06  18:52:47  brad
 * Merged revision 1.31.2.7 from the R1.2 branch.
 *
 * Revision 1.31.2.7  1994/04/05  22:50:15  brad
 * Added an outer loop to pfs_multi_read and pfs_multi_write that breaks
 * large I/O's into smaller chunks, so that at most MAX_IONODE_REQUEST
 * bytes are sent at once to any one I/O node.  This works around many
 * cases of NORMA not being able to handle lots of PFS I/O on the I/O nodes,
 * and improves performance by holding data back on the compute nodes
 * and thus preventing I/O nodes from paging.  Also added performance
 * optimizations for the case where sfactor == 1.
 *
 *  Reviewer: Bob Godley
 *  Risk: Med
 *  Benefit or PTS #: 8744
 *  Testing: Lots of developer tests, fileio and pfs EATs on 64 nodes,
 *     SLAB solvers at various sizes of computation.
 *  Module(s): emulator/pfs2_user_side.c
 *
 * Revision 1.40  1994/03/11  19:08:18  rlg
 * Merged changes from the R1.2 branch to the R1.3 branch (1.31.2.6)
 *
 * Revision 1.31.2.6  1994/03/11  16:32:30  rlg
 * The utimes() function for PFS files was reimplemented following the model
 * of pfs_multi_stat(), so that the header file and all stripe files have the
 * same value set in the access and modification time fields.  The old code
 * only set these fields in the header file.
 *
 *  Reviewer:  Brad Rullman
 *  Risk: medium
 *  Benefit or PTS #:  PTS #6870
 *  Module(s):  emulator/fsvr_user_side.c
 *              emulator/pfs2_user_side.c
 *              server/uxkern/fsvr.defs
 *              server/uxkern/pfs2.defs
 *              server/uxkern/fsvr_server_side.c
 *              server/uxkern/pfs2_server_side.c
 *              server/vfs/vfs_syscalls.c
 *
 * Revision 1.39  1994/02/16  00:42:53  dbm
 * Merge from 1.2 sandbox revision 1.31.2.5
 *
 * Revision 1.31.2.5  1994/02/15  23:56:03  dbm
 * Added code to allow PFS operations to work with async I/O and NFS files.
 * Also added code to allow lsize to work correctly in M_RECORD mode.
 *
 *  Reviewer: Brad Rullman
 *  Risk:L
 *  Benefit or PTS #: 6337, 8067
 *  Testing: Specific test cases, ran PFS Eats several times.
 *  Module(s):
 * 	pfs2_user_side.c
 *
 * Revision 1.38  1994/02/04  19:46:32  brad
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
 * Revision 1.37  1994/01/27  22:28:15  brad
 * Merge of revision 1.31.2.4 from the R1.2 branch.
 *
 * Revision 1.31.2.4  1994/01/27  01:40:11  brad
 * Added a workaround for PTS #7082 ... when the -plk NX switch is used,
 * vm_copy() does not preserve the 'wired' state of the VM pages in the
 * user's buffer.  Workaround by not using vm_copy() when -plk used,
 * resulting in possible PFS performance degradation at higher bandwidths.
 *
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 7082
 *  Testing: Verified workaround with test from PTS report, ran PFS EATs
 *     on 64 nodes, ran PFS SAT on 64 nodes.
 *  Module(s): emulator/{pfs2_user_side.c,pfs_msgutil.c,i860/emul_machdep.c}
 *
 * Revision 1.36  1994/01/26  21:05:55  brad
 * Merge of revision 1.31.2.3 from the R1.2 branch.
 *
 * Revision 1.31.2.3  1994/01/26  03:40:23  brad
 * Added check to check_stripe_attributes() that stripe directories are
 * actually directories.  This way we bail out at mount time, rather
 * than when an attempt is made to create a PFS file in the mounted
 * file system.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 7921
 *  Testing: Performed multiple mounts and verified that the check works.
 *     Ran PFS EATs on a 16-node system.
 *  Module(s): emulator/pfs2_user_side.c
 *
 * Revision 1.35  1994/01/10  02:21:10  brad
 * Removed debug statement that caused compilation failure when compiling with
 * DEBUG_PFS on.  Problem was introduced in previous checkin.
 *
 * Revision 1.34  1994/01/05  17:08:34  brad
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
 * Revision 1.33  1993/12/23  01:47:20  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: None.
 *  Testing: Booted and ran minimal PFS tests.
 *  Module(s): emulator/emul_stack_alloc.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.32  1993/12/20  19:52:23  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: don't declare as 'register' variables whose addresses
 * 	will be taken; so we can pass 'lint'
 *  Testing: builds
 *  Module(s): pfs2_user_side.c
 *
 * Revision 1.31  1993/11/13  21:34:04  brad
 * Added missing return statement to linear_copy() function.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: Fixes PFS EAT failures with new R4.5 compilers.
 *     Neither the R4.5 or the R1.1 compilers caught the missing return, but
 *     the latter compiler always used 0 as the return value, while the former
 *     uses an undefined value (which caused bogus error returns on PFS reads).
 *  Testing: Ran various PFS read tests to verify fix.
 *  Module(s): emulator/pfs2_user_side.c
 *
 * Revision 1.30  1993/11/11  23:00:13  brad
 * Fixed bogus check in eseek(); now lseek() is only called on non-PFS files.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: 7079 - now eseek() works on all valid offsets on PFS files.
 *  Testing: Performed eseeks on many offsets, positive and negative, up to
 *           16 GB, and verified that the succeeded.  Focused on ranges known
 *           not to work previously, as described in bug report for PTS #7079.
 *  Module(s): emulator/{pfs_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.29  1993/10/27  17:36:50  brad
 * Modified pfs_multi_stat and pfs_multi_fstat so that the file block size
 * returned in the stat structure reflects the block size of the stripe data
 * rather than the block size of the PFS header.
 *
 * Revision 1.28  1993/09/02  17:58:21  brad
 * Trivial performance tweak to previous checkin.
 *
 * Revision 1.27  1993/09/02  07:40:44  brad
 * Fix for bugs 5648 and 5762: check open mode of PFS file on reads and writes,
 * since Fast Path doesn't do it.
 *
 * Revision 1.26  1993/09/02  06:44:38  brad
 * Fix for bugs 5636, 5651, and 6230: PFS files with no read permission or
 * no write permission for the user were not handled correctly because stripe
 * attributes could not be read or written, respectively.  Fixed for now by
 * temporarily changing header file permissions in order to perform these
 * operations ... long term fix will be to add new readheader/writeheader
 * vnode operations in the server.
 *
 * Revision 1.25  1993/08/06  23:55:52  brad
 * Removed PFS_NAME_MAX restriction of 219 bytes on a PFS file name.  Now
 * the stripefile name is truncated if necessary in order to add the
 * necessary "uniqueness" suffix and still stay under NAME_MAX (255 bytes) in
 * length.
 *
 * Revision 1.24  1993/07/27  02:12:02  brad
 * Fixed bug #5633: pfs_multi_open was not cleaning up the header file that
 * was created before an ENAMETOOLONG could be detected.  The result was
 * the correct error returned to the user, and an orphan PFS header file.
 *
 * Revision 1.23  1993/07/21  22:36:10  dbm
 * Added check for M_RECORD and M_PARALLEL I/O modes to keep files from being
 * incorrectly truncated.
 *
 * Revision 1.22  1993/07/21  19:22:30  brad
 * Streamlined maintenance of stripefile lengths in pfs_multi_write
 * when a PFS file is being write-extended.
 *
 * Revision 1.21  1993/07/17  02:47:30  brad
 * Made PFS open/creat operations non-interruptible.  Made other mods to
 * pfs_multi_open to reduce the possibility of creating "invalid" PFS
 * files on interrupt or if the system crashes.
 *
 * Revision 1.20  1993/07/16  01:34:43  brad
 * Changed pfs_multi_unlink() so that "invalid" PFS files are removed, rather
 * than returning an error.  E.g. this allows users to force-remove a PFS file
 * whose stripe data is not available.
 *
 * Revision 1.19  1993/07/15  17:33:24  brad
 * Added cleanup of server state via VIO_ERROR to pfs_multi_unlink().  This
 * is part of the support to make removing a PFS file an atomic operation.
 *
 * Revision 1.18  1993/07/09  21:49:32  brad
 * Added pfs_multi_rename().
 *
 * Revision 1.17  1993/06/21  21:38:00  brad
 * Fixed typo found in pfs_multi_lsize().
 *
 * Revision 1.16  1993/06/16  23:10:34  wunder
 * Added checks in pfs_multi_stat and pfs_multi_fstat for requesting info
 * on PFS file greater than 2G-1 bytes, now return EFBIG.
 *
 * Revision 1.15  1993/06/16  22:00:55  brad
 * Added some clarifying comments on MiG operations in pfs_multi_stat,
 * pfs_multi_fstat, and pfs_multi_statfs.
 *
 * Revision 1.14  1993/06/16  21:52:22  brad
 * Don't check for write access to stripedirs when mounting a PFS file system.
 * This allows striping to NFS file systems for users that have appropriate
 * permissions on the remote host.
 *
 * Revision 1.13  1993/06/16  20:33:11  dbm
 * Changed all references to pfs_iomode to pfs_iomode_info to allow single
 * node applications to obtain the PFS I/O mode info.
 *
 * Revision 1.12  1993/06/09  01:20:31  brad
 * Modified pfs_multi_statfs so all numbers of blocks in returned statfs
 * struct are in UBSIZE units.  This avoids inconsistent numbers being
 * reported when different stripe file systems have different fragment sizes,
 * causing statfs to return numbers in different units depending on the file
 * system.
 *
 * Revision 1.11  1993/06/06  01:11:14  brad
 * Changed some EINVAL errors to ENOTPFS.
 *
 * Revision 1.10  1993/05/27  01:54:18  brad
 * Switched to using Fast Path read/write RPC's for both PFS header files
 * and stripefiles.
 *
 * Revision 1.9  1993/05/25  18:38:30  dbm
 * Added isc_multi_register to allow interruptable system calls to work with
 * PFS file systems.
 *
 * Revision 1.8  1993/05/12  00:13:40  brad
 * Added validation of stripe unit size to pfs_check_stripe_attributes.
 *
 * Revision 1.7  1993/05/11  01:04:26  brad
 * Removed references to obsolete fdte->mappable flag.
 *
 * Revision 1.6  1993/05/04  01:04:18  brad
 * Fixed a bug in pfs_multi_open that caused already-existing PFS files that
 * are opened with O_CREAT to be unlinked if an error occured on any of the
 * stripefile open operations.  Now the only problem is that if O_TRUNC is
 * specified, the stripefiles that *are* successfully opened are still
 * truncated if another stripefile gets an error.  Chicken and egg problem: 
 * we don't know if an error occurs on the open/truncate until an
 * open/truncate operation is completed on all stripefiles, but then it is
 * too late to back out on any that succeed.
 *
 * Revision 1.5  1993/04/30  23:50:10  brad
 * Added PFS_NAME_MAX check for more stringent stripefile name length checking.
 * Added optimization to pfs_multi_write to avoid data copying if stripe unit
 * merging is not necessary.
 *
 * Revision 1.4  1993/04/27  16:56:25  brad
 * Added support for PFS debug levels.
 *
 * Revision 1.3  1993/04/15  18:37:00  dbm
 * Added locks to protect fdte token data structures.
 *
 * Revision 1.2  1993/04/03  03:18:14  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.20  1993/03/20  23:18:14  brad
 * Added PFS support for readv()/writev(): pfs_multi_{read,write}() now
 * handle user buffer spaces of varying dimensions by calling generic data
 * copying routines (currently supported are linear_copy() and vector_copy()).
 *
 * Revision 1.1.2.19  1993/03/11  00:30:41  dbm
 * Added pfs_malloc and pfs_free to allocate memory regions outside of
 * emulator memory.  Also added changes  to support statpfs.
 *
 * Revision 1.1.2.18  1993/03/10  06:11:23  brad
 * Finished extended file size and offset support, For Real this time,
 * specifically the ETOOBIG() macro.  Now send iomode in open RPC's.
 * Finished pfs_multi_write() error handling.  Joined pfs_multi_lseek()
 * and pfs_multi_eseek().  Added support for chmod, fchmod, chown,
 * fchown, fsync, and lsize.
 *
 * Revision 1.1.2.17  1993/02/23  20:06:33  brad
 * Added statfsbuf argument to pfs_multi_statfs().
 *
 * Revision 1.1.2.16  1993/02/23  04:56:59  brad
 * Added pfs_check_stripe_attributes() for validation of stripe attributes.
 * Added support for access() and truncate()/ftruncate().
 *
 * Revision 1.1.2.15  1993/02/12  22:43:14  brad
 * Added ENOSDIR error handling to open() and unlink().
 *
 * Revision 1.1.2.14  1993/02/12  17:11:25  dbm
 * Added more debug for debugging PFS file I/O with I/O modes.
 *
 * Revision 1.1.2.13  1993/02/09  22:47:21  brad
 * Renamed pfs_fstat to pfs_fdevstat to avoid naming clash with new
 * asynchronous fstat RPC.  Added PFS support for fstat, estat, and
 * festat.  Made all currently implemented PFS system calls concurrent.
 *
 * Revision 1.1.2.12  1993/02/04  00:49:19  brad
 * Finished support for extended PFS file sizes and offsets (though an
 * extended number of stripe units is NOT currently supported).  We now set
 * stripefile sizes correctly if user seeks past end-of-file on a PFS file
 * and writes.  PFS read-at-offset and write-at-offset Mig stubs are now
 * used, so lseek() RPC's are no longer needed.  Implemented pfs_multi_eseek().
 *
 * Revision 1.1.2.11  1993/01/20  20:26:45  brad
 * Added copy-on-write logic to pfs_multi_read() and pfs_multi_write(), so
 * page-aligned stripeunits can use vm_copy().  Added support for extended
 * PFS file offsets and sizes.
 *
 * Revision 1.1.2.10  1993/01/11  17:24:46  dbm
 * Added changes to support PFS files with I/O modes.
 *
 * Revision 1.1.2.9  1993/01/08  02:11:50  brad
 * Fixed vm_allocate bug in pfs_multi_write (now use emul_vm_map).
 * Added pfs_multi_unlink().  Fixed isc_deregister parameters.
 *
 * Revision 1.1.2.8  1993/01/05  22:37:47  brad
 * Added stripe unit merging capability to pfs_multi_write().  All stripefile
 * writes are now concurrent.
 *
 * Revision 1.1.2.7  1992/12/22  02:25:48  dbm
 * Changed parameter order on file_token_release() function to make it
 * consistent with the file_token_acquire() function.  Fixed multi_open
 * to acquire after stripe files opened.
 *
 * Revision 1.1.2.6  1992/12/22  00:58:02  brad
 * Fixed bug preventing PFS files with no write permissions from being
 * created.
 *
 * Revision 1.1.2.5  1992/12/21  22:08:59  brad
 * Moved stripefile macros to pfs_fdt.h.  Final cleanup of
 * pfs_multi_read().  Added pfs_sync_actuals() for read() and
 * write() error handling.  Added non-fastpath (i.e., temporary)
 * version of pfs_multi_lseek().
 *
 * Revision 1.1.2.4  1992/12/18  18:14:25  brad
 * Added p_magic to statpfs structure, for PFS file header validation.
 *
 * Revision 1.1.2.3  1992/12/16  23:14:35  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.2  1992/12/12  01:48:24  brad
 * Update of latest PFS functionality in preparation for a merge with the
 * NX branch.
 *
 * Revision 1.1.2.1  1992/12/11  21:02:48  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.1  1992/11/25  22:20:11  brad
 * Initial revision
 *
 */

#ifdef	PFS

#include <mach_init.h>
#include <mach/mig_errors.h>
#include <uxkern/fsvr.h>

/* WARNING: the lengthy path name below is necessary to
 * prevent varargs.h from being found in the mk/release
 * subdirectory.  For some reason, even though CPATH has
 * "../server/include" before "mk/release/.../include",
 * the file is found in the latter directory, if it is
 * not qualified with a path prefix.
 */
#include <../server/include/varargs.h>
#include <sys/stat.h>
#include <sys/estat.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <machine/vmparam.h>
#include "emul.h"
#include "fdt.h"
#include "pfs_fdt.h"
#include "pfs_iomode.h"


extern int pfs_debug_flag;
extern int pfs_async_dflt;

#ifdef	DEBUG_PFS
/*
 * Macros for PFS debugging that are private to this module.
 */
#define PFS_DEBUG_SATTR(s, sattr) \
	if (pfs_debug_flag >= 3) { \
		e_printf s; \
		dump_pfsattr(sattr); \
	}
#define PFS_DEBUG_RECV(s1, s2, err) \
	if (pfs_debug_flag >= 3) { \
		e_printf s1; \
		(err) ? e_printf("\n") : e_printf s2; \
	}
#else

#define PFS_DEBUG_SATTR(s, sattr)
#define PFS_DEBUG_RECV(s1, s2, err)

#endif	DEBUG_PFS


/**
 ** Type definitions private to this module.
 **/

/*
 * Array for maintaining stripefile request and actual counts associated
 * with a read or write to a PFS file.
 */
typedef struct sfile_count {
	size_t		requested;	/* # bytes requested before EOF */
	size_t		actual;		/* # bytes actually read */
	off_t		offset;		/* temp offset for multi-staged I/O */
} sfile_count_t;

/*
 * Stripe attribute cache data structures
 */
uint_t          stripe_attr_cache_size = 0;
struct statpfs *stripe_attr_cache = NULL;
#define SA_CACHE_NULL ((struct statpfs *)0)
mach_port_t    *stripe_dir_vnode_ports;


/**
 ** Constants private to this module.
 **/

/*
 * Size of stripefile count array to staticly allocate.
 */
#define	BIG_STRIPE_FACTOR	64

/*
 * Directions for copying of stripefile data.
 */
#define	TO_USER		0	/* read operation */
#define	FROM_USER	1	/* write operation */

/*
 * Potential length of an ASCII string in hexadecimal format representing a
 * stripefile number.
 */
#define	LEN_SFILENUM	(sizeof(uint_t) * 2)

/*
 * Maximum length of the <hdr fname> portion of a PFS stripefile name, given
 * the maximum possible space needed for the suffix part of the name.  The
 * syntax of a stripefile name is:
 *
 *	<hdr fname>.<node#>.<device#>.<file#>.<stripefile#>
 *
 * The # fields are encoded in hexadecimal.
 */
#define	SFILE_HDR_NAME_MAX	\
	NAME_MAX - sizeof(node_t)*2 - sizeof(dev_t)*2 - sizeof(ino_t)*2 \
		 - sizeof(uint_t)*2 - 4

/*
 * Maximum number of bytes of data to write/read to/from any single I/O node.
 * This helps to funnel large amounts of data requested from many compute
 * nodes simultaneously, so that we don't force the I/O node(s) to start paging
 * heavily (resulting in poor I/O performance) or worse (out of memory 
 * deadlock).
 *
 * The value of this constant should be high enough that it does not affect PFS
 * performance.  It should also be a multiple of vm_page_size and of the most
 * common PFS stripe unit size and file system block size (64 KB).  Currently
 * (R1.2) PFS performance from one compute node tops out at a request size of
 * 256 KB per I/O node (i.e. 2 MB if writing to 8 I/O nodes), which is the
 * largest request size that the kernel's Fast Out-Of-Line code handles.
 */
#define	MAX_IONODE_REQUEST (1024*1024)


/**
 ** Macros private to this module.
 **/

/*
 * FS_TO_U_BLOCKS:
 *	Given the number of disk blocks, and the size in bytes of each block,
 *	the value of this macro is the number of blocks in units of size
 *	UBSIZE (defined in sys/param.h).  Extended numbers *not* supported.
 *
 */
#define FS_TO_U_BLOCKS(numblocks, blocksize) (\
	((numblocks) * (blocksize) / UBSIZE) \
)

/*
 * ROUND_DOWN:
 *	Round x down to the nearest multiple of y.
 *
 */
#define ROUND_DOWN(x, y)	(((x) / (y)) * (y))

/*
 * RAND:
 *	Compute a pseudo-random integer in the range 0 to RAND_MAX, using
 *	the given value as the seed value.  This macro is based on the rand(3)
 *	function from libc.
 *
 */
#define RAND_MAX 32767
#define RAND(seed)	((((long)(seed) * 1103515245L + 12345)>>16) & RAND_MAX)

/*
 * MOD_SUB:
 *	Perform a modulus subtraction of y from x.  For example, 
 *	MOD_SUB(3, 6, 8) returns 5.
 *
 */
#define MOD_SUB(x, y, divisor)	((((x) + (divisor)) - (y)) % (divisor))


/**
 ** Global Variables.
 **/

/*
 * Error variable for extended math operations.  Generally, it's assumed that
 * extended math operands generated by the OS will not cause an overflow, so
 * error checking is avoided in these cases.
 */
int	dont_care;


/**
 ** External Declarations.
 **/
extern	fdt_slot_t	fdt[NOFILE];	/* file descriptor table */
extern	int 		fdt_lastfile;	/* HWM of fdt */
extern	spin_lock_t 	fdt_lock;	/* lock protecting fd table */	
extern  esize_t		ex_zero;
extern	esize_t		ex_neg_one;
extern	esize_t		ex_int_max;

extern mach_port_t	pfs_get_reply_port();


/**
 ** Forward references.
 **/
int	linear_copy();
int	pfs_fd_dealloc();
void	ref_file();
void	sf_fdt_init();
void	unref_file();
int	vector_copy();
void	pfs_vnode_port_cache_lookup();



/*
 * Name:
 *	pfs_malloc
 *
 * Description:
 *	Allocate memory beyond EMULATOR_END (beyond the end of emulator address
 *	space).  Memory allocated by this function is not inherited across 
 *	process forks, so should be used for temporary, large data structures
 *	only.
 *
 * Parameters: 
 *	data_addr	Location to return pointer to allocated memory.
 *
 *	data_len	Number of bytes of memory needed.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise ENOMEM.
 */
int
pfs_malloc(data_addr, data_len) 		
void 	**data_addr;
uint_t	data_len;
{
	*data_addr = (char *) EMULATOR_END;		

	if (emul_vm_map(mach_task_self(), (vm_offset_t *)data_addr, data_len,
			0, TRUE, MEMORY_OBJECT_NULL, 0, FALSE,
			VM_PROT_READ|VM_PROT_WRITE, VM_PROT_READ|VM_PROT_WRITE,
			VM_INHERIT_NONE)) {
		*data_addr = NULL;	/* set to NULL like real malloc */
		return(ENOMEM);
	}

	PFS_DEBUG(("  pfs_malloc: buf @ 0x%x size %d\n",*data_addr, data_len));

	return(ESUCCESS);
}

/*
 * Name:
 *	pfs_free
 *
 * Description:
 *	Deallocate memory that was allocated by the pfs_malloc() function.
 *
 * Parameters: 
 *	data_addr	Pointer to allocated memory.
 *
 *	data_len	Number of bytes to deallocate.
 *
 * Returns:
 *	Nothing.
 */
void pfs_free(data_addr, data_len)
void 	*data_addr;
uint_t	data_len;
{
	(void) vm_deallocate(mach_task_self(),
			     (vm_offset_t)data_addr, data_len);
}


/*
 * Name:
 *	rindex (emulator version of rindex(3))
 *
 * Description:
 *	Returns a pointer to the last occurrence of character c in string s,
 *	or a NULL pointer if c does not occur in the string.
 *
 * Parameters: 
 *	s	String to search.
 *
 *	c	Character to search for.
 *
 * Returns:
 *	Pointer to the first occurrence of character c in string s, or NULL
 *	if c does not occur in the string.
 */
char *
rindex(s, c)
	register char *s, c;
{
	register char *save;

	for (save = NULL;; ++s) {
		if (*s == c)
			save = s;
		if (!*s)
			return(save);
	}
	/* NOTREACHED */
}


/*
 * Name:
 *	strncmp (emulator version of strncmp(3))
 *
 * Description:
 *	Compares at most n pairs of characters from the strings pointed to by
 *	s1 and s2, returning an integer as follows:
 *
 *		Less than 0	If s1 is less than s2
 *		Equal to 0	If s1 is equal to s2
 *		Greater than 0	If s1 is greater than s2.
 *
 * Notes:
 *	Handles the pathological case where the value of n equals the maximum
 *	value of an unsigned long integer.
 *
 * Parameters: 
 *	s1	First string.
 *
 *	s2	Second string.
 *
 *	n	Number of characters to compare.
 *
 * Returns:
 *	A negative, zero, or positive value as described above.
 */
int	
strncmp(s1, s2, n)
	char	*s1;
	char	*s2;
	size_t	n;
{
	size_t	i;

	if (s1 == s2)
		return(0);
	for (i = 0; i < n && *s1 == *s2++; i++)
		if (*s1++ == '\0')
			return(0);
	return((i == n) ? 0 : (*(unsigned char *)s1 - *(unsigned char *)--s2));
}


/*
 * Name:
 *	pfs_fd_init
 *
 * Description:	
 *	Allocate and initialize a PFS striped file descriptor.
 *
 * Parameters:
 *	pfs_fd		Pointer to a pfs_fd_t pointer.  If successful, this
 *			function sets pfs_fd to reference a pointer to an
 *			allocated pfs_fd.  On error this function sets pfs_fd
 *			to NULL.
 *
 *	stripe_attr	Pointer to the statpfs structure describing the stripe
 *			attributes of the PFS file for which the descriptor
 *			is being allocated.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_fd_init(pfs_fd, stripe_attr)
	pfs_fd_t	**pfs_fd;	/* out - pointer to PFS descriptor */
	struct statpfs	*stripe_attr;
{
	pfs_fd_t	*pfsd;		/* PFS file descriptor */
	stripe_fd_t	*sf_fdt;	/* stripefile descriptor table */
	uint_t		sfd;		/* stripefile descriptor index */
	register uint_t	sfactor;	/* stripe factor */

	sfactor = stripe_attr->p_sfactor;

	/*
	 * Allocate a PFS file descriptor table entry.
	 */
	pfsd = (pfs_fd_t *) malloc(sizeof(pfs_fd_t) +
				   sizeof(stripe_fd_t)*(sfactor-1));
	if (pfsd == NULL) {
		*pfs_fd = NULL;
		return(ENOMEM);
	}

	fdte_pfsio_lock_init(pfsd);
	pfsd->p_use_token = (pfs_async_dflt ? 0 : 1);
	pfsd->p_offset.shigh = 0;
	pfsd->p_offset.slow = 0;
	pfsd->p_length.shigh = -1;
	pfsd->p_length.slow = -1;
	pfsd->p_stripe_unit_size = stripe_attr->p_sunitsize;
	pfsd->p_stripe_factor = sfactor;
	pfsd->p_start_stripedir = stripe_attr->p_start_sdir;

	sf_fdt = pfsd->p_stripe_fdt;
	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_fdt[sfd].s_fp = MACH_PORT_NULL;
	 	sf_fdt[sfd].s_offset = 0;
	}

	*pfs_fd = pfsd;
	return(ESUCCESS);
}


/*
 * Name:
 *	pfs_fd_dealloc
 *
 * Description:	
 *	Deallocate a PFS descriptor.  Force-close open stripefiles if
 *	necessary.
 *
 * Parameters:
 *	pfs_fd		Pointer to a PFS descriptor to be deallocated.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_fd_dealloc(pfs_fd)
	pfs_fd_t	*pfs_fd;
{
	int		error;


	if (pfs_fd == NULL)
		return(ESUCCESS);

	/*
	 * Close each of the stripe files
	 * and free the PFS file descriptor:
	 */
	error = multi_close(pfs_fd);

	free((void *)pfs_fd);

	return(error);
}


/*
 * Name:
 *	open_hdr_file
 *
 * Description:	
 *	Internal open interface, intended for PFS header files.  Performs a
 *	synchronous open RPC, returning a file port for the open file.  A file
 *	descriptor table entry is NOT allocated and installed in the file 
 *	descriptor table.
 *
 *	Note that the open RPC is not registered as interruptible ... this is 
 *	an attempt to force PFS file opens (particularly creates) to be atomic,
 *	so that "invalid" PFS files are not created when an interrupt occurs.
 *	The transaction ID parameter to the open RPC is 0.
 *
 *	With the same purpose in mind, the file is opened with O_SYNC, so any
 *	header file writes are performed synchronously on the server.
 *
 *	The iomode parameter of VIO_NONE will cause the file to be opened in
 *	the default mode for this file system on the server, which for PFS
 *	is Fast Path mode.
 *
 * Parameters:
 *	path		Pointer to the pathname of the file to open.
 *
 *	len_path	Length, in bytes, of the file pathname.
 *
 *	mode		The open mode (O_CREAT, O_RDWR, ...).
 *
 *	crtmode		The permissions if O_CREAT specified in mode.
 *
 *	fp		Pointer to location to return file port.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
open_hdr_file(path, len_path, mode, crtmode, fp)
	char			*path;
	int			len_path;
	int			mode;
	int			crtmode;
	mach_port_t		*fp;
{
	mach_port_t		start_port;
	int			error;
	ulong_t			iomode = VIO_NONE;

	start_port = (*path == '/') ? rootdir_port : currentdir_port;
	error = fsvr_open(start_port, credentials_port, 0, rootdir_port,
			  path, len_path + 1, mode | O_SYNC, crtmode, fp,
			  &iomode);
	return(error);
}


/*
 * Name:
 *	unlink_hdr_file
 *
 * Description:	
 *	Internal unlink interface.  Performs a synchronous unlink RPC, using
 *	the given I/O mode flag to indicate how the unlink should be handled
 *	by the server.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	path		Pointer to the pathname of the file to unlink.
 *
 *	len_path	Length, in bytes, of the file pathname.
 *
 *	iomode		The I/O mode flag to pass to the server.  Valid modes:
 *
 *			VIO_NONE	Return indication if target is a PFS
 *					file, otherwise remove the target.
 *
 *			VIO_PFS		Remove the target.
 *
 *			VIO_ERROR	Clean up state from a previous unlink.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
unlink_hdr_file(interrupt, path, len_path, iomode)
	boolean_t		*interrupt;
	char			*path;
	int			len_path;
	ulong_t			iomode;
{
	int			error;
	mach_port_t		start_port;
	transaction_id_t 	trans_id;

	start_port = (*path == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_unlink(start_port, credentials_port, trans_id,
			    rootdir_port, path, len_path + 1, &iomode);
	isc_deregister(interrupt);

	return(error);
}


/*
 * Name:
 *	ref_file
 *
 * Description:	
 *	Performs a synchronous fsvr_file_ref() RPC to the server to increment
 *	the reference count on the file structure associated with the given
 *	file port.
 *
 * Parameters:
 *
 *	fp		A valid Mach file port.
 *
 * Returns:
 *	Nothing.
 */
void
ref_file(fp)
	mach_port_t fp;
{
	int error;

	error = fsvr_file_ref(fp, 1);
	if (error) {
		EPRINT(("fsvr_file_ref failed, fp=0x%x, error=0x%x\n",
			fp, error));
		emul_panic("ref_file: fsvr_file_ref failed");
	}
}


/*
 * Name:
 *	unref_file
 *
 * Description:	
 *	Internal close interface.  Performs a synchronous RPC to the server,
 *	telling it to release a reference on the file structure associated
 *	with the given file port, and to move the send right to the server.
 *	The given file port is then set to MACH_PORT_NULL.
 *
 * Parameters:
 *
 *	fp		A valid Mach file port.
 *
 * Returns:
 *	Nothing.
 */
void
unref_file(fp)
	mach_port_t	*fp;
{
	int		error;

	error = fsvr_file_unref(*fp, credentials_port, *fp);
	if (error) {
		EPRINT(("fsvr_file_unref failure: error = 0x%x", error));
		emul_panic("unref_file: fsvr_file_unref failed");
	}

	*fp = MACH_PORT_NULL;
}


/*
 * Name:
 *	get_file_mode
 *
 * Description:	
 *	Gets the current permissions of the given file.  If hdr_fdte is NULL,
 *	a synchronous, non-interruptible stat RPC is used, otherwise a
 *	synchronous, non-interruptible fstat RPC is used.
 *
 * Parameters:
 *	hdr_path	Pathname of the PFS header file.
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.  NULL if the file is not already open.
 *
 *	mode		Pointer to integer to contain mode bits.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
get_file_mode(hdr_path, len_hdr_path, hdr_fdte, mode)
	char		*hdr_path;
	int		len_hdr_path;
	fdt_entry_t	*hdr_fdte;
	int		*mode;
{
	struct stat	statbuf;
	mach_port_t	start_port;
	int		error;
	ulong_t		dont_care;

	PFS_DEBUG(("    get_file_mode: hdr_fdte=0x%x, mode=0x%x\n",
		   hdr_fdte, mode));

	if (hdr_fdte != NULL)
		error = fsvr_fstat(hdr_fdte->fp, credentials_port, 0,
				   &statbuf);
	else {
		start_port = (*hdr_path == '/') ?
			rootdir_port : currentdir_port;
		error = fsvr_stat(start_port, credentials_port, 0, 
				  rootdir_port, hdr_path, len_hdr_path + 1,
				  TRUE, &statbuf, &dont_care);
	}
	if (!error)
		*mode = statbuf.st_mode;

	return(error);
}


/*
 * Name:
 *	set_file_mode
 *
 * Description:	
 *	Sets the current permissions of the given file.  If hdr_fdte is NULL,
 *	a synchronous, non-interruptible chmod RPC is used, otherwise a
 *	synchronous, non-interruptible fchmod RPC is used.
 *
 * Parameters:
 *	hdr_path	Pathname of the PFS header file.
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.  NULL if the file is not already open.
 *
 *	mode		New mode bits.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
set_file_mode(hdr_path, len_hdr_path, hdr_fdte, mode)
	char		*hdr_path;
	int		len_hdr_path;
	fdt_entry_t	*hdr_fdte;
	int		mode;
{
	mach_port_t	start_port;
	int		error;
	ulong_t		dont_care;

	PFS_DEBUG(("    set_file_mode: hdr_fdte=0x%x, mode=%o\n",
		   hdr_fdte, mode));

	if (hdr_fdte != NULL)
		error = fsvr_fchmod(hdr_fdte->fp, credentials_port, 0, mode);
	else {
		start_port = (*hdr_path == '/') ?
			rootdir_port : currentdir_port;
		error = fsvr_chmod(start_port, credentials_port, 0,
				   rootdir_port, hdr_path, len_hdr_path + 1,
				   mode, &dont_care);
	}

	return(error);
}


/*
 * Name:
 *	read_internal
 *
 * Description:	
 *	Internal read interface.  Performs a synchronous Fast Path read RPC.
 *	Assumes we have *exclusive* access to the given file port.
 *
 * Parameters:
 *	fp		Mach port associated with the open file.
 *
 *	data		Pointer to buffer to contain read data.
 *
 *	offset		Offset into file to read.
 *
 *	count		Number of bytes to read from the file.
 *
 *	actual		Pointer to the returned number of bytes actually read.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
read_internal(fp, data, offset, count, actual)
	mach_port_t		fp;
	char			*data;
	off_t			offset;
	size_t			count;
 	uint_t			*actual;	/* out */
{
	mach_port_t		reply_port;
	char			*addr;
	int			error;
	int			copylen;
	uint_t			response_id = 0;

	PFS_DEBUG(("      read_internal: fp=0x%x data=0x%x off=%d count=%d\n",
		   fp, data, offset, count));

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)0);
	error = fsvr_read_at_offset_msg_send(fp, reply_port,
					     credentials_port, &response_id, 
					     offset, count, &addr, actual);
	if (error)
		return(error);

	error = fsvr_read_at_offset_msg_receive(MACH_PORT_NULL, reply_port,
						credentials_port, &response_id,
						0, count, &addr, actual);
	PFS_DEBUG(("      read_internal: read done rc %d *actual=%d addr=%x\n",
		   error,*actual,addr));

	if (error == ESUCCESS) {
		copylen = *actual;
		error = user_bcopy2(addr, data, &copylen);
		if ((error) && (copylen > 0))
			error = ESUCCESS;	/* some bytes were copied */
		(void) vm_deallocate(mach_task_self(), (vm_address_t)addr,
				     *actual);
	}

	return(error);
}


/*
 * Name:
 *	write_internal
 *
 * Description:	
 *	Internal write interface.  Performs a synchronous Fast Path write RPC.
 *	Assumes we have *exclusive* access to the given file port.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	fp		Mach port associated with the open file.
 *
 *	data		Pointer to buffer containing the data to write.
 *
 *	offset		Offset into file to write.
 *
 *	count		Number of bytes to write to the file.
 *
 *	actual		Pointer to the returned number of bytes actually 
 *			written.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
write_internal(interrupt, fp, data, offset, count, actual)
	boolean_t		*interrupt;
	mach_port_t		fp;
	char			*data;
	off_t			offset;
	uint_t			count;
	uint_t			*actual;	/* out */
{
	mach_port_t		reply_port;
	int			error;
	uint_t			response_id = 0;

	PFS_DEBUG(("      write_internal: fp=0x%x data=0x%x off=%d count=%d\n",
		   fp, data, offset, count));

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)0);
	error = fsvr_write_at_offset_msg_send(fp, reply_port,
					      credentials_port,
					      &response_id,
					      offset, data, count,
					      (size_t *)actual);
	if (error)
		return(error);

	error = fsvr_write_at_offset_msg_receive(MACH_PORT_NULL, reply_port,
						 credentials_port,
						 &response_id,
						 0, data, count, 
						 (size_t *)actual);

	PFS_DEBUG(("      write_internal: write completed, *actual=%d\n",
		   *actual));
	if (error == EPIPE) {
		send_sig(SIGPIPE, interrupt);
	} else if (error == EFBIG) {
		send_sig(SIGXFSZ, interrupt);
	}

	return(error);
}


/*
 * Name:
 *	pfs_fdevstat_internal
 *
 * Description:	
 *	Internal pfs_fdevstat interface.  Performs a synchronous pfs_fdevstat
 *	RPC.  pfs_fdevstat is similar to fstat (and to devstat), but it returns
 *	the node number and the "real" device number on which the file resides.
 *	Other information is returned that could be retrieved via a normal 
 *	fstat, but is returned also by pfs_fdevstat to eliminate the need for
 *	a separate fstat operation.
 *
 *	This routine assumes we have *exclusive* access to the given file port.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	fp		Mach port associated with the open file.
 *
 *	psbuf		Pointer to buffer to contain pfs_stat info.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_fdevstat_internal(interrupt, fp, psbuf)
	boolean_t		*interrupt;	/* out */
	mach_port_t		fp;
	struct pfs_stat		*psbuf;
{
	int			error;
	transaction_id_t	trans_id;

	PFS_DEBUG(("    pfs_fdevstat_internal: fp=0x%x, psbuf=0x%x\n",
		   fp, psbuf));

	isc_register(fp, &trans_id);
	error = fsvr_pfs_fdevstat(fp, credentials_port, trans_id, psbuf);
	isc_deregister(interrupt);

	return(error);
}


/*
 * Name:
 *	read_pfs_header
 *
 * Description:	
 *	Attempt to read the contents of a PFS header file.  If successful, a
 *	buffer is allocated and filled in with the stripe attributes;  
 *	stripe_attrp is pointed at the buffer and returned.  If the header
 *	file is empty, a NULL stripe_attrp is returned.  Otherwise an error is
 *	returned.
 *
 * Parameters:
 *	fp		Mach port associated with the open PFS header file.
 *
 *	stripe_attrp	Location to return pointer to stripe attributes.  If
 *			a failure occurs, or the header file is determined to
 *			be invalid, the pointer is set to NULL.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
read_pfs_header(fp, stripe_attrp)
	mach_port_t		fp;		/* fp for PFS header file */
	struct statpfs		**stripe_attrp;	/* out - stripe attributes */
{
	register int		error;
	struct statpfs		*buf;
	size_t			buf_len = STATPFS_BUFSZ;
	int			actual = 0;

	PFS_DEBUG(("    read_pfs_header: fp=0x%x, *stripe_attrp=0x%x\n",
		   fp, *stripe_attrp));

	/*
	 * Allocate a buffer for the stripe attributes.
	 */
	if (error = pfs_malloc((void *)&buf, (uint_t)buf_len))
		return(error);

	/*
	 * Attempt to read the stripe attributes from the PFS header file.
	 */
	error = read_internal(fp, (char *)buf, (off_t)0, buf_len,
			      (uint_t *)&actual);
	if ((error)  || (actual == 0))
		goto bad;

	/*
	 * Check for a corrupted or invalid PFS header file.
	 */
	if (actual < sizeof(struct statpfs)) {
		error = ENOTPFS;
		goto bad;
	}
	if (buf->p_magic != PFS_MAGIC) {
		PFS_DEBUG(("    read_pfs_header: BAD p_magic=%d\n",
			   buf->p_magic));
		error = ENOTPFS;
		goto bad;
	}

	/*
	 * Check that we got all of the stripe attributes.  If not, allocate
	 * a bigger buffer and get the rest.
	 */
	if (buf->p_reclen > buf_len) {
		struct statpfs	*new_buf;
		size_t		new_buf_len;
		size_t 		remaining;

		PFS_DEBUG(("    read_pfs_header: reallocating statpfs buf\n"));
		/* Allocate a bigger buffer */
		new_buf_len = buf->p_reclen;
		if (error = pfs_malloc((void *)&new_buf, (uint_t)new_buf_len))
			goto bad;

		/* Replace old buffer with new buffer */
		bcopy(buf, new_buf, actual);
		pfs_free((void *)buf, (uint_t)buf_len);
		buf = new_buf;
		buf_len = new_buf_len;

		/* Read the remaining part of the header file */
		remaining = buf_len - actual;
		error = read_internal(fp, (((char *)buf)+actual), (off_t)actual,
				      remaining, (uint_t *)&actual);
		if (error)
			goto bad;

		/* Check for a corrupted or invalid header file */
		if (actual < remaining) {
			error = ENOTPFS;
			goto bad;
		}
	}

	*stripe_attrp = buf;
	return(ESUCCESS);

bad:
	pfs_free((void *)buf, (uint_t)buf_len);
	*stripe_attrp = NULL;	/* indicate header is empty (if no error) */
	return(error);
}


/*
 * Name:
 *	write_pfs_header
 *
 * Description:	
 *	Attempt to write stripe attributes to a PFS header file.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	fp		Mach port associated with the open PFS header file.
 *
 *	stripe_attr	Pointer to the stripe attributes to write.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
write_pfs_header(interrupt, fp, stripe_attr)
	boolean_t		*interrupt;
	mach_port_t		fp;		/* fp for PFS header file */
	struct statpfs		*stripe_attr;	/* stripe attributes */
{
	register int		error;
	uint_t			actual = 0;

	PFS_DEBUG(("    write_pfs_header: fp=0x%x, stripe_attr=0x%x\n",
		   fp, stripe_attr));

	/*
	 * Attempt to write the stripe attributes to the PFS header file.
	 */
	error = write_internal(interrupt, fp, (char *)stripe_attr, (off_t)0, 
			       stripe_attr->p_reclen, &actual);
	if (error)
		return(error);

	if (actual != stripe_attr->p_reclen)
		error = ENOTPFS;

	return(error);
}


/*
 * Name:
 *	get_stripefile_name
 *
 * Description:	
 *	Given a PFS header file name, and its node number, device number, and
 *	file number, form a stripefile name with the syntax:
 *
 *	/<header fname>.<node #>.<device #>.<file #>.
 *
 *	Note that this function does not insert the stripedir or the stripefile
 *	number into the stripefile name.
 *
 * Parameters:
 *	hdr_name	The last component of the PFS header file pathname.
 *
 *	node_num	The physical node number on which the PFS header file
 *			resides.
 *
 *	device_num	The number (major/minor) of the device on which the
 *			PFS header file resides, which uniquely identifies the
 *			device on the given I/O node.
 *
 *	inode_num	The file number (inode number) of the PFS header file,
 *			which uniquely identifies the file on the given device.
 *
 *	sfile_name	Pointer to character array in which to place the
 *			stripefile name.
 *
 *	len_sfile_name	Pointer to location to store the length of the 
 *			stripefile name.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
get_stripefile_name(hdr_name, node_num, device_num, inode_num, 
		    sfile_name, len_sfile_name)
	char		*hdr_name;	/* header file name (last component) */
	node_t		node_num;
	dev_t		device_num;
	ino_t		inode_num;
	char		*sfile_name;		/* out */
	int		*len_sfile_name;	/* out */
{
	/*
	 * Allocate buffers big enough to hold ASCII hexadecimal
	 * representations of the unique file information (+ terminating
	 * null byte).
	 */
	char		node_num_buf[(sizeof(node_t)*2)+1];
	char		device_num_buf[(sizeof(dev_t)*2)+1];
	char		inode_num_buf[(sizeof(ino_t)*2)+1];

	sprintf(node_num_buf, "%lx", node_num);
	sprintf(device_num_buf, "%.4lx", device_num);
	sprintf(inode_num_buf, "%lx", inode_num);

	/*
	 * Form the full stripefile name.  The full stripefile *path* name is
	 * defined as (with numbers in hexadecimal format):
	 *
	 * <stripedir>/<header fname>.<node #>.<device #>.<file #>.<stripe #>
	 *
	 * We're only forming the part of this path that is constant across
	 * all of the stripefiles, i.e.:
	 *
	 * /<header fname>.<node #>.<device #>.<file #>.
	 *
	 * If necessary, truncate the <header fname> portion of the stripefile
	 * name so that the full stripefile name cannot exceed NAME_MAX in 
	 * length.
	 */
	sprintf(sfile_name, "/%.*s.%s.%s.%s.",
		MIN(strlen(hdr_name), SFILE_HDR_NAME_MAX),
		hdr_name, node_num_buf, device_num_buf, inode_num_buf);

	*len_sfile_name = strlen(sfile_name);

	return(ESUCCESS);
}


/*
 * Name:
 *	get_mount_attributes
 *
 * Description:	
 *	Get the default stripe attributes of the PFS file system in which the
 *	given PFS header file resides.  If successful, a buffer is allocated
 *	and filled in with the stripe attributes; stripe_attrp is pointed at
 *	the buffer and returned.  If an error occurs, a NULL stripe_attrp is
 *	returned.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	stripe_attrp	Location to return pointer to stripe attributes.  If
 *			a failure occurs, the pointer is set to NULL.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
get_mount_attributes(interrupt, hdr_fdte, stripe_attrp)
	boolean_t		*interrupt;	/* out */
	fdt_entry_t		*hdr_fdte;	/* fdte for PFS header file */
	struct statpfs		**stripe_attrp;
{
	register int		error;
	struct statpfs		*buf;
	size_t			buf_len = STATPFS_BUFSZ;
	size_t			new_buf_len;
	int			rval;

	PFS_DEBUG(("  get_mount_attributes: fdte=0x%x *stripe_attrp=0x%x\n",
		   hdr_fdte, *stripe_attrp));

	/*
	 * Allocate a buffer for the stripe attributes.
	 */
	if (error = pfs_malloc((void *)&buf, (uint_t)buf_len))
		return(error);

	/*
	 * Do an fstatpfs() to retrieve the stripe attributes of the file 
	 * system on which this file resides.
	 */
	if (error = fstatpfs_internal(interrupt, hdr_fdte->fp,
				      buf, (uint_t)buf_len, &rval))
		goto bad;

	/*
	 * Check that we got all of the stripe attributes.  If not, allocate a
	 * bigger buffer and try again.
	 */
	while (buf->p_reclen > buf_len) {
		/*
		 * Normally will not loop more than once, unless there's
		 * a race on someone doing a mount update on the stripe 
		 * attributes (*and* the size of the stripe attributes has
		 * increased).
		 */
		PFS_DEBUG(("  get_mount_attributes: reallocating buf\n"));
		new_buf_len = buf->p_reclen;
		pfs_free((void *)buf, (uint_t)buf_len);
		buf_len = new_buf_len;
		if (error = pfs_malloc((void *)&buf, (uint_t)buf_len))
			return(error);

		if (error = fstatpfs_internal(interrupt, hdr_fdte->fp,
					      buf, (uint_t)buf_len, &rval))
			goto bad;
	}

	*stripe_attrp = buf;
	return(ESUCCESS);

bad:
	pfs_free((void *)buf, (uint_t)buf_len);
	*stripe_attrp = NULL;	/* indicate header is empty (if no error) */
	return(error);
}


/*
 * Name:
 *	get_stripe_attributes
 *
 * Description:	
 *	Get the stripe attributes of an already existing PFS file, identified
 *	by either 1) a file descriptor table entry for the PFS header file, or
 *	2) the PFS (header) file pathname.  If successful, the given
 *	stripe_attrp is pointed at a buffer that is allocated and filled with
 *	the stripe attributes.  If an error occurs, a NULL stripe_attrp is
 *	returned.
 *
 *	Note that it is necessary to start a temporary open session to the file
 *	if 1) a file descriptor for an already-open file is *not* provided, or
 *	2) the file is already open, but not for read access.
 *
 *	It may also be necessary to temporarily add read permissions to the
 *	header file if it has not been created with them.  This can't be done
 *	when the file is first created, because we don't know it's a PFS file
 *	until the emulator returns from the initial open (at which point the
 * 	file creation has completed).  Adding PFS read_hdr/write_hdr vnode ops
 *	(similar to the readlink vnode op) on the server will eliminate these
 *	chicken-and-egg problems.
 *
 * Parameters:
 *	hdr_path	Pathname of the PFS header file.
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.  NULL if the file is not already open.
 *
 *	hdr_mode	The access mode in which the file has been opened, if
 *			the file is already open.
 *
 *	stripe_attrp	Location to return pointer to stripe attributes.  If
 *			a failure occurs, the pointer is set to NULL.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
get_stripe_attributes(hdr_path, len_hdr_path, hdr_fdte, hdr_mode, stripe_attrp)
	char		*hdr_path;
	int		len_hdr_path;
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	int		hdr_mode;
	struct statpfs	**stripe_attrp;	/* out - stripe attributes */
{
	int		error;
	mach_port_t	fp;
	int		mode = 0;	/* current file mode */
	mach_port_t	newfp = MACH_PORT_NULL;

	PFS_DEBUG(("  get_stripe_attributes: fdte=0x%x *stripe_attrp=0x%x\n",
		   hdr_fdte, *stripe_attrp));

	*stripe_attrp = NULL;

	/*
	 * Start a new open session if: 1) we weren't handed an already 
	 * existing file pointer, or 2) the file pointer we were given does
	 * not have read access to the file.
	 */
	if ((hdr_fdte == NULL) || ((hdr_mode & O_ACCMODE) == O_WRONLY)) {
		if (hdr_fdte != NULL)	/* already created by initial open() */
			hdr_mode &= ~(O_CREAT | O_TRUNC);
		hdr_mode &= ~O_WRONLY;
		hdr_mode |= O_RDONLY;
		error = open_hdr_file(hdr_path, len_hdr_path, hdr_mode,
				      0, &newfp);
		if (error == EACCES) {
			/*
			 * If the caller has no read permissions for the PFS 
			 * file, we must temporarily change the permissions on
			 * the already-created header file so that the stripe
			 * attributes can be written.
			 */
			if (error = get_file_mode(hdr_path, len_hdr_path,
						  hdr_fdte, &mode))
				return(error);
			if (error = set_file_mode(hdr_path, len_hdr_path,
						  hdr_fdte, mode | S_IRUSR))
				return(error);
			if (error = open_hdr_file(hdr_path, len_hdr_path,
						  hdr_mode, 0, &newfp))
				goto out;
		} else if (error) {
			return(error);
		}

		fp = newfp;
	} else {
		fp = hdr_fdte->fp;
	}

	error = read_pfs_header(fp, stripe_attrp);

out:
	/*
	 * Remove temporary read permission if necessary.
	 */
	if (mode)
		set_file_mode(hdr_path, len_hdr_path, hdr_fdte, mode);

	if (newfp)
		unref_file(&newfp);

	return(error);
}

/*
 * Name:
 *	pfs_multi_parse
 *
 * Description:	
 *	This function pre-parses the paths to the PFS stripe directories
 *      and vnode ports to those directories are cached for future use.
 *
 *	Note that the open RPC's are not registered as interruptible ... this 
 *	is an attempt to force PFS file opens (particularly creates) to be
 *	atomic, so that "invalid" PFS files are not created when an interrupt 
 *	occurs.  The transaction ID parameter to the open RPC's is 0.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	stripe_attr     Pointer to the mount attributes of the PFS         
 *			file system.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_parse(interrupt, stripe_attr, stripe_attr_size)
    boolean_t	           *interrupt;	/* out */
    struct statpfs	   *stripe_attr;	/* PFS stripe attributes */
    uint_t                  stripe_attr_size;
{
	pathname_t	*sf_path;	/* stripefile pathname */
	register uint_t	sfactor;	/* stripe factor */
	register int	pfs_error;	/* running error returned to caller */

	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */
	mach_port_t	sfp;		/* port representing the stripedir */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;




	/*
	 * Parse all the stripe directories.  This is a two-step process: 
	 *     all parse requests are sent at once, 
	 *     then all responses received at the
	 *     same port.
	 */		
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_parse: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));
		pfs_error = fsvr_pfs_pre_parse_msg_send(rootdir_port,
							reply_port,
							rootdir_port,
							credentials_port,
							0,
							&sfd,
							sf_path->name, 
							sf_path->namelen + 1,
							&sfp);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	/*
	 *  Allocate the vnode port array.
	 */
	pfs_error = vm_allocate(mach_task_self(), 
				(vm_address_t *)&stripe_dir_vnode_ports, 
				(vm_size_t)(sfactor * sizeof(mach_port_t)),
				(boolean_t)TRUE);
	if (pfs_error) 
	    goto out;


	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_pre_parse_msg_receive(MACH_PORT_NULL,
							  reply_port,
							  MACH_PORT_NULL,
							  credentials_port,
							  0,
							 &sfd,
							  NULL,
							  0,
							  &sfp);
		PFS_DEBUG_RECV(("pfs_multi_parse: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d sfp=0x%x\n", sfd, sfp),
			       sf_error);

		if (sf_error) {	/* remember the error and toss the response */
			*(stripe_dir_vnode_ports + sfd) = MACH_PORT_NULL;
#ifdef	DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_parse: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
		*(stripe_dir_vnode_ports + sfd) = sfp;

	}
	/*
	 * Establish the cache
	 */
	PFS_DEBUG(("pfs_multi_parse: stripe_attr_cache:0x%x\n", stripe_attr));
	stripe_attr_cache = stripe_attr;
	stripe_attr_cache_size = stripe_attr_size;


out:
	return(pfs_error);
}


/*
 * Name:
 *	set_stripe_attributes
 *
 * Description:	
 *	Set the stripe attributes of a PFS file by writing them to the header
 *	file.  The header file is identified by either 1) a file descriptor
 *	table entry if the file is already open, or 2) the file pathname.
 *
 *	Note that it is necessary to start a temporary open session to the
 *	header file if the PFS file has not been opened by the user with write
 *	access.  Otherwise the stripe attributes cannot be written to the 
 *	header file.
 *
 *	It may also be necessary to temporarily add write permissions to the
 *	header file if it has not been created with them.  This can't be done
 *	when the file is first created, because we don't know it's a PFS file
 *	until the emulator returns from the initial open (at which point the
 * 	file creation has completed).  Adding PFS read_hdr/write_hdr vnode ops
 *	(similar to the readlink vnode op) on the server will eliminate these
 *	chicken-and-egg problems.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname of the PFS header file.
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	hdr_mode	The access mode in which the file has been opened, if
 *			the file is already open.
 *
 *	crtmode		The O_CREAT permissions specified in the open()
 *			system call.
 *
 *	stripe_attr	Pointer to the stripe attributes to write.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
set_stripe_attributes(interrupt, hdr_path, len_hdr_path, hdr_fdte, hdr_mode,
		      crtmode, stripe_attr)
	boolean_t	*interrupt;
	char		*hdr_path;
	int		len_hdr_path;
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	int		hdr_mode;
	int		crtmode;	/* mode used to create file */
	struct statpfs	*stripe_attr;	/* stripe attributes to write */
{
	int		error;
	mach_port_t	fp;
	int		mode;		/* current file mode */
	mach_port_t	newfp = MACH_PORT_NULL;

	PFS_DEBUG(("  set_stripe_attributes: fdte=0x%x mode=%o crtmode=%o sattr=0x%x\n",
		   hdr_fdte, hdr_mode, crtmode, stripe_attr));

	/*
	 * If the PFS file is being created with no write permissions, we must
	 * temporarily change the permissions on the already-created header
	 * file so that the stripe attributes can be written.
	 */
	if (!(crtmode & S_IWUSR)) {
		/*
		 * Must get current mode (since server applies umask, etc. on
		 * file creation).
		 */
		if (error = get_file_mode(hdr_path, len_hdr_path, hdr_fdte,
					  &mode))
			return(error);
		if (error = set_file_mode(hdr_path, len_hdr_path, hdr_fdte,
					  mode | S_IWUSR))
			return(error);
	}

	/*
	 * Start a new open session if the file pointer we were given does
	 * not have write access to the file.
	 */
	if ((hdr_mode & O_ACCMODE) == O_RDONLY) {
		hdr_mode &= ~O_RDONLY;
		hdr_mode |= O_WRONLY;
		if (error = open_hdr_file(hdr_path, len_hdr_path,
					  hdr_mode, 0, &newfp)) {
			goto out;
		}
		fp = newfp;
	} else {
		fp = hdr_fdte->fp;
	}

	/*
	 * Write the header file.
	 */
	error = write_pfs_header(interrupt, fp, stripe_attr);

out:
	/*
	 * Remove temporary write permission if necessary.
	 */
	if (!(crtmode & S_IWUSR))
		set_file_mode(hdr_path, len_hdr_path, hdr_fdte, mode);

	if (newfp)
		unref_file(&newfp);

	return(error);
}


/*
 * Name:
 *	pfs_check_stripe_attributes
 *
 * Description:	
 *	Validate the given statpfs structure, and verify that the stripe
 *	directories it specifies exist.  Disallow the specification of stripe
 *	directories that reside in the PFS file system (although once the PFS
 *	file system is mounted, it will be possible to create UFS mount points
 *	beneath the PFS mount point, and mount a PFS file system that stripes
 *	into the UFS file systems).
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	stripe_attr	Pointer to the statpfs structure to be validated.
 *
 *	mount_dir	Pointer to pathname of the directory on which the PFS
 *			file system is to be mounted.
 *
 *	len_mount_dir	The length of the pathname pointed to by mount_dir,
 *			excluding the terminating NULL byte.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_check_stripe_attributes(interrupt, stripe_attr, mount_dir, len_mount_dir)
	boolean_t	*interrupt;	/* out */
	struct statpfs	*stripe_attr;
	char		*mount_dir;
	int		len_mount_dir;
{
	pathname_t	*sdir;
	uint_t		len_sdir;
	uint_t		len_attr;
	struct stat	statbuf;
	int		i, error;

	PFS_DEBUG(("pfs_check_stripe_attributes: stripe_attr=0x%x\n",
		   stripe_attr));

	/*
	 * Validate the statpfs structure as a whole.
	 */
	if (!user_rcheck(stripe_attr, stripe_attr->p_reclen))
		return(EFAULT);
	if (stripe_attr->p_reclen < sizeof(struct statpfs)) {
		PFS_DEBUG(("pfs_check_stripe_attributes: bad p_reclen=%d\n",
			   stripe_attr->p_reclen));
		return(EINVAL);
	}

	/*
	 * Validate the stripe unit size.
	 */
	if ((int)stripe_attr->p_sunitsize <= 0) {
		PFS_DEBUG(("pfs_check_stripe_attributes: bad p_sunitsize=%d\n",
			   stripe_attr->p_sunitsize));
		return(EINVAL);
	}

	/*
	 * Validate each stripe directory pathname.  Also ensure that none of
	 * the stripe directories resides in the to-be-mounted PFS file system.
	 */
	len_attr = sizeof(struct statpfs) - sizeof(pathname_t);
	sdir = &stripe_attr->p_sdirs;
	for (i = 0; i < stripe_attr->p_sfactor; i++) {
		if (!user_strlen(sdir->name, &len_sdir))
			return(EFAULT);
		if (len_sdir != sdir->namelen) {
			PFS_DEBUG(("pfs_check_stripe_attributes: bad namelen=%d\n",
				   sdir->namelen));
			return(EINVAL);
		}
		if (sdir->name[0] != '/')
			return(EFSNOTSUPP);
		if (!strncmp(sdir->name, mount_dir, (size_t)len_mount_dir))
			return(EFSNOTSUPP);
		len_attr += PATHSIZE(sdir);
		sdir = NEXTPATH(sdir);
	}

	/*
	 * Validate the statpfs record length.
	 */
	if (len_attr != stripe_attr->p_reclen) {
		PFS_DEBUG(("pfs_check_stripe_attributes: incorrect p_reclen=%d\n",
			   stripe_attr->p_reclen));
		return(EINVAL);
	}
	if (len_attr != stripe_attr->p_reclen) {
		PFS_DEBUG(("pfs_check_stripe_attributes: incorrect p_reclen=%d\n",
			   stripe_attr->p_reclen));
		return(EINVAL);
	}

	/*
	 * Ensure that the stripe directories actually exist, and have search
	 * permissions for the caller.  We don't necessarily want to require
	 * write permissions here: e.g. this might disallow root from mounting
	 * a PFS that stripes into an NFS directory (even root can't write to
	 * an NFS mount if the remote host does not give root permissions,
	 * although a normal user might have permissions).
	 */
	error = multi_access(interrupt, stripe_attr, X_OK|F_OK);
	if (error) {
		PFS_DEBUG(("pfs_check_stripe_attributes: multi_access ret %d\n",
			   error));
		return(error);
	}

	/*
	 * Ensure that none of the stripe directories resides in a PFS file
	 * system.  We do not currently claim to support recursive PFS file
	 * striping!
	 */
	error = pfs_multi_statfs(interrupt, stripe_attr, NULL);
	if (error)
		return(error);

	/*
	 * Ensure that the stripe directories are actually directories.
	 * NOTE: We should eventually merge all these separate RPC's into
	 * some new operation; e.g., "check_stripe_dir()".
	 */
	error = multi_stat(interrupt, stripe_attr, (caddr_t)&statbuf, TRUE,
			   FALSE, TRUE);

	return(error);
}


/*
 * Name:
 *	construct_stripefile_pathnames
 *
 * Description:	
 *	Given the default stripe attributes of a PFS file system, and a PFS 
 *	header pathname, construct a new set of stripe attributes that contain
 *	full stripefile pathnames for the PFS file.
 *
 *	[Remember that the p_sdirs fields of the statpfs for a mounted PFS
 *	file system contain stripe directory pathnames.  But when a file is
 *	created in the file system, the statpfs is written to the PFS header
 *	file, and at this point the p_sdirs fields must contain full stripefile
 *	pathnames.  This allows the PFS file to still be interpreted correctly
 *	if the file system is later mounted with different stripe attributes.]
 *
 *	The syntax of a full stripefile pathname is:
 *
 *	<stripedir path>/<hdr fname>.<node#>.<device#>.<file#>.<stripefile#>
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname of the PFS header file.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.  NULL if the file is not already open.
 *
 *	stripe_attrp	Location of pointer to the default stripe attributes of
 *			the PFS file system.  On return, contains pointer to
 *			full stripe attributes of the given PFS file.  If a
 *			failure occurs, the pointer is left unchanged.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
construct_stripefile_pathnames(interrupt, hdr_path, hdr_fdte, stripe_attrp)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	struct statpfs	**stripe_attrp;	/* in/out - stripe attributes */
{
	int		error;
	struct pfs_stat	hdr_stat;
	char		*hdr_name;
	struct statpfs	*mount_attr = *stripe_attrp;
	struct statpfs	*new_hdr_contents = NULL;
	uint_t		len_hdr_contents;
	uint_t		sfactor = mount_attr->p_sfactor;
	char		sfile_name[MAXPATHLEN]; /* should we malloc this? */
	int		len_sfile_name;
	uint_t 		sdir_num, sfile_num;
	char		sdir_num_buf[LEN_SFILENUM+1];
	pathname_t	*sdir, *sfile;

	PFS_DEBUG(("  construct_stripefile_pathnames: path=%s, *sattrp=0x%x\n",
		   hdr_path, *stripe_attrp));

	/*
	 * Stat the PFS header file to get the inode number,
	 * device number, etc.
	 */
	error = pfs_fdevstat_internal(interrupt, hdr_fdte->fp, &hdr_stat);
	if (error) {
		goto bad;
	}

	/*
	 * Set hdr_name to point to the last component in the PFS 
	 * header filename.
	 */
	hdr_name = rindex(hdr_path, '/');
	if (hdr_name == NULL)
		hdr_name = hdr_path;
	else
		hdr_name++;

	/*
	 * Form the constant part of the stripefile names.
	 */
	error = get_stripefile_name(hdr_name, hdr_stat.pst_node, 
				    hdr_stat.pst_dev, hdr_stat.pst_ino,
				    sfile_name, &len_sfile_name);
	if (error) {
		goto bad;
	}

	/*
	 * Create the stripe attributes to be written to the PFS header file.
	 * These are the same as the mount attributes passed as a parameter to
	 * this function, except:
	 *
	 *     - full stripefile pathnames rather than just pathnames to
	 *       stripe directories are used
	 *
	 *     - striping is started at a random stripe directory in an attempt
	 *       to consume disk space evenly across the stripe group.
	 */

	/*
	 * First allocate enough space for new header contents.
	 * + 1 is terminating null, + 3 is to make sure there's
	 * enough room for word alignment.
	 */
	len_hdr_contents = mount_attr->p_reclen + 
		sfactor*(len_sfile_name + LEN_SFILENUM+1 + 3);
	if (error = pfs_malloc((void *)&new_hdr_contents, len_hdr_contents))
		goto bad;
	
	new_hdr_contents->p_reclen =
		sizeof(struct statpfs) - sizeof(pathname_t);
	new_hdr_contents->p_magic = mount_attr->p_magic;
	new_hdr_contents->p_sunitsize = mount_attr->p_sunitsize;
	new_hdr_contents->p_sfactor = sfactor;
	new_hdr_contents->p_start_sdir =
		(uint_t) RAND(hdr_stat.pst_ino) % sfactor;
	PFS_DEBUG(("  construct_stripefile_pathnames: start_sdir=%d\n",
		   new_hdr_contents->p_start_sdir));

	/*
	 * Form the full stripefile pathnames.  Initialize the sdir path
	 * pointer to point to the stripe directory corresponding to the
	 * start_sdir specified in the stripe attributes.  Then loop through
	 * the new file's stripe group, copying in the pathnames of stripe
	 * files that belong to the new group.
	 */
	sdir = &mount_attr->p_sdirs;
	for (sdir_num = 0; sdir_num < new_hdr_contents->p_start_sdir;
	     sdir_num++) {
		sdir = NEXTPATH(sdir);
	}
	sdir_num = new_hdr_contents->p_start_sdir;
	sfile = &new_hdr_contents->p_sdirs;
	for (sfile_num = 0; sfile_num < sfactor; sfile_num++) {
		sprintf(sdir_num_buf, "%lx", sdir_num);
		sfile->namelen = sdir->namelen + len_sfile_name + 
			strlen(sdir_num_buf);
		if (sfile->namelen > MAXPATHLEN) {
			error = ENAMETOOLONG;
			goto bad;
		}
		sprintf(sfile->name, "%s%s%s",
			sdir->name, sfile_name, sdir_num_buf);
		new_hdr_contents->p_reclen += PATHSIZE(sfile);

		if (++sdir_num == sfactor) {
			sdir_num = 0;
			sdir = &mount_attr->p_sdirs;
		} else {
			sdir = NEXTPATH(sdir);
		}
		sfile = NEXTPATH(sfile);
	}

	/*
	 * Deallocate any unused pages in the newly allocated statpfs buffer.
	 */
	len_hdr_contents -= round_page(new_hdr_contents->p_reclen);
	if (len_hdr_contents > 0) {
		vm_address_t    start;
		start = (vm_address_t) new_hdr_contents +
			round_page(new_hdr_contents->p_reclen);
		pfs_free((void *)start, len_hdr_contents);
	}

	pfs_free((void *)mount_attr, MAX(mount_attr->p_reclen, STATPFS_BUFSZ));
	*stripe_attrp = new_hdr_contents;
	return(ESUCCESS);

bad:
	if (new_hdr_contents)
		pfs_free((void *)new_hdr_contents, len_hdr_contents);
	return(error);
}


/*
 * Name:
 *	sfile_count_init
 *
 * Description:	
 *	If necessary, dynamically allocate an array big enough for all
 *	stripefile request counts, actuals, etc.
 *
 * Parameters:
 *	sf_counts	Points to returned stripefile counts array, if it was
 *			necessary to allocate one.
 *
 *	sfactor		Stripe factor of the PFS file involved in the operation
 *			in progress.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
sfile_count_init(sf_counts, sfactor)
	sfile_count_t	**sf_counts;
	uint_t		sfactor;
{
	int		error;

	/*
	 * Sanity check: Make sure we have a big enough array to hold 
	 * stripefile counts.
	 */
	if (sfactor > BIG_STRIPE_FACTOR) {
		if (error = pfs_malloc((void *)sf_counts, 
				       sfactor * sizeof(struct sfile_count))) {
			*sf_counts = NULL;
			return(error);
		}
	}

	bzero((char *)(*sf_counts), sfactor * sizeof(struct sfile_count));
	return(ESUCCESS);
}


/*
 * Name:
 *	pfs_get_size
 *
 * Description:	
 *	Determine the "logical" size of a PFS file (i.e., the combined size
 *	of all stripefile data).
 *
 *	This function assumes that the PFS token has *already been acquired*.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	pfs_size	Location in which to return PFS file size.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_get_size(interrupt, hdr_fdte, pfs_size)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	esize_t		*pfs_size;	/* out */
{
	struct estat	estatbuf;
	int		error;

	PFS_DEBUG(("  pfs_get_size: hdr_fdte=0x%x\n", hdr_fdte));

	error = pfs_multi_fstat(interrupt, hdr_fdte, (caddr_t)&estatbuf, TRUE);

	*pfs_size = estatbuf.st_size;

	PFS_DEBUG(("  pfs_get_size: returning PFS size of %d,%d bytes\n",
		   pfs_size->shigh, pfs_size->slow));
	return(error);
}


/*
 * Name:
 *	pfs_sync_actuals
 *
 * Description:	
 *	Given an array of stripefile requested/actual counts for an in-progress
 *	PFS operation, check the actual counts for consistency.  Synchronize
 *	them if they are inconsistent, compute the correct "logical" PFS file
 *	actual, and return it.
 *
 *	Synchronizing the actuals is usually necessary for any operation that 
 *	affects the PFS file size or offset.  E.g. when write-extending a
 *	PFS file, one of the concurrent writes to a stripefile may fail due to
 *	a lack of disk space, but all others may succeed.  In this case, the
 *	PFS file size and offset must be restricted to the valid range of
 *	stripefile data that was written.  Or when reading a PFS file, we 
 *	cannot rule out the possibility that a deranged user has edited and 
 *	truncated a stripefile by hand, which would cause the stripefile actual
 *	counts to be out of sync.  In this case, the new PFS file offset must 
 *	be computed using the "correct" PFS actual returned by this routine.
 *	
 * Parameters:
 *	sf_counts	Array of stripefile requested/actual counts for the 
 *			operation in progress.
 *
 *	sf_fdt		Stripefile descriptor table for the PFS file.
 *
 *	sunitsize	Stripe unit size of the PFS file.
 *
 *	sfactor		Stripe factor of the PFS file.
 *
 *	pfs_actual	Location in which to return the corrected PFS actual.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
void
pfs_sync_actuals(sf_counts, sf_fdt, sunitsize, sfactor, pfs_actual)
	sfile_count_t	*sf_counts;	/* stripefile count array */
	stripe_fd_t	*sf_fdt;	/* stripefile descriptor table */
	size_t		sunitsize;	/* stripe unit size */
	uint_t		sfactor;	/* stripe factor */
	esize_t		*pfs_actual;	/* out */
{
	register uint_t	sfd;
	off_t		sf_offset;	/* offset into stripefile */
	esize_t		new_offset;	/* PFS file offset */
	esize_t		new_actual;	/* PFS file actual this transaction */
	off_t		last_sunit;	/* last stripe unit in PFS file */
	esize_t		ex_last_sunit;	/* extended last_sunit */

	last_sunit = UINT_MAX;

	PFS_DEBUG(("  pfs_sync_actuals: *actual=%d,%d sunitsz=%d, sfctor=%d\n",
		   pfs_actual->shigh, pfs_actual->slow, sunitsize, sfactor));
#ifdef	DEBUG_PFS
	if (pfs_debug_flag >= 3) {
		for (sfd = 0; sfd < sfactor; sfd++) {
			e_printf("    sfd=%d, requested=%d, actual=%d\n",
				 sfd, sf_counts[sfd].requested,
				 sf_counts[sfd].actual);
		}
	}
#endif

	new_actual = ex_zero;

	/*
	 * Determine the number of the last stripe unit containing valid data.
	 */
	for (sfd = 0; sfd < sfactor; sfd++) {
		/*
		 * Don't bother checking this stripefile if actual is what was
		 * requested.
		 */
		if (sf_counts[sfd].requested == sf_counts[sfd].actual)
			continue;
		sf_offset = sf_fdt[sfd].s_offset + sf_counts[sfd].actual;
		last_sunit = MIN(last_sunit,
				 SFOFF_TO_SUNUM(sf_offset, sfd, 
						sunitsize, sfactor));
	}

	PFS_DEBUG(("  pfs_sync_actuals: last_sunit=%d\n", last_sunit));
	if (last_sunit == UINT_MAX)	/* nothing to do - actuals in sync */
		return;

	/*
	 * Determine the current PFS file offset from the last stripe unit.
	 */
	ex_last_sunit.shigh = 0;
	ex_last_sunit.slow = last_sunit;
	new_offset = SFOFF_TO_POFF(sf_offset, ex_last_sunit, sunitsize);

	/*
	 * Set the actual counts so they correspond with the current PFS 
	 * file offset.
	 */
	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_counts[sfd].actual = POFF_TO_SFOFF(new_offset, sfd,
						      sunitsize, sfactor)
			- sf_fdt[sfd].s_offset;
		new_actual = __eadd1(new_actual, (long)sf_counts[sfd].actual,
				     &dont_care);
	}

	*pfs_actual = new_actual;
	PFS_DEBUG(("  pfs_sync_actuals: return new_off=%d,%d new_act=%d,%d\n",
		   new_offset.shigh, new_offset.slow, 
		   new_actual.shigh, new_actual.slow));
#ifdef	DEBUG_PFS
	if (pfs_debug_flag >= 3) {
		for (sfd = 0; sfd < sfactor; sfd++) {
			e_printf("    sfd=%d, requested=%d, actual=%d\n",
				 sfd, sf_counts[sfd].requested,
				 sf_counts[sfd].actual);
		}
	}
#endif
}


/*
 * Name:
 *	pfs_multi_open
 *
 * Description:	
 *	This function allocates a PFS file descriptor, initializes it by
 *	reading the PFS file header, and concurrently opens all stripefiles
 *	associated with a PFS file.  It is assumed that the given fdte
 *	representing the header file has not yet been installed in the file
 *	descriptor table.
 *
 *	Note that the open RPC's are not registered as interruptible ... this 
 *	is an attempt to force PFS file opens (particularly creates) to be
 *	atomic, so that "invalid" PFS files are not created when an interrupt 
 *	occurs.  The transaction ID parameter to the open RPC's is 0.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the open() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	mode		The open mode specified in the open() system call.
 *
 *	crtmode		The O_CREAT permissions specified in the open()
 *			system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_open(interrupt, hdr_path, len_hdr_path, hdr_fdte, mode, crtmode)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;
	int		len_hdr_path;
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	int		mode;
	int		crtmode;
{
	pfs_fd_t	*pfs_fd	= NULL;
	register uint_t	sfactor;	/* stripe factor */
	stripe_fd_t	*sf_fdt;	/* stripefile descriptor table */
	pathname_t	*sf_path;	/* stripefile pathname */
	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */
	register int	pfs_error;	/* running error returned to caller */

	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */
	mach_port_t	sfp;		/* port representing open stripefile */

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;

	struct statpfs	*stripe_attr = NULL;	/* PFS stripe attributes */
	boolean_t	new_hdr = 0;	/* TRUE if hdr file already existed */
	ulong_t		iomode = VIO_STRIPED;

	PFS_TRACE(("pfs_multi_open: path=%s fdte=0x%x mode=%o crtmode=%o\n",
		   hdr_path, hdr_fdte, mode, crtmode));

	pfs_error = get_stripe_attributes(hdr_path, len_hdr_path, hdr_fdte,
					  mode, &stripe_attr);
	if (pfs_error) {
		PFS_DEBUG(("pfs_multi_open: get_stripe_attributes ret %d\n",
			   pfs_error));
		goto out;
	}
	if (stripe_attr == NULL) {
		new_hdr = TRUE;
		/*
		 * Add truncate flag to open mode, just in case there are any
		 * old stripefiles lying around with data in them (should
		 * never happen unless there's a bug in pfs_multi_unlink()
		 * or something).
		 */
		mode |= O_TRUNC;

		/*
		 * The PFS header file is empty, which means that the PFS file
		 * is just being created.  Use the default stripe attributes
		 * for this file system that were specified at mount time.
		 */
		pfs_error = get_mount_attributes(interrupt, hdr_fdte,
						 &stripe_attr);
		if (pfs_error) {
			PFS_DEBUG(("pfs_multi_open: get_mount_attributes ret %d\n",
				   pfs_error));
			goto out;
		}

		pfs_error = construct_stripefile_pathnames(interrupt, hdr_path,
							   hdr_fdte,
							   &stripe_attr);
		if (pfs_error) {
			PFS_DEBUG(("pfs_multi_open: construct_stripefile_pathnames ret %d\n",
				   pfs_error));
			goto out;
		}
		PFS_DEBUG_SATTR(("pfs_multi_open: new header contents:\n"),
				stripe_attr);

		/*
		 * Set the stripe attributes for this PFS file if the header
		 * file has just been created.
		 */
		pfs_error = set_stripe_attributes(interrupt,
						  hdr_path, len_hdr_path,
						  hdr_fdte, mode, crtmode,
						  stripe_attr);
		if (pfs_error) {
			PFS_DEBUG(("pfs_multi_open: set_stripe_attributes returned %d\n",
				   pfs_error));
			goto out;
		}
	}
#ifdef	DEBUG_PFS
	else
		PFS_DEBUG_SATTR(("pfs_multi_open: header already existed:\n"),
				stripe_attr);
#endif

	/*
	 * Build a PFS file descriptor table entry.
	 */
	pfs_error = pfs_fd_init(&pfs_fd, stripe_attr);
	if (pfs_error)
		goto out;

	PFS_DEBUG(("pfs_multi_open: pfs_fd = %x\n", pfs_fd));
	PFS_DEBUG(("                p_stripe_unit_size = %d\n",
		   pfs_fd->p_stripe_unit_size));
	PFS_DEBUG(("                p_stripe_factor = %d\n",
		   pfs_fd->p_stripe_factor));
	PFS_DEBUG(("                p_start_stripedir = %d\n",
		   pfs_fd->p_start_stripedir));

	/*
	 * Install the PFS descriptor.
	 * No need to FDT_LOCK() here: we have sole access and open() has
	 * not yet installed the fdte for the PFS header file.
	 */
	hdr_fdte->pfs_fd = pfs_fd;

	/*
	 * Open all the stripefiles.  This is a two-step process: all open
	 * requests are sent at once, then all responses received at the
	 * same port.
	 */		
	sf_path = &stripe_attr->p_sdirs;
	sf_fdt = pfs_fd->p_stripe_fdt;
        sfactor = pfs_fd->p_stripe_factor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_open: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("pfs_multi_open: dir port invalid!\n"));
		    break;
		}

		pfs_error = fsvr_pfs_open_msg_send(dir_port,
						   reply_port,
						   credentials_port,
						   0,
						   rootdir_port, &sfd,
						   mod_path,
						   mod_len + 1,
						   mode, crtmode, &sfp,
						   iomode);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_open_msg_receive(MACH_PORT_NULL,
						     reply_port,
						     credentials_port,
						     0,
						     MACH_PORT_NULL, &sfd,
						     NULL,
						     0,
						     0, 0, &sfp,
						     0);
		PFS_DEBUG_RECV(("pfs_multi_open: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d sfp=0x%x\n", sfd, sfp),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef	DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_open: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}

		sf_fdt[sfd].s_fp = sfp;
	}

	if (pfs_error) {
		/*
		 * Deallocate the PFS file descriptor, and remove any 
		 * stripefiles that were created as a result of this system
		 * call.  The header file is removed at the 'out:' label.
		 */
		(void)pfs_fd_dealloc(hdr_fdte->pfs_fd);
		hdr_fdte->pfs_fd = NULL;
		if ((new_hdr) && (mode & O_CREAT))
			multi_unlink(interrupt, hdr_path, len_hdr_path,
				     stripe_attr, FALSE);
		goto out;
	}
	/*
	 * See if the default is M_ASYNC, ie. bootmagic pfs_async_dflt is
	 * set, if so set the PFS I/O mode to M_ASYNC. 
	 */
	if (pfs_async_dflt) {
		hdr_fdte->pfs_iomode = M_ASYNC;
	}
	/*
	 * If we are truncating the file then we need to obtain the token to
	 * the PFS file first so that the length can be updated.
	 */
	if ((mode & O_TRUNC) && (!new_hdr)) {
		pfs_error = file_token_acquire(hdr_fdte, interrupt,
					       PFS_OP_WRITE, 0,
					       &pfs_offset, &pfs_length);
		if (pfs_error) {
			goto out;
		}
		fdte_lock(hdr_fdte);
		hdr_fdte->modified = 1;	/* set modified to update the length */
		fdte_unlock(hdr_fdte);
		file_token_release(hdr_fdte, interrupt, PFS_OP_WRITE, 0,
				   &ex_zero, &ex_zero);
	}

out:
	if ((pfs_error) && (new_hdr) && (mode & O_CREAT))
		unlink_hdr_file(interrupt, hdr_path, len_hdr_path,
				(ulong_t)VIO_PFS);
	if (stripe_attr)
		pfs_free((void *)stripe_attr, 
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));

	return(pfs_error);
}

/*
 * Name:
 *	pfs_set_sattr
 *
 * Description:	
 *	This function sets the stripe attributes of the given file.  The file
 *	must be open for writing and must be zero-length.  The stripe 
 *	attributes are specified in the form of a sattr structure, and must be
 *	a valid subset of the default stripe attributes of the PFS file system
 *	in which the file resides.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	sattr		Pointer to the user's stripe attributes.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_set_sattr(interrupt, hdr_fdte, sattr)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	struct sattr	*sattr;		/* user's stripe attributes */
{
	pfs_fd_t	*pfs_fd		= hdr_fdte->pfs_fd;
	stripe_fd_t	*sf_fdt		= pfs_fd->p_stripe_fdt;
	uint_t		sfactor		= pfs_fd->p_stripe_factor;
	uint_t		sfd;		/* stripefile descriptor index */
	pathname_t	*sf_path;	/* stripefile pathname */

	uint_t		new_sunitsize  = sattr->s_sunitsize;
	uint_t		new_sfactor    = sattr->s_sfactor;
	uint_t		new_start_sdir = sattr->s_start_sdir;
	uint_t		new_sfd;	/* new stripefile descriptor index */
	pathname_t	*new_sf_path;	/* new stripefile pathname */

	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */

	pfs_fd_t	*new_pfs_fd;
	stripe_fd_t	*new_sf_fdt;
	struct statpfs	*mount_attr = NULL;
	struct statpfs	*stripe_attr = NULL, *new_stripe_attr = NULL;
	register int	error;
	

	PFS_TRACE(("pfs_set_sattr: fdte=0x%x sattr=0x%x\n", hdr_fdte, sattr));

	/*
	 * Sanity check the sattr structure.  User-specified stripe attributes
	 * must be a subset of the file's current attributes.
	 */
	if (!user_rcheck(sattr, sizeof(struct sattr)))
		return(EFAULT);
	if ((new_sunitsize == 0) || (new_sfactor > sfactor) ||
	    (new_sfactor == 0) || (new_start_sdir > (sfactor - 1)))
		return(EINVAL);

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Make sure we have the PFS token for this file.  This updates us
	 * with the latest PFS file offset and length.
	 */
	error = file_token_acquire(hdr_fdte, interrupt, PFS_OP_WRITE, 0,
				   &pfs_offset, &pfs_length);
	PFS_TRACE(("pfs_set_sattr: tok offset=%d,%d, length=%d,%d\n",
		   pfs_offset.shigh, pfs_offset.slow,
		   pfs_length.shigh, pfs_length.slow));
	if (error) {
		fdte_pfsio_unlock(pfs_fd);
		return(error);
	}

	/*
	 * If the PFS file has already been written to, it is too late to
	 * change its stripe attributes, so this is an illegal request.
	 */
	if (!EQUAL(pfs_length, ex_zero)) {
		error = EEXIST;
		goto out;
	}

	/*
	 * Get the current stripe attributes for this file.
	 */
	error = get_stripe_attributes((char *)NULL, 0, hdr_fdte, 0, 
				      &stripe_attr);
	PFS_DEBUG(("pfs_set_sattr: get_stripe_attributes ret %d\n",
		   error));
	if (error)
		goto out;

	/*
	 * Get the default stripe attributes for this file system.
	 */
	error = get_mount_attributes(interrupt, hdr_fdte, &mount_attr);
	PFS_DEBUG(("pfs_set_sattr: get_mount_attributes ret %d\n", error));
	if (error)
		return(error);

	/*
	 * Disallow setting stripe attributes on a file more than once.  It's
	 * not a useful thing to do, so isn't worth the pain to support. (We'd
	 * have to handle cases where stripe file pathnames need to be 
	 * reconstructed, and stripe files recreated with the same create mode
	 * that was specified when the PFS file was created, etc.)  This also
	 * prevents dealing with synchronization problems when the user
	 * erroneously tries to set the attributes from all nodes in the 
	 * application at once, rather than from only one node.
	 * 
	 * The following check covers the cases where we can get into trouble.
	 * The p_start_sdir field can't be used in this check, because when a
	 * PFS file is created it is purposely striped starting at a random 
	 * stripe directory rather than stripe directory 0.
	 */
	if ((stripe_attr->p_sunitsize != mount_attr->p_sunitsize) ||
	    (stripe_attr->p_sfactor != mount_attr->p_sfactor)) {
		error = EBADF;
		goto out;
	}

	/*
	 * Optimize if we're only changing the stripe unit size.  Change the
	 * existing stripe attributes in place, and set them.
	 */
	if ((new_sfactor == stripe_attr->p_sfactor) && (new_start_sdir == 0)) {
		stripe_attr->p_sunitsize = new_sunitsize;
		error = set_stripe_attributes(interrupt, (char *)NULL, 0,
					      hdr_fdte, O_RDWR, S_IWUSR,
					      stripe_attr);
		PFS_DEBUG(("pfs_set_sattr: set_stripe_attributes ret %d\n",
			   error));
		if (!error)
			pfs_fd->p_stripe_unit_size = new_sunitsize;
		goto out;
	}

	/*
	 * Allocate space for a new copy of the full stripe attributes, and
	 * initialize it to the new attributes.
	 */
	error = pfs_malloc((void *)&new_stripe_attr, stripe_attr->p_reclen);
	PFS_DEBUG(("pfs_set_sattr: pfs_malloc ret %d\n", error));
	if (error)
		goto out;

	new_stripe_attr->p_reclen = sizeof(struct statpfs) - 
		sizeof(pathname_t);
	new_stripe_attr->p_magic = stripe_attr->p_magic;
	new_stripe_attr->p_sunitsize = new_sunitsize;
	new_stripe_attr->p_sfactor = new_sfactor;
	new_stripe_attr->p_start_sdir = new_start_sdir;

	/*
	 * Build a new PFS file descriptor table entry.
	 */
	if (error = pfs_fd_init(&new_pfs_fd, new_stripe_attr))
		goto out;
	PFS_DEBUG(("pfs_set_sattr: new_pfs_fd = %x\n", new_pfs_fd));
	new_sf_fdt = new_pfs_fd->p_stripe_fdt;

	/*
	 * Initialize path pointers to old and new stripe attributes to point
	 * to the stripe file corresponding to the start_sdir specified by the
	 * caller.  Remember that: 1) the start_sdir is always specified 
	 * relative to the default start_sdir of a PFS file system, which is 
	 * always 0, and 2) even if the attributes of this file have not been
	 * previously set, the current start_sdir of the file may not be 0 
	 * because it is randomly selected by construct_stripefile_pathnames()
	 * when the file was created.  The first statement below corrects for
	 * this latter point.
	 */
	new_start_sdir = MOD_SUB(new_start_sdir, pfs_fd->p_start_stripedir,
				 sfactor);
	sf_path = &stripe_attr->p_sdirs;
	for (sfd = 0; sfd < new_start_sdir; sfd++) {
		sf_path = NEXTPATH(sf_path);
	}
	new_sf_path = &new_stripe_attr->p_sdirs;

	/*
	 * Loop through the new stripe group, copying in the pathnames of
	 * stripe files that belong to the new group.
	 */
	sfd = new_start_sdir;
	for (new_sfd = 0; new_sfd < new_sfactor; new_sfd++) {
		bcopy(sf_path, new_sf_path, PATHSIZE(sf_path));
		new_stripe_attr->p_reclen += PATHSIZE(new_sf_path);
		*sf_path->name = '\0';	/* mark "don't unlink" */
		PFS_DEBUG(("pfs_set_sattr: copying sfd=%d to new_sfd=%d\n",
			   sfd, new_sfd));
		new_sf_fdt[new_sfd].s_fp = sf_fdt[sfd].s_fp;
		sf_fdt[sfd].s_fp = MACH_PORT_NULL;

		if (++sfd == sfactor) {
			sfd = 0;
			sf_path = &stripe_attr->p_sdirs;
		} else {
			sf_path = NEXTPATH(sf_path);
		}
		new_sf_path = NEXTPATH(new_sf_path);
	}
	PFS_DEBUG_SATTR(("pfs_set_sattr: old header contents:\n"),
			stripe_attr);
	PFS_DEBUG_SATTR(("pfs_set_sattr: new header contents:\n"),
			new_stripe_attr);
	
	/*
	 * Set the new stripe attributes for this PFS file.
	 */
	error = set_stripe_attributes(interrupt, (char *)NULL, 0, hdr_fdte,
				      O_RDWR, S_IWUSR, new_stripe_attr);
	PFS_DEBUG(("pfs_set_sattr: set_stripe_attributes returned %d\n",
		   error));
	if (error) {
		pfs_fd_dealloc(new_pfs_fd);
		goto out;
	}

	/*
	 * Now we're committed to the new attributes.  Close unneeded
	 * stripefiles, deallocate the current PFS file descriptor, and remove
	 * unneeded stripefiles.
	 */
	if (error = pfs_fd_dealloc(pfs_fd)) {
		PFS_DEBUG(("pfs_set_sattr: pfs_fd_dealloc ret %d\n", error));
		error = ESUCCESS;
	}
	if (error = multi_unlink(interrupt, NULL, 0, stripe_attr, FALSE)) {
		PFS_DEBUG(("pfs_set_sattr: multi_unlink ret %d\n", error));
		error = ESUCCESS;
	}

	/*
	 * Install the new PFS descriptor.
	 */
	fdte_lock(hdr_fdte);
	hdr_fdte->pfs_fd = new_pfs_fd;
	fdte_unlock(hdr_fdte);

out:
	file_token_release(hdr_fdte, interrupt, PFS_OP_WRITE, 0, 
			   &ex_neg_one, &ex_neg_one); 
	fdte_pfsio_unlock(pfs_fd);
	if (new_stripe_attr)
		pfs_free((void *)new_stripe_attr, stripe_attr->p_reclen);
	if (stripe_attr)
		pfs_free((void *)stripe_attr, stripe_attr->p_reclen);
	if (mount_attr)
		pfs_free((void *)mount_attr,
			 MAX(mount_attr->p_reclen, STATPFS_BUFSZ));
	return(error);
}


/*
 * Name:
 *	pfs_map_sattr
 *
 * Description:	
 *	This function temporarily maps the stripe attributes of an open file.
 *	The mapped attributes only apply to the threads using the given file
 *	descriptor, and go away when the file is closed. 
 *
 *	This function should only be called if the file has been opened read
 *	only, otherwise if the file is written to it can become corrupted since
 *	the mapped attributes are not the "real" (permanent) attributes of the
 *	file.  The stripe attributes are specified in the form of a sattr
 *	structure, and must be a valid subset of the current stripe attributes
 *	of the file.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	sattr		Pointer to the user's stripe attributes.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_map_sattr(hdr_fdte, sattr)
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	struct sattr	*sattr;		/* user's stripe attributes */
{
	pfs_fd_t	*pfs_fd		= hdr_fdte->pfs_fd;
	stripe_fd_t	*sf_fdt		= pfs_fd->p_stripe_fdt;
	uint_t		sfactor		= pfs_fd->p_stripe_factor;
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		new_sunitsize  = sattr->s_sunitsize;
	uint_t		new_sfactor    = sattr->s_sfactor;
	uint_t		new_start_sdir = sattr->s_start_sdir;
	uint_t		new_sfd;	/* new stripefile descriptor index */

	pfs_fd_t	*new_pfs_fd;
	stripe_fd_t	*new_sf_fdt;
	register int	error;
	

	PFS_TRACE(("pfs_map_sattr: fdte=0x%x sattr=0x%x\n", hdr_fdte, sattr));

	/*
	 * Sanity check the sattr structure.  User-specified stripe attributes
	 * must be a subset of the file's current attributes.
	 */
	if (!user_rcheck(sattr, sizeof(struct sattr)))
		return(EFAULT);
	if ((new_sunitsize == 0) || (new_sfactor > sfactor) ||
	    (new_sfactor == 0) || (new_start_sdir > (sfactor - 1)))
		return(EINVAL);

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Build a new PFS file descriptor table entry.
	 */
	new_pfs_fd = (pfs_fd_t *) malloc(sizeof(pfs_fd_t) +
				   sizeof(stripe_fd_t)*(new_sfactor-1));
	if (new_pfs_fd == NULL) {
		error = ENOMEM;
		goto out;
	}

	fdte_pfsio_lock_init(new_pfs_fd);
	new_pfs_fd->p_offset.shigh = 0;
	new_pfs_fd->p_offset.slow = 0;
	new_pfs_fd->p_length.shigh = pfs_fd->p_length.shigh;
	new_pfs_fd->p_length.slow = pfs_fd->p_length.slow;
	new_pfs_fd->p_stripe_unit_size = new_sunitsize;
	new_pfs_fd->p_stripe_factor = new_sfactor;
	new_pfs_fd->p_start_stripedir = new_start_sdir;
	new_sf_fdt = new_pfs_fd->p_stripe_fdt;

	/*
	 * Loop through the new stripe group, copying in needed open stripe
	 * files.  
	 */
	sfd = new_start_sdir;
	for (new_sfd = 0; new_sfd < new_sfactor; new_sfd++) {
		PFS_DEBUG(("pfs_map_sattr: copying sfd=%d to new_sfd=%d\n",
			   sfd, new_sfd));
		new_sf_fdt[new_sfd].s_fp = sf_fdt[sfd].s_fp;
		sf_fdt[sfd].s_fp = MACH_PORT_NULL;

		if (++sfd == sfactor)
			sfd = 0;
	}
	PFS_DEBUG(("pfs_map_sattr: new_pfs_fd = %x\n", new_pfs_fd));

	/*
	 * Close unneeded stripefiles and deallocate the current PFS file
	 * descriptor.
	 */
	if (error = pfs_fd_dealloc(pfs_fd)) {
		PFS_DEBUG(("pfs_map_sattr: pfs_fd_dealloc ret %d\n", error));
		error = ESUCCESS;
	}

	/*
	 * Install the new PFS descriptor.
	 */
	fdte_lock(hdr_fdte);
	hdr_fdte->pfs_fd = new_pfs_fd;
	fdte_unlock(hdr_fdte);

out:
	fdte_pfsio_unlock(pfs_fd);
	return(error);
}


/*
 * Name:
 *	pfs_get_stripe_attributes
 *
 * Description:	
 *	This function gets the full PFS stripe attributes, defined by the
 *	statpfs structure, for the given file.  The full attributes differ
 *	from the user-settable attributes defined by the sattr structure;
 *	the latter is a subset of the former, and does not include the explicit
 *	stripe file pathnames which are generally hidden from the user.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	stripe_attr	Pointer to the caller's stripe attributes buffer.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_get_stripe_attributes(hdr_fdte, stripe_attr)
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	struct statpfs	*stripe_attr;	/* caller's stripe attributes */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	struct statpfs	*sa = NULL;
	uint_t		len_stripe_attr;
	register int	error;

	PFS_TRACE(("pfs_get_stripe_attributes: fdte=0x%x stripe_attr=0x%x\n",
		   hdr_fdte, stripe_attr));

	/*
	 * Validate the user's buffer.
	 */
	len_stripe_attr = stripe_attr->p_reclen;
	if (len_stripe_attr == 0) {
		return(EINVAL);
	}
	if (!user_rcheck(stripe_attr, len_stripe_attr))
		return(EFAULT);

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Get the stripe attributes, and copy them into the user's buffer.
	 */
	if (error = get_stripe_attributes((char *)NULL, 0, hdr_fdte, 0, &sa)) {
		PFS_DEBUG(("pfs_get_stripe_attributes: get_stripe_attributes ret %d\n",
			   error));
		goto out;
	}

	len_stripe_attr = MIN(len_stripe_attr, sa->p_reclen);
	PFS_DEBUG(("pfs_get_stripe_attributes: copying %d bytes to user buf\n",
		   len_stripe_attr));
	error = user_bcopy2(sa, stripe_attr, &len_stripe_attr);
	
out:
	fdte_pfsio_unlock(pfs_fd);

	if (sa)
		pfs_free((void *)sa, MAX(sa->p_reclen, STATPFS_BUFSZ));
	return(error);
}


/*
 * Name:
 *	pfs_multi_close
 *
 * Description:	
 *	This function closes the stripefiles associated with an open PFS file,
 *	and deallocates the PFS descriptor.
 *
 * Parameters:
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 * Returns:
 *	None.
 */
int
pfs_multi_close(hdr_fdte)
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
{
	int		error;
	pfs_fd_t	*pfs_fd	= hdr_fdte->pfs_fd;


	PFS_DEBUG(("pfs_multi_close: hdr_fdte=0x%x\n", hdr_fdte));

	/*
	 * Deallocate the PFS file descriptor and close
	 * each stripefile and the header file:
	 */
	error = pfs_fd_dealloc(pfs_fd);
	hdr_fdte->pfs_fd = NULL;

	return error;
}


/*
 * Name:
 *	multi_close
 *
 * Description:	
 *	This function asynchronously closes all the stripefiles associated
 *	with an open PFS file.
 *
 * Parameters:
 *	pfs_fd		Pointer to the file's PFS striped file descriptor.
 *
 * Returns:
 *	None.
 */
int
multi_close(pfs_fd)
	pfs_fd_t	*pfs_fd;
{
	int		pfs_error;	/* running error returned to caller */
	int		sf_error;	/* stripefile operation error */
	uint_t		sfd;
	mach_port_t	reply_port;

	int		num_sent = 0;
	stripe_fd_t	*sf_fdt	 = pfs_fd->p_stripe_fdt;
	register uint_t	sfactor	 = pfs_fd->p_stripe_factor;


	PFS_DEBUG(("  multi_close: pfs_fd=0x%x\n", pfs_fd));

	/*
	 * Perform the unref operations concurrently on all the stripefiles:
	 */
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	for (sfd = 0; sfd < sfactor; sfd++) {
		if (sf_fdt[sfd].s_fp != MACH_PORT_NULL) {
			PFS_DEBUG(("  multi_close: SEND sfd=%d\n", sfd));
			pfs_error = 
				fsvr_pfs_file_unref_msg_send(sf_fdt[sfd].s_fp,
							     reply_port,
							     credentials_port,
							     &sfd,
							     sf_fdt[sfd].s_fp);
			if (pfs_error)
				break;

			num_sent++;
		}
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_file_unref_msg_receive(MACH_PORT_NULL,
							   reply_port,
							   credentials_port,
							   &sfd,
							   MACH_PORT_NULL);
		PFS_DEBUG_RECV(("  multi_close: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_close: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_read
 *
 * Description:	
 *	This function performs concurrent read operations to one or more
 *	stripefiles to satisfy a read system call on a PFS file.  Both read and
 *	readv operations are supported.
 *
 *	The request may be broken into smaller "chunks" to avoid flooding I/O
 *	nodes with requests for large amounts of data simultaneously from the
 *	typically much more numerous compute nodes.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	read_op		Set to PFS_OP_READ if standard read being performed;
 *			data parameter points to the caller's buffer into which
 *			stripefile data is to be coalesced.
 *
 *			Set to PFS_OP_READV if vector readv being performed;
 *			data parameter points to the caller's array of iovec
 *			structures into which coalesced stripefile data is to
 *			be scattered.
 *
 *	data		Pointer to caller's buffer area, interpreted as
 *			described above.
 *
 *	pfs_count	Number of bytes to read from the PFS file.
 *
 *	rval		Pointer to the return value of the read/readv system
 *			call.  This value will be either the number of bytes 
 *			actually read if successful or -1 if an error occurred.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_read(interrupt, hdr_fdte, read_op, data, pfs_count, rval)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	int		read_op;	/* type of read operation */
	caddr_t		data;		/* data buffer area */
	uint_t		pfs_count;	/* total requested count of data */
	int		*rval;		/* out */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */
	size_t		pfs_actual = 0;	/* actual count returned to caller */
	uint_t		pfs_resid = pfs_count;	/* remaining to be read */
	size_t		pfs_max_request;/* max # bytes written at once */
	int		pfs_error;	/* running error returned to caller */

	uint_t		sfd;		/* stripefile descriptor index */
	sfile_count_t	sf_count_array[BIG_STRIPE_FACTOR];	/* array for */
	sfile_count_t	*sf_counts = sf_count_array;	/* stripefile counts */
	ptuple_t	start;		/* stripefile location of data start */
	ptuple_t	end;		/* stripefile location of data end */
	int		(*copy_func)();	/* copy function to call */
	esize_t		ex_temp;	/* extended temporary variable */
	uint_t		count;		/* # bytes written each snd/rcv loop */
	size_t		actual = 0;	/* actual bytes written each loop */
	mach_port_t	reply_port;

	PFS_TRACE(("pfs_multi_read: hdr_fdte=0x%x data=0x%x pfs_count=%d\n",
		   hdr_fdte, data, pfs_count));

	if ((int)pfs_count < 0)
		return(EINVAL);

	if ((hdr_fdte->fmode & FREAD) == 0)
		return(EBADF);

	pfs_error = sfile_count_init(&sf_counts, sfactor);
	if (pfs_error)
		return(pfs_error);

	/*
	 * Perform initialization that is dependent on the type of read
	 * operation being performed (read or readv).
	 */
	if (read_op == PFS_OP_READ)
		copy_func = linear_copy;
	else /* read_op is PFS_OP_READV */
		copy_func = vector_copy;

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Make sure we have the PFS token for this file.  This updates us
	 * with the latest PFS file offset and length.
	 */
	pfs_error = file_token_acquire(hdr_fdte, interrupt, read_op,
				       pfs_count, &pfs_offset, &pfs_length);
	PFS_TRACE(("pfs_multi_read: tok offset=%d,%d length=%d,%d\n",
		   pfs_offset.shigh, pfs_offset.slow,
		   pfs_length.shigh, pfs_length.slow));
	if (pfs_error)
		goto out;

	/*
	 * Restrict the amount to read by the end-of-file.
	 */
	if (LESS(pfs_offset, pfs_length)) {
		ex_temp.shigh = 0;
		ex_temp.slow = pfs_count;
		ex_temp = EMIN(__esub(pfs_length, pfs_offset, &dont_care),
			       ex_temp);
		pfs_count = ex_temp.slow;
	} else {
		pfs_count = 0;
	}

	/*
	 * Try to restrict the amount of data requested at once from any one
	 * I/O node, to avoid potential flooding of an I/O node from multiple
	 * compute nodes.  Currently only supported if the stripe unit size
	 * is less than MAX_IONODE_REQUEST.
	 */
	pfs_max_request = (sunitsize < MAX_IONODE_REQUEST) ? 
		(ROUND_DOWN(MAX_IONODE_REQUEST, sunitsize) * sfactor) :
		UINT_MAX;

	/*
	 * Initialize sf_counts array with stripefile offsets, and grab a reply
	 * port.
	 */
	for (sfd = 0; sfd < sfactor; sfd++)
		sf_counts[sfd].offset = sf_fdt[sfd].s_offset;
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	/*
	 * Performance optimization: if we have to break the request into
	 * multiple "chunks" (if pfs_max_request < pfs_resid), round the 
	 * ending offset of the first chunk down to the nearest page for more
	 * efficient page-aligned IPC and disk I/O on the remaining chunks.
	 * Assumes pfs_max_request is a multiple of vm_page_size.
	 */
	PTUPLE_INIT(start, pfs_offset, sunitsize, sfactor);
	if (pfs_max_request < pfs_resid)
		count = pfs_max_request - 
			(start.sunitoff - trunc_page(start.sunitoff));
	else
		count = pfs_resid;

	/*
	 * Loop through the user's buffer area, reading chunks of size no
	 * greater than pfs_max_request.
	 */
	do {
		PTUPLE_INIT(end,
			    __eadd1(pfs_offset, (long)(pfs_actual + count),
				    &dont_care),
			    sunitsize,
			    sfactor);

		PFS_DEBUG(("pfs_multi_read: count=%lu pfs_max_request=%lu\n",
			   count, pfs_max_request));
		PFS_DEBUG(("                 pfs_actual=%lu pfs_resid=%lu\n",
			   pfs_actual, pfs_resid));
		pfs_error = multi_read(sf_fdt, start, end, reply_port,
				       data, (off_t)pfs_actual, copy_func,
				       sf_counts, sunitsize, sfactor, &actual);
		if (pfs_error)
			break;

		pfs_actual += actual;
		pfs_resid -= actual;
		if (actual != count)
			break;

		start = end;
		count = MIN(pfs_max_request, pfs_resid);
	} while (pfs_resid);

	if (pfs_error) {
		file_token_release(hdr_fdte, interrupt, read_op,
				   0, &ex_neg_one, &ex_neg_one); 
		goto out;
	}

	if (pfs_actual < pfs_count) {
		/*
		 * Verify consistency of actual counts.
		 */
		PFS_DEBUG(("pfs_multi_read: pfs_cnt=%d pfs_act=%d\n",
			   pfs_count, pfs_actual));
		PFS_DEBUG(("                call pfs_sync_actuals\n"));
		pfs_sync_actuals(sf_counts, sf_fdt, sunitsize, sfactor,
				 &ex_temp);
		pfs_actual = ex_temp.slow;
		PFS_DEBUG(("pfs_multi_read: pfs_cnt=%d, pfs_act=%d\n",
			   pfs_count, pfs_actual));
	}

	/*
	 * Update PFS file and stripefile offsets.
	 */
	pfs_offset = __eadd1(pfs_offset, (long)pfs_actual, &dont_care);
	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_fdt[sfd].s_offset += sf_counts[sfd].actual;
	}
	*rval = pfs_actual;

	file_token_release(hdr_fdte, interrupt, read_op,
			   pfs_actual, &pfs_offset, &ex_neg_one);

out:
	fdte_pfsio_unlock(pfs_fd);
	if (sf_counts != sf_count_array)    /* a bigger array was malloc'd */
		pfs_free((void *)sf_counts,
			 (sfactor * sizeof(struct sfile_count)));
	PFS_DEBUG(("pfs_multi_read: return PFS offset=%d,%d pfs_actual=%d pfs_error=%d\n",
		   pfs_fd->p_offset.shigh, pfs_fd->p_offset.slow, pfs_actual,
		   pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	multi_read
 *
 * Description:	
 *	This function performs concurrent Fast Path read operations to one or
 *	more stripefiles to satisfy a read system call on a PFS file.
 *
 *	Stripe unit merging is done so that at most one request is sent to
 *	each stripefile.  Eventually, we might want to add optimizations for 
 *	requests of size <= 1 stripe (it often would be more efficient to loop
 *	through stripe units on these requests, rather than looping through
 *	stripefiles).  However, in most cases any speedup achieved from doing
 *	this would probably be negligible, and not worth the additional
 *	complexity.
 *
 *	Note that this function attempts to employ VM optimizations
 *	(vm_copy() of page-aligned data instead of user_bcopy2()) when
 *	copying data.
 *
 * Parameters:
 *	sf_fdt		Array of stripefile descriptors for the PFS file.
 *
 *	start		(snum, sfile, sunitoff) tuple describing the offset
 *			in the PFS file at which to start writing, in terms of
 *			the stripe number, stripefile, and offset in the first
 *			stripe unit.
 *
 *	end		tuple describing the PFS offset corresponding to the
 *			end of the data to be written.
 *
 *	reply_port	Per-thread port at which IPC responses are received.
 *
 *	data		Pointer to user's buffer area, either a simple buffer
 *			or an array of iovec structures.
 *
 *	data_offset	Offset into user data at which we are currently 
 *			reading.
 *
 *	copy_func	Function to call to copy data into the user's buffer
 *			area, if necessary.
 *
 *	sf_counts	Points to temporary array for tracking running 
 *			stripefile requested/actual counts, etc.
 *
 *	sunitsize	Stripe unit size, in bytes.
 *
 *	sfactor		Stripe factor.
 *
 *	actual		The returned number of bytes actually read if no error
 *			was encountered.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_read(sf_fdt, start, end, reply_port, data, data_offset, copy_func, 
	   sf_counts, sunitsize, sfactor, actual)
	stripe_fd_t	*sf_fdt;
	ptuple_t	start;
	ptuple_t	end;
	mach_port_t	reply_port;
	caddr_t		data;
	off_t		data_offset;
	int		(*copy_func)();
	sfile_count_t	*sf_counts;
	size_t		sunitsize;
	uint_t		sfactor;
	size_t		*actual;	/* OUT    */
{
	size_t		sf_count;	/* # bytes stripefile data to read */
	size_t		sf_actual;	/* actual count of stripefile data */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	register int	pfs_error;	/* running error returned to caller */
	char		*mig_data;	/* pointer to data returned by MiG */
	size_t		total_actual = 0;
	uint_t		num_sent = 0;

	PFS_TRACE(("  multi_read: start snum=%d, sfile=%d, sunitoff=%d\n",
		   start.snum, start.sfile, start.sunitoff));
	PFS_TRACE(("               end   snum=%d, sfile=%d, sunitoff=%d\n",
		   end.snum, end.sfile, end.sunitoff));

	/*
	 * For each stripefile that contains stripe units that fall into the
	 * requested range of data, issue one Fast Path read.  Scatter the
	 * data received in each response into the user's buffer area.
	 */
	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_count = PTUPLE_TO_SFOFF(end.snum, end.sfile, end.sunitoff,
					   sfd, sunitsize)
			- sf_counts[sfd].offset;
		sf_counts[sfd].requested += sf_count;

		if ((int)sf_count <= 0)
			continue;

		PFS_DEBUG(("  multi_read: SEND sfd=%d sf_off=%d sf_cnt=%d\n",
			   sfd, sf_counts[sfd].offset, sf_count));
		pfs_error = fsvr_read_at_offset_msg_send(sf_fdt[sfd].s_fp,
							 reply_port,
							 credentials_port,
							 &sfd,
							 sf_counts[sfd].offset,
							 sf_count,
							 &mig_data,
							 &sf_actual);
		if (pfs_error) {
			sf_counts[sfd].requested = 0;
			break;
		}
		num_sent++;
	}

	/*
	 * Receive the responses to the read requests, and scatter the stripe
	 * unit data into the caller's buffer.
	 */
	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_read_at_offset_msg_receive(MACH_PORT_NULL,
							   reply_port,
							   credentials_port,
							   &sfd,
							   0,
							   sf_count,
							   &mig_data,
							   &sf_actual);
		PFS_DEBUG_RECV(("  multi_read: RECEIVE sf_error=%d", sf_error),
			       (" sfd=%d sf_actual=%d\n", sfd, sf_actual),
			       sf_error);

		if (pfs_error) {	/* toss the response */
			if (sf_error)
				continue;
			else
				goto free;
		}
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_read: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}

		/*
		 * Scatter the stripe unit data into the caller's buffer area.
		 */
		pfs_error = (*copy_func)(
			data,
			data_offset + PTUPLE_TO_DOFF(start.snum, start.sfile,
						     start.sunitoff, sfd,
						     sunitsize, sfactor),
			mig_data,
			MIN(sunitsize - SFOFF_TO_SUOFF(sf_counts[sfd].offset,
						       sunitsize),
			    sf_actual),
			TO_USER,
			sf_actual,
			sunitsize,
			sfactor);

		sf_counts[sfd].actual += sf_actual;
		sf_counts[sfd].offset += sf_actual;
		total_actual += sf_actual;

free:
		(void) vm_deallocate(mach_task_self(), (vm_offset_t)mig_data,
				     sf_actual);
	}

	*actual = total_actual;
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_write
 *
 * Description:	
 *	This function performs concurrent write operations to one or more
 *	stripefiles to satisfy a write system call on a PFS file.  Both write
 *	and writev operations are supported.
 *
 *	The request may be broken into smaller "chunks" to avoid flooding I/O
 *	nodes with large amounts of data simultaneously from the typically
 *	much more numerous compute nodes.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	write_op	Set to PFS_OP_WRITE if standard write being performed;
 *			data parameter points to the caller's buffer from which
 *			data is to be striped into stripefiles.
 *
 *			Set to PFS_OP_WRITEV if vector writev being performed;
 *			data parameter points to the caller's array of iovec
 *			structures from which data is to be gathered and 
 *			striped into stripefiles.
 *
 *	data		Pointer to caller's buffer area, interpreted as 
 *			described above.
 *
 *	pfs_count	Number of bytes to write.
 *
 *	rval		Pointer to the return value of the write/writev system
 *			call.  This value will be either the number of bytes 
 *			actually written if successful or -1 if an error 
 *			occurred.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_write(interrupt, hdr_fdte, write_op, data, pfs_count, rval)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	int		write_op;	/* type of write operation */
	caddr_t		data;		/* data buffer area */
	uint_t		pfs_count;	/* total bytes to be written */
	int		*rval;		/* out */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */
	size_t		pfs_actual = 0;	/* actual count returned to caller */
	uint_t		pfs_resid = pfs_count;	/* remaining to be written */
	size_t		pfs_max_request;/* max # bytes written at once */
	int		pfs_error;	/* running error returned to caller */

	uint_t		sfd;		/* stripefile descriptor index */
	sfile_count_t	sf_count_array[BIG_STRIPE_FACTOR];	/* array for */
	sfile_count_t	*sf_counts = sf_count_array;	/* stripefile counts */
	ptuple_t	start;		/* stripefile location of data start */
	ptuple_t	end;		/* stripefile location of data end */
	char		*mig_data;	/* pointer to data handed to MiG */
	vm_size_t	mig_data_len = 0;/* size of MiG data buffer */
	int		(*copy_func)();	/* copy function to call */
	esize_t		ex_temp;	/* extended temporary variable */
	uint_t		count;		/* # bytes written each snd/rcv loop */
	size_t		actual = 0;	/* actual bytes written each loop */
	boolean_t	extend_sfiles = FALSE;
	mach_port_t	reply_port;

	PFS_TRACE(("pfs_multi_write: hdr_fdte=0x%x data=0x%x pfs_count=%d\n",
		   hdr_fdte, data, pfs_count));

	if ((int)pfs_count < 0)
		return(EINVAL);

	if ((hdr_fdte->fmode & FWRITE) == 0)
		return(EBADF);

	pfs_error = sfile_count_init(&sf_counts, sfactor);
	if (pfs_error)
		return(pfs_error);

	/*
	 * Perform initialization that is dependent on the type of write
	 * operation being performed (write or writev).
	 */
	if (write_op == PFS_OP_WRITE)
		copy_func = linear_copy;
	else /* write_op is PFS_OP_WRITEV */
		copy_func = vector_copy;

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Make sure we have the PFS token for this file.  This updates us
	 * with the latest PFS file offset and length.
	 */
	pfs_error = file_token_acquire(hdr_fdte, interrupt, write_op,
				       pfs_count, &pfs_offset, &pfs_length);
	PFS_TRACE(("pfs_multi_write: tok offset=%d,%d, length=%d,%d\n",
		   pfs_offset.shigh, pfs_offset.slow,
		   pfs_length.shigh, pfs_length.slow));
	if (pfs_error)
		goto out;

	/*
	 * Must make this check only *after* the file token is acquired,
	 * so some file sharing modes can synchronize properly.
	 */
	if ((int)pfs_count == 0) {
		file_token_release(hdr_fdte, interrupt, write_op, 0,
				   &ex_neg_one, &ex_neg_one); 
		goto out;
	}

	/*
	 * Handle append mode writes.  If appending, we must reset the
	 * stripefile offsets as well as the PFS offset.
	 */
	if ((hdr_fdte->fmode & FAPPEND) &&
	    ((hdr_fdte->pfs_iomode_info == NULL) || 
	     (!PFS_TOKENMGT(hdr_fdte)))) {
		pfs_offset = pfs_length;
		for (sfd = 0; sfd < sfactor; sfd++) {
			sf_fdt[sfd].s_offset = sf_counts[sfd].offset =
				POFF_TO_SFOFF(pfs_offset, sfd,
					      sunitsize, sfactor);
		}
	} else {	/* just initialize sf_counts array */
		for (sfd = 0; sfd < sfactor; sfd++)
			sf_counts[sfd].offset = sf_fdt[sfd].s_offset;
	}

	/*
	 * Try to restrict the amount of data sent at once to any one I/O
	 * node, to avoid potential flooding of an I/O node from multiple
	 * compute nodes.  Currently only supported if the stripe unit size
	 * is less than MAX_IONODE_REQUEST.
	 */
	pfs_max_request = (sunitsize < MAX_IONODE_REQUEST) ? 
		(ROUND_DOWN(MAX_IONODE_REQUEST, sunitsize) * sfactor) :
		UINT_MAX;

	/*
	 * Allocating a gather buffer for stripe unit merging is necessary
	 * only if: 1) any one stripefile write operation will span more than
	 * a single stripe unit, and the file is striped more than one way, or
	 * 2) we're performing a writev, in which case we need to gather the
	 * data anyway.  In either case, allocate a gather buffer that is big
	 * enough to handle the maximum amount of data that might be written
	 * to a stripefile.
	 */
	PTUPLE_INIT(start, pfs_offset, sunitsize, sfactor);
	if ((sfactor > 1) &&
	    ((pfs_count + start.sunitoff) >
	     (MIN(pfs_max_request, SSIZ(sunitsize, sfactor)))) ||
	    (write_op == PFS_OP_WRITEV)) {
		mig_data_len = ((pfs_count/SSIZ(sunitsize, sfactor)) + 1)
				   * sunitsize;
		if (sunitsize < MAX_IONODE_REQUEST)
			mig_data_len = MIN(mig_data_len,
					   ROUND_DOWN(MAX_IONODE_REQUEST,
						      sunitsize));
		pfs_error = pfs_malloc((void *)&mig_data, mig_data_len);
		if (pfs_error) {
			PFS_DEBUG(("pfs_multi_write: pfs_malloc error=%d\n",
				   pfs_error));
			mig_data_len = 0;
			file_token_release(hdr_fdte, interrupt, write_op, 0,
					   &ex_neg_one, &ex_neg_one); 
			goto out;
		}
	}

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	/*
	 * Performance optimization: if we have to break the request into
	 * multiple "chunks" (if pfs_max_request < pfs_resid), round the 
	 * ending offset of the first chunk down to the nearest page for more
	 * efficient page-aligned IPC and disk I/O on the remaining chunks.
	 * Assumes pfs_max_request is a multiple of vm_page_size.
	 */
	if (pfs_max_request < pfs_resid)
		count = pfs_max_request - 
			(start.sunitoff - trunc_page(start.sunitoff));
	else
		count = pfs_resid;

	/*
	 * Loop through the user's data, writing chunks of size no greater
	 * than pfs_max_request.
	 */
	do {
		PTUPLE_INIT(end,
			    __eadd1(pfs_offset, (long)(pfs_actual + count),
				    &dont_care),
			    sunitsize,
			    sfactor);

		PFS_DEBUG(("pfs_multi_write: count=%lu pfs_max_request=%lu\n",
			   count, pfs_max_request));
		PFS_DEBUG(("                 pfs_actual=%lu pfs_resid=%lu\n",
			   pfs_actual, pfs_resid));
		pfs_error = multi_write(sf_fdt, start, end, reply_port,
					data, (off_t)pfs_actual, copy_func,
					mig_data, mig_data_len, sf_counts,
					pfs_length, sunitsize, sfactor,
					&extend_sfiles, &actual);
		if (pfs_error)
			break;

		pfs_actual += actual;
		pfs_resid -= actual;
		if (actual != count)
			break;

		start = end;
		count = MIN(pfs_max_request, pfs_resid);
	} while (pfs_resid);

	if (pfs_error) {
		/*
		 * Truncate stripefiles to their proper length, since in most
		 * error cases all but one of the stripefile writes will have
		 * succeeded.  This frees up disk space and keeps the
		 * stripefile lengths in sync.
		 */
		multi_ftruncate(interrupt, hdr_fdte, pfs_length);

		file_token_release(hdr_fdte, interrupt, write_op, 0,
				   &ex_neg_one, &ex_neg_one); 
		goto out;
	}

	if (pfs_actual < pfs_count) {
		/*
		 * Verify consistency of actual counts.
		 */
		PFS_DEBUG(("pfs_multi_write: pfs_cnt=%d pfs_act=%d\n",
			   pfs_count, pfs_actual));
		PFS_DEBUG(("                call pfs_sync_actuals\n"));
		pfs_sync_actuals(sf_counts, sf_fdt, sunitsize, sfactor,
				 &ex_temp);
		pfs_actual = ex_temp.slow;
		PFS_DEBUG(("pfs_multi_write: pfs_cnt=%d pfs_act=%d\n",
			   pfs_count, pfs_actual));
	}

	/*
	 * Update PFS file and stripefile offsets.
	 */
	pfs_offset = __eadd1(pfs_offset, (long)pfs_actual, &dont_care);
	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_fdt[sfd].s_offset += sf_counts[sfd].actual;
	}

	/*
	 * Update PFS file length.
	 */
	if (GREATER(pfs_offset, pfs_length))
		pfs_length = pfs_offset;

	/*
	 * Truncate stripefiles to their proper length if our actual is less
	 * than what was requested.  (For instance, the write operation may
	 * have been causing the PFS file to be extended, and one stripefile
	 * write may have run out of disk space, but the others may have
	 * succeeded.  So now we have to go back and truncate the others to
	 * free up disk space and keep the stripefile lengths in sync.)
	 *
	 * Or, use truncate on the stripefiles if one or more of them needs to
	 * be extended to match the new PFS file length.
	 */
	if (((pfs_actual < pfs_count) || (extend_sfiles)) &&
	    ((hdr_fdte->pfs_iomode != M_RECORD) && 
	     (hdr_fdte->pfs_iomode != M_ASYNC))) 
		multi_ftruncate(interrupt, hdr_fdte, pfs_length);

	*rval = pfs_actual;
	fdte_lock(hdr_fdte);
	hdr_fdte->modified = 1;	/* updates length on token release */
	fdte_unlock(hdr_fdte);

	file_token_release(hdr_fdte, interrupt, write_op,
			   pfs_actual, &pfs_offset, &pfs_length);

out:
	fdte_pfsio_unlock(pfs_fd);
	if (mig_data_len > 0)
		pfs_free((void *)mig_data, mig_data_len);
	if (sf_counts != sf_count_array)    /* a bigger array was malloc'd */
		pfs_free((void *)sf_counts,
			 (sfactor * sizeof(struct sfile_count)));

	PFS_DEBUG(("pfs_multi_write: ret error=%d pfs_off=%d,%d pfs_act=%d\n",
		   pfs_error, pfs_fd->p_offset.shigh, pfs_fd->p_offset.slow,
		   pfs_actual));
	return(pfs_error);
}


/*
 * Name:
 *	multi_write
 *
 * Description:	
 *	This function performs concurrent Fast Path write operations to one or
 *	more stripefiles to satisfy a write system call on a PFS file.
 *
 *	Stripe unit merging is done so that at most one request is sent to
 *	each stripefile.  Eventually, we might want to add optimizations for 
 *	requests of size <= 1 stripe (it often would be more efficient to loop
 *	through stripe units on these requests, rather than looping through
 *	stripefiles).  However, in most cases any speedup achieved from doing
 *	this would probably be negligible, and not worth the additional
 *	complexity.
 *
 *	Note that this function attempts to employ VM optimizations
 *	(vm_copy() of page-aligned data instead of user_bcopy2()) when
 *	copying data.
 *
 * Parameters:
 *	sf_fdt		Array of stripefile descriptors for the PFS file.
 *
 *	start		(snum, sfile, sunitoff) tuple describing the offset
 *			in the PFS file at which to start writing, in terms of
 *			the stripe number, stripefile, and offset in the first
 *			stripe unit.
 *
 *	end		tuple describing the PFS offset corresponding to the
 *			end of the data to be written.
 *
 *	reply_port	Per-thread port at which IPC responses are received.
 *
 *	data		Pointer to user's buffer area, either a simple buffer
 *			or an array of iovec structures.
 *
 *	data_offset	Offset into user data from which we are currently
 *			writing.
 *
 *	copy_func	Function to call to copy data from the user's buffer
 *			area, if necessary.
 *
 *	mig_data	Pointer to user data sent in the IPC.  If stripe unit
 *			merging is being done, this must point to a gather 
 *			buffer big enough to copy the max amount of data 
 *			destined for any one stripefile.
 *
 *	mig_data_len	Size of the buffer pointed to by mig_data; 0 if no
 *			mig_data buffer	has been allocated (which implies that
 *			stripe unit merging is not necessary).
 *
 *	sf_counts	Points to temporary array for tracking running 
 *			stripefile requested/actual counts, etc.
 *
 *	pfs_length	Length of the PFS file before the write operation was
 *			initiated by the user.
 *
 *	sunitsize	Stripe unit size, in bytes.
 *
 *	sfactor		Stripe factor.
 *
 *	extend_sfiles	In/out parameter that points to a Boolean value that
 *			is TRUE if the write operation in progress has created
 *			a sparse PFS file, in which case stripefiles need to be
 *			extended in order to keep their lengths	in sync, and
 *			FALSE otherwise.  This function sets this value if it
 *			is not already TRUE; it does not change its state if
 *			it is already TRUE.
 *
 *	actual		The returned number of bytes actually written if no
 *			error was encountered.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_write(sf_fdt, start, end, reply_port, data, data_offset, copy_func,
	    mig_data, mig_data_len, sf_counts, pfs_length, sunitsize, sfactor,
	    extend_sfiles, actual)
	stripe_fd_t	*sf_fdt;
	ptuple_t	start;
	ptuple_t	end;
	mach_port_t	reply_port;
	caddr_t		data;
	off_t		data_offset;
	int		(*copy_func)();
	char		*mig_data;
	vm_size_t	mig_data_len;
	sfile_count_t	*sf_counts;
	esize_t		pfs_length;
	size_t		sunitsize;
	uint_t		sfactor;
	boolean_t	*extend_sfiles;	/* IN/OUT */
	size_t		*actual;	/* OUT    */
{
	size_t		sf_count;	/* # bytes stripefile data to write */
	size_t		sf_length;	/* current length of stripefile */
	size_t		sf_actual;	/* actual count of stripefile data */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	register int	pfs_error;	/* running error returned to caller */
	size_t		total_actual = 0;
	boolean_t	sparse_file = *extend_sfiles;
	uint_t		num_sent = 0;

	PFS_TRACE(("  multi_write: start snum=%d, sfile=%d, sunitoff=%d\n",
		   start.snum, start.sfile, start.sunitoff));
	PFS_TRACE(("               end   snum=%d, sfile=%d, sunitoff=%d\n",
		   end.snum, end.sfile, end.sunitoff));

	/*
	 * For each stripefile that contains stripe units that fall into the
	 * target range of data, gather stripe data from the user's buffer area
	 * and issue one Fast Path write.
	 */
	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_count = PTUPLE_TO_SFOFF(end.snum, end.sfile, end.sunitoff,
					   sfd, sunitsize)
			- sf_counts[sfd].offset;
		sf_counts[sfd].requested += sf_count;

		if ((int)sf_count <= 0) {
			/*
			 * Check for case where user has seeked past the
			 * end-of-file, in which case the stripefile length may
			 * need to be set consistent with the new PFS file
			 * length even though we're not writing to it.
			 * (Current PFS design requires that stripefile lengths
			 * add up to the PFS file length at all times.)
			 */
			if (!(sparse_file)) {
				sf_length = POFF_TO_SFOFF(pfs_length, sfd,
							  sunitsize, sfactor);
				if (sf_fdt[sfd].s_offset > sf_length) {
					sparse_file = TRUE;
					PFS_DEBUG(("  multi_write: stripefile %d length is %d, will set to %d\n",
						   sfd, sf_length, 
						   sf_fdt[sfd].s_offset));
				}
			}
			continue;
		}
#ifdef	DEBUG_PFS
		if ((mig_data_len > 0) && (sf_count > mig_data_len)) {
			PFS_DEBUG(("  multi_write: %d byte MiG buf\n",
				   mig_data_len));
			PFS_DEBUG(("               too small for %d bytes\n",
				   sf_count));
			emul_panic("  multi_write: bad MiG buffer\n");
		}
#endif
		EASSERT((mig_data_len > 0) ?
			(sf_count <= mig_data_len) : TRUE);

		/*
		 * If we're merging stripe units, gather the stripe unit data
		 * from the caller's buffer area into the MiG gather buffer.
		 * Otherwise, the data for this stripefile is contiguous and
		 * can be pulled directly from the caller's buffer area.
		 */
		if (mig_data_len > 0) {
			pfs_error = (*copy_func)(
				data,
				data_offset + PTUPLE_TO_DOFF(start.snum,
							     start.sfile,
							     start.sunitoff,
							     sfd, sunitsize,
							     sfactor),
				mig_data,
				MIN(sunitsize - SFOFF_TO_SUOFF(
							sf_counts[sfd].offset,
							sunitsize),
				    sf_count),
				FROM_USER,
				sf_count,
				sunitsize,
				sfactor);
			if (pfs_error) {
				sf_counts[sfd].requested = 0;
				break;
			}
		} else {
			mig_data = data + data_offset + 
				PTUPLE_TO_DOFF(start.snum, start.sfile,
					       start.sunitoff, sfd,
					       sunitsize, sfactor);
		}

		PFS_DEBUG(("  multi_write: SEND sfd=%d sf_off=%d sf_cnt=%d\n",
			   sfd, sf_counts[sfd].offset, sf_count));
		pfs_error = fsvr_write_at_offset_msg_send(
						sf_fdt[sfd].s_fp, reply_port,
						credentials_port, &sfd,
						sf_counts[sfd].offset,
						mig_data, sf_count,
						&sf_actual);
		if (pfs_error) {
			sf_counts[sfd].requested = 0;
			break;
		}
		num_sent++;
	}

	/*
	 * Receive the responses to the Fast Path write requests.
	 */
	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_write_at_offset_msg_receive(
						MACH_PORT_NULL, reply_port,
						credentials_port, &sfd,
						0,
						mig_data, sf_count,
						&sf_actual);
		PFS_DEBUG_RECV(("  multi_write: RECEIVE sf_error=%d",sf_error),
			       (" sfd=%d sf_actual=%d\n", sfd, sf_actual),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_write: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}

		sf_counts[sfd].actual += sf_actual;
		sf_counts[sfd].offset += sf_actual;
		total_actual += sf_actual;
	}

	if (sparse_file)
		*extend_sfiles = TRUE;
	*actual = total_actual;

	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_stat
 *
 * Description:	
 *	Given a stat or estat struct containing statistics on a PFS header 
 *	file, this function performs stat operations on the stripefiles 
 *	associated with the PFS file.  The stat struct is then updated with 
 *	the total size of the logical PFS file, as well as the proper 
 *	timestamps, block size, and number of blocks used by the file.
 *	The block size returned is the minimum file block size of the
 *	stripefiles (typically the block size of each stripefile will be 
 *	identical, if the file systems in which they reside have been
 *	configured optimally).
 *
 *	Note that, as an optimization, this function does not acquire/release
 *	the PFS token.  If the caller does not already hold the token (which
 *	is the case if an stat() or estat() system call is being handled),
 *	it is assumed that only a snapshot of the current file statistics is 
 *	desired.  E.g. if the caller does not already hold the token, it's
 *	possible that the stripefile lengths we get will not be "consistent" 
 *	since the stripefile stat operations are not performed atomically.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the stat() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	statbuf		Pointer to either a stat struct or an estat struct, as
 *			indicated by the extended_flag parameter.
 *
 *	follow		TRUE if following symbolic links.
 *
 *	extended_flag	TRUE if the given statbuf is for an extended stat
 *			structure (estat), else the statbuf is filled with
 *			a regular OSF/1 stat structure.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_stat(interrupt, hdr_path, len_hdr_path, statbuf, follow, 
	       extended_flag)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;	/* PFS header file name */
	int		len_hdr_path;
	caddr_t		statbuf;
	boolean_t	follow;
	boolean_t	extended_flag;
{
	struct statpfs	*stripe_attr = NULL;
	register int	error;

	PFS_TRACE(("pfs_multi_stat: hdr_path=%s, len_hdr_path=%d\n", 
		   hdr_path, len_hdr_path));

	/*
	 * Get the stripe attributes for this PFS file.
	 */
	error = get_stripe_attributes(hdr_path, len_hdr_path,
				      (fdt_entry_t *)NULL, 0, &stripe_attr);
	if (error) {
		PFS_DEBUG(("pfs_multi_stat: get_stripe_attributes ret %d\n",
			   error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_stat: BAD stripe attributes\n"));
		error = ENOTPFS;
		goto out;
	}

	PFS_DEBUG_SATTR(("pfs_multi_stat: received stripe attributes:\n"),
			 stripe_attr);

	error = multi_stat(interrupt, stripe_attr, (caddr_t)statbuf, follow,
			   extended_flag, FALSE);

out:
	if (stripe_attr)
		pfs_free((void *)stripe_attr, 
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
#ifdef	DEBUG_PFS
	PFS_DEBUG(("pfs_multi_stat: returning error=%d\n", error));
#endif
	return(error);
}


/*
 * Name:
 *	multi_stat
 *
 * Description:	
 *	Given the stripe attributes of a PFS file, perform concurrent stat
 *	operations on the pathnames referenced in the stripe attributes.
 *	Then update the stat struct with the total size of the logical PFS
 *	file, as well as the proper timestamps, block size, and number of
 *	blocks used by the file. The block size returned is the minimum file
 *	block size of the stripefiles (typically the block size of each
 *	stripefile will be identical, if the file systems in which they reside
 *	have been configured optimally).
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	stripe_attr	Pointer to the statpfs structure describing the stripe
 *			attributes of the PFS file.
 *
 *	statbuf		Pointer to either a stat struct or an estat struct, as
 *			indicated by the extended_flag parameter.
 *
 *	follow		TRUE if following symbolic links.
 *
 *	extended_flag	TRUE if the given statbuf is for an extended stat
 *			structure (estat), else the statbuf is filled with
 *			a regular OSF/1 stat structure.
 *
 *	isdir_flag	TRUE if the stripe attributes reference stripe 
 *			directories (as would be the case if they are PFS 
 *			mount attributes) rather than stripefiles (as would be
 *			the case if they are attributes for a specific file).
 *			If TRUE, a check is made to ensure that all files
 *			stat'ed are indeed directories.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_stat(interrupt, stripe_attr, statbuf, follow, extended_flag, isdir_flag)
	boolean_t	*interrupt;	/* out */
	struct statpfs	*stripe_attr;
	caddr_t		statbuf;
	boolean_t	follow;
	boolean_t	extended_flag;
	boolean_t	isdir_flag;
{
	register uint_t	sfactor;	/* stripe factor */
	pathname_t	*sf_path;	/* stripefile pathname */

	int	pfs_error;		/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	mach_port_t     dir_port;       /* Vnode port to send stat message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	struct estat	*pfs_estat = (struct estat *)statbuf;
	struct stat	*pfs_stat  = (struct stat *)statbuf;
	struct stat	sf_stat;	/* stripefile stat struct */

	PFS_TRACE(("  multi_stat: stripe_attr=0x%x extended_flag=0x%x\n", 
		   stripe_attr, extended_flag));

	/*
	 * Clear the appropriate fields in the stat buf, which currently
	 * contains info pertaining only to the PFS header file.
	 */
	if (extended_flag) {
		pfs_estat->st_size.shigh = 0;
		pfs_estat->st_size.slow = 0;
		pfs_estat->st_blksize = INT_MAX;
		pfs_estat->st_blocks = 0;
		pfs_estat->st_atime = 0;
		pfs_estat->st_mtime = 0;
		pfs_estat->st_ctime = 0;
	} else {
		pfs_stat->st_size = 0;
		pfs_stat->st_blksize = INT_MAX;
		pfs_stat->st_blocks = 0;
		pfs_stat->st_atime = 0;
		pfs_stat->st_mtime = 0;
		pfs_stat->st_ctime = 0;
	}

	/*
	 * Stat all the stripefiles (or stripedirs).  Note that the &sf_stat
	 * parameter in the send message is really a dummy argument since this
	 * is a MiG 'out' parameter ... the sf_stat struct is not filled in 
	 * until the receive is done.
	 */
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("  multi_stat: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("multi_stat: dir port invalid!\n"));
		    break;
		}
		pfs_error = fsvr_pfs_stat_msg_send(dir_port,
						   reply_port,
						   credentials_port,
						   trans_id++, rootdir_port,
						   &sfd,
						   mod_path,
						   mod_len + 1,
						   follow, &sf_stat);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_stat_msg_receive(MACH_PORT_NULL,
						     reply_port,
						     credentials_port,
						     0, MACH_PORT_NULL,
						     &sfd,
						     NULL,
						     0,
						     0, &sf_stat);
		PFS_DEBUG_RECV(("  multi_stat: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d, st_size=%d\n", sfd, sf_stat.st_size),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_stat: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}

		if ((isdir_flag) && (!S_ISDIR(sf_stat.st_mode))) {
			pfs_error = ENOTDIR;
			continue;
		}

		if (extended_flag) {
			pfs_estat->st_size = __eadd1(pfs_estat->st_size,
						     (long)sf_stat.st_size,
						     &pfs_error);
			pfs_estat->st_atime = MAX(pfs_estat->st_atime,
						  sf_stat.st_atime);
			pfs_estat->st_mtime = MAX(pfs_estat->st_mtime,
						  sf_stat.st_mtime);
			pfs_estat->st_ctime = MAX(pfs_estat->st_ctime,
						  sf_stat.st_ctime);
			pfs_estat->st_blksize = MIN(pfs_estat->st_blksize,
						    sf_stat.st_blksize);
			pfs_estat->st_blocks += sf_stat.st_blocks;
		} else {
			pfs_stat->st_size += sf_stat.st_size;
			/*
			 * Test that target file is not an extended file.
			 */
			if ((long)pfs_stat->st_size < 0) {
				pfs_error = EFBIG;
				continue;
			}
			pfs_stat->st_atime = MAX(pfs_stat->st_atime,
						 sf_stat.st_atime);
			pfs_stat->st_mtime = MAX(pfs_stat->st_mtime,
						 sf_stat.st_mtime);
			pfs_stat->st_ctime = MAX(pfs_stat->st_ctime,
						 sf_stat.st_ctime);
			pfs_stat->st_blksize = MIN(pfs_stat->st_blksize,
						   sf_stat.st_blksize);
			pfs_stat->st_blocks += sf_stat.st_blocks;
		}
	}
	isc_deregister(interrupt);

out:
#ifdef	DEBUG_PFS
	PFS_DEBUG(("  multi_stat: returning pfs_error=%d\n", pfs_error));
	if (extended_flag)
		PFS_DEBUG(("  multi_stat: st_size=%d,%d st_blksize=%d st_blocks=%d\n",
			   pfs_estat->st_size.shigh, pfs_estat->st_size.slow,
			   pfs_estat->st_blksize, pfs_estat->st_blocks));
	else
		PFS_DEBUG(("  multi_stat: st_size=%d, st_blksize=%d st_blocks=%d\n",
			   pfs_stat->st_size, pfs_stat->st_blksize,
			   pfs_stat->st_blocks));
#endif
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_fstat
 *
 * Description:	
 *	Given a stat or estat struct containing statistics on a PFS header 
 *	file, this function performs fstat operations on the stripefiles 
 *	associated with the PFS file.  The stat struct is then updated with 
 *	the total size of the logical PFS file, as well as the proper 
 *	timestamps, block size, and number of blocks used by the file.
 *	The block size returned is the minimum file block size used in the
 *	stripefiles (typically the block size of each stripefile will be 
 *	identical, if the file systems in which they reside have been
 *	configured optimally).
 *
 *	Note that, as an optimization, this function does not acquire/release
 *	the PFS token.  If the caller does not already hold the token (which
 *	is the case if an fstat() or festat() system call is being handled),
 *	it is assumed that only a snapshot of the current file statistics is 
 *	desired.  E.g. if the caller does not already hold the token, it's
 *	possible that the stripefile lengths we get will not be "consistent" 
 *	since the stripefile fstat operations are not performed atomically.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	statbuf		Pointer to either a stat struct or an estat struct, as
 *			indicated by the extended_flag parameter.
 *
 *	extended_flag	TRUE if the given statbuf is for an extended stat
 *			structure (estat), else the statbuf is filled with
 *			a regular OSF/1 stat structure.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_fstat(interrupt, hdr_fdte, statbuf, extended_flag)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	caddr_t		statbuf;
	boolean_t	extended_flag;
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	int		pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	struct estat	*pfs_estat = (struct estat *)statbuf;
	struct stat	*pfs_stat  = (struct stat *)statbuf;
	struct stat	sf_stat;	/* stripefile stat struct */

	PFS_TRACE(("pfs_multi_fstat: hdr_fdte=0x%x\n", hdr_fdte));

	/*
	 * Clear the appropriate fields in the stat buf, which currently
	 * contains info pertaining only to the PFS header file.
	 */
	if (extended_flag) {
		pfs_estat->st_size.shigh = 0;
		pfs_estat->st_size.slow = 0;
		pfs_estat->st_blksize = INT_MAX;
		pfs_estat->st_blocks = 0;
	} else {
		pfs_stat->st_size = 0;
		pfs_stat->st_blksize = INT_MAX;
		pfs_stat->st_blocks = 0;
	}

	/*
	 * Fstat all the stripefiles.  Note that the &sf_stat parameter in the
	 * send message is really a dummy argument since this is a MiG 'out'
	 * parameter ... the sf_stat struct is not filled in until the receive
	 * is done.
	 */
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register_chk_async(rootdir_port, sfactor, pfs_fd, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_fstat: SEND sfd=%d\n", sfd));
		pfs_error = fsvr_pfs_fstat_msg_send(sf_fdt[sfd].s_fp,
						    reply_port,
						    credentials_port,
						    trans_id++, &sfd,
						    &sf_stat);
		if (pfs_error)
			break;

		num_sent++;
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_fstat_msg_receive(MACH_PORT_NULL,
						      reply_port,
						      credentials_port,
						      0, &sfd,
						      &sf_stat);
		PFS_DEBUG_RECV(("pfs_multi_fstat: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d, st_size=%d\n", sfd, sf_stat.st_size),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_fstat: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}

		if (extended_flag) {
			pfs_estat->st_size = __eadd1(pfs_estat->st_size,
						     (long)sf_stat.st_size,
						     &pfs_error);
			pfs_estat->st_atime = MAX(pfs_estat->st_atime,
						  sf_stat.st_atime);
			pfs_estat->st_mtime = MAX(pfs_estat->st_mtime,
						  sf_stat.st_mtime);
			pfs_estat->st_ctime = MAX(pfs_estat->st_ctime,
						  sf_stat.st_ctime);
			pfs_estat->st_blksize = MIN(pfs_estat->st_blksize,
						    sf_stat.st_blksize);
			pfs_estat->st_blocks += sf_stat.st_blocks;
		} else {
			pfs_stat->st_size += sf_stat.st_size;
			/*
			 * Test that target file is not an extended file.
			 */
			if ((long)pfs_stat->st_size < 0) {
				pfs_error = EFBIG;
				continue;
			}
			pfs_stat->st_atime = MAX(pfs_stat->st_atime,
						 sf_stat.st_atime);
			pfs_stat->st_mtime = MAX(pfs_stat->st_mtime,
						 sf_stat.st_mtime);
			pfs_stat->st_ctime = MAX(pfs_stat->st_ctime,
						 sf_stat.st_ctime);
			pfs_stat->st_blksize = MIN(pfs_stat->st_blksize,
						   sf_stat.st_blksize);
			pfs_stat->st_blocks += sf_stat.st_blocks;
		}
	}
	isc_deregister(interrupt);

out:
#ifdef	DEBUG_PFS
	PFS_DEBUG(("pfs_multi_fstat: returning pfs_error=%d\n", pfs_error));
	if (extended_flag)
		PFS_DEBUG(("pfs_multi_fstat: st_size=%d,%d st_blksize=%d st_blocks=%d\n",
			   pfs_estat->st_size.shigh, pfs_estat->st_size.slow,
			   pfs_estat->st_blksize, pfs_estat->st_blocks));
	else
		PFS_DEBUG(("pfs_multi_fstat: st_size=%d, st_blksize=%d st_blocks=%d\n",
			   pfs_stat->st_size, pfs_stat->st_blksize,
			   pfs_stat->st_blocks));
#endif
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_lseek
 *
 * Description:	
 *	Perform concurrent lseek operations on the stripefiles associated
 *	with a PFS file.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	offset		Offset specified in the lseek()/eseek() system call
 *			(must be an extended number).
 *
 *	whence		"Whence" parameter in lseek()/eseek() system call.
 *
 *	extended_flag	TRUE if the operation is allowed to seek to a logical
 *			PFS offset greater than 2GB-1 (i.e. TRUE if the caller
 *			is doing an eseek rather than an lseek).
 *
 *	ret_offset	Pointer to the returned new file offset if successful.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_lseek(interrupt, hdr_fdte, offset, whence, extended_flag, ret_offset)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	esize_t		offset;
	int		whence;
	boolean_t	extended_flag;
	esize_t		*ret_offset;	/* out */
{
	pfs_fd_t	*pfs_fd = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */
	int		error;
	
	PFS_TRACE(("pfs_multi_lseek: hdr_fdte=0x%x offset=%d,%d whence=%d\n",
		   hdr_fdte, offset.shigh, offset.slow, whence));

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Make sure we have the PFS token for this file.  This updates us
	 * with the latest PFS file offset and length.
	 */
	error = file_token_acquire(hdr_fdte, interrupt, PFS_OP_LSEEK, 0,
				   &pfs_offset, &pfs_length);
	PFS_TRACE(("pfs_multi_lseek: tok offset=%d,%d length=%d,%d error=%d\n",
		   pfs_offset.shigh, pfs_offset.slow, 
		   pfs_length.shigh, pfs_length.slow, error));
	if (error)
		goto out;

	switch (whence) {

	case L_INCR:
		pfs_offset = __eadd(pfs_offset, offset, &error);
		break;

	case L_XTND:
		pfs_offset = __eadd(pfs_length, offset, &error);
		break;

	case L_SET:
		pfs_offset = offset;
		break;

	default:	
		pfs_offset = ex_neg_one;
		error = EINVAL;
		/* fall through */
	}

	/*
	 * Ensure that the new offset is not negative.
	 */
	if ((!error) && (LESS(pfs_offset, ex_zero))) {
		pfs_offset = ex_neg_one; 	/* offset won't be changed */
		error = EINVAL;
	}

	/*
	 * If the caller is performing an eseek operation, ensure that the
	 * new PFS file offset will not overflow 32-bit stripefile parameters.
	 *
	 * Otherwise, if the caller is performing an lseek operation, ensure
	 * that the new file offset is not too big to return as a long.
	 */
	if (!error) {
		if (extended_flag) {
			if (ETOOBIG(pfs_offset, sunitsize, sfactor)) {
				pfs_offset = ex_neg_one;
				error = EINVAL;
			}
		} else if (GREATER(pfs_offset, ex_int_max)) {
			pfs_offset = ex_neg_one;	/* won't be changed */
			error = EINVAL;
		}
	}

	if (!error) {
		PFS_DEBUG(("pfs_multi_lseek: setting stripefile offsets\n"));
		error = pfs_set_stripefile_offsets(hdr_fdte, pfs_offset);
		if (error)
			pfs_offset = ex_neg_one;
		PFS_DEBUG(("pfs_multi_lseek: pfs_offset=%d,%d, error=%d\n",
			   pfs_fd->p_offset.shigh, pfs_fd->p_offset.slow,
			   error));
	}

	file_token_release(hdr_fdte, interrupt, PFS_OP_LSEEK, 0,
			   &pfs_offset, &ex_neg_one);
	/* if error, ret_offset will be -1,-1 which is what we want */
	*ret_offset = pfs_offset;
out:
	fdte_pfsio_unlock(pfs_fd);
	PFS_DEBUG(("pfs_multi_lseek: returning, pfs_offset=%d,%d, error=%d\n",
		   pfs_fd->p_offset.shigh, pfs_fd->p_offset.slow, error));
	return(error);
}


/*
 * Name:
 *	pfs_multi_unlink
 *
 * Description:	
 *	This function unlinks the stripefiles associated with the given 
 *	PFS file, and then unlinks the PFS file header.  On entry to this
 *	function, the contents of the header file must of course be intact
 *	so that the stripefile pathnames can be determined.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the unlink() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_unlink(interrupt, hdr_path, len_hdr_path)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;
	int		len_hdr_path;
{
	struct statpfs	*stripe_attr = NULL;
	int		error;

	PFS_TRACE(("pfs_multi_unlink: hdr_path=%s\n", hdr_path));

	error = get_stripe_attributes(hdr_path, len_hdr_path,
				      (fdt_entry_t *)NULL, O_RDONLY,
				      &stripe_attr);
	if (error) {
		PFS_DEBUG(("pfs_multi_unlink: get_stripe_attributes ret %d\n",
			   error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_unlink: BAD stripe attributes\n"));
		error = ENOTPFS;
		goto out;
	}
	PFS_DEBUG_SATTR(("pfs_multi_unlink: header contents: \n"),
			stripe_attr);

	error = multi_unlink(interrupt, hdr_path, len_hdr_path, stripe_attr,
			     TRUE);

out:
	if (error) {
		if ((error == ENOTPFS) || (error == ENOSDIR)) {
			/*
			 * If this is an invalid PFS file, blow the header file
			 * away anyway (e.g. this gives users some way to clean
			 * up orphan PFS header files).
			 */
			error = ESUCCESS;
			unlink_hdr_file(interrupt, hdr_path, len_hdr_path,
					(ulong_t)VIO_PFS);
		} else if (error == EACCES) {
			/*
			 * The user does not have permission to unlink at
			 * least one of the stripefiles, even though the
			 * user does have permission to unlink the header
			 * file.  Remove the header file but report the
			 * permission problem:
			 */
			error = ENOSDIR;
			unlink_hdr_file(interrupt, hdr_path, len_hdr_path,
					(ulong_t)VIO_PFS);
		} else {
			/*
			 * Use VIO_ERROR to flag the server to clean up state
			 * (the "marked for deletion" flag in the vnode for
			 * the PFS header file must be cleared).
			 */
			unlink_hdr_file(interrupt, hdr_path, len_hdr_path,
					VIO_ERROR);
		}
	}

	if (stripe_attr)
		pfs_free((void *)stripe_attr,
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
	return(error);
}


/*
 * Name:
 *	multi_unlink
 *
 * Description:	
 *	Given the stripe attributes of a PFS file, unlink the stripefiles
 *	associated with the PFS file, and then unlink the PFS file header
 *	if flagged to do so.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the unlink() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	stripe_attr	Pointer to the statpfs structure describing the stripe
 *			attributes of the PFS file.
 *
 *	remove_hdr	TRUE if the PFS header file is to be removed.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_unlink(interrupt, hdr_path, len_hdr_path, stripe_attr, remove_hdr)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;
	int		len_hdr_path;
	struct statpfs	*stripe_attr;
	boolean_t	remove_hdr;
{
	register uint_t	sfactor  = stripe_attr->p_sfactor;
	pathname_t	*sf_path = &stripe_attr->p_sdirs;

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	PFS_DEBUG(("  multi_unlink: hdr_path=%s stripe_attr=0x%x remove_hdr=%d\n",
		   hdr_path, stripe_attr, remove_hdr));

	/*
	 * Unlink all the stripefiles.
	 */		
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		if (*sf_path->name == '\0') {
			/* this is a valid state: we're changing stripe attr */
			PFS_DEBUG(("  multi_unlink: not unlinking sfd %d\n",
				   sfd));
			sf_path = NEXTPATH(sf_path);
			continue;
		}
		PFS_DEBUG(("  multi_unlink: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("multi_unlink: dir port invalid!\n"));
		    break;
		}

		pfs_error = fsvr_pfs_unlink_msg_send(dir_port,
						     reply_port,
						     credentials_port,
						     trans_id++, rootdir_port,
						     &sfd,
						     mod_path, 
						     mod_len + 1);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_unlink_msg_receive(MACH_PORT_NULL,
						       reply_port,
						       credentials_port,
						       0, MACH_PORT_NULL,
						       &sfd,
						       NULL,
						       0);
		PFS_DEBUG_RECV(("  multi_unlink: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);
		if ((pfs_error) && (pfs_error != EACCES))
			/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_unlink: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);

	if (remove_hdr) {
		/*
		 * Now unlink the header file, which still remains.  Flag to 
		 * unlink() via VIO_PFS that it's now OK to remove the header
		 * file.
		 */
		sf_error = unlink_hdr_file(interrupt, hdr_path, len_hdr_path,
					   (ulong_t)VIO_PFS);
		if (!pfs_error)
			pfs_error = sf_error;
	}

out:
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_rename
 *
 * Description:	
 *	This function is called if the destination file in a rename operation
 *	exists and is a PFS file.  In this case, it is necessary to unlink
 *	the stripefiles associated with the destination PFS file.  This is
 *	not done until *after* the rename is done, in case an error is 
 *	returned from the rename.
 *
 * Parameters:
 *	interrupt		Pointer to interrupt Boolean, initially set to
 *				FALSE.  On return from isc_deregister(),
 *				indicates whether an interrupt occurred during
 *				the operation.
 *
 *	from_hdr_path		Source pathname specified by the rename() 
 *				system call.
 *
 *	len_from_hdr_path	Length, in bytes, of the source header file 
 *				pathname.
 *
 *	to_hdr_path		Destination pathname specified by the rename() 
 *				system call.
 *
 *	len_to_hdr_path		Length, in bytes, of the destination header 
 *				file pathname.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_rename(interrupt, from_hdr_path, len_from_hdr_path,
		 to_hdr_path, len_to_hdr_path)
	boolean_t	*interrupt;	/* out */
	char		*from_hdr_path;
	int		len_from_hdr_path;
	char		*to_hdr_path;
	int		len_to_hdr_path;
{
	struct statpfs	*stripe_attr = NULL;
	mach_port_t	from_start_port;
	mach_port_t	to_start_port;
	transaction_id_t trans_id;
	ulong_t		iomode = VIO_PFS;
	int		error;

	PFS_TRACE(("pfs_multi_rename: from_hdr_path=%s to_hdr_path=%s\n",
		   from_hdr_path, to_hdr_path));

	error = get_stripe_attributes(to_hdr_path, len_to_hdr_path,
				      (fdt_entry_t *)NULL, O_RDONLY,
				      &stripe_attr);
	if (error) {
		PFS_DEBUG(("pfs_multi_rename: get_stripe_attributes ret %d\n",
			   error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_rename: BAD stripe attributes\n"));
		error = ENOTPFS;
		goto out;
	}
	PFS_DEBUG_SATTR(("pfs_multi_rename: header contents: \n"),
			stripe_attr);

	/*
	 * At this point, we have the stripe attributes of the target PFS
	 * file, so perform the rename.
	 */
	from_start_port =
		(*from_hdr_path == '/') ? rootdir_port : currentdir_port;
	to_start_port =
		(*to_hdr_path == '/') ? rootdir_port : currentdir_port;

	isc_register(from_start_port, &trans_id);
	error = fsvr_rename(from_start_port, credentials_port, trans_id,
			    rootdir_port, from_hdr_path, len_from_hdr_path + 1,
			    to_start_port, to_hdr_path, len_to_hdr_path + 1,
			    &iomode);
	isc_deregister(interrupt);
	if (error)
		goto out;

	/*
	 * Now unlink just the old target stripefiles.
	 */
	multi_unlink(interrupt, to_hdr_path, len_to_hdr_path, stripe_attr,
		     FALSE);

out:
	if (stripe_attr)
		pfs_free((void *)stripe_attr,
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
	return(error);
}


/*
 * Name:
 *	pfs_multi_access
 *
 * Description:	
 *	This function performs concurrent access operations on the stripefiles 
 *	associated with the given PFS file.  If any of the access operations
 *	fail, this function returns a failure indication.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the access() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	fmode		The access mode specified in the access() system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_access(interrupt, hdr_path, len_hdr_path, fmode)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;	/* PFS header file name */
	int		len_hdr_path;
	int		fmode;
{
	struct statpfs	*stripe_attr = NULL;
	int		error;

	PFS_TRACE(("pfs_multi_access: hdr_path=%s len_hdr_path=%d fmode=%o\n", 
		   hdr_path, len_hdr_path, fmode));

	/*
	 * Get the stripe attributes for this PFS file.
	 */
	error = get_stripe_attributes(hdr_path, len_hdr_path,
				      (fdt_entry_t *)NULL, 0, &stripe_attr);
	if (error) {
		PFS_DEBUG(("pfs_multi_access: get_stripe_attributes ret %d\n",
			   error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_access: BAD stripe attributes\n"));
		error = ENOTPFS;
		goto out;
	}

	PFS_DEBUG_SATTR(("pfs_multi_access: received stripe attributes:\n"),
			stripe_attr);

	error = multi_access(interrupt, stripe_attr, fmode);

out:
	if (stripe_attr)
		pfs_free((void *)stripe_attr,
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
	PFS_DEBUG(("pfs_multi_access: returning %d\n", error));
	return(error);
}


/*
 * Name:
 *	multi_access
 *
 * Description:	
 *	Given the stripe attributes of a PFS file, perform concurrent access
 *	operations on the pathnames referenced in the stripe attributes.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	stripe_attr	Pointer to the statpfs structure describing the stripe
 *			attributes of the PFS file.
 *
 *	fmode		Specifies the type of access to check, as in access().
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_access(interrupt, stripe_attr, fmode)
	boolean_t	*interrupt;	/* out */
	struct statpfs	*stripe_attr;
	int		fmode;
{
	register uint_t	sfactor  = stripe_attr->p_sfactor;
	pathname_t	*sf_path = &stripe_attr->p_sdirs;

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	PFS_DEBUG(("  multi_access: stripe_attr=0x%x, fmode=%o\n",
		   stripe_attr, fmode));

	/*
	 * Perform an access operation on all the stripe pathnames.
	 */
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("  multi_access: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("multi_access: dir port invalid!\n"));
		    break;
		}

		pfs_error = fsvr_pfs_access_msg_send(dir_port,
						     reply_port,
						     credentials_port,
						     trans_id++, rootdir_port,
						     &sfd,
						     mod_path, 
						     mod_len + 1,
						     fmode);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_access_msg_receive(MACH_PORT_NULL,
						       reply_port,
						       credentials_port,
						       0, MACH_PORT_NULL,
						       &sfd,
						       NULL,
						       0,
						       0);

		PFS_DEBUG_RECV(("  multi_access: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_access: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);
out:
	PFS_DEBUG(("  multi_access: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_truncate
 *
 * Description:	
 *	This function performs concurrent truncate operations on the
 *	stripefiles associated with the given PFS file.  If any of the
 *	truncate operations fail, this function returns a failure indication.
 *	However, note that this function cannot back out of asynchronous 
 *	stripefile truncate operations that have already completed before an 
 *	error is encountered.
 *
 *	PFS file truncation requires the PFS file token to be held, so that
 * 	the truncation can be performed atomically across all stripefiles.
 *	However, the emulator file token interfaces require that the file be
 *	open, with corresponding entries in the emulator and server file
 *	tables.  For this reason, it is necessary to perform a complete open
 *	operation on the PFS file (including the stripefiles) so that the
 *	token can be obtained.
 *
 *	(This might be remedied in the future by performing the PFS truncate
 *	from the server owning the header file.  In this case, the PFS token
 *	could be obtained on the server side without having to open the file.)
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the truncate() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	length		The new length of the logical PFS file, as specified
 *			in the truncate() system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_truncate(interrupt, hdr_path, len_hdr_path, length)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;	/* PFS header file name */
	int		len_hdr_path;
	size_t		length;		/* new length of PFS file */
{
	register int	close_error;
	register int	error;
	fdt_entry_t	*hdr_fdte;	/* fdte for PFS header file */
	mach_port_t	fp;

	int		hdr_mode = O_RDWR;


	PFS_TRACE(("pfs_multi_truncate: path=%s len_path=%d length=%d\n", 
		   hdr_path, len_hdr_path, length));

	/*
	 * Allocate an fdte to be used temporarily ... this is necessary 
	 * because the PFS token interfaces require an fdte with associated
	 * pfs_fd structure.
	 */
	hdr_fdte = (fdt_entry_t *) malloc(sizeof(fdt_entry_t));
	if (hdr_fdte == NULL)
		return(ENOMEM);
	fdte_init(hdr_fdte);

	/*
	 * Open the header file.
	 */
	if (error = open_hdr_file(hdr_path, len_hdr_path, hdr_mode, 0, &fp)) {
		PFS_DEBUG(("pfs_multi_truncate: open_hdr_file returned %d\n",
			   error));
		goto out;
	}
	hdr_fdte->fp = fp;
	hdr_fdte->fmode = hdr_mode;

	/*
	 * Open the stripefiles.
	 */
	error = pfs_multi_open(interrupt, hdr_path, len_hdr_path,
			       hdr_fdte, hdr_mode, 0);
	if (error)
		goto out;

	/*
	 * Since we now have an fdte, we can just call pfs_multi_ftruncate()
	 * to do the work.
	 */
	error = pfs_multi_ftruncate(interrupt, hdr_fdte, length);

	/*
	 * Close the PFS file ... release the PFS token to the server, close
	 * the stripefiles, and close the header file.
	 */
	if (hdr_fdte->flags) {
		/*
		 * Release the token.
		 */
		EASSERT(!hdr_fdte->must_release);
		token_release_to_server(hdr_fdte, FALSE);
	}
	close_error = pfs_multi_close(hdr_fdte);
	if (close_error) {
		if (!error)
			error = close_error;
		PFS_DEBUG(("pfs_multi_truncate: pfs_multi_close returned %d\n",
			   close_error));
	}
	unref_file(&hdr_fdte->fp);

out:
	free((void *)hdr_fdte);
	PFS_DEBUG(("pfs_multi_truncate: returning %d\n", error));
	return(error);
}


/*
 * Name:
 *	pfs_multi_ftruncate
 *
 * Description:	
 *	Perform concurrent ftruncate operations on the stripefiles associated
 *	with a PFS file.  If any of the ftruncate operations fail, this 
 *	function returns a failure indication.  However, note that this
 *	function cannot back out of asynchronous stripefile truncate 
 *	operations that have already completed before an error is encountered.
 *
 *	Note that since this operation changes the file length, a PFS token
 *	MUST be acquired in this function so that the truncation is done
 *	atomically across the group of stripefiles.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	length		The new length of the logical PFS file, as specified
 *			in the ftruncate() system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_ftruncate(interrupt, hdr_fdte, length)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	size_t		length;		/* new length of PFS file */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */
	esize_t		new_pfs_length;	/* new (extended) length of PFS file */
	register int	error;

	PFS_TRACE(("pfs_multi_ftruncate: hdr_fdte=0x%x length=%d\n",
		   hdr_fdte, length));

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Make sure we have the PFS token for this file.  This updates us
	 * with the latest PFS file offset and length.
	 */
	error = file_token_acquire(hdr_fdte, interrupt, PFS_OP_WRITE, 0,
				   &pfs_offset, &pfs_length);
	PFS_TRACE(("pfs_multi_ftruncate: tok offset=%d,%d length=%d,%d\n",
		   pfs_offset.shigh, pfs_offset.slow,
		   pfs_length.shigh, pfs_length.slow));
	if (error)
		goto out;

	/*
	 * Truncate the PFS stripefiles.
	 */
	new_pfs_length.shigh = 0;
	new_pfs_length.slow = length;
	error = multi_ftruncate(interrupt, hdr_fdte, new_pfs_length);

	/*
	 * Release the token.  If any errors occurred, there's really nothing
	 * we can do about it ... we can't back out from the stripefile 
	 * truncations that did occur successfully.  So just set the PFS file
	 * size to what was requested no matter what happened.
	 */
	fdte_lock(hdr_fdte);
	hdr_fdte->modified = 1;		/* set modified to update the length */
	fdte_unlock(hdr_fdte);
	file_token_release(hdr_fdte, interrupt, PFS_OP_WRITE,
			   0, &ex_neg_one, &new_pfs_length);

out:
	fdte_pfsio_unlock(pfs_fd);
	PFS_DEBUG(("pfs_multi_ftruncate: returning %d\n", error));
	return(error);
}


/*
 * Name:
 *	multi_ftruncate
 *
 * Description:	
 *	Perform concurrent ftruncate operations on the stripefiles associated
 *	with a PFS file.  If any of the ftruncate operations fail, this 
 *	function returns a failure indication.  However, note that this
 *	function cannot back out of asynchronous stripefile truncate 
 *	operations that have already completed before an error is encountered.
 *
 *	This function assumes that the PFS io lock is held, and the PFS token
 *	has *already been acquired*.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	pfs_length	The new (extended) length of the logical PFS file.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_ftruncate(interrupt, hdr_fdte, pfs_length)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	esize_t		pfs_length;	/* new (extended) length of PFS file */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	register int	pfs_error;	/* running error returned to caller */

	size_t		sf_length;	/* length of stripefile */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	ptuple_t	end;		/* stripefile location of data end */

	PFS_DEBUG(("  multi_ftruncate: hdr_fdte=0x%x, pfs_length=%d,%d\n",
		   hdr_fdte, pfs_length.shigh, pfs_length.slow));

	PTUPLE_INIT(end, pfs_length, sunitsize, sfactor);
	PFS_DEBUG(("  multi_ftruncate: end   snum=%d, sfile=%d, sunitoff=%d\n",
		   end.snum, end.sfile, end.sunitoff));

	/*
	 * Perform truncate operations concurrently on all the stripefiles.
	 */
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register_chk_async(rootdir_port, sfactor, pfs_fd, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_length = PTUPLE_TO_SFOFF(end.snum, end.sfile, end.sunitoff,
					    sfd, sunitsize);
		PFS_DEBUG(("  multi_ftruncate: SEND sfd=%d sf_len=%d\n",
			   sfd, sf_length));
		pfs_error = fsvr_pfs_ftruncate_msg_send(sf_fdt[sfd].s_fp,
							reply_port,
							credentials_port,
							trans_id++,
							&sfd,
							sf_length);
		if (pfs_error)
			break;

		num_sent++;
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_ftruncate_msg_receive(MACH_PORT_NULL,
						      reply_port,
						      credentials_port,
						      0,
						      &sfd,
						      0);
		PFS_DEBUG_RECV(("  multi_ftruncate: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_ftruncate: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);
	PFS_DEBUG(("  multi_ftruncate: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_lsize
 *
 * Description:	
 *	Perform concurrent lsize operations on the stripefiles associated
 *	with a PFS file.  If any of the lsize operations fail, this 
 *	function truncates all stripefiles to their original size and returns
 *	a failure indication.
 *
 *	Note that since this operation changes the file length, a PFS token
 *	MUST be acquired in this function so that the size operation is done
 *	atomically across the group of stripefiles.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	offset		Number of bytes specified in the lsize()/esize() system
 *			call (must be an extended number).
 *
 *	whence		Indicates how offset affects the file size, as 
 *			specified in the lsize()/esize() system call.
 *
 *	extended_flag	TRUE if the operation is allowed to create a logical 
 *			PFS file greater than 2GB-1 in size (i.e. TRUE if the
 *			caller is performing an esize operation, vs. an lsize 
 *			operation).
 *
 *	actual		The new file length if successful (may be less than
 *			what was requested).
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_lsize(interrupt, hdr_fdte, offset, whence, extended_flag, actual)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	esize_t		offset;
	int		whence;
	boolean_t	extended_flag;
	esize_t		*actual;	/* out */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	esize_t		pfs_offset;	/* user offset into PFS file */
	esize_t		pfs_length;	/* length of PFS file */
	esize_t		new_pfs_length;	/* new (extended) length of PFS file */
	int		error;

	PFS_TRACE(("pfs_multi_lsize: hdr_fdte=0x%x offset=%d,%d whence=%d\n",
		   hdr_fdte, offset.shigh, offset.slow, whence));

	*actual = ex_neg_one;		/* on error, size won't be changed */

	/*
	 * Lock out other threads competing for this pfs_fd.
	 */
	fdte_pfsio_lock(pfs_fd);

	/*
	 * Make sure we have the PFS token for this file.  This updates us
	 * with the latest PFS file offset and length.
	 */
	error = file_token_acquire(hdr_fdte, interrupt, PFS_OP_LSIZE, 0,
				   &pfs_offset, &pfs_length);
	PFS_TRACE(("pfs_multi_lsize: tok offset=%d,%d length=%d,%d\n",
		   pfs_offset.shigh, pfs_offset.slow,
		   pfs_length.shigh, pfs_length.slow));
	if (error)
		goto out;

	/*
	 * First convert offset and whence to an absolute PFS file size, so
	 * stripefile offsets can be computed.
	 */
	switch (whence) {

	case SIZE_SET:
		new_pfs_length = EMAX(pfs_length, offset);
		break;

	case SIZE_CUR:
		new_pfs_length = __eadd(pfs_offset, offset, &error);
		if (!error)
			new_pfs_length = EMAX(pfs_length, new_pfs_length);
		break;

	case SIZE_END:
		new_pfs_length = __eadd(pfs_length, offset, &error);
		if (!error)
			new_pfs_length = EMAX(pfs_length, new_pfs_length);
		break;

	default:
		error = EINVAL;
	}

	/*
	 * Ensure that the new file size is not negative.
	 */
	if ((!error) && (LESS(new_pfs_length, ex_zero)))
		error = EINVAL;

	/*
	 * If the caller is performing an esize operation, ensure that the
	 * new PFS file size will not overflow 32-bit stripefile parameters.
	 *
	 * Otherwise, if the caller is performing an lsize operation, ensure
	 * that the new file size is not too big to return as a long.
	 */
	if (!error) {
		if (extended_flag) {
			if (ETOOBIG(new_pfs_length, sunitsize, sfactor))
				error = EINVAL;
		} else if (GREATER(new_pfs_length, ex_int_max)) {
			error = EINVAL;
		}
	}

	/*
	 * Extend the PFS stripefiles if necessary.
	 */
	if (!error) {
		if (GREATER(new_pfs_length, pfs_length)) {
			error = multi_lsize(interrupt, hdr_fdte, 
					    new_pfs_length, pfs_length, 
					    whence, actual);
			if (!error) {
				/* set modified to update the length */
				fdte_lock(hdr_fdte);
				hdr_fdte->modified = 1;
				fdte_unlock(hdr_fdte);
			}
		} else {  /* nothing to do */
			*actual = pfs_length;
		}
	}
	file_token_release(hdr_fdte, interrupt, PFS_OP_LSIZE,
			   0, &ex_neg_one, actual);

out:
	fdte_pfsio_unlock(pfs_fd);
	PFS_DEBUG(("pfs_multi_lsize: returning %d, actual=%d,%d\n",
		   error, actual->shigh, actual->slow));
	return(error);
}


/*
 * Name:
 *	multi_lsize
 *
 * Description:	
 *	Perform concurrent lsize operations on the stripefiles associated
 *	with a PFS file.
 *
 *	This function assumes that the PFS io lock is held, and the PFS token
 *	has *already been acquired*.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	new_pfs_length	Extended value to lsize the PFS file length to.
 *
 *	cur_pfs_length	Current extended PFS file length.
 *
 *	whence		Indicates how offset affects the file size, as 
 *			specified in the lsize()/esize() system call.
 *
 *	pfs_actual	Pointer to the extended actual file length to be
 *			mapped to the return value of the lsize() system call:
 *			either the new file length if successful, or -1 if an
 *			error occurred.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_lsize(interrupt, hdr_fdte, new_pfs_length, cur_pfs_length, whence, pfs_actual)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	esize_t		new_pfs_length;	/* new (extended) length of PFS file */
	esize_t		cur_pfs_length;	/* current length of PFS file */
	int		whence; 	/* offset affect. */
	esize_t		*pfs_actual;	/* out - extended actual length */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	int		pfs_error;	/* running error returned to caller */

	size_t		sf_extend;	/* amount to extend stripefile */
	size_t		cur_sf_length;	/* current length of stripefile */
	size_t		new_sf_length;	/* new length of stripefile */
	register int	sf_error;	/* stripefile operation error */
	size_t		sf_actual;	/* actual count of stripefile data */
	uint_t		sfd;		/* stripefile descriptor index */

	sfile_count_t	sf_count_array[BIG_STRIPE_FACTOR];	/* array for */
	sfile_count_t	*sf_counts = sf_count_array;	/* stripefile counts */
	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;
	int rpc_whence;

	ptuple_t	start;		/* stripefile location of data start */
	ptuple_t	end;		/* stripefile location of data end */

	PFS_DEBUG(("  multi_lsize: hdr_fdte=0x%x, new_pfs_length=%d,%d\n",
		   hdr_fdte, new_pfs_length.shigh, new_pfs_length.slow));

	pfs_error = sfile_count_init(&sf_counts, sfactor);
	if (pfs_error)
		return(pfs_error);

	PTUPLE_INIT(start, cur_pfs_length, sunitsize, sfactor);
	PTUPLE_INIT(end, new_pfs_length, sunitsize, sfactor);
	PFS_DEBUG(("  multi_lsize: start   snum=%d, sfile=%d, sunitoff=%d\n",
		   start.snum, start.sfile, start.sunitoff));
	PFS_DEBUG(("  multi_lsize: end   snum=%d, sfile=%d, sunitoff=%d\n",
		   end.snum, end.sfile, end.sunitoff));

	/*
	 * Perform lsize operations concurrently on all stripefiles that
	 * are affected by the lsize on the PFS file.
	 */
	*pfs_actual = ex_zero;
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	isc_multi_register(MACH_PORT_NULL, sfactor, pfs_fd, &trans_id);
	for (sfd = 0; sfd < sfactor; sfd++) {
		cur_sf_length = PTUPLE_TO_SFOFF(start.snum, start.sfile,
						start.sunitoff, sfd, 
						sunitsize);
		new_sf_length = PTUPLE_TO_SFOFF(end.snum, end.sfile,
						end.sunitoff, sfd, sunitsize);
		sf_counts[sfd].requested = new_sf_length;
		if (whence ==  SIZE_SET) {
			/*
			 * Use an absolute value incase we are doing
			 * the lsize from multiple nodes in parallel.
			 * This will allow the size to be set to the
			 * specified size no matter which node request
			 * arrives first.
			 */
			sf_extend = new_sf_length;
			rpc_whence = SIZE_SET;
		} else {
			rpc_whence = SIZE_END;
			sf_extend = new_sf_length - cur_sf_length;
		}

		if ((int)sf_extend <= 0) {
			/* include current stripefile length in actual count */
			*pfs_actual = __eadd1(*pfs_actual, (long)cur_sf_length,
					      &dont_care);
			continue;
		}

		PFS_DEBUG(("  multi_lsize: SEND sfd=%d sf_extend=%d\n",
			   sfd, sf_extend));
		pfs_error = fsvr_pfs__lsize_msg_send(sf_fdt[sfd].s_fp,
						     reply_port,
						     credentials_port,
						     trans_id++, &sfd,
						     sf_extend, rpc_whence,
						     &sf_actual);
		if (pfs_error) {
			sf_counts[sfd].requested = 0;
			break;
		}
		num_sent++;
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs__lsize_msg_receive(MACH_PORT_NULL,
						       reply_port,
						       credentials_port,
						       0, &sfd,
						       0, 0,
						       &sf_actual);
		PFS_DEBUG_RECV(("  multi_lsize: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d, sf_actual=%d\n", sfd, sf_actual),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_lsize: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}

		sf_counts[sfd].actual = sf_actual;
		*pfs_actual = __eadd1(*pfs_actual, (long)sf_actual,
				      &pfs_error);
	}

	isc_deregister(interrupt);

	if (pfs_error) {
		/*
		 * Truncate stripefiles to their proper length, since in most
		 * error cases all but one of the stripefile writes will have
		 * succeeded.  This frees up disk space and keeps the
		 * stripefile lengths in sync.
		 */
		multi_ftruncate(interrupt, hdr_fdte, cur_pfs_length);
	} else if (LESS(*pfs_actual, new_pfs_length)) {
		/*
		 * Verify consistency of actual counts.
		 */
		PFS_DEBUG(("pfs_multi_lsize: pfs_len=%d,%d pfs_act=%d,%d\n",
			   new_pfs_length.shigh, new_pfs_length.slow,
			   pfs_actual->shigh, pfs_actual->slow));
		PFS_DEBUG(("                call pfs_sync_actuals\n"));
		pfs_sync_actuals(sf_counts, sf_fdt, sunitsize, sfactor,
				 pfs_actual);
		PFS_DEBUG(("pfs_multi_lsize: pfs_len=%d,%d pfs_act=%d,%d\n",
			   new_pfs_length.shigh, new_pfs_length.slow,
			   pfs_actual->shigh, pfs_actual->slow));
		/*
		 * Sync stripefile lengths, and free up disk space.
		 */
		multi_ftruncate(interrupt, hdr_fdte, *pfs_actual);
	}

	if (sf_counts != sf_count_array)    /* a bigger array was malloc'd */
		pfs_free((void *)sf_counts,
			 (sfactor * sizeof(struct sfile_count)));
	PFS_DEBUG(("  multi_lsize: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_chmod
 *
 * Description:	
 *	Perform concurrent chmod operations on the stripefiles associated
 *	with a PFS file.  If any of the chmod operations fail, this function
 *	returns a failure indication.  However, this function does not attempt
 *	to back out of any asynchronous stripefile chmod operations that
 *	successfully completed before an error is encountered.
 *
 *	We don't acquire the PFS token on a chmod operation.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the chmod() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	mode		The access permissions specified in the chmod() 
 *			system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_chmod(interrupt, hdr_path, len_hdr_path, mode)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;	/* PFS header file name */
	int		len_hdr_path;
	int		mode;		/* new access permissions */
{
	struct statpfs	*stripe_attr = NULL;
	register uint_t	sfactor;	/* stripe factor */
	pathname_t	*sf_path;	/* stripefile pathname */

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	PFS_TRACE(("pfs_multi_chmod: hdr_path=%s len_hdr_path=%d mode=%o\n", 
		   hdr_path, len_hdr_path, mode));

	/*
	 * Get the stripe attributes for this PFS file.
	 */
	pfs_error = get_stripe_attributes(hdr_path, len_hdr_path,
					  (fdt_entry_t *)NULL, 0,
					  &stripe_attr);
	if (pfs_error) {
		PFS_DEBUG(("pfs_multi_chmod: get_stripe_attributes ret %d\n",
			   pfs_error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_chmod: BAD stripe attributes\n"));
		pfs_error = ENOTPFS;
		goto out;
	}

	PFS_DEBUG_SATTR(("pfs_multi_chmod: received stripe attributes:\n"),
			stripe_attr);

	/*
	 * Chmod all the stripefiles.
	 */
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);
	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_chmod: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("pfs_multi_chmod: dir port invalid!\n"));
		    break;
		}

		pfs_error = fsvr_pfs_chmod_msg_send(dir_port,
						    reply_port,
						    credentials_port,
						    trans_id++, rootdir_port,
						    &sfd,
						    mod_path, 
						    mod_len + 1,
						    mode);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_chmod_msg_receive(MACH_PORT_NULL, 
						      reply_port,
						      credentials_port,
						      0, MACH_PORT_NULL,
						      &sfd,
						      NULL,
						      0,
						      0);
		PFS_DEBUG_RECV(("pfs_multi_chmod: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_chmod: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);
out:
	if (stripe_attr)
		pfs_free((void *)stripe_attr,
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
	PFS_DEBUG(("pfs_multi_chmod: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_fchmod
 *
 * Description:	
 *	Perform concurrent fchmod operations on the stripefiles associated
 *	with a PFS file.  If any of the fchmod operations fail, this function
 *	returns a failure indication.  However, this function does not attempt
 *	to back out of any asynchronous stripefile chmod operations that
 *	successfully completed before an error is encountered.
 *
 *	We don't acquire the PFS token on a chmod operation.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	mode		The access permissions specified in the fchmod() 
 *			system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_fchmod(interrupt, hdr_fdte, mode)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	int		mode;		/* new access permissions */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	PFS_TRACE(("pfs_multi_fchmod: hdr_fdte=0x%x mode=%o\n",
		   hdr_fdte, mode));

	/*
	 * Perform fchmod operations concurrently on all the stripefiles.
	 */
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	isc_multi_register(MACH_PORT_NULL, sfactor, pfs_fd, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_fchmod: SEND sfd=%d\n", sfd));
		pfs_error = fsvr_pfs_fchmod_msg_send(sf_fdt[sfd].s_fp,
						     reply_port,
						     credentials_port,
						     trans_id++, &sfd, mode);
		if (pfs_error)
			break;

		num_sent++;
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_fchmod_msg_receive(MACH_PORT_NULL, 
						       reply_port,
						       credentials_port,
						       0, &sfd, 0);
		PFS_DEBUG_RECV(("pfs_multi_fchmod: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_fchmod: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);
	PFS_DEBUG(("pfs_multi_fchmod: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_chown
 *
 * Description:	
 *	Perform concurrent chown operations on the stripefiles associated
 *	with a PFS file.  If any of the chown operations fail, this function
 *	returns a failure indication.  However, this function does not attempt
 *	to back out of any asynchronous stripefile chown operations that
 *	successfully completed before an error is encountered.
 *
 *	We don't acquire the PFS token on a chown operation.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the chown() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *	uid		The user ID specified in the chown() system call.
 *
 *	gid		The group ID specified in the chown() system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_chown(interrupt, hdr_path, len_hdr_path, uid, gid)
	boolean_t	*interrupt;	/* out */
	char		*hdr_path;	/* PFS header file name */
	int		len_hdr_path;
	int		uid;		/* user ID */
	int		gid;		/* group ID */
{
	struct statpfs	*stripe_attr = NULL;
	register uint_t	sfactor;	/* stripe factor */
	pathname_t	*sf_path;	/* stripefile pathname */

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;  

	PFS_TRACE(("pfs_multi_chown: path=%s len_path=%d uid=%d gid=%d\n",
		   hdr_path, len_hdr_path, uid, gid));

	/*
	 * Get the stripe attributes for this PFS file.
	 */
	pfs_error = get_stripe_attributes(hdr_path, len_hdr_path,
					  (fdt_entry_t *)NULL, 0,
					  &stripe_attr);
	if (pfs_error) {
		PFS_DEBUG(("pfs_multi_chown: get_stripe_attributes ret %d\n",
			   pfs_error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_chown: BAD stripe attributes\n"));
		pfs_error = ENOTPFS;
		goto out;
	}

	PFS_DEBUG_SATTR(("pfs_multi_chown: received stripe attributes:\n"),
			 stripe_attr);

	/*
	 * Chown all the stripefiles.
	 */
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);

	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);
	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_chown: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("pfs_multi_chown: dir port invalid!\n"));
		    break;
		}

		pfs_error = fsvr_pfs_chown_msg_send(dir_port,
						    reply_port,
						    credentials_port,
						    trans_id++, rootdir_port,
						    &sfd,
						    mod_path,
						    mod_len + 1,
						    uid, gid);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_chown_msg_receive(MACH_PORT_NULL, 
						      reply_port,
						      credentials_port,
						      0, MACH_PORT_NULL,
						      &sfd,
						      NULL,
						      0,
						      0, 0);
		PFS_DEBUG_RECV(("pfs_multi_chown: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_chown: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}
	isc_deregister(interrupt);

out:
	if (stripe_attr)
		pfs_free((void *)stripe_attr,
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
	PFS_DEBUG(("pfs_multi_chown: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_fchown
 *
 * Description:	
 *	Perform concurrent fchown operations on the stripefiles associated
 *	with a PFS file.  If any of the fchown operations fail, this function
 *	returns a failure indication.  However, this function does not attempt
 *	to back out of any asynchronous stripefile chown operations that
 *	successfully completed before an error is encountered.
 *
 *	We don't acquire the PFS token on a chown operation.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	uid		The user ID specified in the chown() system call.
 *
 *	gid		The group ID specified in the chown() system call.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_fchown(interrupt, hdr_fdte, uid, gid)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	int		uid;		/* user ID */
	int		gid;		/* group ID */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	PFS_TRACE(("pfs_multi_fchown: hdr_fdte=0x%x uid=%d gid=%d\n",
		   hdr_fdte, uid, gid));

	/*
	 * Perform fchown operations concurrently on all the stripefiles.
	 */
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(MACH_PORT_NULL, sfactor, pfs_fd, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_fchown: SEND sfd=%d\n", sfd));
		pfs_error = fsvr_pfs_fchown_msg_send(sf_fdt[sfd].s_fp,
						     reply_port,
						     credentials_port,
						     trans_id++, &sfd,
						     uid, gid);
		if (pfs_error)
			break;

		num_sent++;
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_fchown_msg_receive(MACH_PORT_NULL,
						       reply_port,
						       credentials_port,
						       0, &sfd,
						       0, 0);
		PFS_DEBUG_RECV(("pfs_multi_fchown: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_fchown: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);
	PFS_DEBUG(("pfs_multi_fchown: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_fsync
 *
 * Description:	
 *	Perform concurrent fsync operations on the stripefiles associated
 *	with a PFS file.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_fsync(interrupt, hdr_fdte)
	boolean_t	*interrupt;	/* out */
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;

	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	PFS_TRACE(("pfs_multi_fsync: hdr_fdte=0x%x\n", hdr_fdte));

	/*
	 * Perform fsync operations concurrently on all the stripefiles.
	 */
	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(NULL, sfactor, pfs_fd, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("pfs_multi_fsync: SEND sfd=%d\n", sfd));
		pfs_error = fsvr_pfs_fsync_msg_send(sf_fdt[sfd].s_fp,
						    reply_port,
						    credentials_port,
						    trans_id++, &sfd);
		if (pfs_error)
			break;

		num_sent++;
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_fsync_msg_receive(MACH_PORT_NULL,
						      reply_port,
						      credentials_port,
						      0, &sfd);
		PFS_DEBUG_RECV(("pfs_multi_fsync: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("pfs_multi_fsync: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);
	PFS_DEBUG(("pfs_multi_fsync: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_statfs
 *
 * Description:	
 *	Given the stripe attributes of a PFS file, perform concurrent statfs
 *	operations on the pathnames referenced in the stripe attributes and
 *	return an error if any of them reside in a PFS file system.
 *
 *	Optionally, an estatfs structure for the PFS header file system is 
 *	accepted and its bsize, blocks, bfree, and bavail fields are updated
 *	to reflect the set of file systems in which the stripe directories
 *	reside.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	stripe_attr	Pointer to the statpfs structure describing the stripe
 *			attributes of the PFS file.
 *
 *	estatfsbuf	Pointer to estatfs struct that already contains 
 *			statistics for the PFS header file system, or NULL if
 *			PFS stripe file system statistics are not needed.
 *
 * Returns:
 *	ESUCCESS if no operational error occurs and none of the last
 *	components of the pathnames reside in a PFS file system, otherwise the
 *	value to set errno to.
 */
int
pfs_multi_statfs(interrupt, stripe_attr, estatfsbuf)
	boolean_t	*interrupt;	/* out */
	struct statpfs	*stripe_attr;
	struct estatfs	*estatfsbuf;
{
	register uint_t	sfactor  = stripe_attr->p_sfactor;
	pathname_t	*sf_path = &stripe_attr->p_sdirs;

	int		pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	uint_t		num_sent = 0;
	mach_port_t	reply_port;
	transaction_id_t trans_id;

	struct statfs	sf_statfs;
	int		fsid_cnt = 0;
	struct fsid {
		fsid_t	f_fsid;	
	} *fsidlst = NULL;  

	PFS_TRACE(("  pfs_multi_statfs: stripe_attr=0x%x estatfsbuf=0x%x\n",
		   stripe_attr, estatfsbuf));

	if (estatfsbuf != NULL) {	/* we are to update statfs struct */
		if (pfs_error = pfs_malloc((void *)&fsidlst,
					   sizeof(struct fsid) * sfactor)) {
			return(pfs_error);
		}
		estatfsbuf->f_fsize = 0;
		estatfsbuf->f_bsize = 0;
		estatfsbuf->f_blocks = ex_zero;
		estatfsbuf->f_bfree = ex_zero;
		estatfsbuf->f_bavail = ex_zero;
	}

	/*
	 * Perform a statfs operation on all the stripe pathnames.  Note that
	 * the &sf_statfs parameter in the send message is really a dummy
	 * argument since this is a MiG 'out' parameter ... the sf_statfs
	 * struct is not filled in until the receive is done.
	 */
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("  pfs_multi_statfs: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("pfs_multi_statfs: dir port invalid!\n"));
		    break;
		}

		pfs_error = fsvr_pfs_statfs_msg_send(dir_port,
						     reply_port,
						     credentials_port,
						     trans_id++, rootdir_port,
						     &sfd,
						     mod_path, 
						     mod_len + 1,
						     &sf_statfs);
		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_statfs_msg_receive(MACH_PORT_NULL,
						       reply_port,
						       credentials_port,
						       0, MACH_PORT_NULL,
						       &sfd,
						       NULL,
						       0,
						       &sf_statfs);
		PFS_DEBUG_RECV(("  pfs_multi_statfs: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  pfs_multi_statfs: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
		if (sf_statfs.f_type == MOUNT_PFS) {
			pfs_error = EFSNOTSUPP;	/* PFS can't be a stripedir */
			PFS_DEBUG(("  pfs_multi_statfs: f_type=0x%x, err=%d\n",
				   sf_statfs.f_type, pfs_error));
			continue;
		}
		if (estatfsbuf != NULL) {	/* update the statfs struct */
			/*
			 * Make sure that this file system not already
			 * accounted for. 
			 */
			boolean_t	found = FALSE;
			int		i;

			for (i = 0; i < fsid_cnt; i++) {
				if ((fsidlst[i].f_fsid.val[0] == 
				     sf_statfs.f_fsid.val[0]) &&
				    (fsidlst[i].f_fsid.val[1] == 
				     sf_statfs.f_fsid.val[1])) {
					found = TRUE;
					break;
				}
			}

			if (found)
				continue;

			fsidlst[fsid_cnt].f_fsid.val[0] =
				sf_statfs.f_fsid.val[0];
			fsidlst[fsid_cnt].f_fsid.val[1] =
				sf_statfs.f_fsid.val[1];
			fsid_cnt++;

			/*
			 * Check for varying fragment size and block size
			 * among the stripe file systems.  If they aren't all
			 * consistent, we can't report one accurate value
			 * so set the field to -1.
			 */
			if (estatfsbuf->f_fsize == 0) {	/* first time thru */
				estatfsbuf->f_fsize = sf_statfs.f_fsize;
				estatfsbuf->f_bsize = sf_statfs.f_bsize;
			} else {
				if (estatfsbuf->f_fsize != sf_statfs.f_fsize)
					estatfsbuf->f_fsize = -1;
				if (estatfsbuf->f_bsize != sf_statfs.f_bsize)
					estatfsbuf->f_bsize = -1;
			}

			/*
			 * Convert file system fragments to UBSIZE blocks.
			 * This is necessary because different stripe file
			 * systems may have different size blocks, so we
			 * need to convert to a consistent block size.
			 */
			estatfsbuf->f_blocks =
				__eadd1(estatfsbuf->f_blocks,
					FS_TO_U_BLOCKS(sf_statfs.f_blocks,
						       sf_statfs.f_fsize),
					&pfs_error);
			estatfsbuf->f_bfree =
				__eadd1(estatfsbuf->f_bfree,
					FS_TO_U_BLOCKS(sf_statfs.f_bfree,
						       sf_statfs.f_fsize),
					&pfs_error);
			estatfsbuf->f_bavail =
				__eadd1(estatfsbuf->f_bavail,
					FS_TO_U_BLOCKS(sf_statfs.f_bavail,
						       sf_statfs.f_fsize),
					&pfs_error);
		}
	}
	isc_deregister(interrupt);

out:
	if ((estatfsbuf != NULL) && (fsidlst != NULL))
		pfs_free((void *)fsidlst, sizeof(struct fsid) * sfactor); 
	PFS_DEBUG(("  pfs_multi_statfs: returning %d\n", pfs_error));
	return(pfs_error);
}


/*
 * Name:
 *	pfs_multi_utimes
 *
 * Description:	
 *      This routine sets the access and modification time fields of all
 *      the stripefiles associated with a PFS file.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	hdr_path	Pathname specified by the utimes() system call (the
 *			pathname of the PFS header file).
 *
 *	len_hdr_path	Length, in bytes, of the header file pathname.
 *
 *      times           Pointer to an array of two timeval structures
 *                      containing the access and modification times.
 *                      If this pointer is NULL, then these time fields are
 *                      set to the current time.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_multi_utimes(interrupt, hdr_path, len_hdr_path, times)

	boolean_t	*interrupt;	/* out */
	char		*hdr_path;
	int		len_hdr_path;
        timeval_2_t     times;
{
	struct statpfs	*stripe_attr = NULL;
	register int	error;

	PFS_TRACE(("pfs_multi_utimes: hdr_path=%s, len_hdr_path=%d\n", 
		   hdr_path, len_hdr_path));

	/*
	 * Get the stripe attributes for this PFS file.
	 */
	error = get_stripe_attributes(hdr_path, len_hdr_path,
				      (fdt_entry_t *)NULL, 0, &stripe_attr);
	if (error) {
		PFS_DEBUG(("pfs_multi_utimes: get_stripe_attributes ret %d\n",
			   error));
		goto out;
	}
	if (stripe_attr == NULL) {
		PFS_DEBUG(("pfs_multi_utimes: BAD stripe attributes\n"));
		error = ENOTPFS;
		goto out;
	}

	PFS_DEBUG_SATTR(("pfs_multi_utimes: received stripe attributes:\n"),
			 stripe_attr);

	error = multi_utimes(interrupt, stripe_attr, times);

out:
	if (stripe_attr)
		pfs_free((void *)stripe_attr, 
			 MAX(stripe_attr->p_reclen, STATPFS_BUFSZ));
	PFS_DEBUG(("pfs_multi_utimes: returning error=%d\n", error));

	return(error);
}


/*
 * Name:
 *	multi_utimes
 *
 * Description:	
 *	Given the stripe attributes of a PFS file or directory, this routine
 *      loops through each stripefile and makes an asynchronus call to the
 *      server to set access and modification times.  It then loops to get
 *      the status of the utimes request on each of the stripefiles.
 *
 * Parameters:
 *	interrupt	Pointer to interrupt Boolean, initially set to FALSE.
 *			On return from isc_deregister(), indicates whether an
 *			interrupt occurred during the operation.
 *
 *	stripe_attr	Pointer to the statpfs structure describing the stripe
 *			attributes of the PFS file.
 *
 *      times           Pointer to an array of two timeval structures
 *                      containing the access and modification times.
 *                      If this pointer is NULL, then these time fields are
 *                      set to the current time.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
multi_utimes(interrupt, stripe_attr, times)
	boolean_t	*interrupt;	/* out */
	struct statpfs	*stripe_attr;
        timeval_2_t     times;
{
	register uint_t	sfactor;	/* stripe factor */
	pathname_t	*sf_path;	/* stripefile pathname */
	register int	pfs_error;	/* running error returned to caller */
	register int	sf_error;	/* stripefile operation error */
	uint_t		sfd;		/* stripefile descriptor index */
	timeval_2_t 	mig_times;	/* dummy array used as a valid
					     address if the input times
					     argument is NULL*/

	mach_port_t     dir_port;       /* Vnode port to send open message. */
	char            *mod_path;      /* Strip file path after cache lookup*/
	uint_t          mod_len;        /* Length of path */

	mach_port_t	reply_port;
	transaction_id_t trans_id;

	uint_t		num_sent = 0;


	PFS_TRACE(("  multi_utimes: stripe_attr=0x%x times=0x%x\n", 
		   stripe_attr, times));

	/*
	 * Set the access and modification time fields of all the
	 * stripefiles to the value specified in the times  argument.
	 */
	sf_path = &stripe_attr->p_sdirs;
	sfactor = stripe_attr->p_sfactor;

	reply_port = pfs_get_reply_port((mach_port_msgcount_t)sfactor);
	isc_multi_register(rootdir_port, sfactor, NULL, &trans_id);

	for (sfd = 0; sfd < sfactor; sfd++) {
		PFS_DEBUG(("  multi_utimes: SEND sfd=%d (%s), len=%d\n",
			   sfd, sf_path->name, sf_path->namelen));

		pfs_vnode_port_cache_lookup(sf_path, &dir_port, 
					    &mod_path, &mod_len);

                if (!MACH_PORT_VALID(dir_port)) {
		    PFS_DEBUG(("multi_utimes: dir port invalid!\n"));
		    break;
		}
		pfs_error = fsvr_pfs_utimes_msg_send(dir_port,
						     reply_port,
						     credentials_port,
						     trans_id++, rootdir_port,
						     &sfd,
						     mod_path,
						     mod_len + 1,
						     times ? times : mig_times,
						     times == NULL);

		if (pfs_error)
			break;

		num_sent++;
		sf_path = NEXTPATH(sf_path);
	}

	for (; num_sent > 0; num_sent--) {
		sf_error = fsvr_pfs_utimes_msg_receive(MACH_PORT_NULL,
						       reply_port,
						       credentials_port,
						       0, MACH_PORT_NULL,
						       &sfd,
						       NULL,
						       0,
						       times ? times:mig_times,
						       times == NULL);

		PFS_DEBUG_RECV(("  multi_utimes: RECEIVE sf_error=%d",
				sf_error),
			       (" sfd=%d\n", sfd),
			       sf_error);

		if (pfs_error)	/* toss the response */
			continue;
		if (sf_error) {	/* remember the error and toss the response */
			pfs_error = (sf_error == ENOENT) ? ENOSDIR : sf_error;
#ifdef  DEBUG_PFS
			if (sf_error < 0)
				PFS_DEBUG(("  multi_utimes: mach_msg: %s\n",
					   mach_error_string(sf_error)));
#endif
			continue;
		}
	}

	isc_deregister(interrupt);

out:
	PFS_DEBUG(("  multi_utimes: returning pfs_error=%d\n", pfs_error));

	return(pfs_error);
}


/*
 * Name:
 *	pfs_set_stripefile_offsets
 *
 * Description:	
 *	This function sets the stripefile offsets given the logical PFS file
 *	offset.  A valid PFS offset is limited by the largest possible PFS file
 *	that can be created given the stripe factor and maximum size of a
 *	stripefile.
 *
 *	Note that the largest size of a stripefile is NOT simply the OSF/1 
 *	limit of 2GB-1, because only full stripe units are written into
 *	stripefiles.  So in general, each stripefile can be up to 
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
 *
 * Parameters:
 *	hdr_fdte	Pointer to the file descriptor table entry of the PFS
 *			header file.
 *
 *	pfs_offset	The new PFS file offset.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_set_stripefile_offsets(hdr_fdte, pfs_offset)
	fdt_entry_t	*hdr_fdte;	/* hdr_fdte for PFS header file */
	esize_t		pfs_offset;	/* PFS file offset. */
	
{
	pfs_fd_t	*pfs_fd   = hdr_fdte->pfs_fd;
	size_t		sunitsize = pfs_fd->p_stripe_unit_size;
	uint_t		sfactor   = pfs_fd->p_stripe_factor;
	stripe_fd_t	*sf_fdt   = pfs_fd->p_stripe_fdt;
	off_t		sf_offset;
	ptuple_t	end;		/* stripefile location of end offset */
	uint_t		sfd;		/* stripefile descriptor index */

	PFS_DEBUG(("  pfs_set_stripefile_offsets: fdte=0x%x pfs_off=%d,%d\n",
		   hdr_fdte, pfs_offset.shigh, pfs_offset.slow));

	PTUPLE_INIT(end, pfs_offset, sunitsize, sfactor);
	PFS_DEBUG(("  pfs_set_stripefile_offsets: end=(%d, %d, %d)\n",
		   end.snum, end.sfile, end.sunitoff));

	for (sfd = 0; sfd < sfactor; sfd++) {
		sf_offset = PTUPLE_TO_SFOFF(end.snum, end.sfile, end.sunitoff,
					    sfd, sunitsize);
		PFS_DEBUG(("  pfs_set_stripefile_offsets: sfd=%d sf_off=%d\n",
			   sfd, sf_offset));
		/*
		 * Sanity check: Restrict seek to the valid OSF/1 range of
		 * 0 ... 2GB-1.  This "should never happen", because before we
		 * get here the ETOOBIG() macro should have been used to check
		 * seeks past the valid range of PFS file data.  Nevertheless,
		 * if an error occurs here, it should always occur on the
		 * first stripefile (sfd == 0) since the offset of the first
		 * stripefile should always be >= the offsets of the others.
		 * So no need to add code that backs out any s_offsets already 
		 * set in this loop.
		 */
		if ((int)sf_offset < 0) {
			EASSERT(sfd == 0);
			PFS_DEBUG(("  pfs_set_stripefile_offsets: OUT OF RANGE PFS OFFSET\n"));
			PFS_DEBUG(("    pfs_off=%d,%d, sf_off=%d, sfact=%d\n",
				   pfs_offset.shigh, pfs_offset.slow, 
				   sf_offset, sfactor));
			return(EINVAL);
		}

		sf_fdt[sfd].s_offset = sf_offset;
	}

	return(ESUCCESS);
}


/*
 * Name:
 *	linear_copy
 *
 * Description:	
 *	This function provides a mapping between contiguous data from a PFS
 *	stripefile and the appropriate locations in a user's buffer.  It may
 *	be used to copy data in either direction, depending on the 'direction'
 *	argument.
 *
 * Parameters:
 *	buf		Pointer to user's buffer.
 *
 *	buf_offset	Offset into user's buffer corresponding to the 
 *			beginning of the stripefile data.  I.e. this is the
 *			offset from which we start copying stripefile data
 *			to/from.
 *
 *	su		Pointer to first stripe unit in the stripefile data.
 *
 *	su_len		Length of first stripe unit partial in the PFS file
 *			data (required since the PFS offset in the file may
 *			lie somewhere in the middle of a stripe unit).
 *
 *	direction	Indicates if data is being copied FROM the user's
 *			buffer space (write operation) or TO the user's buffer
 *			space (read operation).
 *
 *	count		Total number of bytes to copy.
 *
 *	sunitsize	Stripe unit size of the PFS file that the data belongs
 *			to.
 *
 *	sfactor		Stripe factor of the PFS file.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
linear_copy(buf, buf_offset, su, su_len, direction, count, sunitsize, sfactor)
	char		*buf;		/* user's data buffer */
	off_t		buf_offset;	/* offset into user's buffer */
	char		*su;		/* stripe unit data from stripefile */
	uint_t		su_len;		/* length first stripe unit partial */
	int		direction;	/* direction of copy */
	uint_t		count;		/* number of bytes to copy */
	size_t		sunitsize;	/* stripe unit size */
	uint_t		sfactor;	/* stripe factor */
{
	char		**copyfrom;	/* location of ptr to source */
	char		**copyto;	/* location of ptr to destination */
	uint_t		sofar;		/* total amount of data copied */
	int		error = ESUCCESS;

	PFS_DEBUG(("  linear_copy: buf=0x%x buf_offset=%d su=0x%x su_len=%d\n",
		   buf, buf_offset, su, su_len));
	PFS_DEBUG(("               direct=%d cnt=%d sunitsize=%d sfactor=%d\n",
		   direction, count, sunitsize, sfactor));

	/*
	 * A little messy, but this allows code reuse of this routine for
	 * either copy direction.
	 */
	if (direction == FROM_USER) {
		copyfrom = &buf;
		copyto = &su;
	} else {
		copyfrom = &su;
		copyto = &buf;
	}

	sofar = 0;
	buf += buf_offset;

	/*
	 * Optimize the case where we're only striping one way; in this
	 * case, there is really no scatter/gather going on, so the stripe
	 * unit size can "logically" be set to the size of the copy.
	 */
	if (sfactor == 1)
		su_len = count;

	while (sofar < count) {
		/*
		 * Copy as much as we can into the current stripe unit from 
		 * the data buffer.
		 */
		error = pfs_copy(*copyfrom, *copyto, su_len);
		if (error)
			break;

		sofar += su_len;
		buf += su_len + ((sfactor - 1) * sunitsize);
		su += su_len;
		su_len = MIN(sunitsize, count - sofar);
	}

	return(error);
}


/*
 * Name:
 *	vector_copy
 *
 * Description:	
 *	This function provides a mapping between contiguous data from a PFS
 *	stripefile and the appropriate locations in a user's array of I/O
 *	vectors.  It may be used to copy data in either direction, depending
 *	on the 'direction' argument.
 *
 *	XXXPFS - There may be better, more mathematical and generalized
 *	algorithms for doing this.
 *
 * Parameters:
 *	iov		Pointer to array of iovec's that comprise the user's
 *			buffer space.
 *
 *	iov_offset	Offset into the user's buffer space (which consists of
 *			all the iovec's) corresponding to the beginning of the
 *			stripefile data.  I.e. this is the offset at which we
 *			start copying stripefile data to/from.
 *
 *	su		Pointer to first stripe unit in the stripefile data.
 *
 *	su_len		Length of first stripe unit partial in the stripefile
 *			data (required since the PFS offset in the file may
 *			lie somewhere in the middle of a stripe unit).
 *
 *	direction	Indicates if data is being copied FROM the user's
 *			buffer space (write operation) or TO the user's buffer
 *			space (read operation).
 *
 *	count		Total number of bytes to copy.
 *
 *	sunitsize	Stripe unit size of the PFS file that the stripefile
 *			data belongs to.
 *
 *	sfactor		Stripe factor of the PFS file.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
vector_copy(iov, iov_offset, su, su_len, direction, count, sunitsize, sfactor)
	struct iovec	*iov;		/* user's array of io vectors */
	off_t		iov_offset;	/* offset in iov buffer space */
	char		*su;		/* stripe unit data from stripefile */
	uint_t		su_len;		/* length first stripe unit partial */
	int		direction;	/* direction of copy */
	uint_t		count;		/* number of bytes to copy */
	size_t		sunitsize;	/* stripe unit size */
	uint_t		sfactor;	/* stripe factor */
{
	char		**copyfrom;	/* location of ptr to source */
	char		**copyto;	/* location of ptr to destination */
	char		*data;		/* location in IO vector to copy to */
	uint_t		sofar;		/* total amount of data copied */
	off_t		cur_iov_offset;	/* current offset into iov buf space */
	struct iovec	*v_p;		/* pointer to current io vector */
	off_t		v_offset;	/* offset into current io vector */
	uint_t		v_remaining;	/* remaining in current io vector */
	uint_t		v_skiplen;	/* amount to advance vector offset */
	uint_t		v_copylen;	/* amount to copy to io vector */
	int		error = ESUCCESS;

	PFS_DEBUG(("  vector_copy: iov=0x%x iov_offset=%d su=0x%x su_len=%d\n",
		   iov, iov_offset, su, su_len));
	PFS_DEBUG(("               direct=%d cnt=%d sunitsize=%d sfactor=%d\n",
		   direction, count, sunitsize, sfactor));

	/*
	 * A little messy, but this allows code reuse of this routine for
	 * either copy direction (and prevents the 'if' from being in the
	 * while loop below).
	 */
	if (direction == FROM_USER) {
		copyfrom = &data;
		copyto = &su;
	} else {
		copyfrom = &su;
		copyto = &data;
	}

	sofar = 0;
	cur_iov_offset = 0;
	v_p = iov;
	v_offset = 0;
	v_remaining = v_p->iov_len;

	while (sofar < count) {
		/*
		 * Advance to the next io vector that we need to copy to/from.
		 */
		while ((cur_iov_offset + v_remaining) <= iov_offset) {
			cur_iov_offset += v_remaining;
			v_p++;
			v_offset = 0;
			v_remaining = v_p->iov_len;
		}

		/*
		 * Advance to the offset in this io vector to copy to.
		 */
		v_skiplen = iov_offset - cur_iov_offset;
		v_offset += v_skiplen;
		v_remaining -= v_skiplen;
		cur_iov_offset += v_skiplen;

		/*
		 * Copy as much as we can from the current stripe unit into
		 * the current io vector.
		 */
		data = v_p->iov_base + v_offset;
		v_copylen = MIN(su_len, v_remaining);
		error = pfs_copy(*copyfrom, *copyto, v_copylen);
		if (error)
			break;

		/*
		 * Advance pointers past what we just copied.
		 */
		su += v_copylen;
		iov_offset += v_copylen;
		cur_iov_offset += v_copylen;
		sofar += v_copylen;
		v_offset += v_copylen;
		if ((v_remaining -= v_copylen) == 0) {
			/*
			 * Advance to next IO vector.
			 */
			v_p++;
			v_offset = 0;
			v_remaining = v_p->iov_len;
		}
		if ((su_len -= v_copylen) == 0) {
			/*
			 * Advance to next stripe unit.
			 */
			iov_offset += ((sfactor - 1) * sunitsize);
			su_len = MIN(sunitsize, count - sofar);
		}
	}

	return(error);
}


/*
 * Name:
 *	pfs_copy
 *
 * Description:	
 *	Copy function that attempts to employ VM optimizations (vm_copy() of
 *	page-aligned data instead of user_bcopy2()) when copying data.
 *
 * Parameters:
 *	src		Pointer to start of source data.
 *
 *	dst		Pointer to destination.
 *
 *	count		Number of bytes to copy from source to destination.
 *
 * Returns:
 *	ESUCCESS if successful, otherwise the value to set errno to.
 */
int
pfs_copy(src, dst, count)
char	*src;
char	*dst;
uint_t	count;
{
	int			error = ESUCCESS;
	extern boolean_t	nx_process_lock;
					/* TRUE if -plk NX switch used */

	PFS_DEBUG(("    pfs_copy: src=0x%x, dst=0x%x, count=%d\n",
		   src, dst, count));

	if (count == 0)
		return(error);

	/*
	 * If source and destination are page-aligned and length is a multiple
	 * of page size, use copy-on-write.
	 *
	 * NOTE: The 'nx_process_lock' check is a WORKAROUND for the fact that
	 * vm_copy() has not been modified to support NX -plk semantics
	 * (vm_copy() does not leave the user's buffer wired if it swizzles
	 * page pointers).  See PTS bug #7082.
	 */
	if ((!nx_process_lock) &&
	    (dst == (char *)trunc_page(dst)) &&
	    (src == (char *)trunc_page(src)) &&
	    (count % vm_page_size == 0)) {
		PFS_DEBUG(("    pfs_copy: USING VMCOPY on %d bytes\n", count));
		if (vm_copy(mach_task_self(), (vm_address_t)src,
			    (vm_size_t)count, (vm_address_t)dst))
			error = EFAULT;
	} else {
		error = user_bcopy2(src, dst, &count);
		if ((error) && (count > 0))  /* some copied successfully */
				error = ESUCCESS;
	}

	return(error);
}


/*
 * Name: pfs_vnode_port_cache_lookup
 *	
 *
 * Description:	
 *	Lookup the pathname to the stripe file to see if we have a vnode port
 *	to its stripe directory cached.  If so, use that port instead of
 *      rootdir_port.
 *
 * Parameters:
 *	sf_path         Pointer to the pathname to the stripe file
 *
 *	dir_port        Out parameter which is set to either rootdir_port or
 *                      the cached vnode port for the files stripe dir.
 */

void
pfs_vnode_port_cache_lookup(sf_path, dir_port, strip_path, strip_len)
	pathname_t	*sf_path;
	mach_port_t	*dir_port;
	char		**strip_path;
	uint_t		*strip_len;
{
	pathname_t	*cache_path;	/* stripefile pathname */
	uint_t		sfactor;	/* stripe factor */
	uint_t		sfd;		/* stripefile descriptor index */
	char		*tmp_path;

	/* next name from last match */
	static pathname_t	*cp_hint=(pathname_t *)0;
	static uint_t		sfd_hint; /* stripefile descriptor index */

	/*
	 * Check for a cache
	 */		
	if (stripe_attr_cache == SA_CACHE_NULL) 
	    goto out;

	sfactor = stripe_attr_cache->p_sfactor;

	/*
	 * Do we have a valid hint?
	 */
	if ( cp_hint ) {
		if (!strncmp(cp_hint->name, sf_path->name, cp_hint->namelen)) {
		    /*
		     * We have a match
		     */
		    tmp_path = (char *)&sf_path->name[0] + cp_hint->namelen;
		    if (*tmp_path == '/') {
			cache_path = cp_hint;
			sfd = sfd_hint;
			goto	match;
		    }
		}
	}

	/*
	 * Compare the path against the cached pathnames.
	 */		
	cache_path = &stripe_attr_cache->p_sdirs;

	for (sfd = 0; sfd < sfactor; sfd++) {

		if (!strncmp(cache_path->name, sf_path->name, 
			    cache_path->namelen)) {

		    /*
		     * We have a match
		     */

		    tmp_path = (char *)&sf_path->name[0] + 
			        cache_path->namelen;

                    /*
                     *  strip_path now MUST be pointing to a slash,
                     *  if not, then we really do not have a match.
                     */
		    if (*tmp_path == '/') {
match: 
			*dir_port = *(stripe_dir_vnode_ports + sfd);
			if (*dir_port == MACH_PORT_NULL)
			    break;

			*strip_path = ++tmp_path; /* Skip the slash */
			*strip_len = sf_path->namelen - cache_path->namelen;
			PFS_TRACE(("pfs_vnode_port_cache_lookup: hit on '%s' port %x\n",
				   *strip_path, *dir_port));
			/*
			 * setup a hint for next lookup.
			 */
			if ( (sfd_hint = ++sfd) < sfactor ) {
				cp_hint = NEXTPATH(cache_path);
			}
			else
				cp_hint = (pathname_t *)0; /* end of cache */

			return;
		    }
		}
		cache_path = NEXTPATH(cache_path);
	}
out:
    *dir_port = rootdir_port;
    *strip_path = (char *)&sf_path->name[0];
    *strip_len = sf_path->namelen;
    return;
}


/*
 * Name: pfs_vnode_port_cache_insert_rights
 *	
 *
 * Description:	
 *	If a pfs vnode cache has been set up then insert send rights
 *	into the destination task.
 *
 * Parameters:
 *	dest_task       The destination task.
 */
pfs_vnode_port_cache_insert_rights(dest_task)
task_t        dest_task;
{
    uint_t	    sfactor;	/* stripe factor */
    uint_t	    sfd;		/* stripefile index */
    kern_return_t   ret;


    PFS_DEBUG(("pfs_vnode_port_cache_insert_rights\n"));
    
    /*
     * Check for a cache
     */		
    if (stripe_attr_cache == SA_CACHE_NULL) 
	return;

    /*
     * Insert the send rights
     */
    sfactor = stripe_attr_cache->p_sfactor;

    for (sfd = 0; sfd < sfactor; sfd++) {

	ret = mach_port_insert_right(dest_task, 
				     *(stripe_dir_vnode_ports + sfd),
				     *(stripe_dir_vnode_ports + sfd),
				     MACH_MSG_TYPE_COPY_SEND);
	if (ret != KERN_SUCCESS) {
	    PFS_DEBUG(("pfs_vnode_port_cache_insert_rights: ret=0x%x\n", ret));
	}
    };
}

/*
 * Name: pfs_get_vnode_port_cache_rights()
 *	
 *
 * Description:	
 *	Return the number of vnode ports in the cache and possibly
 *	copy the ports to an out arrary
 *
 * Parameters:
 *	port_array_p     An array of ports.
 */
uint_t
pfs_get_vnode_port_cache_rights( port_array_p )
mach_port_t        *port_array_p;
{
    uint_t	    sfactor;	/* stripe factor */
    uint_t	    sfd;		/* stripefile index */
    kern_return_t   ret;


    PFS_DEBUG(("pfs_get_vnode_port_cache_rights\n"));
    
    /*
     * Check for a cache
     */		
    if (stripe_attr_cache == SA_CACHE_NULL) 
	return 0;

    /*
     * Get the number of ports/stripes
     */
    sfactor = stripe_attr_cache->p_sfactor;

    /*
     * If port array pointer is NULL, then
     * just return the count of ports.
     */
    if (port_array_p == (mach_port_t *)NULL)
	return sfactor;

    /*
     * Collect the ports
     */
    for (sfd = 0; sfd < sfactor; sfd++) {

	*port_array_p++ = *(stripe_dir_vnode_ports + sfd);
    };
    return sfactor;
}

#ifdef	DEBUG_PFS
dump_pfsattr(sattr)
	struct statpfs *sattr;
{
	pathname_t	*sdir = &sattr->p_sdirs;
	int 		i;

	e_printf("pfs stripe attributes: \n");
	e_printf("  record length=%d\n", sattr->p_reclen);
	e_printf("  magic=0x%x\n", sattr->p_magic);
	e_printf("  stripe unit size=%d\n", sattr->p_sunitsize);
	e_printf("  stripe factor=%d\n", sattr->p_sfactor);
	e_printf("  start sdir=%d\n", sattr->p_start_sdir);

	for (i = 0; i < sattr->p_sfactor; i++) {
		e_printf("  stripedir %d, length=%d, dir=%s\n",
			 i, sdir->namelen, sdir->name);
		sdir = NEXTPATH(sdir);
	}
}
#endif	DEBUG_PFS
#endif	PFS
