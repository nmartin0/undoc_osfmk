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
static char rcsid[] = "@(#)$RCSfile: kls_client.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:40 $";
#endif

#include <sys/types.h>
#include <loader.h>

#include <loader/kloadsrv.h>
#include <loader/kls_ipc.h>
#include <loader/kls_message.h>

int
kls_client_load(file_pathname, load_flags, module)
	char *file_pathname;
	ldr_load_flags_t load_flags;
	ldr_module_t *module;
{
	kls_request_header_t *request;
	kls_load_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_load_request(file_pathname, load_flags,
	    &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	*module = reply->kls_module;
	return(reply->kls_return_value);
}

int
kls_client_unload(module)
	ldr_module_t *module;
{
	kls_request_header_t *request;
	kls_unload_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_unload_request(module, &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	return(reply->kls_return_value);
}

int
kls_client_entry(module, entry_pt)
	ldr_module_t module;
	ldr_entry_pt_t *entry_pt;
{
	kls_request_header_t *request;
	kls_entry_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_entry_request(module, &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	*entry_pt = reply->kls_entry_pt;
	return(reply->kls_return_value);
}

int
kls_client_lookup(module, symbol_name, symbol_addr_ptr)
	ldr_module_t *module;
	char *symbol_name;
	void **symbol_addr_ptr;
{
	kls_request_header_t *request;
	kls_lookup_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_lookup_request(module, symbol_name,
	    &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	*symbol_addr_ptr = reply->kls_symbol_addr;
	return(reply->kls_return_value);
}

int
kls_client_lookup_package(package_name, symbol_name, symbol_addr_ptr)
	char *package_name;
	char *symbol_name;
	void **symbol_addr_ptr;
{
	kls_request_header_t *request;
	kls_lookup_package_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_lookup_package_request(package_name, symbol_name,
	    &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	*symbol_addr_ptr = reply->kls_symbol_addr;
	return(reply->kls_return_value);
}

int
kls_client_next_module(module)
	ldr_module_t *module;
{
	kls_request_header_t *request;
	kls_next_module_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_next_module_request(*module, &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	*module = reply->kls_module;
	return(reply->kls_return_value);
}

int
kls_client_inq_module(module, info, info_size, ret_size)
	ldr_module_t module;
	ldr_module_info_t *info;
	size_t info_size;
	size_t *ret_size;
{
	kls_request_header_t *request;
	kls_inq_module_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_inq_module_request(module, &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	(void)memcpy((void *)info, (void *)&reply->kls_info, info_size);
	*ret_size = reply->kls_ret_size;
	return(reply->kls_return_value);
}

int
kls_client_inq_region(module, region, info, info_size, ret_size)
	ldr_module_t module;
	ldr_region_t region;
	ldr_region_info_t *info;
	size_t info_size;
	size_t *ret_size;
{
	kls_request_header_t *request;
	kls_inq_region_reply_t *reply;
	int rc;

	if ((rc = kls_message_create_inq_region_request(module, region,
	    &request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_send_request(request)) < 0)
		return(rc);
	if ((rc = kls_client_ipc_receive_reply((kls_reply_header_t **)&reply)) < 0)
		return(rc);
	(void)memcpy((void *)info, (void *)&reply->kls_info, info_size);
	*ret_size = reply->kls_ret_size;
	return(reply->kls_return_value);
}
