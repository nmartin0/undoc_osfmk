/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	Copyright (c) Locus Computing, 1991-92
 * 	This is UNPUBLISHED source code that is
 * 	the property of Locus Computing, containing
 *	proprietary secrets of LCC.  Any disclosure
 *	is strictly prohibited.  Locus makes no warantee,
 *	explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: cdb.c,v $
 * Revision 1.2  1994/11/18  20:52:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:47:50  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:07:25  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.1  93/06/10  11:54:24  chrisp
 * 	$EndLog$
 * 
 */
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#define	SHOW_UTT
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/access.h>
#include <sys/mode.h>
#include <sys/fcntl.h>
#include <uxkern/import_mach.h>
#include <uxkern/bsd_types.h>
#define	_KERNEL
#include <tnc/chkpnt.h>

boolean_t	dump_proc_state = TRUE;
boolean_t	dump_image = FALSE;
boolean_t	dump_mi = FALSE;
boolean_t	dump_thread_state = FALSE;
boolean_t	dump_file_state = FALSE;

main(int argc, char *argv[])
{
	int		error = 0;
	int		exit_code = 0;
	extern char	*optarg;
	extern int	optind;
	char		ch;
	char		*my_name;
	char		*last_slash;
	char		*directory = NULL;
	path_name_t	chkpnt_dir;
	boolean_t	pgrp = FALSE;
	pid_t		id;
	struct stat	stat_buf;
	DIR		*dfd;
	struct dirent	*dp;

	last_slash = strrchr(argv[0], '/');
	if (last_slash == NULL)
		my_name = argv[0];
	else
		my_name = last_slash + 1;

	while ((ch = getopt(argc, argv, "d:fgilmpt")) != EOF) {
		switch (ch) {
		case 'd':
			directory = optarg;
			break;
		case 'f':
			dump_file_state = TRUE;
			break;
		case 'g':
			pgrp = TRUE;
			break;
		case 'i':
			dump_image = TRUE;
			break;
		case 'l':
			dump_mi = TRUE;
			dump_proc_state = TRUE;
			dump_thread_state = TRUE;
			dump_file_state = TRUE;
			break;
		case 'm':
			dump_mi = TRUE;
			dump_proc_state = TRUE;
			break;
		case 'p':
			dump_proc_state = TRUE;
			break;
		case 't':
			dump_thread_state = TRUE;
			dump_proc_state = TRUE;
			break;
		case '?':
		default:
			error++;
		}
	}
	
	if (optind == (argc - 1) && argv[optind] != NULL)
		id = atoi(argv[optind]);
	else if (directory == NULL)
		error++;
	if (error) {
		printf("usage: %s [options] [-d directory | id ]\n"
		       "           options: -g id is process group\n"
		       "                    -p dump process state\n"
		       "                    -m dump mi data structure (=>-p)\n"
		       "                    -t dump thread state (=>-p)\n"
		       "                    -f dump file state\n"
		       "                    -l dump everything\n",
		       my_name);
		exit(1);
	}

	/*
	 * Compose the pathname of, or through which, the checkpoint
	 * directory is accessed.
	 */
	if (directory != NULL) {
		sprintf(chkpnt_dir, "%s", directory);
	} else {
		sprintf(chkpnt_dir, "/chkpnt/%s.%d",
			pgrp ? "pgrp" : "proc", id);
	}

	/*
	 * Does the chkpnt directory exist?
	 */
	error = stat(chkpnt_dir, &stat_buf);
	if (error != ESUCCESS || (stat_buf.st_mode & S_IFMT) != S_IFDIR) {
		fprintf(stderr, "cannot access directory %s\n", chkpnt_dir);
		exit(1);
	}

	dfd = opendir(chkpnt_dir);
	if (dfd == NULL) {
		fprintf(stderr, "cannot open checkpoint directory %s\n",
			chkpnt_dir);
		exit(1);
	}
	while (((dp = readdir(dfd)) != NULL)) {
		int		node;
		int		pid;
		int		ppid;
		int		pgid;
		path_name_t	type;
		path_name_t	fpath;
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;
		sprintf(fpath, "%s/%s", chkpnt_dir, dp->d_name);
		if (sscanf(dp->d_name, "%d.%d.%d.%d.%s",
			   &pid, &ppid, &pgid, &node, &type) != 0) {
			if (strcmp(type, "stat") == 0
			    && (dump_proc_state || dump_file_state)) {
				printf("Node %d, pid %d state:\n", node, pid);
				print_state(fpath);
			}
			if (strcmp(type, "core") == 0
			    && dump_image) {
				printf("Node %d, pid %d image:\n", node, pid);
				print_image(fpath);
			}
		} else
			fprintf(stderr,
				"unrecognized file %s\n", dp->d_name);
	}
	(void) closedir(dfd);

	exit(exit_code);
}

int
print_state(
	char *state_fn)
{
	chkpnt_proc_state_t	ps;
	chkpnt_file_state_t	fs;
	int			nbytes;
	int			fdes;

	fdes = open(state_fn, O_RDONLY);
	if (fdes < 0) {
		fprintf(stderr, "can't open file %s\n", state_fn);
		return(-1);
	}
	
	nbytes = read(fdes, (char *) &ps, sizeof(ps));
	if (nbytes != sizeof(ps)) {
		fprintf(stderr, "can't read proc state\n");
		return(-1);
	}
	if (ps.version != CHKPNT_FILE_VERSION) {
		printf("\t*** Warning: file version %d, %d expected ***\n",
			ps.version, CHKPNT_FILE_VERSION);
	}
	if (dump_proc_state) {
		printf("\t%-20s : %d\n", "chkpnt version", ps.version);
		printf("\t%-20s : %d\n", "file state size", ps.fs_size);
		printf("\t%-20s : %d, %d, %d, %d\n", "pid, ppid, pgid, sid",
			ps.pid, ps.ppid, ps.pgid, ps.sid);
		printf("\t%-20s : <%d>(%d,%d)\n", "ctty",
			ps.ctty.node,
			major(ps.ctty.device), minor(ps.ctty.device));
		if (dump_thread_state)
			print_thread_state(&ps.thread_state);
		if (dump_mi)
			print_mi_data(&ps.mi_data);
		printf("\t%-20s : %s\n", "command_name", ps.command_name);
		printf("\t%-20s : %s\n", "logname", ps.logname);
	}
	
	if (dump_file_state) {
		nbytes = read(fdes, (char *) &fs, ps.fs_size);
		if (nbytes != ps.fs_size) {
			fprintf(stderr, "can't read file state\n");
			return(-1);
		}
		print_file_state(&fs);
	}

	(void) close(fdes);
}

int
print_image(
	char *image_fn)
{
	int			nbytes;
	int			fdes;
	struct user		uarea;

	fdes = open(image_fn, O_RDONLY);
	if (fdes < 0) {
		fprintf(stderr, "can't open file %s\n", image_fn);
		return(-1);
	}
	
	nbytes = read(fdes, (char *) &uarea, sizeof(uarea));
	if (nbytes != sizeof(uarea)) {
		fprintf(stderr, "can't read uarea info\n");
		return(-1);
	}

	printf("\tuarea info:\n");

#define U_PRINTF(ua,mem,fmt) \
	printf("\t\t%-20s : " #fmt "\n", #mem, ua.mem);

#undef u_comm
	U_PRINTF(uarea,u_comm,%s);
#undef u_tsize
	U_PRINTF(uarea,u_tsize,0x%x);
#undef u_dsize
	U_PRINTF(uarea,u_dsize,0x%x);
#undef u_ssize
	U_PRINTF(uarea,u_ssize,0x%x);
#undef u_cmask
	U_PRINTF(uarea,u_cmask,0x%x);
#undef	u_start
	U_PRINTF(uarea,u_start,0x%x);
#undef u_logname
	U_PRINTF(uarea,u_logname,%s);
#undef u_ru
	U_PRINTF(uarea,u_ru.ru_stime.tv_sec,%d);
	U_PRINTF(uarea,u_ru.ru_stime.tv_usec,%d);
	U_PRINTF(uarea,u_ru.ru_utime.tv_sec,%d);
	U_PRINTF(uarea,u_ru.ru_utime.tv_usec,%d);

	(void) close(fdes);
}

int
print_mi_data(
	MI_DATA_T	*mi)
{
	int	i;

#define MI_PRINTF(mip,mem,fmt) \
	printf("\t\t%-20s : " #fmt "\n", #mem, mip->mi_##mem);

	printf("\tmi_data:\n");

	MI_PRINTF(mi,p_nice,%d);
	MI_PRINTF(mi,p_cursig,%d);
	MI_PRINTF(mi,p_sig,0x%x);
	MI_PRINTF(mi,p_sigmask,0x%x);
	MI_PRINTF(mi,p_sigignore,0x%x);
	MI_PRINTF(mi,p_sigcatch,0x%x);
	MI_PRINTF(mi,p_flag,0x%x);
	MI_PRINTF(mi,p_ruid,%d);
	MI_PRINTF(mi,p_svuid,%d);
	MI_PRINTF(mi,p_rgid,%d);
	MI_PRINTF(mi,p_svgid,%d);
	MI_PRINTF(mi,p_cr_uid,%d);
	MI_PRINTF(mi,p_cr_gid,%d);
	MI_PRINTF(mi,p_pid,%d);
	MI_PRINTF(mi,p_ppid,%d);
	MI_PRINTF(mi,p_pgid,%d);
	MI_PRINTF(mi,p_sid,%d);
	MI_PRINTF(mi,p_realtimer_coe,%d);
	MI_PRINTF(mi,p_stopsig,0x%x);
	MI_PRINTF(mi,p_realtimer.it_interval.tv_sec,%d);
	MI_PRINTF(mi,p_realtimer.it_interval.tv_usec,%d);
	MI_PRINTF(mi,p_realtimer.it_value.tv_sec,%d);
	MI_PRINTF(mi,p_realtimer.it_value.tv_usec,%d);
	MI_PRINTF(mi,p_realtimer,0x%x);
	MI_PRINTF(mi,p_logdev,0x%x);
	MI_PRINTF(mi,sigwait,0x%x);
	MI_PRINTF(mi,u_tsize,0x%x);
	MI_PRINTF(mi,u_dsize,0x%x);
	MI_PRINTF(mi,u_ssize,0x%x);
	MI_PRINTF(mi,u_text_start,0x%x);
	MI_PRINTF(mi,u_data_start,0x%x);
	MI_PRINTF(mi,u_stack_start,0x%x);
	MI_PRINTF(mi,u_stack_end,0x%x);
	MI_PRINTF(mi,u_stack_grows_up,0x%x);
	MI_PRINTF(mi,u_outime,0x%x);
        for (i=0; i<NSIG+1; ++i) {
		MI_PRINTF(mi,u_signal[i],0x%x);
		MI_PRINTF(mi,u_sigmask[i],0x%x);
        }
#ifdef i386
	MI_PRINTF(mi,u_sigreturn,0x%x);
#endif
#ifdef multimax
	MI_PRINTF(mi,u_sigcatch,0x%x);
#endif
#if defined(balance) || defined(mips) || defined(i860)
	MI_PRINTF(mi,u_sigtramp,0x%x);
#endif
	MI_PRINTF(mi,u_sigonstack,0x%x);
	MI_PRINTF(mi,u_sigintr,0x%x);
	MI_PRINTF(mi,u_oldmask,0x%x);
	MI_PRINTF(mi,u_sigstack_ss_sp,0x%x);
	MI_PRINTF(mi,u_sigstack_ss_onstack,0x%x);
	MI_PRINTF(mi,u_cmask,0x%x);
	MI_PRINTF(mi,u_ioch,0x%x);
	MI_PRINTF(mi,u_ru,0x%x);
	MI_PRINTF(mi,u_cru,0x%x);
        for (i=0; i<3; ++i) {
		MI_PRINTF(mi,u_timer[i],0x%x);
        }

	MI_PRINTF(mi,u_prof_base,0x%x);
	MI_PRINTF(mi,u_prof_size,0x%x);
	MI_PRINTF(mi,u_prof_off,0x%x);
	MI_PRINTF(mi,u_prof_scale,0x%x);
	MI_PRINTF(mi,u_maxuprc,0x%x);

        /* handle array of rlimits structures */
        for (i=0; i<RLIM_NLIMITS; ++i) {
		MI_PRINTF(mi,u_rlimit[i],0x%x);
        }

	MI_PRINTF(mi,u_shmsegs,0x%x);
	MI_PRINTF(mi,u_argp,0x%x);
	MI_PRINTF(mi,u_envp,0x%x);
	MI_PRINTF(mi,u_arg_size,0x%x);
	MI_PRINTF(mi,u_env_size,0x%x);
}

int
print_thread_state(
	thread_state_t	*ts)
{
	printf("\tthread state:\n");

#if	defined(i386)
#define I386_THREAD_PRINTF(tsp,mem)			\
	printf("\t\t%-5s : 0x%x (%d.) (%u)\n", #mem,	\
		((struct i386_thread_state *)tsp)->mem,	\
		((struct i386_thread_state *)tsp)->mem,	\
		((struct i386_thread_state *)tsp)->mem);

	I386_THREAD_PRINTF(ts,gs);
	I386_THREAD_PRINTF(ts,fs);
	I386_THREAD_PRINTF(ts,es);
	I386_THREAD_PRINTF(ts,ds);
	I386_THREAD_PRINTF(ts,edi);
	I386_THREAD_PRINTF(ts,esi);
	I386_THREAD_PRINTF(ts,ebp);
	I386_THREAD_PRINTF(ts,ebx);
	I386_THREAD_PRINTF(ts,edx);
	I386_THREAD_PRINTF(ts,ecx);
	I386_THREAD_PRINTF(ts,eax);
	I386_THREAD_PRINTF(ts,eip);
	I386_THREAD_PRINTF(ts,cs);
	I386_THREAD_PRINTF(ts,efl);
	I386_THREAD_PRINTF(ts,uesp);
	I386_THREAD_PRINTF(ts,ss);

#elif	defined(i860)
#define I860_THREAD_PRINTF(tsp,mem)			\
	printf("\t\t%-5s : 0x%x (%d.) (%u)\n", #mem,	\
		((struct i860_thread_state *)tsp)->mem,	\
		((struct i860_thread_state *)tsp)->mem,	\
		((struct i860_thread_state *)tsp)->mem);

	I860_THREAD_PRINTF(ts,r0);
	I860_THREAD_PRINTF(ts,r1);
	I860_THREAD_PRINTF(ts,sp);
	I860_THREAD_PRINTF(ts,fp);
	I860_THREAD_PRINTF(ts,r4);
	I860_THREAD_PRINTF(ts,r5);
	I860_THREAD_PRINTF(ts,r6);
	I860_THREAD_PRINTF(ts,r7);
	I860_THREAD_PRINTF(ts,r8);
	I860_THREAD_PRINTF(ts,r9);
	I860_THREAD_PRINTF(ts,r10);
	I860_THREAD_PRINTF(ts,r11);
	I860_THREAD_PRINTF(ts,r12);
	I860_THREAD_PRINTF(ts,r13);
	I860_THREAD_PRINTF(ts,r14);
	I860_THREAD_PRINTF(ts,r15);
	I860_THREAD_PRINTF(ts,r16);
	I860_THREAD_PRINTF(ts,r17);
	I860_THREAD_PRINTF(ts,r18);
	I860_THREAD_PRINTF(ts,r19);
	I860_THREAD_PRINTF(ts,r20);
	I860_THREAD_PRINTF(ts,r21);
	I860_THREAD_PRINTF(ts,r22);
	I860_THREAD_PRINTF(ts,r23);
	I860_THREAD_PRINTF(ts,r24);
	I860_THREAD_PRINTF(ts,r25);
	I860_THREAD_PRINTF(ts,r26);
	I860_THREAD_PRINTF(ts,r27);
	I860_THREAD_PRINTF(ts,r28);
	I860_THREAD_PRINTF(ts,r29);
	I860_THREAD_PRINTF(ts,r30);
	I860_THREAD_PRINTF(ts,r31);
	I860_THREAD_PRINTF(ts,f0);
	I860_THREAD_PRINTF(ts,f1);
	I860_THREAD_PRINTF(ts,f2);
	I860_THREAD_PRINTF(ts,f3);
	I860_THREAD_PRINTF(ts,f4);
	I860_THREAD_PRINTF(ts,f5);
	I860_THREAD_PRINTF(ts,f6);
	I860_THREAD_PRINTF(ts,f7);
	I860_THREAD_PRINTF(ts,f8);
	I860_THREAD_PRINTF(ts,f9);
	I860_THREAD_PRINTF(ts,f10);
	I860_THREAD_PRINTF(ts,f11);
	I860_THREAD_PRINTF(ts,f12);
	I860_THREAD_PRINTF(ts,f13);
	I860_THREAD_PRINTF(ts,f14);
	I860_THREAD_PRINTF(ts,f15);
	I860_THREAD_PRINTF(ts,f16);
	I860_THREAD_PRINTF(ts,f17);
	I860_THREAD_PRINTF(ts,f18);
	I860_THREAD_PRINTF(ts,f19);
	I860_THREAD_PRINTF(ts,f20);
	I860_THREAD_PRINTF(ts,f21);
	I860_THREAD_PRINTF(ts,f22);
	I860_THREAD_PRINTF(ts,f23);
	I860_THREAD_PRINTF(ts,f24);
	I860_THREAD_PRINTF(ts,f25);
	I860_THREAD_PRINTF(ts,f26);
	I860_THREAD_PRINTF(ts,f27);
	I860_THREAD_PRINTF(ts,f28);
	I860_THREAD_PRINTF(ts,f29);
	I860_THREAD_PRINTF(ts,f30);
	I860_THREAD_PRINTF(ts,f31);
	I860_THREAD_PRINTF(ts,spc_t);
	I860_THREAD_PRINTF(ts,spc_ki);
	I860_THREAD_PRINTF(ts,spc_kr);
	I860_THREAD_PRINTF(ts,spc_merge);
	I860_THREAD_PRINTF(ts,psv_l1);
	I860_THREAD_PRINTF(ts,psv_l2);
	I860_THREAD_PRINTF(ts,psv_l3);
	I860_THREAD_PRINTF(ts,psv_a1);
	I860_THREAD_PRINTF(ts,psv_a2);
	I860_THREAD_PRINTF(ts,psv_a3);
	I860_THREAD_PRINTF(ts,psv_m1);
	I860_THREAD_PRINTF(ts,psv_m2);
	I860_THREAD_PRINTF(ts,psv_m3);
	I860_THREAD_PRINTF(ts,psv_i1);
	I860_THREAD_PRINTF(ts,fsr1);
	I860_THREAD_PRINTF(ts,fsr2);
	I860_THREAD_PRINTF(ts,fsr);	/* 3rd FP pipe stage */
	I860_THREAD_PRINTF(ts,psr);
	I860_THREAD_PRINTF(ts,pc);	/* fir */
	I860_THREAD_PRINTF(ts,dirbase);
	I860_THREAD_PRINTF(ts,db);
	I860_THREAD_PRINTF(ts,epsr);
	I860_THREAD_PRINTF(ts,fp1);	/* operands for USER mode IEEE FP */
	I860_THREAD_PRINTF(ts,fp2);
	I860_THREAD_PRINTF(ts,fp3);
	I860_THREAD_PRINTF(ts,fp4);
	I860_THREAD_PRINTF(ts,fpe_trapped_op);	/* FP source error opcode */
	I860_THREAD_PRINTF(ts,fpe_rdest);
	I860_THREAD_PRINTF(ts,fpe_src1);
	I860_THREAD_PRINTF(ts,fpe_ieee_status);
#endif
}

int
print_file_state(
	chkpnt_file_state_t	*fs)
{
	int	i;

	printf("\tfile state:\n");

	printf("\t\t%-20s : %s\n", "exec'ed file", fs->exec_fname);

	printf("\t\troot dir at exec:\n");
	print_file_info(&fs->exec_rootdir);
	printf("\t\tcurrent dir at exec:\n");
	print_file_info(&fs->exec_currentdir);

	printf("\t\troot dir:\n");
	print_file_info(&fs->rootdir);
	printf("\t\tcurrent dir:\n");
	print_file_info(&fs->currentdir);

	printf("\t\t%-20s : 0x%x\n", "on_checkpoint()",
		fs->on_routines.on_checkpoint);
	printf("\t\t%-20s : 0x%x\n", "on_restart()",
		fs->on_routines.on_restart);

	printf("\t\t%-20s : 0x%x\n", "signal_trampoline",
		fs->signal_trampoline);

	printf("\t\t%-20s : %d\n", "num files open", fs->n_open_files);
	printf("\t\t%-20s :", "fdes->file");
	for (i = 0; i < OPEN_MAX; i++)
		if (fs->fdt[i] != FD_UNUSED)
			printf(" %d->%d", i, fs->fdt[i]);
	printf("\n");

	for (i = 0; i < fs->n_open_files; i++) {
		printf("\t\topen file %d:\n", i);
		print_file_info(&fs->file_state[i]);
	}
}

int
print_file_info(
	file_info_t	*fi)
{

	printf("\t\t\t%-14s : %d\n", "offset", fi->offset);
	printf("\t\t\t%-14s : 0x%x\n", "fmode", fi->fmode);

	printf("\t\t\tfile stat:\n");

	printf("\t\t\t\t%-14s : (%d,%d)\n", "st_dev",
		major(fi->fstat.st_dev), minor(fi->fstat.st_dev));

#define MEM_PRINTF(s,mem,fmt) \
	printf("\t\t\t\t%-14s : " #fmt "\n", #mem, s.mem);

	MEM_PRINTF(fi->fstat,st_ino,%u);
	MEM_PRINTF(fi->fstat,st_mode,0x%x);
	MEM_PRINTF(fi->fstat,st_nlink,%u);
	MEM_PRINTF(fi->fstat,st_uid,%u);
	MEM_PRINTF(fi->fstat,st_gid,%u);
	printf("\t\t\t\t%-14s : (%d,%d)\n", "st_rdev",
		major(fi->fstat.st_rdev), minor(fi->fstat.st_rdev));
	MEM_PRINTF(fi->fstat,st_size,%u);
	printf("\t\t\t\t%-14s : %s", "st_atime", ctime(&fi->fstat.st_atime));
	printf("\t\t\t\t%-14s : %s", "st_mtime", ctime(&fi->fstat.st_mtime));
	printf("\t\t\t\t%-14s : %s", "st_ctime", ctime(&fi->fstat.st_ctime));
	MEM_PRINTF(fi->fstat,st_blksize,%u);
	MEM_PRINTF(fi->fstat,st_blocks,%u);
	MEM_PRINTF(fi->fstat,st_flags,0x%x);
	MEM_PRINTF(fi->fstat,st_gen,%d);

	switch (fi->fstat.st_mode & S_IFMT) {
	    case S_IFCHR:
	    case S_IFBLK:
		printf("\t\t\tdevice stat:\n");
		MEM_PRINTF(fi->fs_or_dev.devstat,dst_node,%d);
		MEM_PRINTF(fi->fs_or_dev.devstat,dst_mountid,%u);
		break;
	    default:
		printf("\t\t\tfilesystem stat:\n");
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_type,0x%x);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_flags,0x%x);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_fsize,%d);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_blocks,%d);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_bfree,%d);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_bavail,%d);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_files,%d);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_ffree,%d);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_fsid,0x%x);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_mntonname,%s);
		MEM_PRINTF(fi->fs_or_dev.fstatfs,f_mntfromname,%s);
	}
}
