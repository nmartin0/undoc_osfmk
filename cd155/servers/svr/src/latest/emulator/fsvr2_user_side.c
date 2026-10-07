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
 * HISTORY
 * $Log: fsvr2_user_side.c,v $
 * Revision 1.8  1994/11/18  20:23:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  17:31:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:23:16  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  20:14:59  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:41  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  1993/04/24  18:44:17  klh
 * 	Revision 2.9  93/03/30  16:07:26  roy
 * 		Rewrote.
 * 		[93/02/16            roy]
 *
 * 	Revision 2.8  93/03/10  10:44:52  mmp
 * 		Cast fdt_unref_entry calls to (void).
 *
 * 	Revision 2.7  93/01/12  17:05:46  roy
 * 		fdte has 'iomode' field instead of fast path-specific
 *		 'fpio_mode' field.
 * 		[93/01/11            roy]
 *
 * Revision 1.5  1993/04/06  18:01:46  wunder
 * Modified e_read2 and e_write2 to accept fdte pointer instead of fdt_index
 * in order to remove multiple calls to fdt_ref_entry.
 *
 * Revision 1.4  1993/04/03  03:17:51  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.3  1992/12/11  02:51:44  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  05:57:09  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:00:56  brad
 * Added first cut at PFS file striping capability.
 * 
 * Revision 1.2  1992/11/30  22:08:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/26  01:39:09  brad
 * Fixed Fast Path I/O compilation problem.
 *
 * Revision 1.1.2.1  1992/11/05  22:15:50  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.6  92/11/23  10:27:55  klh
 * Get rid of TNC notify hooks, mapped files files never need notification
 * (klh for mjl)
 * 
 * Revision 2.5  92/08/26  12:09:31  loverso
 * 	Fix bugs in write2 to pass the correct variable as size and update
 * 	the seek pointer (pjg).
 * 
 * 	Add appropriate isc_register/isc_deregister calls.  This is easy
 * 	now, but may become difficult for striped files.  (loverso)
 * 
 * Revision 2.4  92/08/13  19:12:29  rabii
 * 	Added the interrupt parameter to send_sig. (rabii)
 * 
 * Revision 2.3  92/07/29  08:39:01  rabii
 * 	Added e_write2 (pjg)
 * 
 * Revision 2.2  92/06/08  18:16:03  pjg
 * 	Created (pjg).
 * 
 * $EndLog$
 */
/*
 * User-side routines for FAST_PATH_IO operations.
 */

#include <mach_init.h>
#include <mach/mig_errors.h>
#include <uxkern/fsvr.h>
#include <sys/file.h>
#include "emul.h"
#include "fdt.h"

/*
 * The fsvr_read_at_offset and fsvr_write_at_offset messages are not
 * interruptible.  However, the emul_blocking/emul_unblocking calls
 * allow a thread to pick up a pending signal before leaving the emulator.
 */

int
e_read2(fdte, count, data, amount)
	fdt_entry_t	*fdte;
	int		count;		/* in */
	char		**data;		/* out */
	int		*amount;	/* out */
{
	int		error;
	unsigned	response_id;
	mach_port_t	reply_port;

	EASSERT(fdte->iomode == VIO_FASTPATH);

	reply_port = mig_get_reply_port();
	fdte_fpio_lock(fdte);
	emul_blocking();
	error = fsvr_read_at_offset_msg_send(fdte->fp, reply_port,
					     credentials_port,
					     &response_id, fdte->fpio_offset,
					     count, data, amount);

	error = fsvr_read_at_offset_msg_receive(fdte->fp, reply_port,
						credentials_port,
						&response_id, fdte->fpio_offset,
						count, data, amount);

	emul_unblocking();

	fdte->fpio_offset += *amount;
	fdte_fpio_unlock(fdte);

	/*
	 * XXX Note that fast path reading is relying on the caller to
	 * copy data to the user's buffer.  If one wants to add the
	 * capability for vm_copy optimization, then this module should
	 * export a copy routine that can be used by the caller.
	 */

	return(error);
}

int
e_write2(fdte, data, count, amount)
	fdt_entry_t	*fdte;
	char		*data;
	unsigned int	count;
	int		*amount;	/* out */
{
	int		error;
	unsigned	response_id;
	mach_port_t	reply_port;

	EASSERT(fdte->iomode == VIO_FASTPATH);

	reply_port = mig_get_reply_port();
	fdte_fpio_lock(fdte);
	emul_blocking();
	error = fsvr_write_at_offset_msg_send(fdte->fp, reply_port,
					      credentials_port,
					      &response_id, 
					      fdte->fpio_offset,
					      data, count, amount);

	error = fsvr_write_at_offset_msg_receive(fdte->fp, reply_port,
						 credentials_port,
						 &response_id, 
						 fdte->fpio_offset,
						 data, count, amount);
	
	emul_unblocking();

	fdte->fpio_offset += *amount;
	fdte_fpio_unlock(fdte);

	return(error);
}

int
e_lseek2(fdte, offset, sbase, rval)
	fdt_entry_t	*fdte;
	off_t		offset;
	int		sbase;
	int		*rval;		/* OUT */
{
	off_t		newoffset;
	int		error;

	EASSERT(fdte->iomode == VIO_FASTPATH);

	switch (sbase) {
	      case L_SET:
		newoffset = offset; 
		break;
	      case L_INCR:
		/* not implemented */
	      case L_XTND:
		/* not implemented */
	      default:	
		newoffset = -1;
		break;
	}
	if ((int) newoffset < 0) 
		error = EINVAL;
	else {
		error = ESUCCESS;
		fdte_fpio_lock(fdte);
		fdte->fpio_offset = newoffset;
		fdte_fpio_unlock(fdte);
	}

	rval[0] = newoffset;
	return(error);
}

