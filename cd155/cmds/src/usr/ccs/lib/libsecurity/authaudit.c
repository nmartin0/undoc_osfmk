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
/*
 * Copyright (c) 1988-90 SecureWare, Inc.  All rights reserved.
 */

#ident "@(#)authaudit.c	5.1 16:46:43 8/15/90 SecureWare"
/*
 * Based on:
 *	@(#)authaudit.c	2.10.1.2 11:42:06 2/3/90
 */

/*
 * This file contains a set of routines used to make programs
 * more secure.  Specifically, it contains routines responsible
 * for producing audit records dealing with the authentication
 * database.
 */

#include <sys/secdefines.h>

#if SEC_BASE /*{*/

#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/security.h>
#include <sys/secioctl.h>
#include <sys/audit.h>
#include <prot.h>
#include <signal.h>

#define	ROOT_DIR		"/"
#define COMMAND_LENGTH		255
#define NOT_VALID_SUBSYSTEM	"NoNe"

#if SEC_MAC
char *er_buffer;	/* for Sensitivity level auditing */
#endif
#if SEC_NCAV
char *ncav_buffer;	/* for Caveat set auditing */
#endif

static void audit_write();
static void generate_audit_record();

extern char *strrchr();
extern char *strncpy();
extern char *calloc();
extern long time();

extern priv_t	*privvec();

/*
 *
 *	This routine produces an audit record noting a security failure
 *	in a protected subsystem or TCB.  Even though kernel auditing may be
 *	done at the system call level, this auditing done here notes a
 *	high-level security problem having significance to a subsystem.
 *
 */

void
audit_security_failure(object, action, result, event_type)

int	object;
char	*action;
char	*result;
int	event_type;

{
	struct database_activity	*record_ptr;
	char			*trailer_ptr;
	int			record_size = 0;
	int			command_size = 0;
	int			action_size = 0;
	int			result_size = 0;

	command_size = strlen(command_name) + 1;

	if (action != (char *) 0)
		action_size = strlen(action) + 1;

	if (result != (char *) 0)
		result_size = strlen(result) + 1;
	
	record_size = DATABASEACTIVITY_SIZE + command_size + 
		      action_size + result_size;

	record_ptr = (struct database_activity *) calloc(1, record_size);
	if (record_ptr == (struct database_activity *) 0)
		return;

	record_ptr->aud_hdr.rec_length  = record_size;
	record_ptr->aud_hdr.tstamp      = time((long *) 0);
	record_ptr->aud_hdr.event_type  = event_type;
	record_ptr->aud_hdr.record_type = RT_DATABASE;
	record_ptr->aud_hdr.pid         = getpid();

	record_ptr->command.count = command_size;
	record_ptr->action.count  = action_size;
	record_ptr->result.count  = result_size;
	record_ptr->code          = ES_SEC_FAILURE;
	record_ptr->object        = (ushort) object;

	trailer_ptr = (char *) (record_ptr + 1);
	if (command_size)
		strncpy(trailer_ptr, command_name, command_size);
	trailer_ptr += command_size;
	if (action_size)
		strncpy(trailer_ptr, action, action_size);
	trailer_ptr += action_size;
	if (result_size)
		strncpy(trailer_ptr, result, result_size);

	generate_audit_record((char *) record_ptr, record_size);
	free((char *) record_ptr);
}

/*
 *
 *	In an action with the TCB, record both that a resource has expired
 *	and the problem for the TCB that results from the resource depletion.
 *	An audit record is produced for this.
 *
 */

void
audit_no_resource(resource, object, problem, event_type)

char	*resource;
int	object;
char	*problem;
int	event_type;

{
	struct database_activity	*record_ptr;
	char			*trailer_ptr;
	int			record_size;
	int			command_size = 0;
	int			resource_size = 0;
	int			problem_size = 0;

	command_size = strlen(command_name) + 1;

	if (resource != (char *) 0)
		resource_size = strlen(resource) + 1;

	if (problem != (char *) 0)
		problem_size = strlen(problem) + 1;
	
	record_size = DATABASEACTIVITY_SIZE + command_size + 
		      resource_size + problem_size;

	record_ptr = (struct database_activity *) calloc(1, record_size);
	if (record_ptr == (struct database_activity *) 0)
		return;

	record_ptr->aud_hdr.rec_length  = record_size;
	record_ptr->aud_hdr.tstamp      = time((long *) 0);
	record_ptr->aud_hdr.event_type  = event_type;
	record_ptr->aud_hdr.record_type = RT_DATABASE;
	record_ptr->aud_hdr.pid         = getpid();

	record_ptr->command.count = command_size;
	record_ptr->action.count  = resource_size;
	record_ptr->result.count  = problem_size;
	record_ptr->code          = ES_DB_RESOURCE;
	record_ptr->object        = (ushort) object;

	trailer_ptr = (char *) (record_ptr + 1);
	if (command_size)
		strncpy(trailer_ptr, command_name, command_size);
	trailer_ptr += command_size;
	if (resource_size)
		strncpy(trailer_ptr, resource, resource_size);
	trailer_ptr += resource_size;
	if (problem_size)
		strncpy(trailer_ptr, problem, problem_size);

	generate_audit_record((char *) record_ptr, record_size);
	free((char *) record_ptr);
}

/*
 *
 *	Produce an audit record when something is wrong with the Authentication
 *	database.  Note the type of database, the entry name, and the problem.
 *
 */

void
audit_auth_entry(desired_entry, type, problem, event_type)

char	*desired_entry;
int	type;
char	*problem;
int	event_type;

{
	struct database_activity	*record_ptr;
	char			*trailer_ptr;
	int			record_size;
	int			command_size = 0;
	int			entry_size = 0;
	int			problem_size = 0;

	command_size = strlen(command_name) + 1;

	if (desired_entry != (char *) 0)
		entry_size = strlen(desired_entry) + 1;

	if (problem != (char *) 0)
		problem_size = strlen(problem) + 1;
	
	record_size = DATABASEACTIVITY_SIZE + command_size + 
		      entry_size + problem_size;

	record_ptr = (struct database_activity *) calloc(1, record_size);
	if (record_ptr == (struct database_activity *) 0)
		return;

	record_ptr->aud_hdr.rec_length  = record_size;
	record_ptr->aud_hdr.tstamp      = time((long *) 0);
	record_ptr->aud_hdr.event_type  = event_type;
	record_ptr->aud_hdr.record_type = RT_DATABASE;
	record_ptr->aud_hdr.pid         = getpid();

	record_ptr->command.count = command_size;
	record_ptr->action.count  = entry_size;
	record_ptr->result.count  = problem_size;
	record_ptr->code          = ES_DB_INTEGRITY;
	record_ptr->object        = (ushort) type;

	trailer_ptr = (char *) (record_ptr + 1);
	if (command_size)
		strncpy(trailer_ptr, command_name, command_size);
	trailer_ptr += command_size;
	if (entry_size)
		strncpy(trailer_ptr, desired_entry, entry_size);
	trailer_ptr += entry_size;
	if (problem_size)
		strncpy(trailer_ptr, problem, problem_size);

	generate_audit_record((char *) record_ptr, record_size);
	free((char *) record_ptr);
}

/*
 *
 *	Audit an action peculiar to a subsystem.  Note the subsystem, the
 *	action that had been occurring, and the result of the action.
 *
 */

void
audit_subsystem(action, result, event_type)

char	*action;
char	*result;
int	event_type;

{
	struct subsystem_activity	*record_ptr;
	char			*trailer_ptr;
	int			record_size;
	int			command_size = 0;
	int			result_size = 0;
	int			action_size = 0;

	command_size = strlen(command_line) + 1;

	if (action != (char *) 0)
		action_size = strlen(action) + 1;

	if (result != (char *) 0)
		result_size = strlen(result) + 1;
	
	record_size = SUBSYSTEMACTIVITY_SIZE + command_size + 
		      action_size + result_size;

	record_ptr = (struct subsystem_activity *) calloc(1, record_size);
	if (record_ptr == (struct subsystem_activity *) 0)
		return;

	record_ptr->aud_hdr.rec_length  = record_size;
	record_ptr->aud_hdr.tstamp      = time((long *) 0);
	record_ptr->aud_hdr.event_type  = event_type;
	record_ptr->aud_hdr.record_type = RT_SUBSYSTEM;
	record_ptr->aud_hdr.pid         = getpid();

	record_ptr->command.count = command_size;
	record_ptr->action.count  = action_size;
	record_ptr->result.count  = result_size;
	record_ptr->egid          = starting_egid();

	trailer_ptr = (char *) (record_ptr + 1);
	if (command_size)
		strncpy(trailer_ptr, command_line, command_size);
	trailer_ptr += command_size;
	if (action_size)
		strncpy(trailer_ptr, action, action_size);
	trailer_ptr += action_size;
	if (result_size)
		strncpy(trailer_ptr, result, result_size);

	generate_audit_record((char *) record_ptr, record_size);
	free((char *) record_ptr);
}

/*
 * Audit both successful and unsuccessful logins.  This audit record is always
 * done directly to the audit device.
 */

void
audit_login(pr, pwd, terminal_name, problem, code)

register struct pr_passwd	*pr;
register struct passwd		*pwd;
register char			*terminal_name;
char				*problem;
int				code;

{
#if SEC_MAC
	register int	size_sec_level;
#endif
#if SEC_NCAV
	register int	size_ncav;
#endif
	register struct login_audit	*record;
	register char	*record_trailer;
	register char	*current_directory;
	register int	size_cdir;
	register int	size_terminal;
	register int	tot_size;
	int		size_problem;
	int		calloced = 0;
	struct stat	statbuf;
#if SEC_GROUPS
	int		groups[SEC_GROUPS];
	int		size_groups;
#endif


	if ((pwd != (struct passwd *) 0) && (chdir(pwd->pw_dir) == 0))
		current_directory = pwd->pw_dir;
	else if (chdir(ROOT_DIR) == 0)
		current_directory = ROOT_DIR;
	else
		current_directory = ".";
	size_cdir = strlen(current_directory) + 1;

	if (problem == (char *) 0)
		size_problem = 0;
	else
		size_problem = strlen(problem) + 1;

	if (terminal_name == (char *) 0)
		size_terminal = 0;
	else
		size_terminal = strlen(terminal_name) + 1;
#if SEC_GROUPS
	size_groups = getgroups(SEC_GROUPS, groups) * sizeof(gid_t);
#endif
#if SEC_MAC
	if (er_buffer == (char *) 0)
		size_sec_level = 0;
	else
		size_sec_level = strlen(er_buffer) + 1;
#endif
#if SEC_NCAV
	if (ncav_buffer == (char *) 0)
		size_ncav = 0;
	else
		size_ncav = strlen(ncav_buffer) + 1;
#endif

	/*
	 * Allocate an audit buffer.
	 */
	tot_size = sizeof(struct login_audit) + size_cdir + size_terminal +
		   size_problem;
#if SEC_MAC
	tot_size += size_sec_level;
#endif
#if SEC_NCAV
	tot_size += size_ncav;
#endif
#if SEC_GROUPS
	tot_size += size_groups;
#endif
	record = (struct login_audit *) calloc(1, tot_size);
	if (record == (struct login_audit *) 0)
		return;

	/*
	 * Fill in fixed login audit record.
	 */
	record->aud_hdr.rec_length = tot_size;
	record->aud_hdr.event_type = ET_LOGIN;
	record->aud_hdr.record_type = RT_LOGIN;
	if (pr && pr->uflg.fg_name)
		strncpy(record->username, pr->ufld.fd_name,
			sizeof(record->username));
	else
		strncpy(record->username, "UNKNOWN", sizeof(record->username));
	record->code = (ushort) code;
	if (pr && pr->uflg.fg_uid)
		record->luid = pr->ufld.fd_uid;
	else
		record->luid = getluid();
	if (pwd == (struct passwd *) 0)
		record->rgid = getgid();
	else
		record->rgid = pwd->pw_gid;
	if (fstat(fileno(stdin), &statbuf) == 0)
		record->ttyd = statbuf.st_rdev;
	else
		record->ttyd = (dev_t) 0;
#if SEC_GROUPS
	record->groups.count = size_groups;
#endif
	record->problem.count = size_problem;
	record->cdir.count = size_cdir;
	record->terminal.count = size_terminal;
#if SEC_MAC
	record->sec_level.count = size_sec_level;
#endif
#if SEC_NCAV
	record->ncav.count = size_ncav;
#endif

	/*
	 * Start putting in the variable length strings
	 * immediately after the audit record, so we
	 * can do this in one write operation.
#if SEC_GROUPS
	 * NOTE: multiple groups must go first for machine
	 * architectures that require aligned values.
#endif
	 */
	record_trailer = (char *) (record+1);
#if SEC_GROUPS
	if (size_groups > 0) {
		int	i;
		gid_t	*rt_groups = (gid_t *) record_trailer;

		for (i = 0; i < (size_groups / sizeof(gid_t)) ; i++)
			*rt_groups++ = (gid_t) groups[i];
		record_trailer = (char *) rt_groups;
	}
#endif
	if (problem != (char *) 0)
		strncpy(record_trailer,problem, size_problem);
	record_trailer += size_problem;
	strncpy(record_trailer, current_directory, size_cdir);
	record_trailer += size_cdir;
	if (terminal_name != (char *) 0)
		strncpy(record_trailer, terminal_name, size_terminal);
#if SEC_MAC
	record_trailer += size_terminal;
	if (er_buffer != (char *) 0)
		strncpy(record_trailer, er_buffer, size_sec_level);
#endif
#if SEC_NCAV
	record_trailer += size_sec_level;
	if (ncav_buffer != (char *) 0)
		strncpy(record_trailer, ncav_buffer, size_ncav);
#endif
	generate_audit_record((char *) record, tot_size);
	free((char *) record);
}


/*
 * Audit both successful and unsuccessful password changes.  This audit
 * record is always done directly to the audit device.
 */

void
audit_passwd(name, code, event_type)

char	*name;
int	code;
int	event_type;

{
	register struct passwd_audit *record;


	record = (struct passwd_audit *) calloc(sizeof(*record), 1);
	if (record == (struct passwd_audit *) 0)
		return;

	/*
	 * Fill in password audit record.
	 */
	record->aud_hdr.rec_length = sizeof(struct passwd_audit);
	record->aud_hdr.event_type = event_type;
	record->aud_hdr.record_type = RT_PASSWORD;
	strncpy(record->username, name, sizeof(record->username));
	record->code = (ushort) code;
	generate_audit_record((char *) record, sizeof(struct passwd_audit));
	free((char *) record);
}


/*
 * Audit both successful and unsuccessful database locking operations.
 * This audit record is always done directly to the audit device.
 */

void
audit_lock(name, code, trys, event_type)

char	*name;
int	code;
int	trys;
int	event_type;

{
	register struct lock_audit *record;

	record = (struct lock_audit *) calloc(sizeof(*record), 1);
	if (record == (struct lock_audit *) 0)
		return;

/*
 *
 *	Fill in lock audit record.
 *
 */

	record->aud_hdr.rec_length = sizeof(struct lock_audit);
	record->aud_hdr.event_type = event_type;
	record->aud_hdr.record_type = RT_LOCK;
	strncpy(record->username, name, sizeof(record->username));
	record->code = (ushort) code;
	record->trys = (ushort) trys;
	generate_audit_record((char *) record, sizeof(struct lock_audit));
	free((char *) record);
}


/*
 * If the user has a special mask, present it here to the audit subsystem.
 */

void
audit_adjust_mask(pr)
	register struct pr_passwd *pr;
{
	register int audit;
	register int scan_mask;
	struct audit_ioctl buf;

	if (pr->uflg.fg_auditcntl && pr->uflg.fg_auditdisp)  {
		for (scan_mask = 0; scan_mask < AUDIT_MASK_SIZE; scan_mask++)  {
			buf.user_control[scan_mask] =
				(long) pr->ufld.fd_auditcntl[scan_mask];
			buf.user_disp[scan_mask] =
				(long) pr->ufld.fd_auditdisp[scan_mask];
		}

		audit = open (AUDIT_WDEVICE, 1);
		if (audit != -1)  {
			ioctl(audit, AUDIOC_USERMASK, &buf);
			close(audit);
		}
	}
}

/*
 *
 *	Produce an audit record for lock and unlock of user terminals.
 *
 */

void
sa_audit_lock(code, user_term)

int	code;
char	*user_term;

{
	struct lock_audit	*record_ptr;
	int			record_size;

	record_size = LOCKAUDIT_SIZE;
	record_ptr = (struct lock_audit *) calloc(1, record_size);
	if (record_ptr == (struct lock_audit *) 0)
		return;

	record_ptr->aud_hdr.rec_length  = record_size;
	record_ptr->aud_hdr.tstamp      = time((long *) 0);
	record_ptr->aud_hdr.event_type  = ET_SYS_ADMIN;
	record_ptr->aud_hdr.record_type = RT_LOCK;
	record_ptr->aud_hdr.pid         = getpid();

	record_ptr->code = code;
	record_ptr->trys = 0;
	strncpy(record_ptr->username, user_term, sizeof(record_ptr-> username));
	generate_audit_record((char *) record_ptr, record_size);
	free((char *) record_ptr);
}

/*
 * This routine produces an audit record for the audit record type
 * The routine writes directly to the audit device, because processes
 * running on behalf of the audit subsystem have selfaudit privilege.
 */
void
sa_audit_audit(code, string)
	int code;
	char *string;
{
	register int tot_size;
	register struct audit_actions *record;
	register int string_size;
	register char *record_trailer;
	int calloced = 0;

	if (string == (char *) 0)
		string_size = 0;
	else
		string_size = strlen(string) + 1;

	tot_size = sizeof(struct audit_actions) + string_size;
	record = (struct audit_actions *) calloc(1, tot_size);
	if (record == (struct audit_actions *) 0)
		return;

	record->aud_hdr.rec_length = tot_size;
	record->aud_hdr.tstamp = time ((long *) 0);
	record->aud_hdr.event_type = ET_SYS_ADMIN;
	record->aud_hdr.record_type = RT_AUDIT;
	record->aud_hdr.pid = getpid();
	record->code = code;
	record_trailer = (char *) (record + 1);
	if (string_size > 0)
		strncpy(record_trailer, string, string_size);

	generate_audit_record((char *) record, tot_size);
	free((char *) record);
}

static void
generate_audit_record(audit_record, record_size)

char	*audit_record;
int	record_size;

{
	char	*subsystem_name;
	char	subsystem_filename[sizeof(AUTH_SUBSYS_TICKET) + 1];
	int	subsystem_fd;
	FILE	*file_ptr;
	char	command_buffer[COMMAND_LENGTH];
	privvec_t	saveprivs;

	if (forceprivs(privvec(SEC_WRITE_AUDIT, -1), saveprivs) == 0) {
		audit_write(audit_record, record_size);
		seteffprivs(saveprivs, (priv_t *) 0);
		return;
	}

	subsystem_name = gr_idtoname(starting_egid());
	if (subsystem_name == (char *) 0)
		subsystem_name = NOT_VALID_SUBSYSTEM;
	strcpy(subsystem_filename, AUTH_SUBSYS_TICKET);
	strcpy(strrchr(subsystem_filename, '/') + 1, subsystem_name);
	if (strcmp(subsystem_name, NOT_VALID_SUBSYSTEM) == 0)
		subsystem_fd = 0;
	else
		subsystem_fd = open(subsystem_filename, O_RDONLY);
	memset(command_buffer, '\0', COMMAND_LENGTH);
	sprintf(command_buffer, "%s -f%d -s%s", AUTH_AUDITSUB, 
			      subsystem_fd, subsystem_name);
	file_ptr = popen(command_buffer, "w");
	if (file_ptr != (FILE *) 0) {
		setbuf(file_ptr, NULL);
		fwrite(audit_record, record_size, 1, file_ptr);
		pclose(file_ptr);
	}
	if (strcmp(subsystem_name, NOT_VALID_SUBSYSTEM) != 0)
		close(subsystem_fd);
}

/*
 *	Write to the audit trail directly.
 *
 *	Assumes that the caller has the authorization to do the write.
 *
 */

static void
audit_write(audit_record, record_size)

char	*audit_record;
int	record_size;

{
	int	audit_file_ptr;

	audit_file_ptr = open(AUDIT_WDEVICE, 1);
	if (audit_file_ptr != -1) {
		write(audit_file_ptr, audit_record, record_size);
		close(audit_file_ptr);
	}
}

#endif /*} SEC_BASE */
