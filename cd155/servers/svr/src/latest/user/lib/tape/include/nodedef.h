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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	nodedef.h 11.1 94/03/25 15:52:09
 */
/** static char	nodedefh_ver[] = "@(#) nodedef.h 11.1 94/03/25 15:52:09";
/**/

/*
 * .{ TEXT
 * .H 4 nodedef.h
 *
 * This module defines all system node numbers.
 *
 * The following table shows the physical and virtual node number spaces.
 * The physical space is used on the Direct Connect network, and the
 * virtual and direct combined comprise the node numbering seen by
 * applications.
 * 
 * Node		Physical		Virtual			Direct
 * ----		--------		-------			------
 * 
 * Global send	-7 - -1			-7 - -1			505 - 511
 * 								(DIRECT+phys)
 * 
 * Compute node	0 - 127			0 - numnodes()-1	512 - 639
 * 								(DIRECT+phys)
 * 
 * SRM		128						640
 * 		(PSRM_NODE)					(SRM_NODE)
 * 
 * I/O nodes	129 - 255					641 - 767
 * 								(DIRECT+phys)
 * 
 * Host		256 - 265		numnodes() or myhost()	768 - 777
 * 		(PRHOST_NODE+cubeid)				(DIRECT+phys)
 * 
 * CFS anchor	one of 0 - 255		numnodes() + 1		one of 512 - 767
 *					(numnodes() + CFS_ANCHOR)
 * 
 * .} TEXT
 */


/*
 * Virtual node numbers
 */

			/* Base for direct node addressing */
#define	DIRECT			512

			/* Maximum number of compute nodes */
#define	MAXSUBCUBE		(MAXNODES/2)

			/* Maximum number of direct node numbers */
#define MAXDIRECT		(PRHOST_NODE+NUMCUBES+1)

			/* Maximum range of positive virtual node numbers */
#define NODESPACE		(DIRECT+MAXDIRECT)

			/* Maximum global send depth */
#define	MAXGLOBAL		  8



			/* Wild card node */
#define	WILD_NODE		 -1

			/* virtual SRM node */
#define SRM_NODE		(DIRECT+PSRM_NODE)

			/* CFS anchor node */
#define CFS_ANCHOR		(numnodes() + 1)


/*
 * Physical node numbers
 */

			/* Number of physical slots, including I/O */
#define MAXNODES		256

			/* Physical node numbers */
#define MAXPHYS			DIRECT



			/* physical SRM node */
#define PSRM_NODE		128

			/* physical remote host node */
#define PRHOST_NODE		MAXNODES


/*
 * Miscellaneous related constants
 */

			/* Maximum number of subcubes */
#define NUMCUBES		 10

			/* Number of cardcages and USM's */
#define MAXUSM			 32

			/* Number of physical slots, including I/O */
#define MAXSLOTS		(MAXUSM*16)

#define SETBYTES		(MAXNODES/8)

#define MAX_SIO			(16)	/* MAX # of socket boards */

#define MAX_ARGS	1000	/* Max # of arguments on the command line */

#ifndef NPTY
#define NPTY		MAXNODES * 2
#endif
