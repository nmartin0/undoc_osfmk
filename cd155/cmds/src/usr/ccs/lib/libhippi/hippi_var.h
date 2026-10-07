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
 
/*
 * Copyright 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: hippi_var.h,v $
 * Revision 1.9  1995/03/07  00:42:17  arlin
 * Added hippi_status.h to pick up filter
 * structure.
 *
 * Revision 1.8  1995/03/06  23:30:39  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection.
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *  Module(s):
 *     hippi_memfree.c, hippi_status.h, hippi_memget.c,
 *     hippi_var.h, hippi_open.c, hippi_write.c,
 *     hippi_bind.c, hippi_read.c, raw_hippi.h,
 *     hippi_close.c, hippi_read_complete.c,
 *     hippi_config.c, hippi_read_request.c
 *
 * Revision 1.7  1995/02/25  02:53:49  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 * 	    hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.6  1994/11/19  02:24:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/10/26  00:08:14  hobbes
 *  Reviewer: Arlin
 *  Risk: Low
 *  Benefit or PTS #: 10942
 *  Testing: Developer tests
 *  Module(s): hippi_read_complete.c, hippi_read_request.c, Makefile, hippi_var.h
 *
 * Revision 1.4  1994/06/08  18:16:00  hobbes
 * Initial changes to support R1.3
 *
 * Revision 1.3  1994/01/17  23:20:17  hobbes
 * Merged R1.2 changes into the mainline. See checkin below for details.
 *
 * Revision 1.2.4.1  1994/01/15  00:59:27  hobbes
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: ran EATS against a WW03 server/emulator.
 *  Module(s): hippi_var.h hippi_config.c
 *
 * Revision 1.2  1993/07/16  22:19:27  hobbes
 * Changes to make HIPPI_DATA mode work.
 *
 * Revision 1.1  1993/05/28  02:27:54  hobbes
 * Initial checkin.
 *
 */

/*
 * hippi_var.h
 *
 */

#ifndef _hippi_var_h
#define _hippi_var_h

#include <device/device.h>
#include <device/hippi_status.h>


#define HLIB_DBG	0

#define MAXCONNS 64
#define MAX_D1_SIZE 127
#define PAGE_SIZE 8192

struct hippi_table {
	int 		fd;
	int		pending;
	mach_port_t	master_port;
	mach_port_t	reply_port;
        mach_msg_header_t  *rmsg;
	u_long		hippi_mode;
	u_long		port;
	u_long		ulp;
	u_long		ifield;
	u_long		b;
	u_long		rdf;
	int		rulp;
	int		rport;
	int		first_write;
	u_long		node_mem;
	char		*d1_data;
	u_short		d1_size;
};

typedef struct {
	mach_msg_header_t Head;
	mach_msg_type_t return_codeType;
	kern_return_t return_code;
	mach_msg_type_long_t dataType;
	io_buf_ptr_t data;
} Request;

#endif _hippi_var_h
