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
 
/*++ sizedb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/sizedb.c,v $
 *
 * DESCRIPTION:
 *
 *	Return the number of bytes required to represent the specified
 *	generic descriptor structure which must be completely initialized
 *	with the exception of the size field.  The size value returned
 *	does NOT include the bytes required to store the "size" field
 *	of a generic descriptor.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:58 $ $State: Exp $)
 * $Log: sizedb.c,v $
 * Revision 1.3  1994/11/19  02:27:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:50  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:10  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:22  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"

extern int strlen ();			/* String length */
extern void nqs_abort();		/* Abort NQS execution */
extern int errno;			/* System call error number */

/*** sizedb
 *
 *
 *	int sizedb():
 *
 *	Return the number of bytes required to represent the specified
 *	generic descriptor structure which must be completely initialized
 *	with the exception of the size field.  The size value returned
 *	does NOT include the bytes required to store the "size" field
 *	of a generic descriptor.
 */
int sizedb (descr, descrtype)
register struct gendescr *descr;	/* Descriptor */
register int descrtype;			/* Descriptor type:  DSC_ */
{
	register int size;

	switch (descrtype) {
	case DSC_QCOMPLEX:
		size = sizeof (descr->v.qcom);
		break;
	case DSC_QUEUE:
		size = sizeof (descr->v.que);
		break;
	case DSC_DEVICE:
		size = sizeof (descr->v.dev) - MAX_PATHNAME
		       + strlen (descr->v.dev.fullname);
		break;
	case DSC_QMAP:
		size = sizeof (descr->v.map);
		break;
	case DSC_PIPEQDEST:
		size = sizeof (descr->v.dest);
		break;
	case DSC_PARAM:
		switch (descr->v.par.paramtype) {
		case PRM_GENPARAMS:
			size = sizeof (descr->v.par.v.genparams);
			break;
		case PRM_LOGFILE:
			size = sizeof (descr->v.par.v.logfile);
			break;
		case PRM_NETPROCS:
			size = sizeof (descr->v.par.v.netprocs);
			break;
		case PRM_NETPEER:
			size = sizeof (descr->v.par.v.netpeer);
			break;
		default:
			printf ("F$Invalid NQS general parameters file ");
			printf ("descriptor type\n");
			printf ("F$specified in call to sizedb().\n");
			printf ("I$Type was %1d.\n", descr->v.par.paramtype);
			fflush (stdout);
			errno = 0;		/* Not a system call error */
			nqs_abort();		/* Abort execution */
		}
		/*
		 *  Add in the space for the paramtype field.
		 */
		size += (((char *) &descr->v.par.v) -
			 ((char *) &descr->v.par.paramtype));
		break;
	case DSC_MGR:
		size = sizeof (descr->v.mgr);
		break;
	case DSC_FORM:
		size = sizeof (descr->v.form);
		break;
	default:
		printf ("F$Invalid NQS database descriptor type.\n");
		printf ("I$Type was %1d.\n", descrtype);
	}
	return (size);
}
