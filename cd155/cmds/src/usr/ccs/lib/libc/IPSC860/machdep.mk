# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0
#
# derived from PMAX machdep.mk

IPSC860CFLAGS =-DBIT_ZERO_ON_RIGHT
INCFLAGS = -I../common_nx_c -I../libc

FP_SFILES=ldexp.o ieee_div.o i860rt.o fpsetrnd.o fpgetrnd.o \
	fpgetmask.o fpgetround.o fpgetsticky.o fpsetmask.o \
	fpsetround.o fpsetsticky.o isnan.o infinity.o

IPSC860COBJS = autoinit.o \
	setjmperr.o bcmp.o fabs.o ffs.o frexp.o \
	index.o rindex.o insque.o remque.o fp_const.o getlogin.o \
	sigprocmask.o sigsuspend.o nlist.o modf.o \
	msem_init.o msem_remove.o msem_lock.o msem_unlock.o

IPSC860SOBJS = Ovfork.o _exit.o setjmp.o _setjmp.o alloca.o sigsetjmp.o \
	_sigprocmask.o _sigpending.o _sigsuspend.o _getlogin.o  \
	accept.o access.o acct.o adjtime.o asyncdaemon.o \
	bind.o brk.o bzero.o \
	cerror.o chdir.o chmod.o chown.o chroot.o \
	close.o connect.o devstat.o dup.o \
	dup2.o exect.o \
	execwl.o \
	abs.o fchmod.o fchown.o fcntl.o flock.o \
	fstat.o \
	fstatfs.o fsync.o ftruncate.o getaddrconf.o \
	getdirentri.o getdomainnm.o getdtablesize.o \
	getegid.o geteuid.o getfh.o getfsstat.o getgid.o \
	getgroups.o gethostid.o gethostname.o getitimer.o getpagesize.o \
	getpeername.o getpgrp.o getpid.o getppid.o getpriority.o \
	getrlimit.o getrusage.o getsockname.o getsockopt.o gettimeofday.o \
	getuid.o htonl.o htons.o \
	ioctl.o joinrootpart.o kill.o kloadcall.o link.o \
	listen.o lseek.o lstat.o madvise.o memccpy.o memchr.o memcmp.o \
	memcpy.o mkdir.o momemset.o rmknod.o mknod.o \
	mmap.o mount.o mprotect.o msgctl.o msgget.o \
	msgrcv.o msgsnd.o munmap.o mvalid.o nfssvc.o ntohl.o \
	msleep.o msync.o mwakeup.o msem_tas.o \
	naccept.o ngetsockname.o nrecvfrom.o  \
	ngetpeername.o nrecvmsg.o nsendmsg.o \
	ntohs.o open.o pipe.o pfs_host_init.o plock.o poll.o profil.o ptrace.o \
	quotactl.o read.o readlink.o readv.o reboot.o \
	recv.o recvfrom.o recvmsg.o rename.o \
	rmdir.o select.o semctl.o semget.o \
	semop.o send.o sendmsg.o sendto.o setdomainnm.o setgid.o \
	setgroups.o sethostid.o sethostname.o setitimer.o \
	setlogin.o setpattr.o setpgid.o setpriority.o \
	setregid.o setreuid.o setrlimit.o setsid.o setsockopt.o \
	settimeofday.o setuid.o shmat.o shmctl.o shmdt.o shmget.o \
	shutdown.o sigreturn.o sigstack.o socket.o \
	socketpair.o stat.o statfs.o strcat.o strchr.o strcmp.o \
	strcpy.o strlen.o strncmp.o strncpy.o strrchr.o swapon.o \
	symlink.o sync.o syscall.o sigaction.o debug_portal.o table.o \
	truncate.o umask.o uname.o unlink.o utimes.o wait4.o \
	write.o writev.o chflags.o fchdir.o fchflags.o \
	setpgrp.o umount.o revoke.o sbrk.o \
	${FP_SFILES} 

umount.o_CFLAGS	= -DSYS_umount=SYS_unmount

crt0.o: crt0.c
	${_CC_} -DCRT0 ${_CCFLAGS_} -c $*.c

guard.o: guard.c
	${_CC_} ${_CCFLAGS_} -c $*.c

__denormal.o: __denormal.s
	${_CC_} ${_CCFLAGS_} -c $*.s

mon.o: mon.c
	${_CC_} ${_CCFLAGS_} -c $*.c

moncrt0.o: crt0.c
	${_CC_} -DMCRT0 ${_CCFLAGS_} -S crt0.c
	${ANSI_AS} -o moncrt0.o crt0.s
	${RM} -f crt0.s

mcrt0.o: moncrt0.o mon.o
	${ANSI_LD} -r -o mcrt0.o moncrt0.o mon.o


${IPSC860SOBJS:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${IPSC860SOBJS}: $${@:.o=.S}

	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES $*.S | as860 -o $@

#	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES $*.S | as860.exe -o $@
#
# Debug until as860 correctly processes cpp directives #file & #line.
#
#	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES $*.S >  `basename $@ .o`.s
#	${ANSI_AS}  -o $@  `basename $@ .o`.s
#	mv `basename $@ .o`.s `basename $@ .o`.ss	# can't be left as '.s'

