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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: ns_config.h,v $
 * Revision 1.5  1994/11/18  20:36:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:12:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:35:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:01:41  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:30:55  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:59  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:52  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:45:47  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:23  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/12/06  14:03:21  devrcs
 * 	Fixed up HISTORY marker.
 * 	[90/11/04  17:15:16  gm]
 * 
 * Revision 1.3  90/10/07  14:37:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:18:28  gm]
 * 
 * Revision 1.2  90/09/23  15:56:05  devrcs
 * 	Make config version different from XNS.
 * 	[90/09/15  15:42:18  tmt]
 * 
 * 	Created, configuration structure for XNS protocol family.
 * 	[90/09/15  14:04:55  tmt]
 * 
 * $EndLog$
 */

#define NS_CONFIG_VERSION_1	0x02091590

#define NS_DEFAULT_VALUE	-12345
#define NS_USEVALUE		0x01
#define NS_USEDEFAULTS		0x02

typedef struct ns_config {

	int	version;
	int	errcode;
	int	flags;

	/* TBD */
} ns_config_t;
