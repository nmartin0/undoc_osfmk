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
 * Copyright (c) 1988-1990 SecureWare, Inc.  All rights reserved.
 */
#ident "@(#)cron_sec.c	4.1 10:16:35 7/12/90 SecureWare"
/*
 * Based on:	@(#)cron_sec.c	2.16.1.1 18:08:21 12/29/89
 */

#include <sys/secdefines.h>

#if SEC_BASE

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dir.h>
#include <dirent.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>
#include <protcmd.h>

#if SEC_MAC
#include <mandatory.h>
#endif
#if SEC_NCAV
#include <ncav.h>
#endif

#define	CANTGETAUTH	"can't get authentication information for you.\nYour commands will not be executed."
#define	AUTHMISMATCH	"authentication mismatch on your account.\nYour commands will not be executed."
#define	CANTSETPRIV	"system privileges cannot be set for you.\nYour commands will not be executed."
#define	CANTSETLUID	"can't set login UID.\nYour commands will not be executed."
#if SEC_MAC
#define	CANTGETCLEAR	"can't get clearance.\nYour commands will not be executed."
#define	CANTSETCLEAR	"can't set clearance.\nYour commands will not be executed."
#define	CANTSETLEVEL	"can't set security level.\nYour commands will not be executed."
#endif
#if SEC_ILB
#define	CANTSETILEVEL	"can't set information label.\nYour commands will not be executed."
#endif
#if SEC_NCAV
#define CANTSETNCAV	"can't set nationality caveats.\nYour commands will not be executed."
#endif

#define	SECURE_MODE	0060	/* R/W by cron group only */

static int cron_user;
static struct pr_field *dummy;	/* used for sizeof below only */
static char this_user[sizeof(dummy->fd_name)];
static privvec_t our_sprivs;
static privvec_t our_bprivs;
static privvec_t nosprivs = { 0 };

#if SEC_MAC || SEC_NCAV
static char *message;
static int msg_size;
void cron_setlevel();
#if SEC_MAC
static mand_ir_t *subproclevel = (mand_ir_t *) 0;
#endif
#if SEC_NCAV
static ncav_ir_t ncav_level;
#endif
#endif

static void close_auth_files();
#if SEC_MAC
static void mldscan();
#endif

extern long time();
extern char *strcpy();
extern char *malloc();

extern priv_t *privvec();
#if SEC_MAC || SEC_NCAV
extern char *tempnam();
extern char *strrchr();
#endif

extern int errno;


/*
 * Startup initialization code.  Make sure that our luid is not set.
 */
cron_init(argc, argv)
	int	argc;
	char	*argv[];
{
	set_auth_parameters(argc, argv);
	initprivs();

	errno = 0;
	getluid();
	if (errno == 0)		/* no error, luid is already set */
		crabort("must be started by init.");

#if SEC_MAC
	if (mand_init() || (subproclevel = mand_alloc_ir()) == (mand_ir_t *) 0)
		crabort("cannot initialize for sensitivity labels.");
#endif
#if SEC_NCAV
	if (ncav_init())
		crabort("cannot initialize for nationality caveats.");
#endif
}


/*
 * Set the process file mask to a value that totally prevents unauthorized
 * access.
 */
int
cron_secure_mask()
{
	return umask(~SEC_DEFAULT_MODE);
}


/*
 * First, on the first pass through here, get the gid for the cron group.
 * Make the FIFO file so that at and crontab can communicate new jobs to the
 * daemon.  Make sure the file is only accessible through the cron group.
 *
 * By the time we leave this routine, the fifo file is good or it is removed.
 */
int
cron_set_communications(file, nofork)
	register char *file;
	int nofork;
{
	register int good_fifo = 0, save_umask;
	privvec_t saveprivs;
	struct stat stb;
	static been_here_before = 0;
	static int cron_group;

	if (!been_here_before)  {
		been_here_before = 1;
		cron_user = pw_nametoid("cron");
		if (cron_user == -1)
#ifdef SYSV_3
			crabort("cannot find cron user", 0);
#else
			crabort("cannot find cron user");
#endif
		cron_group = gr_nametoid("cron");
		if (cron_group == -1)
#ifdef SYSV_3
			crabort("cannot find cron group", 0);
#else
			crabort("cannot find cron group");
#endif
	}

	/*
	 * If there is a file already there, make sure it's a fifo that
	 * is only readable by the cron group.  If not, remove it so that
	 * we can replace it with a file that meets our needs.
	 */
	if (stat(file, &stb) == 0)  {
#ifdef SYSV_3
		if (nofork)
			sleep(60);
		crabort("cannot start cron; FIFO exists", 0);

#else
		if (stb.st_mode == (S_IFIFO | SECURE_MODE) &&
		    stb.st_uid == cron_user && stb.st_gid == cron_group)
			good_fifo = 1;
		else
			unlink(file);
#endif
	}
#ifdef SYSV_3
	else if (errno != ENOENT)  {
		if (nofork)
			sleep(60);
		perror("FIFO");
		crabort("cannot access fifo queue", 1);
	}
#endif

	/*
	 * If the FIFO is not there or had the wrong attributes,
	 * make a new one with correct attributes.
	 */
	if (!good_fifo) {
		good_fifo = 1;
		save_umask = umask(0);
		disableprivs(privvec(SEC_SUSPEND_AUDIT, -1), saveprivs);
		if (mknod(file, S_IFIFO | SECURE_MODE, 0) == -1 ||
		    chown(file, cron_user, cron_group) == -1)
			good_fifo = 0;
		umask(save_umask);

#if SEC_MAC || SEC_NCAV
		if (good_fifo && islabeledfs(file)) {
#if SEC_MAC
			if (chslabel(file, mand_syshi) == -1)
				good_fifo = 0;
#endif
#if SEC_NCAV
			if (chncav(file, ncav_max_ir) == -1)
				good_fifo = 0;
#endif
		}
#endif
		seteffprivs(saveprivs, (priv_t *) 0);
		if (!good_fifo)
#ifdef SYSV_3
			crabort("cannot create fifo queue", 1);
#else
			unlink(file);
#endif
	}

	return good_fifo;
}

#if SEC_MAC || SEC_NCAV
/*
 * Find an available file in the temp dir.  On systems configured with
 * SEC_MAC, the pathname must include the name of the MLD diversion
 * directory.
 */
char *
cron_tempnam(dir, prefix, level)
	char *dir;
	char *prefix;
	char *level;
{
	char *fullpath;

#if SEC_MAC
	{
		register char *subdirpath;
		register char *subdir;

		subdir = ir_to_subdir(dir, (mand_ir_t *) level);

		subdirpath = malloc(strlen(dir) + strlen(subdir) + 2);
		if (subdirpath == (char *) 0)
			fullpath = (char *) 0;
		else  {
			sprintf(subdirpath, "%s/%s", dir, subdir);
			fullpath = tempnam(subdirpath, prefix);
			free(subdirpath);
		}
	}
#else
	fullpath = tempnam(dir, prefix);
#endif

	cron_setlevel(level);
		
	return fullpath;
}


/*
 * Set the notion of the `current' security level.  This level is in
 * force while processing a new (file or fifo) request, executing a
 * task, or cleaning up a finished task.
 */
void
cron_setlevel(level)
	char *level;
{
#if SEC_MAC
	mand_copy_ir((mand_ir_t *) level, subproclevel);
	level += mand_bytes();
#endif
#if SEC_NCAV
	memcpy((char *) &ncav_level, level, sizeof ncav_level);
#endif
}

#if SEC_MAC
/*
 * Form the line for mailing the results of the job.  We form the longname
 * minus the name minus the subdirectory name, so the path appears as
 * it would to a process without the MULTILEVELDIR privilege.  This
 * assumes the subdirectory is the second to last component of longname.
 */
void
cron_mail_line(line, mailpgm, name, longname)
	char *line;
	char *mailpgm;
	char *name;
	char *longname;
{
	register char *last_comp;
	register char *seclast_comp;

	(void) sprintf(line, "%s %s < \"", mailpgm, name);

	last_comp = (char *) strrchr(longname, '/');
	if (last_comp == (char *) 0)
		strcat(line, longname);
	else  {
		*last_comp = '\0';
		seclast_comp = (char *) strrchr(longname, '/');
		*last_comp = '/';

		if (seclast_comp == (char *) 0)
			strcat(line, longname);
		else  {
			strncat(line, longname, seclast_comp - longname);
			strcat(line, last_comp);
		}
	}

	strcat(line, "\"\n");
}
#endif /* SEC_MAC */
#endif /* SEC_MAC || SEC_NCAV */


/*
 * Set the LUID for the user (it should not be set until now).
 * Also, set the privileges to what the user normally gets upon
 * login.  If we can't 1) set the LUID, 2) get the user from the
 * protected passwd database, or 3) set the privileges, don't
 * run the at or crontab file since it may have an insecure
 * environment.
 *
 * There is no need to do a chroot() if we separate all cron
 * daemons and run one for each restricted environment.  Because
 * cron (and at and crontab) have hard-wired locations for the
 * files it uses, it is not too hard to separate cron's -- one
 * for each environment.  Administratively, we just don't link the
 * FIFOs or work files.  Since this is not in the cron code, and
 * general users will not have access to these files, there is no
 * problem.
 */
void
#if SEC_MAC || SEC_NCAV
cron_set_user_environment(level, user_name, luid)
	char *level;
#else
cron_set_user_environment(user_name, luid)
#endif
	register char *user_name;
	register int luid;
{
	register struct pr_passwd *pr;
	register priv_t *ptr;
#if SEC_MAC
	register mand_ir_t *clearance;
#endif

	pr = getprpwnam(user_name);
	if (pr == (struct pr_passwd *) 0)  {
		audit_auth_entry(user_name, OT_PRPWD,
		   "cannot find Protected Password entry for cron verification",
				ET_SUBSYSTEM);
		mail(user_name, CANTGETAUTH, 2);
		exit(1);
	}

	/*
	 * If user's account is frozen, don't bother mailing to him because
	 * his account may be locked for a long time and the mail will
	 * pile up.
	 */
	if (locked_out(pr))
		exit(1);

	/*
	 * Check consistency between /etc/passwd and
	 * protected password entry.
	 */
	if (!pr->uflg.fg_uid || (pr->ufld.fd_uid != luid))  {
		audit_auth_entry(user_name, OT_PRPWD,
		  "mismatch in /etc/passwd and Protected Password UIDs",
		  ET_SUBSYSTEM);
		mail(user_name, AUTHMISMATCH, 2);
		exit(1);
	}

	/* check for retired account */

	if (pr->uflg.fg_retired && pr->ufld.fd_retired) {
		audit_auth_entry(user_name, OT_PRPWD,
		  "user cannot submit cron jobs because account is retired",
		  ET_SUBSYSTEM);
		exit(1);
	}

	/*
	 * Just like login, set a secure umask.  Let the user explicitly
	 * relax the mode.
	 */
	(void) umask (~SEC_DEFAULT_MODE);

#if SEC_MAC
	if (pr->uflg.fg_clearance)
		clearance = &pr->ufld.fd_clearance;
	else if (pr->sflg.fg_clearance)
		clearance = &pr->sfld.fd_clearance;
	else
		clearance = (mand_ir_t *) 0;

	if (clearance == (mand_ir_t *) 0)  {
		audit_subsystem("get clearance for at/cron session",
			"no clearance in protected password entry, session is aborted", ET_SUBSYSTEM);
		mail(user_name, CANTGETCLEAR, 2);
		exit(1);
	}

	if (setclrnce(clearance) != 0)  {
		audit_subsystem("set clearance for at/cron session",
			"setclrnce failed, session is aborted", ET_SEC_LEVEL);
		mail(user_name, CANTSETCLEAR, 2);
		exit(1);
	}

	if (setslabel((mand_ir_t *) level) != 0)  {
		audit_subsystem("set level for at/cron session",
			"setslabel failed, session is aborted", ET_SEC_LEVEL);
		mail(user_name, CANTSETLEVEL, 2);
		exit(1);
	}
	level += mand_bytes();
#endif
#if SEC_ILB
	if (setilabel(mand_syslo) != 0) {
		audit_subsystem(
			"set information label for at/cron session",
			"setilabel failed, session is aborted", ET_SEC_LEVEL);
		mail(user_name, CANTSETILEVEL, 2);
		exit(1);
	}
#endif
#if SEC_NCAV
	if (setncav((ncav_ir_t *) level) != 0) {
		audit_subsystem(
			"set nationality caveats for at/cron session",
			"setncav failed, session is aborted", ET_SEC_LEVEL);
		mail(user_name, CANTSETNCAV, 2);
		exit(1);
	}
#endif

	if (setluid(luid) != 0)  {
		audit_subsystem("set login UID for at/cron session",
			"setluid failed, session is aborted", ET_SUBSYSTEM);
		mail(user_name, CANTSETLUID, 2);
		exit(1);
	}

	if (pr->uflg.fg_sprivs)
		ptr = pr->ufld.fd_sprivs;
	else if (pr->sflg.fg_sprivs)
		ptr = pr->sfld.fd_sprivs;
	else
		ptr = nosprivs;
	memcpy(our_sprivs, ptr, sizeof our_sprivs);

	if (pr->uflg.fg_bprivs)
		ptr = pr->ufld.fd_bprivs;
	else if (pr->sflg.fg_bprivs)
		ptr = pr->sfld.fd_bprivs;
	else
		ptr = nosprivs;
	memcpy(our_bprivs, ptr, sizeof our_bprivs);

	/*
	 * Set the user's special audit parameters.
	 */
	audit_adjust_mask(pr);

	(void) strcpy(this_user, pr->ufld.fd_name);

	/*
	 * Make sure all the database files are closed so that the
	 * user does not get leftover file descriptors in his batch
	 * session.
	 */
	close_auth_files();
}


/*
 * We turn off self-audit after the UID and GID values are fixed so that 
 * when auditing starts up, the process will immediately reflect the user's
 * environment.
 *
 * Note:  This can only be used after cron_set_user_environment() because it
 *	  defines both user_name and our_sprivs;
 */
void
cron_adjust_privileges()
{
	if (setsysauths(our_sprivs) || setbaseprivs(our_bprivs)) {
		audit_subsystem("set privileges for at/cron session",
				"setpriv failed and session is aborted",
				ET_SUBSYSTEM);
		if (*this_user)
			mail(this_user, CANTSETPRIV, 2);
	}
#if SEC_MAC
	/*
	 * This is done so the mktemp file will be created in the
	 * appropriate subdirectory.
	 */
	disablepriv(SEC_MULTILEVELDIR);
#endif

	/*
	 * This is done so the nice() will give a high priority, if the
	 * job class calls for it.
	 */
	forcepriv(SEC_LIMIT);
}


/*
 * Close all open files.
 */
void
cron_close_files()
{
	register int scan_files;
	int nfile =
#if defined(AUX) || defined(BSD) || defined(_OSF_SOURCE)
			getdtablesize();
#else
			_NFILE;
#endif

	/*
	 * We must do this first so that the static information in the
	 * database files will be in sync with the true state of all the
	 * files being closed.
	 */
	close_auth_files();

	/*
	 * For those files where we can get access via file
	 * pointers, use fclose so that the buffers will drain.
	 */

	(void) fclose(stdin);
	(void) fclose(stdout);
	(void) fclose(stderr);

	for (scan_files = 0; scan_files < nfile; scan_files++)
		(void) close(scan_files);
}


/*
 * Sending mail must be done by a subprocess because the mail command
 * is SGID and we must set identity before the exec.  However, doing
 * a setluid() in the parent process spoils it for all future jobs.
 * This returns the PID of the child, 0 for the child itself, and -1 for
 * a failed fork.
 */
int
cron_mail_setup(same_proc)
	int same_proc;
{
	int pid;
	int status;
	privvec_t privs;

	if (same_proc)
		pid = 0;
	else
		pid = fork();

	if (pid == 0)  {
		(void) forcepriv(SEC_SETPROCIDENT);
		(void) setluid(cron_user);
		(void) setuid(cron_user);
		(void) disablepriv(SEC_SETPROCIDENT);

#if SEC_MAC
		(void) disablesysauth(SEC_MULTILEVELDIR);
		if ((setclrnce(subproclevel) != 0) ||
		    (setslabel(subproclevel) != 0))
			exit(1);
#endif
#if SEC_ILB
		if (setilabel(mand_syslo))
			exit(1);
#endif
#if SEC_NCAV
		if (setncav(&ncav_level) != 0)
			exit(1);
#endif
	}

	return pid;
}


/*
 * The only thing the subprocess needs to do is mail the message.
 * After that, it is done.
 */
void
cron_mail_finish()
{
	exit(0);
}


#if SEC_MAC
void
cron_existing_jobs(read_error, fn, iscrontab)
	char *read_error;
	int (*fn)();
	int iscrontab;
{
	MDIR *dir;

	dir = openmultdir(".", MAND_MLD_ALLDIRS, (mand_ir_t *) 0);
	if (dir == (MDIR *) 0)  {
		if (iscrontab)
#ifdef SYSV_3
			crabort(read_error, 1);
#else
			crabort(read_error);
#endif
		else
			msg(read_error);
	}
	else  {
		mldscan(dir, fn);
		closemultdir(dir);
	}
}


char *
cron_jobname(curr_dir_code, req_dir_code, dir_name, user_name, buf)
	int curr_dir_code;
	int req_dir_code;
	char *dir_name;
	char *user_name;
	register char *buf;
{
	register char *subdir;

	if(curr_dir_code != req_dir_code)
		strcpy(buf, dir_name);
	else
		strcpy(buf, ".");

	/*
	 * Get the name of the subdirectory with the given IR.
	 * If we can't get the name, fill in the buffer with a path that
	 * will make the path check fail.
	 */
	subdir = ir_to_subdir(buf, subproclevel);
	if (subdir == (char *) 0)  {
		buf[0] = (char) ~0;
		buf[1] = 0;
	}
	else  {
		strcat(buf, "/");
		strcat(buf, subdir);
		strcat(buf, "/");
		strcat(buf, user_name);
	}

	return buf;
}
#endif /* SEC_MAC */

#if SEC_MAC || SEC_NCAV
char *
cron_getlevel()
{
	register char *cp, *buffer;
	int buflen = 0;

#if SEC_MAC
	buflen += mand_bytes();
#endif
#if SEC_NCAV
	buflen += sizeof(ncav_ir_t);
#endif
	buffer = malloc(buflen);
	cp = buffer;
	if (cp != (char *) 0) {
#if SEC_MAC
		mand_copy_ir(subproclevel, (mand_ir_t *) cp);
		cp += mand_bytes();
#endif
#if SEC_NCAV
		memcpy(cp, (char *) &ncav_level, sizeof ncav_level);
#endif
	}
	return buffer;
}


char *
cron_set_message(base_size)
	int base_size;
{
	if (message == (char *) 0)  {
		msg_size = base_size;
#if SEC_MAC
		msg_size += mand_bytes();
#endif
#if SEC_NCAV
		msg_size += sizeof(ncav_ir_t);
#endif
		message = malloc(msg_size);
	}

	return message;
}


/*
 * Read the message from at or crontab and save the security level
 * sent as part of the message to be used later in job identification.
 */
int
cron_read_message(fd, buffer, orig_size, amt_read)
	int fd;
	char *buffer;
	int orig_size;
	int *amt_read;
{
	int correct_size;

	*amt_read = read(fd, buffer, msg_size);
	correct_size = (*amt_read == msg_size);

	if (correct_size)
		cron_setlevel(buffer + orig_size);
	return correct_size;
}


/*
 * Release the space for the security level when the job is deleted.
 */
void
cron_release_ir(level)
	char *level;
{
	if (level != (char *) 0)
		free(level);
}
#endif /* SEC_MAC || SEC_NCAV */


#if SEC_MAC
/*
 * Job matching involves a match of both the user AND the security
 * level.
 */
int
cron_id_match(req_name, name, level)
	char *req_name;
	char *name;
	char *level;
{
	int equal;

	equal = ((strcmp(name, req_name) == 0) &&
		 (memcmp(level, subproclevel, mand_bytes()) == 0));

	return equal;
}
#endif


/*
 * Close all Authentication database-related files.
 */
static void
close_auth_files()
{
	endpwent();
	endgrent();
	endprpwent();
	endprfient();
	endprdfent();
}


#if SEC_MAC
/*
 * Scan all the files within a multilevel directory and call fn for
 * each of the files.
 */
static void
mldscan(df, fn)
	MDIR *df;
	int (*fn)();
{

	register	i;
	struct		dirent	*dp;
	char		fullname[256];
	char            compname[NAME_MAX + 1];
	int		ret;

	readmultdir(df, fullname, &dp);

	while (dp != (struct dirent *) 0)  {
		strcat(fullname, "/");
		strcat(fullname, dp->d_name);
		if (statslabel(fullname, subproclevel) == 0) {
#if SEC_NCAV
			ret = statncav(fullname, &ncav_level);
#endif
			/* some systems use a static buffer for readdir */
			strcpy(compname, dp->d_name);
			(*fn) (compname);
		}
		readmultdir(df, fullname, &dp);
	}
}
#endif /* SEC_MAC */

#endif /* SEC_BASE */
