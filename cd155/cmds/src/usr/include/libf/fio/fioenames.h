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
 *	Copyright 1991, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/* fioenames.h - define names of functions called from outside ftn I/O lib: */

#define Ftn_exit ftn_exit
#define Fio_src_info fio_src_info
#define Fio_open fio_open
#define Fio_nopen fio_nopen
#define Fio_close fio_close
#define Fio_inquire fio_inquire
#define Fio_ninquire fio_ninquire
#define Fio_rewind fio_rewind
#define Fio_backspace fio_backspace
#define Fio_swbackspace fio_swbackspace
#define Fio_endfile fio_endfile
#define Fio_encode_fmt fio_encode_fmt

#define Fio_fmtr_init fio_fmtr_init
#define Fio_fmtr_intern_init fio_fmtr_intern_init
#define Fio_fmt_read fio_fmt_read
#define Fio_fmtr_end fio_fmtr_end

#define Fio_fmtw_init fio_fmtw_init
#define Fio_fmtw_intern_init fio_fmtw_intern_init
#define Fio_fmt_write fio_fmt_write
#define Fio_fmtw_end fio_fmtw_end

#define Fio_ldr_init fio_ldr_init
#define Fio_ldr_intern_init fio_ldr_intern_init
#define Fio_ldr fio_ldr
#define Fio_ldr_end fio_ldr_end

#define Fio_ldw_init fio_ldw_init
#define Fio_ldw_intern_init fio_ldw_intern_init
#define Fio_ldw fio_ldw
#define Fio_ldw_end fio_ldw_end

#define Fio_nml_read fio_nml_read
#define Fio_nml_write fio_nml_write

#define Fio_unf_init fio_unf_init
#define Fio_unf_read fio_unf_read
#define Fio_unf_write fio_unf_write
#define Fio_unf_end fio_unf_end

#define Fio_usw_init fio_usw_init
#define Fio_usw_read fio_usw_read
#define Fio_usw_write fio_usw_write
#define Fio_usw_end fio_usw_end
