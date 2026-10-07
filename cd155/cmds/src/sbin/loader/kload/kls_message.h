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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

extern int
kls_message_create_kls_load_reply(int, ldr_module_t, kls_reply_header_t **);

extern int
kls_message_create_kls_unload_reply(int, kls_reply_header_t **);

extern int
kls_message_create_kls_entry_reply(int, ldr_entry_pt_t, kls_reply_header_t **);

extern int
kls_message_create_kls_lookup_reply(int, void *, kls_reply_header_t **);

extern int
kls_message_create_kls_lookup_package_reply(int, void *, kls_reply_header_t **);

extern int
kls_message_create_kls_next_module_reply(int, ldr_module_t, kls_reply_header_t **);

extern int
kls_message_create_kls_inq_module_reply(int, ldr_module_info_t *, kls_reply_header_t **);

extern int
kls_message_create_kls_inq_region_reply(int, ldr_region_info_t *, kls_reply_header_t **);
