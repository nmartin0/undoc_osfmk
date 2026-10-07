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

#ifndef _NO_PROTO

extern int
kls_message_create_kls_load_request(char *, ldr_load_flags_t, kls_request_header_t **);

extern int
kls_message_create_kls_unload_request(ldr_module_t, kls_request_header_t **);

extern int
kls_message_create_kls_entry_request(ldr_module_t, kls_request_header_t **);

extern int
kls_message_create_kls_lookup_request(ldr_module_t, char *, kls_request_header_t **);

extern int
kls_message_create_lookup_package_request(char *, char *, kls_request_header_t **);

extern int
kls_message_create_kls_next_module_request(ldr_module_t, kls_request_header_t **);

extern int
kls_message_create_kls_inq_region_request(ldr_module_t, ldr_region_t, kls_request_header_t **);

#else

extern int
kls_message_create_kls_load_request();

extern int
kls_message_create_kls_unload_request();

extern int
kls_message_create_kls_entry_request();

extern int
kls_message_create_kls_lookup_request();

extern int
kls_message_create_lookup_package_request();

extern int
kls_message_create_kls_next_module_request();

extern int
kls_message_create_kls_inq_region_request();

#endif
