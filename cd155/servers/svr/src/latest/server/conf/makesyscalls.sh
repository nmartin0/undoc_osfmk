#! /bin/sh -
# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# @OSF_COPYRIGHT@
#
#
# Copyright (c) 1991, Locus Computing Corporation
# All rights reserved
# 
# HISTORY
# $Log: makesyscalls.sh,v $
# Revision 1.14  1995/04/11  02:08:32  yazz
#  Reviewer: Tom Check
#  Risk: Lo
#  Benefit or PTS #: 12916 H-*
#  Testing: builds of several configs, as root and not root
#  Module(s): server/conf/makesyscalls.sh
# As is done elsewhere in this script, a recently added rm command now
# checks that it is not removing "/dev/null".
#
# Revision 1.13  1995/04/11  02:01:34  yazz
#
# Revision 1.12  1995/03/07  21:47:56  yazz
# Corrected error in previous checkin.  #12496 now fixed in R1.3 WW10.
#
# Revision 1.11  1995/02/27  22:31:30  yazz
#  Reviewer: Brad Figg, Suri Brahmaroutu
#  Risk: Lo
#  Benefit or PTS #: #12496
#  Testing: All configs built & rebuilt; unneeded build steps no longer occur.
#  Module(s):
# 	svr/server/Makefile
# 	svr/server/conf/makesyscalls.sh
#
# Revision 1.10  1994/11/18  20:29:47  mtm
# Copyright additions/changes
#
# Revision 1.9  1994/04/05  21:48:36  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: works around 'feature' in awk which breaks this
# 	script in sandboxes with long filename paths.  Checked in over a
# 	year ago as version 1.4, somebody merged it back out.
#  Testing: builds server
#  Module(s): makesyscalls.sh
#
# Revision 1.8  1993/10/29  11:56:42  paul
# Add support for setting and using the RPM distributed time-of-day clock.
#
# Revision 1.7  1993/07/14  17:54:23  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  19:12:43  cfj
# Adding new code from vendor
#
# Revision 1.6  1993/05/06  19:14:47  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:27:50  cfj
# Initial 1.0.3 code drop
#
# Revision 1.5  1993/04/03  03:05:38  brad
# Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
# Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
#
# Revision 1.4  1993/02/24  00:03:48  dleslie
# Workaround for undocumented awk 'feature' whereby trying to print strings
# longer than 150 characters with 'printf' causes a fatal 'string too long'
# message.  This can cause mysterious build failures if your BCSSBASE happens
# to be a long path (it happened to me!).
#
# Revision 1.1.2.1.2.1  1992/12/16  05:59:49  brad
# Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
# into the PFS branch.
#
# Revision 1.3  1992/12/11  02:56:04  cfj
# Merged 12-1-92 bug drop from Locus.
#
# Revision 1.2  1992/11/30  22:19:25  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:18:35  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.11  93/06/02  09:52:34  yazz
# For Sys V IPC under TNC add additional system call category "TNC5MSG".
# 
# Revision 2.10  92/11/23  15:55:01  klh
# 	Revision 2.10  92/11/05  17:28:28  roy
# 		Remove define of e_setrlimit to bsd_setrlimit.
# 		[92/10/27            roy]
#
# 	Revision 2.9  92/07/08  14:52:01  rabii
# 		Remove extra lines from end of file (loverso)
#
# Revision 2.9  92/07/09  10:09:02  klh
# 	Revision 2.8  92/06/30  22:46:31  loverso
# 		Since nosys uses psignal, always run serial. (loverso)
# 
# Revision 2.8  92/04/06  15:55:31  chrisp
# Add support for "TNCPARALLEL" as an alternative option to SERIAL and PARALLEL.
# With TNC defined, TNCPARALLEL will build as PARALLEL. Without TNC, TNCPARALLEL
# will be SERIAL (as was previously the case).
# 
# Revision 2.7  92/01/02  18:48:27  roy
# 	1991/09/22  16:04:27  noemi
# 	Added support for vnode service.
# 
# Revision 2.6  91/12/16  14:58:04  roy
# 	91/11/13  14:55:58  barbou
# 	Fix for bug #1: compute and define SYS_nosys, to be used by emulator.
# 
# 	91/10/30  17:43:00  bernadat
# 	Added <mach_afs.h> include statements.
# 
# Revision 2.5  91/10/17  13:15:33  roy
# Fix revision history comment leader.
# 
# Revision 2.4  91/10/15  09:24:00  sjs
# 	 91/09/06  13:49:50  condict
# 	 Remove attempt to trap sig 11, to fix error when building on OSF/1.
# 	 
# 
# Revision 2.3  91/09/16  16:38:46  rabii
#	 Merge of V2.0 and Locus (locus check-in by roman)
#	 Added a new second column value of "NOSERV". This is for system calls
#	 handled entirely in the emulator (with no code in the server
#	 whatsoever). Added a new fourth column value of "FSMSG". This is
#	 for system calls handled in the emulator by emul_fs_generic()
#	 (generic file system system call handler) rather the emul_generic().
#	 Also added a new fourth column value of "VPMSG". This is for
#	 system calls handled in the emulator by emul_vp_generic().
#
# Revision 2.4  91/10/15  09:24:00  sjs
# 	 91/09/06  13:49:50  condict
# 	 Remove attempt to trap sig 11, to fix error when building on OSF/1.
# 	 
# Revision 2.2  91/08/31  13:30:57  rabii
#	Initial V2.0 Checkin
#
# Revision 3.2  91/08/28  14:01:39  condict
# 	Update to OSF 1.0.2.
# 
# Revision 1.8  90/10/07  13:23:43  devrcs
# 	Added EndLog Marker.
# 	[90/09/28  09:09:03  gm]
# 
# Revision 1.7  90/09/23  15:44:45  devrcs
# 	Fixed for ANSI compliance
# 	[90/09/05  17:12:55  rossi]
# 
# Revision 1.6  90/08/24  11:21:29  devrcs
# 	Removed cmu system call table
# 	[90/08/17  17:41:00  nags]
# 
# Revision 1.5  90/06/22  20:08:44  devrcs
# 	nags merge
# 	[90/06/12  21:50:48  gmf]
# 
# 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
# 	[90/06/09  18:42:29  seiden]
# 
# 	Added ANSI specification for generation of compat
# 	[90/05/31  15:10:33  kevins]
# 
# 	Compressed history (reverse chronology):
# 	New syscall format.				nags@encore.com
# 	STDALT, for syscalls at alternate numbers.	ers@osf.org
# 	Added OSF Copyright marker.			gm@osf.org
# 	Option:  make header file only.			coren@osf.org
# 	Adapted from 4.4BSD; invoke C pre-processor.	coren@osf.org
# 	[90/05/31  15:11:59  kevins]
# 
# $EndLog$
Options="-[aehk]"
if [ $# -ge 2 ] && expr "X$1" : '^X'$Options'*$' >/dev/null; then
	atabl=no
	case "$1" in
	    -*a*) atabl=yes ;;
	esac
	emul=no
	case "$1" in
	    -*e*) emul=yes ;;
	esac
	header=no
	case "$1" in
	    -*h*) header=yes ;;
	esac
	kernel=no
	case "$1" in
	    -*k*) kernel=yes ;;
	esac
else
	if [ $# -ge 2 ]; then
		echo "Unrecognized option in: $1" 1>&2
	fi
	echo "Usage: $0 $Options input-file [config-dir]" 1>&2
	exit 1
fi

INPUT=$2
if [ ! -f "$INPUT" ]; then
	echo "$INPUT: No such file" 1>&2
	exit 1
fi

if [ $kernel = yes ]; then
    if [ $# != 3 ]; then
	echo "Config directory must be given when -k specified." 1>&2
	echo "Usage: $0 $Options input-file [config-dir]" 1>&2
	exit 1
    else
	CONFIG=$3
    fi
fi

# name of compat option:
compat=COMPAT_43

# output files:
if [ $kernel = yes ]; then
	sysnames=${CONFIG}/"syscalls.c"
	syssw="${CONFIG}/init_sysent.c"
else
	sysnames="/dev/null"
	syssw="/dev/null"
fi

if [ $header = yes ]; then
	finalsyshdr="./syscall.h"
	syshdr="/tmp/mksyscall.$$"
else
	syshdr="/dev/null"
fi

if [ $emul = yes ]; then
	sysemul="./emul_sysent.c"
else
	sysemul="/dev/null"
fi
if [ $atabl = yes ]; then
	audsys="audit.syscalls"
else
	audsys="/dev/null"
fi

# Names of temp files:
sysdcl_t="sysent.dcl"
syscompat_t="sysent.compat"
sysent_t="sysent.switch"
emul_dcl_t="emul.dcl"
emul_switch_t="emul.switch"

trap "/bin/rm -f $sysdcl_t $syscompat_t $sysent_t $emul_dcl_t $emul_switch_t; \
      exit 1"   1 2 3 4 5 6 7 8 10 12 13 14 15

# Initialize the beginning of the temp files:

line_1=`sed 1q $INPUT`

cat >$sysdcl_t <<+EOF+ #------------------------------------------------------
/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from$line_1
 */

#include <unix_locks.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <mach_afs.h>
#include <mach_afs_30.h>

#if	UNIX_LOCKS
#define	UPAR	1
#else
#define	UPAR	0
#endif

#ifdef	TNC
#define	TNCPAR	1
#else
#define	TNCPAR	0
#endif

#include <sys/secdefines.h>

int	nosys();

+EOF+

cat >$syscompat_t <<+EOF+ #---------------------------------------------------

#if $compat
#ifdef __STDC__
#define compat(n, name) n, 0, o##name
#define pcompat(n, name) n, 1, o##name
#define upcompat(n, name) n, UPAR, o##name
#define tncpcompat(n, name) n, TNCPAR, o##name

#else
#define compat(n, name) n, 0, o/**/name
#define pcompat(n, name) n, 1, o/**/name
#define upcompat(n, name) n, UPAR, o/**/name
#define tncpcompat(n, name) n, TNCPAR, o/**/name

#endif
+EOF+

cat >$sysent_t <<+EOF+ #------------------------------------------------------

struct sysent sysent[] = {
+EOF+

cat >$syshdr <<+EOF+ #------------------------------------------------------
/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from$line_1
 */

+EOF+

cat >$sysnames <<+EOF+ #----------------------------------------------------
/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from$line_1
 */

#include <mach_afs.h>
#include <mach_afs_30.h>

char *syscallnames[] = {
+EOF+

cat >$emul_dcl_t <<+EOF+ #----------------------------------------------------
/*
 * System call stub actions.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from$line_1
 */
#include <syscall_table.h>

#define	syss(routine, nargs)	{ nargs, routine }
#define	sysg			{ E_GENERIC, emul_generic }
#define	sysf			{ E_GENERIC, emul_fs_generic }
#define	sysv			{ E_GENERIC, emul_vproc_generic }
#ifdef TNC
#define	sys5			{ E_GENERIC, emul_svipc_generic }
#else
#define sys5			sysg
#endif
#define	sysn			{ E_GENERIC, emul_vnode_generic }
#define	sysr(routine)		{ E_CHANGE_REGS, routine }
#if defined(ECACHE) || defined(MAP_UAREA)
#define syse(routine)		{ E_GENERIC, routine }
#endif defined(ECACHE) || defined(MAP_UAREA)

/*
 * Routines in system call table
 */
int	emul_generic();		/* catchall */
int	emul_fs_generic();	/* catchall for file service */
int	emul_vproc_generic();	/* catchall for vproc service */
int	emul_svipc_generic();	/* catchall for Sys V IPC service */
int	emul_vnode_generic();	/* catchall for vnode service */
int	bsd_task_by_pid(), bsd_pid_by_task(), bsd_init_process();
int	e_htg_syscall();
#define e_setgroups	bsd_setgroups
#define e_sethostname	bsd_sethostname
#define e_vfork		e_fork
+EOF+

cat >$emul_switch_t <<+EOF+ #-------------------------------------------------
/*
 * System call table
 */
struct sysent sysent[] = {
+EOF+

#-----------------------------------------------------------------------------

awk < ${INPUT} '
	BEGIN {
		if_nest_level = -1
		exit_code = 0

		sysdcl_t = "'$sysdcl_t'"
		syscompat_t = "'$syscompat_t'"
		sysent_t = "'$sysent_t'"
		sysnames = "'$sysnames'"
		syshdr = "'$syshdr'"
		emul_dcl_t = "'$emul_dcl_t'"
		emul_switch_t = "'$emul_switch_t'"
		compat = "'$compat'"
		audsys = "'$audsys'"

	}
	NR == 1 || NF == 0 || $1 ~ /^;/ {
		next
	}
	$1 ~ /^#/ {
		if ($1 ~ /^#[ 	]*if/) {
			scallnum_at_if[++if_nest_level] = syscall
			scallnum_at_else[if_nest_level] = -1
		} else if ($1 ~ /^#[ 	]*else/ || $1 ~ /^#[ 	]*elif/) {
			if (if_nest_level < 0) {
				printf "'$INPUT':%d: If-less else or elif.\n", NR
				exit_code = 1
			} else {
				if (scallnum_at_else[if_nest_level] != -1 && \
				    scallnum_at_else[if_nest_level] != syscall) {
					printf "'$INPUT':%d: Syscall numbers out of synch", \
						NR
					printf " in branches of if\n\t(%d-%d vs. %d-%d)\n", \
						scallnum_at_if[if_nest_level], \
						scallnum_at_else[if_nest_level] - 1, \
						scallnum_at_if[if_nest_level], \
						syscall - 1
					exit_code = 1
				}
				scallnum_at_else[if_nest_level] = syscall
				syscall = scallnum_at_if[if_nest_level]
			}
		} else if ($1 ~ /^#[ 	]*end/) {
			if (if_nest_level < 0) {
				printf "'$INPUT':%d: If-less endif.\n", NR
				exit_code = 1
			} else {
				if (scallnum_at_else[if_nest_level] == -1) {
				    if (scallnum_at_if[if_nest_level] != syscall) {
					# There was no else.  Dangerous because we cant be
					# sure about synchronization:
					printf "'$INPUT':%d: (Warning) else-less if", NR
					printf " contains syscall declarations.\n"
					printf "\tCannot guarantee synchronization.\n"
					syscall = scallnum_at_if[if_nest_level]
				    }
				} else if (scallnum_at_else[if_nest_level] != syscall) {
					printf "'$INPUT':%d: Syscall numbers out of synch", \
						NR
					printf " in branches of if\n\t(%d-%d vs. %d-%d)\n", \
						scallnum_at_if[if_nest_level], \
						scallnum_at_else[if_nest_level] - 1, \
						scallnum_at_if[if_nest_level], \
						syscall - 1
					exit_code = 1
				}
				if_nest_level--
			}
		}
		print >> sysent_t
		print >> sysdcl_t
		print >> syscompat_t
		print >> emul_dcl_t
		print >> emul_switch_t
		print >> sysnames
		print >> syshdr
		print > audsys
		next
	}
	syscall != $1 {
		printf "'$INPUT':%d: Syscall number out of synch", NR
		printf " (%d expected).  Line is:\n", syscall
		print "\t" $0
		syscall = $1
		exit_code = 1
	}
	{	comment = $6
		for (i = 7; i <= NF; i++)
			comment = comment " " $i
		if (NF < 7)
			$7 = $6
		efunc = $7;
	}

	$2 == "STD" {
		printf("int\t%s();\n", $6) >> sysdcl_t
		parallel = "0";
		if ($3 == "PARALLEL") parallel="1"
		if ($3 == "UPARALLEL") parallel="UPAR"
		if ($3 == "TNCPARALLEL") parallel="TNCPAR"
		printf("\t%d, %s, %s,\t\t\t/* %d = %s */\n", \
		    $5, parallel, $6, syscall, $7) > sysent_t
		printf("\t\"%s\",\t\t\t/* %d = %s */\n", \
		    $7, syscall, $7) > sysnames
		printf("#define\tSYS_%s\t%d\n", \
	    		$7, syscall) > syshdr
		if (if_nest_level < 0)	# Not inside conditional, so:
			SYS_defined[$7] = 1
                printf("%s\t%d\t%s\n", $7, syscall, comment) > audsys
        }
	$2 == "NOSERV" {
		printf("\t%d, 0, nosys,\t\t\t/* %d = %s */\n", \
		    $5, syscall, $7) > sysent_t
		printf("\t\"%s\",\t\t\t/* %d = %s */\n", \
		    $7, syscall, $7) > sysnames
		printf("#define\tSYS_%s\t%d\n", \
	    		$7, syscall) > syshdr
		if (if_nest_level < 0)	# Not inside conditional, so:
			SYS_defined[$7] = 1
                printf("%s\t%d\t%s\n", $7, syscall, comment) > audsys
        }
        $2 == "PSTD" {
		parallel = "0";
		if ($3 == "PARALLEL") parallel="1"
		if ($3 == "UPARALLEL") parallel="UPAR"
		if ($3 == "TNCPARALLEL") parallel="TNCPAR"
		printf("\t%d, %s, %s,\t\t\t/* %d = %s (alternate) */\n", \
		    $5, parallel, $6, syscall, $7) > sysent_t
		printf("\t\"alternate %s\",\t\t/* %d = %s (alternate) */\n", \
		    $7, syscall, $7) > sysnames
                printf("%s\t%d\t%s\n", $7, syscall, comment) > audsys
        }
        $2 == "STDALT" {
		parallel = "0";
		if ($3 == "PARALLEL") parallel="1"
		if ($3 == "UPARALLEL") parallel="UPAR"
		if ($3 == "TNCPARALLEL") parallel="TNCPAR"
		printf("\t%d, %s, %s,\t\t\t/* %d = %s (alternate) */\n", \
		    $5, parallel, $6, syscall, $7) > sysent_t
		printf("\t\"alternate %s\",\t\t/* %d = %s (alternate) */\n", \
		    $7, syscall, $7) > sysnames
                printf("%s\t%d\t%s\n", $7, syscall, comment) > audsys
        }
        $2 == "COMPAT" {
		efunc = "o" efunc;
		printf("int\to%s();\n", $6) >> syscompat_t
		parallel=""
		if ($3 == "PARALLEL") parallel="p"
		if ($3 == "UPARALLEL") parallel="up"
		if ($3 == "TNCPARALLEL") parallel="tncp"
		printf("\t%scompat(%d,%s),\t\t/* %d = old %s */\n", \
		    parallel, $5, $6, syscall, $7) > sysent_t
		printf("\t\"old %s\",\t\t/* %d = old %s */\n", \
		    $7, syscall, $7) > sysnames
		if (!SYS_defined[$7]) {
			# There may be no other def, so remember this one:
			number_for[$7] = syscall
			SYS_defined[$7] = 0
		}
		printf("#define\tSYS_o%s\t%d", \
	    		$7, syscall) > syshdr
		printf("\t\t\t\t/* %d is old %s */\n", \
		    syscall, comment) > syshdr
		printf("o%s\t%d\told %s\n", $7, syscall, comment) > audsys
	}
	$2 == "OBSOL" {
		printf("\t0, 0, nosys,\t\t\t/* %d = obsolete %s */\n", \
		    syscall, comment) > sysent_t
		printf("\t\"obs %s\",\t\t\t/* %d = obsolete %s */\n", \
		    $6, syscall, comment) > sysnames
		printf("\t\t\t\t/* %d is obsolete %s */\n", \
		    syscall, comment) > syshdr
		printf("nosys\t%d\tobsolete %s\n", syscall, comment) > audsys
		nosys = syscall
	}
	$2 == "UNIMPL" {
		printf("\t0, 0, nosys,\t\t\t/* %d = %s */\n", \
		    syscall, comment) > sysent_t
		printf("\t\"#%d\",\t\t\t/* %d = %s */\n", \
		    syscall, syscall, comment) > sysnames
		printf("nosys\t%d\tunimplemented\n", syscall) > audsys
		nosys = syscall
	}
	$2 !~ /^(STD|NOSERV|PSTD|STDALT|COMPAT|OBSOL|UNIMPL)$/ {
		printf "'$INPUT':%d: Column 2 must be one of:\n", NR
		printf "\tSTD, NOSERV, PSTD, STDALT, COMPAT, OBSOL or UNIMPL.\n", NR
		exit_code = 1
	}
	$4 == "MSG" {
		printf("\tsysg,\t\t\t/* %d = %s */\n", \
		    syscall, comment) > emul_switch_t
	}
	$4 == "FSMSG" {
		printf("\tsysf,\t\t\t/* %d = %s */\n", \
		    syscall, comment) > emul_switch_t
	}
	$4 == "VPMSG" {
		printf("\tsysv,\t\t\t/* %d = %s */\n", \
		    syscall, comment) > emul_switch_t
	}
	$4 == "TNC5MSG" {		# if not tnc, TNC5MSG=MSG & sys5=sysg
		printf("\tsys5,\t\t\t/* %d = %s */\n", \
		    syscall, comment) > emul_switch_t
	}
	$4 == "VNMSG" {
		printf("\tsysn,\t\t\t/* %d = %s */\n", \
		    syscall, comment) > emul_switch_t
	}
	$4 == "STUB" {
		printf("\tsyss(e_%s, %d),\t/* %d = %s */\n", \
		    efunc, $5, syscall, comment) >> emul_switch_t
		printf("int\te_%s();\n", efunc) >> emul_dcl_t
	}
	$4 == "EMUL" {
		printf("\tsyse(e_%s),\t\t/* %d = %s */\n", \
		    efunc, syscall, comment) > emul_switch_t
		printf("int\te_%s();\n", efunc) >> emul_dcl_t
	}
	$4 == "RMUL" {
		printf("\tsysr(e_%s),\t\t/* %d = %s */\n", \
		    efunc, syscall, comment) > emul_switch_t
		printf("int\te_%s();\n", efunc) >> emul_dcl_t
	}
	$4 !~ /^(MSG|FSMSG|VPMSG|TNC5MSG|VNMSG|STUB|EMUL|RMUL)$/{
		printf "'$INPUT':%d: ", NR
		printf "Column 4 must be MSG, VPMSG, TNC5MSG, FSMSG, VNMSG, STUB, EMUL or RMUL.\n"
		exit_code = 1
	}
	{ syscall++
	}
	END {
		if (if_nest_level != -1) {
			printf "'$INPUT':%d: Unmatched if.\n", NR
			exit_code = 1
		}

		printf("\n#if %s\n", compat) >> syshdr
		for (i in SYS_defined) {
			if (!SYS_defined[i]) {
				printf("#ifndef\tSYS_%s\n", i) >> syshdr
				printf("#define\tSYS_%s\t%d\n", \
			    		i, number_for[i]) > syshdr
				printf("#endif\n") >> syshdr
			}
		}
		printf("#endif /* %s */\n\n", compat) >> syshdr

		printf("\n#else /* %s */\n", compat) >> syscompat_t
		printf("#define compat(n, name) 0, 0, nosys\n") >> syscompat_t
		printf("#define pcompat(n, name) 0, 0, nosys\n") >> syscompat_t
		printf("#define upcompat(n, name) 0, 0, nosys\n") >> syscompat_t
		printf("#define tnccompat(n, name) 0, TNCPAR, nosys\n") >> syscompat_t
		printf("#endif /* %s */\n\n", compat) >> syscompat_t

		printf("#define\tSYS_nosys\t%d\n\n", nosys) >> syshdr

		printf("};\n\n") >> sysent_t
		printf("int\tnsysent = sizeof(sysent) / sizeof(sysent[0]);\n") >> sysent_t

		print "};" >>emul_switch_t
		print "" >>emul_switch_t
		print "int     nsysent = sizeof(sysent)/sizeof(struct sysent);" >>emul_switch_t
		print "" >>emul_switch_t
		print "struct sysent   sysent_task_by_pid =" >>emul_switch_t
		print "        syss(bsd_task_by_pid, 1);" >>emul_switch_t
		print "" >>emul_switch_t
		print "struct sysent   sysent_pid_by_task =" >>emul_switch_t
		print "        syss(bsd_pid_by_task, 4);" >>emul_switch_t
		print "" >>emul_switch_t
		print "struct sysent   sysent_htg_ux_syscall =" >>emul_switch_t
		print "        sysr(e_htg_syscall);" >>emul_switch_t
		print "" >>emul_switch_t
		print "struct sysent   sysent_init_process =" >>emul_switch_t
		print "        syss(bsd_init_process, 0);" >>emul_switch_t

		printf("};\n") >> sysnames
		exit(exit_code)
	}'

if [ $? -ne 0 ]; then
	# The awk script detected an error in the master file:
	echo "$0: Errors in '$INPUT'. No output produced." 1>&2};
	rm $sysdcl_t $syscompat_t $sysent_t $emul_dcl_t $emul_switch_t
	if [ "$syshdr" != /dev/null ]; then
		rm $syshdr
	fi
	if [ "$sysnames" != /dev/null ]; then
		rm $sysnames
	fi
	exit 1
else
	cat $sysdcl_t $syscompat_t $sysent_t >$syssw
	cat $emul_dcl_t $emul_switch_t >$sysemul
	rm $sysdcl_t $syscompat_t $sysent_t $emul_dcl_t $emul_switch_t
	rm -f $finalsyshdr
	cp $syshdr $finalsyshdr
	if [ -d ./include/sys ] ; then
		cp $syshdr ./include/sys/$finalsyshdr
	fi
	if [ "$syshdr" != /dev/null ]; then
		rm $syshdr
	fi
	exit 0
fi
