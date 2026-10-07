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
 * $Log: raw_hippi.h,v $
 * Revision 1.7  1995/03/21  23:26:47  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 12723 hippi_read is typed u_long should be long
 *  Testing: HIPPI EATS
 *  Module(s): include/raw_hippi.h usr/ccs/lib/libhippi/hippi_read.c
 *
 * Revision 1.6  1995/03/02  23:53:09  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection.
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *    Also developed special applications to test
 *    new MPC and CONT modes.
 *  Module(s):
 *    hippi_memfree.c, hippi_status.h, hippi_memget.c,
 *    hippi_var.h, hippi_open.c, hippi_write.c,
 *    hippi_bind.c, hippi_read.c, raw_hippi.h,
 *    hippi_close.c, hippi_read_complete.c,
 *    hippi_config.c, hippi_read_request.c
 *
 * Revision 1.5  1995/01/17  22:16:24  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 11145
 *  Testing: Developer testing.
 *  Module(s): raw_hippi.h
 *
 * Revision 1.4  1994/11/19  02:40:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/06/08  18:19:50  hobbes
 * Initial changes for R1.3.
 *
 * Revision 1.2  1994/01/12  19:53:37  hobbes
 * Merged in the R1.2 changes.
 *
 * Revision 1.1.6.1  1994/01/12  19:11:34  hobbes
 *  added function prototype for ANSI C
 *
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 5273
 *  Testing: developer testing on a R1.2 WW50 installed system
 *  Module(s): raw_hippi.h
 *
 * Revision 1.1  1993/05/28  02:24:05  hobbes
 * Initial checkin.
 *
 */

/*
 * raw_hippi.h
 *
 */

#ifndef _raw_hippi_h
#define _raw_hippi_h

/*
 * HiPPI Ifield, FP formats.
 */

struct hippi_ifield {
	u_long Ifield;
	u_long filler[7];
};

union hippi_fp_header {
	struct bits {
		int	d2_off:3;
		int	d1_size:8;
		int	res:11;
		int	b:1;
		int	p:1;
		int	ulp:8;
		int	d2_size;
	} fields;

	struct {
		u_long w1;
		u_long w2;
	} words;
};

struct hippi_header {
	struct hippi_ifield	cci;
	union  hippi_fp_header	fp;
};

#define HIPPI_RAW     0x01
#define HIPPI_DATA    0x02
#define HIPPI_MPC     0x04 /* multiple pkts per connection */
#define HIPPI_CNT     0x08 /* continuation, multiple I/O per pkt & connection */
#define HIPPI_EXC     0x10 /* driver and interface in exlusive mode */


#ifdef _NO_PROTO
extern	int	hippi_bind();
extern	int	hippi_close();
extern	int	hippi_config();
extern	int	hippi_memfree();
extern	char *	hippi_memget();
extern	int	hippi_open();
extern	long	hippi_read();
extern	int	hippi_write();
extern	int	hippi_read_request();
extern	long	hippi_read_complete();
#else
extern	int	hippi_bind(int ihandle, u_char ulp, short port);
extern	int	hippi_close(int ihandle);
extern	int	hippi_config(int ihandle, u_long ifield, u_char ulp, u_long b,
			 		char *d1_data, u_short d1_len);
extern	int	hippi_memfree(int ihandle, char *ptr, u_long size, int how);
extern	char *	hippi_memget(int ihandle, u_long size);
extern	int	hippi_open(char *dev_name, u_long hippi_mode, u_long mode);
extern	long	hippi_read(int ihandle, char **ptr, u_long len);
extern	int	hippi_write(int ihandle, char *ptr, u_long len);
extern	int	hippi_read_request(int ihandle, long len);
extern	long	hippi_read_complete(int ihandle, char **ptr);
#endif _NO_PROTO
#endif _raw_hippi_h
