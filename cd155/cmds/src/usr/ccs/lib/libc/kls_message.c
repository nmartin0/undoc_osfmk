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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: kls_message.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:41 $";
#endif

/*
 * kls_message.c
 *
 * The file implements the functions that package requests
 * into messages.  Here are a few points about this implementation.
 * The only entity that creates requests is the client and the only
 * entity that creates replies is the server.  In general, messages
 * must only be created in order to be sent.  Each client and the
 * server are expected to send and/or receive only one message at a
 * time.  Single static buffers are used to hold the messages.
 */

#include <sys/types.h>
#include <errno.h>
#include <loader.h>

#include <loader/kloadsrv.h>
#include <loader/kls_message.h>

static char  client_send_buffer[KLS_MAX_MESSAGE_SIZE];

extern int errno;

/*
 * load()
 */
int
kls_message_create_load_request(file_pathname, load_flags, request)
	char *file_pathname;
	ldr_load_flags_t load_flags;
	kls_request_header_t **request;
{
	kls_load_request_t *p;
	long size;

	size = 	p->kls_file_pathname - ((char *)p) + strlen(file_pathname) + 1;
	if (size > KLS_MAX_MESSAGE_SIZE) {
		errno = EMSGSIZE;
		return(-errno);
	}
	p = (kls_load_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_LOAD_REQUEST;
	p->kls_msg_size = size;
	p->kls_load_flags = load_flags;
	(void)strcpy(p->kls_file_pathname, file_pathname);
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * unload()
 */
int
kls_message_create_unload_request(module, request)
	ldr_module_t module;
	kls_request_header_t **request;
{
	kls_unload_request_t *p;

	p = (kls_unload_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_UNLOAD_REQUEST;
	p->kls_msg_size = sizeof(*p);
	p->kls_module = module;
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * entry()
 */
int
kls_message_create_entry_request(module, request)
	ldr_module_t module;
	kls_request_header_t **request;
{
	kls_entry_request_t *p;

	p = (kls_entry_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_ENTRY_REQUEST;
	p->kls_msg_size = sizeof(*p);
	p->kls_module = module;
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * lookup()
 */
int
kls_message_create_lookup_request(module, symbol_name, request)
	ldr_module_t module;
	char *symbol_name;
	kls_request_header_t **request;
{
	kls_lookup_request_t *p;
	long size;

	size = 	p->kls_symbol_name - ((char *)p) + strlen(symbol_name) + 1;
	if (size > KLS_MAX_MESSAGE_SIZE) {
		errno = EMSGSIZE;
		return(-errno);
	}
	p = (kls_lookup_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_LOOKUP_REQUEST;
	p->kls_msg_size = size;
	p->kls_module = module;
	(void)strcpy(p->kls_symbol_name, symbol_name);
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * lookup_package()
 */
int
kls_message_create_lookup_package_request(package_name, symbol_name, request)
	char *package_name;
	char *symbol_name;
	kls_request_header_t **request;
{
	kls_lookup_package_request_t *p;
	long lpn, lsn;
	long size;

	lpn = strlen(package_name) + 1;
	lsn = strlen(symbol_name) + 1;
	size = 	p->kls_strings - ((char *)p) + lpn + lsn;
	if (size > KLS_MAX_MESSAGE_SIZE) {
		errno = EMSGSIZE;
		return(-errno);
	}
	p = (kls_lookup_package_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_LOOKUP_PACKAGE_REQUEST;
	p->kls_msg_size = size;
	p->kls_package_name_offset = 0;
	p->kls_symbol_name_offset = p->kls_package_name_offset + lpn;
	(void)strcpy(&p->kls_strings[p->kls_package_name_offset], package_name);
	(void)strcpy(&p->kls_strings[p->kls_symbol_name_offset], symbol_name);
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * next_module()
 */
int
kls_message_create_next_module_request(module, request)
	ldr_module_t module;
	kls_request_header_t **request;
{
	kls_next_module_request_t *p;

	p = (kls_next_module_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_NEXT_MODULE_REQUEST;
	p->kls_msg_size = sizeof(*p);
	p->kls_module = module;
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * inq_module()
 */
int
kls_message_create_inq_module_request(module, request)
	ldr_module_t module;
	kls_request_header_t **request;
{
	kls_inq_module_request_t *p;

	p = (kls_inq_module_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_INQ_MODULE_REQUEST;
	p->kls_msg_size = sizeof(*p);
	p->kls_module = module;
	*request = (kls_request_header_t *)p;
	return(0);
}

/*
 * inq_region()
 */
int
kls_message_create_inq_region_request(module, region, request)
	ldr_module_t module;
	kls_request_header_t **request;
{
	kls_inq_region_request_t *p;

	p = (kls_inq_region_request_t *)client_send_buffer;
	p->kls_msg_type = KLS_INQ_REGION_REQUEST;
	p->kls_msg_size = sizeof(*p);
	p->kls_module = module;
	p->kls_region = region;
	*request = (kls_request_header_t *)p;
	return(0);
}
