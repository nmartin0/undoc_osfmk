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
#ifndef	_I860PARAGON_CONSLOG_H_
#define	_I860PARAGON_CONSLOG_H_ 1

/*
 * Micro-kernel console logger message defines. Also utilized by mklogd
 * user-land pgm.
 */

#define CONS_DATA_MAX_SIZE 4096

typedef struct cons_log_msg {
	mach_msg_header_t	hdr;
	mach_msg_type_t		node_desc;
	unsigned		nodeNum;
	mach_msg_type_t		data_desc;
	char			body[CONS_DATA_MAX_SIZE];
} cons_msg;

typedef cons_msg        *cons_msg_t;

#endif	/* _I860PARAGON_CONSLOG_H_ */
