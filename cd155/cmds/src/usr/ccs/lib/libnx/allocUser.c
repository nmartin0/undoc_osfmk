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
 
#include <nx/alloc.h>
#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/mig_errors.h>
#include <mach/msg_type.h>
#ifndef	KERNEL
#include <strings.h>
#endif	KERNEL
/* LINTLIBRARY */

extern mach_port_t mig_get_reply_port();
extern void mig_dealloc_reply_port();

#ifndef	mig_internal
#define	mig_internal	static
#endif

#ifndef	mig_external
#define mig_external
#endif

#ifndef	TypeCheck
#define	TypeCheck 1
#endif

#ifndef	UseExternRCSId
#ifdef	hc
#define	UseExternRCSId		1
#endif
#endif

#ifndef	UseStaticMsgType
#if	!defined(hc) || defined(__STDC__)
#define	UseStaticMsgType	1
#endif
#endif

#define msgh_request_port	msgh_remote_port
#define msgh_reply_port		msgh_local_port


/* Routine init_appl */
mig_external kern_return_t init_appl
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	uid_t uid,
	uid_t e_uid,
	gid_t gid,
	gid_t e_gid,
	gid_t nx_acctid,
	NX_INIT_T nx_info,
	int *rows,
	int *cols,
	pid_t pgroup,
	LP_MAP_T *lp_map,
	mach_msg_type_number_t *lp_mapCnt,
	int *retcode
)
#else
	(server_port, uid, e_uid, gid, e_gid, nx_acctid, nx_info, rows, cols, pgroup, lp_map, lp_mapCnt, retcode)
	mach_port_t server_port;
	uid_t uid;
	uid_t e_uid;
	gid_t gid;
	gid_t e_gid;
	gid_t nx_acctid;
	NX_INIT_T nx_info;
	int *rows;
	int *cols;
	pid_t pgroup;
	LP_MAP_T *lp_map;
	mach_msg_type_number_t *lp_mapCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t e_uidType;
		uid_t e_uid;
		mach_msg_type_t gidType;
		gid_t gid;
		mach_msg_type_t e_gidType;
		gid_t e_gid;
		mach_msg_type_t nx_acctidType;
		gid_t nx_acctid;
		mach_msg_type_t nx_infoType;
		NX_INIT_T nx_info;
		mach_msg_type_t pgroupType;
		pid_t pgroup;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t rowsType;
		int rows;
		mach_msg_type_t colsType;
		int cols;
		mach_msg_type_long_t lp_mapType;
		LP_MAP_T lp_map;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t e_uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t e_gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t nx_acctidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t nx_infoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		17,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t pgroupType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t rowsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t colsCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->e_uidType = e_uidType;
#else	UseStaticMsgType
	InP->e_uidType.msgt_name = 2;
	InP->e_uidType.msgt_size = 32;
	InP->e_uidType.msgt_number = 1;
	InP->e_uidType.msgt_inline = TRUE;
	InP->e_uidType.msgt_longform = FALSE;
	InP->e_uidType.msgt_deallocate = FALSE;
	InP->e_uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->e_uid = e_uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

#if	UseStaticMsgType
	InP->e_gidType = e_gidType;
#else	UseStaticMsgType
	InP->e_gidType.msgt_name = 2;
	InP->e_gidType.msgt_size = 32;
	InP->e_gidType.msgt_number = 1;
	InP->e_gidType.msgt_inline = TRUE;
	InP->e_gidType.msgt_longform = FALSE;
	InP->e_gidType.msgt_deallocate = FALSE;
	InP->e_gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->e_gid = e_gid;

#if	UseStaticMsgType
	InP->nx_acctidType = nx_acctidType;
#else	UseStaticMsgType
	InP->nx_acctidType.msgt_name = 2;
	InP->nx_acctidType.msgt_size = 32;
	InP->nx_acctidType.msgt_number = 1;
	InP->nx_acctidType.msgt_inline = TRUE;
	InP->nx_acctidType.msgt_longform = FALSE;
	InP->nx_acctidType.msgt_deallocate = FALSE;
	InP->nx_acctidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->nx_acctid = nx_acctid;

#if	UseStaticMsgType
	InP->nx_infoType = nx_infoType;
#else	UseStaticMsgType
	InP->nx_infoType.msgt_name = 2;
	InP->nx_infoType.msgt_size = 32;
	InP->nx_infoType.msgt_number = 17;
	InP->nx_infoType.msgt_inline = TRUE;
	InP->nx_infoType.msgt_longform = FALSE;
	InP->nx_infoType.msgt_deallocate = FALSE;
	InP->nx_infoType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->nx_info = nx_info;

#if	UseStaticMsgType
	InP->pgroupType = pgroupType;
#else	UseStaticMsgType
	InP->pgroupType.msgt_name = 2;
	InP->pgroupType.msgt_size = 32;
	InP->pgroupType.msgt_number = 1;
	InP->pgroupType.msgt_inline = TRUE;
	InP->pgroupType.msgt_longform = FALSE;
	InP->pgroupType.msgt_deallocate = FALSE;
	InP->pgroupType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pgroup = pgroup;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 750;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 850) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size != 72) || msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->rowsType != * (int *) &rowsCheck)
#else	UseStaticMsgType
	if ((OutP->rowsType.msgt_inline != TRUE) ||
	    (OutP->rowsType.msgt_longform != FALSE) ||
	    (OutP->rowsType.msgt_name != 2) ||
	    (OutP->rowsType.msgt_number != 1) ||
	    (OutP->rowsType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*rows = OutP->rows;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->colsType != * (int *) &colsCheck)
#else	UseStaticMsgType
	if ((OutP->colsType.msgt_inline != TRUE) ||
	    (OutP->colsType.msgt_longform != FALSE) ||
	    (OutP->colsType.msgt_name != 2) ||
	    (OutP->colsType.msgt_number != 1) ||
	    (OutP->colsType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*cols = OutP->cols;

#if	TypeCheck
	if ((OutP->lp_mapType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->lp_mapType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->lp_mapType.msgtl_name != 2) ||
	    (OutP->lp_mapType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*lp_map = OutP->lp_map;

	*lp_mapCnt = OutP->lp_mapType.msgtl_number;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine remove_nx_appl */
mig_external kern_return_t remove_nx_appl
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	int *retcode
)
#else
	(server_port, pgroup, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t pgroupType;
		pid_t pgroup;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t pgroupType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->pgroupType = pgroupType;
#else	UseStaticMsgType
	InP->pgroupType.msgt_name = 2;
	InP->pgroupType.msgt_size = 32;
	InP->pgroupType.msgt_number = 1;
	InP->pgroupType.msgt_inline = TRUE;
	InP->pgroupType.msgt_longform = FALSE;
	InP->pgroupType.msgt_deallocate = FALSE;
	InP->pgroupType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pgroup = pgroup;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 751;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 851) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine set_nx_pri */
mig_external kern_return_t set_nx_pri
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	int pri,
	int *retcode
)
#else
	(server_port, pgroup, pri, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	int pri;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t pgroupType;
		pid_t pgroup;
		mach_msg_type_t priType;
		int pri;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t pgroupType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t priType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->pgroupType = pgroupType;
#else	UseStaticMsgType
	InP->pgroupType.msgt_name = 2;
	InP->pgroupType.msgt_size = 32;
	InP->pgroupType.msgt_number = 1;
	InP->pgroupType.msgt_inline = TRUE;
	InP->pgroupType.msgt_longform = FALSE;
	InP->pgroupType.msgt_deallocate = FALSE;
	InP->pgroupType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pgroup = pgroup;

#if	UseStaticMsgType
	InP->priType = priType;
#else	UseStaticMsgType
	InP->priType.msgt_name = 2;
	InP->priType.msgt_size = 32;
	InP->priType.msgt_number = 1;
	InP->priType.msgt_inline = TRUE;
	InP->priType.msgt_longform = FALSE;
	InP->priType.msgt_deallocate = FALSE;
	InP->priType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pri = pri;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 752;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 852) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine allocator_mkpart */
mig_external kern_return_t allocator_mkpart
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T buffer,
	mach_msg_type_number_t bufferCnt,
	int *num_nodes,
	int *retcode
)
#else
	(server_port, uid, gid, buffer, bufferCnt, num_nodes, retcode)
	mach_port_t server_port;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T buffer;
	mach_msg_type_number_t bufferCnt;
	int *num_nodes;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
		mach_msg_type_long_t bufferType;
		MIG_BITMAP_T buffer;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t num_nodesType;
		int num_nodes;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t bufferType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t num_nodesCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

#if	UseStaticMsgType
	InP->bufferType = bufferType;
#else	UseStaticMsgType
	InP->bufferType.msgtl_name = 8;
	InP->bufferType.msgtl_size = 8;
	InP->bufferType.msgtl_header.msgt_name = 0;
	InP->bufferType.msgtl_header.msgt_size = 0;
	InP->bufferType.msgtl_header.msgt_number = 0;
	InP->bufferType.msgtl_header.msgt_inline = FALSE;
	InP->bufferType.msgtl_header.msgt_longform = TRUE;
	InP->bufferType.msgtl_header.msgt_deallocate = FALSE;
	InP->bufferType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->buffer = buffer;

	InP->bufferType.msgtl_number = bufferCnt;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 753;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 853) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 48) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->num_nodesType != * (int *) &num_nodesCheck)
#else	UseStaticMsgType
	if ((OutP->num_nodesType.msgt_inline != TRUE) ||
	    (OutP->num_nodesType.msgt_longform != FALSE) ||
	    (OutP->num_nodesType.msgt_name != 2) ||
	    (OutP->num_nodesType.msgt_number != 1) ||
	    (OutP->num_nodesType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*num_nodes = OutP->num_nodes;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine part_remove */
mig_external kern_return_t part_remove
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	string pathname,
	mach_msg_type_number_t pathnameCnt,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	int fflg,
	int *retcode
)
#else
	(server_port, pathname, pathnameCnt, inode, uid, gid, fflg, retcode)
	mach_port_t server_port;
	string pathname;
	mach_msg_type_number_t pathnameCnt;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	int fflg;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t pathnameType;
		string pathname;
		mach_msg_type_t inodeType;
		ino_t inode;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
		mach_msg_type_t fflgType;
		int fflg;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_long_t pathnameType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t inodeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t fflgType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->pathnameType = pathnameType;
#else	UseStaticMsgType
	InP->pathnameType.msgtl_name = 8;
	InP->pathnameType.msgtl_size = 8;
	InP->pathnameType.msgtl_header.msgt_name = 0;
	InP->pathnameType.msgtl_header.msgt_size = 0;
	InP->pathnameType.msgtl_header.msgt_number = 0;
	InP->pathnameType.msgtl_header.msgt_inline = FALSE;
	InP->pathnameType.msgtl_header.msgt_longform = TRUE;
	InP->pathnameType.msgtl_header.msgt_deallocate = FALSE;
	InP->pathnameType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pathname = pathname;

	InP->pathnameType.msgtl_number = pathnameCnt;

#if	UseStaticMsgType
	InP->inodeType = inodeType;
#else	UseStaticMsgType
	InP->inodeType.msgt_name = 2;
	InP->inodeType.msgt_size = 32;
	InP->inodeType.msgt_number = 1;
	InP->inodeType.msgt_inline = TRUE;
	InP->inodeType.msgt_longform = FALSE;
	InP->inodeType.msgt_deallocate = FALSE;
	InP->inodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inode = inode;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

#if	UseStaticMsgType
	InP->fflgType = fflgType;
#else	UseStaticMsgType
	InP->fflgType.msgt_name = 2;
	InP->fflgType.msgt_size = 32;
	InP->fflgType.msgt_number = 1;
	InP->fflgType.msgt_inline = TRUE;
	InP->fflgType.msgt_longform = FALSE;
	InP->fflgType.msgt_deallocate = FALSE;
	InP->fflgType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->fflg = fflg;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 754;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 854) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine chpart */
mig_external kern_return_t chpart
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	string buffer,
	mach_msg_type_number_t bufferCnt,
	uid_t uid,
	gid_t gid,
	int *retcode
)
#else
	(server_port, buffer, bufferCnt, uid, gid, retcode)
	mach_port_t server_port;
	string buffer;
	mach_msg_type_number_t bufferCnt;
	uid_t uid;
	gid_t gid;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t bufferType;
		string buffer;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_long_t bufferType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->bufferType = bufferType;
#else	UseStaticMsgType
	InP->bufferType.msgtl_name = 8;
	InP->bufferType.msgtl_size = 8;
	InP->bufferType.msgtl_header.msgt_name = 0;
	InP->bufferType.msgtl_header.msgt_size = 0;
	InP->bufferType.msgtl_header.msgt_number = 0;
	InP->bufferType.msgtl_header.msgt_inline = FALSE;
	InP->bufferType.msgtl_header.msgt_longform = TRUE;
	InP->bufferType.msgtl_header.msgt_deallocate = FALSE;
	InP->bufferType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->buffer = buffer;

	InP->bufferType.msgtl_number = bufferCnt;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 755;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 855) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine pspart */
mig_external kern_return_t pspart
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t serv_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T *app_list,
	mach_msg_type_number_t *app_listCnt,
	int *retcode
)
#else
	(serv_port, inode, uid, gid, app_list, app_listCnt, retcode)
	mach_port_t serv_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T *app_list;
	mach_msg_type_number_t *app_listCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t inodeType;
		ino_t inode;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t app_listType;
		MIG_BITMAP_T app_list;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t inodeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->inodeType = inodeType;
#else	UseStaticMsgType
	InP->inodeType.msgt_name = 2;
	InP->inodeType.msgt_size = 32;
	InP->inodeType.msgt_number = 1;
	InP->inodeType.msgt_inline = TRUE;
	InP->inodeType.msgt_longform = FALSE;
	InP->inodeType.msgt_deallocate = FALSE;
	InP->inodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inode = inode;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = serv_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 756;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 856) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size != 56) || msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->app_listType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->app_listType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->app_listType.msgtl_name != 8) ||
	    (OutP->app_listType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*app_list = OutP->app_list;

	*app_listCnt = OutP->app_listType.msgtl_number;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine register_daemon_allocator */
mig_external kern_return_t register_daemon_allocator
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	int daemon_id,
	smd_iface_t socket,
	mach_msg_type_number_t socketCnt,
	int *retcode
)
#else
	(server_port, daemon_id, socket, socketCnt, retcode)
	mach_port_t server_port;
	int daemon_id;
	smd_iface_t socket;
	mach_msg_type_number_t socketCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t daemon_idType;
		int daemon_id;
		mach_msg_type_long_t socketType;
		smd_iface_t socket;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t daemon_idType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t socketType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->daemon_idType = daemon_idType;
#else	UseStaticMsgType
	InP->daemon_idType.msgt_name = 2;
	InP->daemon_idType.msgt_size = 32;
	InP->daemon_idType.msgt_number = 1;
	InP->daemon_idType.msgt_inline = TRUE;
	InP->daemon_idType.msgt_longform = FALSE;
	InP->daemon_idType.msgt_deallocate = FALSE;
	InP->daemon_idType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->daemon_id = daemon_id;

#if	UseStaticMsgType
	InP->socketType = socketType;
#else	UseStaticMsgType
	InP->socketType.msgtl_name = 8;
	InP->socketType.msgtl_size = 8;
	InP->socketType.msgtl_header.msgt_name = 0;
	InP->socketType.msgtl_header.msgt_size = 0;
	InP->socketType.msgtl_header.msgt_number = 0;
	InP->socketType.msgtl_header.msgt_inline = FALSE;
	InP->socketType.msgtl_header.msgt_longform = TRUE;
	InP->socketType.msgtl_header.msgt_deallocate = FALSE;
	InP->socketType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->socket = socket;

	InP->socketType.msgtl_number = socketCnt;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 757;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 857) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine lock_part */
mig_external kern_return_t lock_part
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	int *locked,
	int *retcode
)
#else
	(server_port, locked, retcode)
	mach_port_t server_port;
	int *locked;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t lockedType;
		int locked;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t lockedCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 758;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 858) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 48) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->lockedType != * (int *) &lockedCheck)
#else	UseStaticMsgType
	if ((OutP->lockedType.msgt_inline != TRUE) ||
	    (OutP->lockedType.msgt_longform != FALSE) ||
	    (OutP->lockedType.msgt_name != 2) ||
	    (OutP->lockedType.msgt_number != 1) ||
	    (OutP->lockedType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*locked = OutP->locked;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine unlock_part */
mig_external kern_return_t unlock_part
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	int *retcode
)
#else
	(server_port, retcode)
	mach_port_t server_port;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 759;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 859) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 40) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine get_part_attributes */
mig_external kern_return_t get_part_attributes
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	PARTREQ_T *partinfo,
	int *retcode
)
#else
	(server_port, inode, uid, gid, partinfo, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	PARTREQ_T *partinfo;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t inodeType;
		ino_t inode;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t partinfoType;
		PARTREQ_T partinfo;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t inodeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t partinfoCheck = {
		/* msgt_name = */		9,
		/* msgt_size = */		8,
		/* msgt_number = */		56,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->inodeType = inodeType;
#else	UseStaticMsgType
	InP->inodeType.msgt_name = 2;
	InP->inodeType.msgt_size = 32;
	InP->inodeType.msgt_number = 1;
	InP->inodeType.msgt_inline = TRUE;
	InP->inodeType.msgt_longform = FALSE;
	InP->inodeType.msgt_deallocate = FALSE;
	InP->inodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inode = inode;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 760;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 860) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 100) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->partinfoType != * (int *) &partinfoCheck)
#else	UseStaticMsgType
	if ((OutP->partinfoType.msgt_inline != TRUE) ||
	    (OutP->partinfoType.msgt_longform != FALSE) ||
	    (OutP->partinfoType.msgt_name != 9) ||
	    (OutP->partinfoType.msgt_number != 56) ||
	    (OutP->partinfoType.msgt_size != 8))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*partinfo = OutP->partinfo;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine get_part_node_list */
mig_external kern_return_t get_part_node_list
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	LP_MAP_T *nd_list,
	mach_msg_type_number_t *nd_listCnt,
	int *retcode
)
#else
	(server_port, inode, uid, gid, nd_list, nd_listCnt, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	LP_MAP_T *nd_list;
	mach_msg_type_number_t *nd_listCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t inodeType;
		ino_t inode;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t nd_listType;
		LP_MAP_T nd_list;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t inodeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->inodeType = inodeType;
#else	UseStaticMsgType
	InP->inodeType.msgt_name = 2;
	InP->inodeType.msgt_size = 32;
	InP->inodeType.msgt_number = 1;
	InP->inodeType.msgt_inline = TRUE;
	InP->inodeType.msgt_longform = FALSE;
	InP->inodeType.msgt_deallocate = FALSE;
	InP->inodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inode = inode;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 761;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 861) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size != 56) || msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->nd_listType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->nd_listType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->nd_listType.msgtl_name != 2) ||
	    (OutP->nd_listType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*nd_list = OutP->nd_list;

	*nd_listCnt = OutP->nd_listType.msgtl_number;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine get_part_bitmap */
mig_external kern_return_t get_part_bitmap
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T *bitmap,
	mach_msg_type_number_t *bitmapCnt,
	int *retcode
)
#else
	(server_port, inode, uid, gid, bitmap, bitmapCnt, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T *bitmap;
	mach_msg_type_number_t *bitmapCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t inodeType;
		ino_t inode;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t bitmapType;
		MIG_BITMAP_T bitmap;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t inodeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->inodeType = inodeType;
#else	UseStaticMsgType
	InP->inodeType.msgt_name = 2;
	InP->inodeType.msgt_size = 32;
	InP->inodeType.msgt_number = 1;
	InP->inodeType.msgt_inline = TRUE;
	InP->inodeType.msgt_longform = FALSE;
	InP->inodeType.msgt_deallocate = FALSE;
	InP->inodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inode = inode;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 762;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 862) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size != 56) || msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->bitmapType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->bitmapType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->bitmapType.msgtl_name != 8) ||
	    (OutP->bitmapType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*bitmap = OutP->bitmap;

	*bitmapCnt = OutP->bitmapType.msgtl_number;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine get_nx_app_info */
mig_external kern_return_t get_nx_app_info
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	uid_t uid,
	gid_t gid,
	nx_app_info_t *app_attr,
	int *retcode
)
#else
	(server_port, pgroup, uid, gid, app_attr, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	uid_t uid;
	gid_t gid;
	nx_app_info_t *app_attr;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t pgroupType;
		pid_t pgroup;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t app_attrType;
		nx_app_info_t app_attr;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t pgroupType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t app_attrCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		9,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->pgroupType = pgroupType;
#else	UseStaticMsgType
	InP->pgroupType.msgt_name = 2;
	InP->pgroupType.msgt_size = 32;
	InP->pgroupType.msgt_number = 1;
	InP->pgroupType.msgt_inline = TRUE;
	InP->pgroupType.msgt_longform = FALSE;
	InP->pgroupType.msgt_deallocate = FALSE;
	InP->pgroupType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pgroup = pgroup;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 763;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 863) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;

	if ((OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX) ||
	    ((msgh_size != 80) &&
	     ((msgh_size != sizeof(mig_reply_header_t)) ||
	      (OutP->RetCode == KERN_SUCCESS))))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->app_attrType != * (int *) &app_attrCheck)
#else	UseStaticMsgType
	if ((OutP->app_attrType.msgt_inline != TRUE) ||
	    (OutP->app_attrType.msgt_longform != FALSE) ||
	    (OutP->app_attrType.msgt_name != 2) ||
	    (OutP->app_attrType.msgt_number != 9) ||
	    (OutP->app_attrType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*app_attr = OutP->app_attr;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine get_app_node_list */
mig_external kern_return_t get_app_node_list
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	uid_t uid,
	gid_t gid,
	LP_MAP_T *nd_list,
	mach_msg_type_number_t *nd_listCnt,
	int *retcode
)
#else
	(server_port, pgroup, uid, gid, nd_list, nd_listCnt, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	uid_t uid;
	gid_t gid;
	LP_MAP_T *nd_list;
	mach_msg_type_number_t *nd_listCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t pgroupType;
		pid_t pgroup;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t nd_listType;
		LP_MAP_T nd_list;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t pgroupType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->pgroupType = pgroupType;
#else	UseStaticMsgType
	InP->pgroupType.msgt_name = 2;
	InP->pgroupType.msgt_size = 32;
	InP->pgroupType.msgt_number = 1;
	InP->pgroupType.msgt_inline = TRUE;
	InP->pgroupType.msgt_longform = FALSE;
	InP->pgroupType.msgt_deallocate = FALSE;
	InP->pgroupType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->pgroup = pgroup;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 764;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 864) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size != 56) || msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->nd_listType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->nd_listType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->nd_listType.msgtl_name != 2) ||
	    (OutP->nd_listType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*nd_list = OutP->nd_list;

	*nd_listCnt = OutP->nd_listType.msgtl_number;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}

/* Routine get_part_free_nodes */
mig_external kern_return_t get_part_free_nodes
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T *bitmap,
	mach_msg_type_number_t *bitmapCnt,
	int *retcode
)
#else
	(server_port, inode, uid, gid, bitmap, bitmapCnt, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T *bitmap;
	mach_msg_type_number_t *bitmapCnt;
	int *retcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t inodeType;
		ino_t inode;
		mach_msg_type_t uidType;
		uid_t uid;
		mach_msg_type_t gidType;
		gid_t gid;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t bitmapType;
		MIG_BITMAP_T bitmap;
		mach_msg_type_t retcodeType;
		int retcode;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
#if	TypeCheck
	unsigned int msgh_size;
#endif	TypeCheck

#if	UseStaticMsgType
	static mach_msg_type_t inodeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t uidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t gidType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t retcodeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->inodeType = inodeType;
#else	UseStaticMsgType
	InP->inodeType.msgt_name = 2;
	InP->inodeType.msgt_size = 32;
	InP->inodeType.msgt_number = 1;
	InP->inodeType.msgt_inline = TRUE;
	InP->inodeType.msgt_longform = FALSE;
	InP->inodeType.msgt_deallocate = FALSE;
	InP->inodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inode = inode;

#if	UseStaticMsgType
	InP->uidType = uidType;
#else	UseStaticMsgType
	InP->uidType.msgt_name = 2;
	InP->uidType.msgt_size = 32;
	InP->uidType.msgt_number = 1;
	InP->uidType.msgt_inline = TRUE;
	InP->uidType.msgt_longform = FALSE;
	InP->uidType.msgt_deallocate = FALSE;
	InP->uidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->uid = uid;

#if	UseStaticMsgType
	InP->gidType = gidType;
#else	UseStaticMsgType
	InP->gidType.msgt_name = 2;
	InP->gidType.msgt_size = 32;
	InP->gidType.msgt_number = 1;
	InP->gidType.msgt_inline = TRUE;
	InP->gidType.msgt_longform = FALSE;
	InP->gidType.msgt_deallocate = FALSE;
	InP->gidType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->gid = gid;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = server_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_seqno = 0;
	InP->Head.msgh_id = 765;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, sizeof(Request), sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		switch (msg_result) {
		case MACH_SEND_INVALID_REPLY:
		case MACH_RCV_INVALID_NAME:
		case MACH_RCV_IN_SET:
		case MACH_RCV_PORT_DIED:
		case MACH_RCV_PORT_CHANGED:
		case MACH_SEND_INVALID_MEMORY:
		case MACH_SEND_INVALID_RIGHT:
		case MACH_SEND_INVALID_TYPE:
		case MACH_SEND_MSG_TOO_SMALL:
			mig_dealloc_reply_port();
			/*break;*/
		}
		return msg_result;
	}

	if (OutP->Head.msgh_id != 865) {
		if (OutP->Head.msgh_id == MACH_NOTIFY_SEND_ONCE)
		return MIG_SERVER_DIED;
		else
		return MIG_REPLY_MISMATCH;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	if (((msgh_size != 56) || msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

#if	TypeCheck
	if ((OutP->bitmapType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->bitmapType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->bitmapType.msgtl_name != 8) ||
	    (OutP->bitmapType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*bitmap = OutP->bitmap;

	*bitmapCnt = OutP->bitmapType.msgtl_number;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->retcodeType != * (int *) &retcodeCheck)
#else	UseStaticMsgType
	if ((OutP->retcodeType.msgt_inline != TRUE) ||
	    (OutP->retcodeType.msgt_longform != FALSE) ||
	    (OutP->retcodeType.msgt_name != 2) ||
	    (OutP->retcodeType.msgt_number != 1) ||
	    (OutP->retcodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*retcode = OutP->retcode;

	return KERN_SUCCESS;
}
