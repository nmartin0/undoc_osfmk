/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 

#ifndef	_NORMA_NODE_STATUS_H
#define	_NORMA_NODE_STATUS_H 

/*
 * Norma node status bitvector type used by
 * norma_node_{set/get}_status() calls
 *
 * matches MiG type def in mach/mach_norma.defs
 */
typedef unsigned int	node_status_t[128];

#endif	/* _NORMA_NODE_STATUS_H */
