/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
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
 */
/*
 * HISTORY
 * $Log: state.h,v $
 * Revision 2.5  1994/11/18  20:39:36  mtm
 * Copyright additions/changes
 *
 * Revision 2.4  1993/06/30  22:31:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.3  1992/09/26  21:41:32  SSD
 * no longer used.
 *
 * Revision 2.3  91/06/18  20:51:51  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:58:53  jsb]
 * 
 * Revision 2.2  91/06/17  15:45:10  jsb
 * 	First checkin.
 * 	[91/06/17  10:16:53  jsb]
 * 
 */

typedef struct processor_state {

/*  0*/	unsigned long longreg[32]; /*****************************************/
/* 32*/	unsigned long ufltreg[32]; /*                                       */
/* 64*/	unsigned long ut_lo;       /*                                       */
/* 65*/	unsigned long ut_hi;       /*                                       */
/* 66*/	unsigned long uki_lo;      /*                                       */
/* 67*/	unsigned long uki_hi;      /*                                       */
/* 68*/	unsigned long ukr_lo;      /* These items must be in this order and */
/* 69*/	unsigned long ukr_hi;      /* at the beginning of the proc_state    */
/* 70*/	unsigned long umerge_lo;   /* structure.                            */
/* 71*/	unsigned long umerge_hi;   /*                                       */
/* 72*/	unsigned long ul1_lo;      /*                                       */
/* 73*/	unsigned long ul1_hi;      /*                                       */
/* 74*/	unsigned long ul2_lo;      /*                                       */
/* 75*/	unsigned long ul2_hi;      /*                                       */
/* 76*/	unsigned long ul3_lo;      /*                                       */
/* 77*/	unsigned long ul3_hi;      /*                                       */
/* 78*/	unsigned long ua1_lo;      /*                                       */
/* 79*/	unsigned long ua1_hi;      /*                                       */
/* 80*/	unsigned long ua2_lo;      /*                                       */
/* 81*/	unsigned long ua2_hi;      /*                                       */
/* 82*/	unsigned long ua3_lo;      /*                                       */
/* 83*/	unsigned long ua3_hi;      /*                                       */
/* 84*/	unsigned long um1_lo;      /*                                       */
/* 85*/	unsigned long um1_hi;      /*                                       */
/* 86*/	unsigned long um2_lo;      /*                                       */
/* 87*/	unsigned long um2_hi;      /*                                       */
/* 88*/	unsigned long um3_lo;      /*                                       */
/* 89*/	unsigned long um3_hi;      /*                                       */
/* 90*/	unsigned long ui3_lo;      /*                                       */
/* 91*/	unsigned long ui3_hi;      /*                                       */
/* 92*/	unsigned long ufsr1;       /*                                       */
/* 93*/	unsigned long ufsr2;       /*                                       */
/* 94*/	unsigned long ufsr;        /*                                       */
/* 95*/	unsigned long upsr;        /*                                       */
/* 96*/	unsigned long ufir;        /*                                       */
/* 97*/	unsigned long udirbase;    /*                                       */
/* 98*/	unsigned long udb;         /*                                       */
/* 99*/	unsigned long uepsr;        /*                                       */
/*100*/	unsigned long ufp1[2];     /*                                       */
/*102*/	unsigned long ufp2[2];     /*                                       */
/*104*/	unsigned long ufp3[2];     /*                                       */
/*106*/	unsigned long ufp4[2];     /*                                       */
/*108*/	unsigned long utrapped_opcode; /*                                   */
/*109*/	unsigned long urdest;      /*                                       */
/*110*/	unsigned long usrc1;       /*                                       */
/*111*/ unsigned long ur13;        /*                                       */
/*112*/ unsigned long ur14;        /*                                       */
/*113*/ unsigned long ur15;        /*****************************************/

} proc_state;;

