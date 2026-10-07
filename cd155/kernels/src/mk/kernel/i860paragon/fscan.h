/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 *
 * $Id: fscan.h,v 1.17 1995/04/07 17:21:06 lenb Exp $
 *
 */

#ifndef __FSCAN_H__
#define __FSCAN_H__

/*
 * External Interfaces to the FSCAN driver.
 */
 
#include <sys/types.h>
/*
 * Driver entry points
 */
extern boolean_t       fscan_cons_autoconf();
extern int fscan_cons_probe(caddr_t ignored, struct bus_device *device);
extern int fscan_cons_param(struct tty *tp, int line);
extern int fscan_cons_start(struct tty *tp);
extern int fscan_cons_putc(int unit, int line, unsigned char ch);
extern int fscan_cons_getc(int unit, int line, int wait, int raw);
extern int fscan_cons_pollc(int unit, boolean_t on);
extern int fscan_cons_mctl(dev_t dev, int bits, int how);
extern int fscan_cons_softCAR(int unit, int line, int on_or_off);

/*
 * Machine Dependent Entry points
 */
extern void	fscan_reboot();
extern int	fscan_interrupt();
extern void	fscan_start_watchdog(int enable);
extern void	fscan_provide_state();

/*
 * Global data
 */
extern int	fscan_secondary_console;

#endif	/*  __FSCAN_H__ */
