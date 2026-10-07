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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: mi_types.h,v $
 * Revision 1.7  1995/02/01  23:16:56  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:51:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:48:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:13:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/18  03:21:04  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.3  1993/05/06  19:32:17  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 3.1  93/05/05  22:28:31  mjl
 * Add ifname_t type (not yet used).
 * 
 * Revision 3.0  1992/04/20  17:21:09  bhk
 * Genesis
 *
 * 
 */

#ifndef	_MI_MI_TYPES_H_
#define	_MI_MI_TYPES_H_

#include <vsocket/mi_types_gen.h>
/*
 * Types for MI server to server interface.
 */

typedef char	mi_pkt_data_t[MI_PKT_SIZE];
typedef	char	ifname_t[IFNAMSIZ];

#endif	_MI_MI_TYPES_H_

