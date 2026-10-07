# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
#
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
# OSF/1 Release 1.0.1


AT386COBJS = calls.o fixunsdfsi.o frexp.o ldexp.o \
	    msem_init.o msem_remove.o msem_lock.o msem_unlock.o \
	    nlist.o \
	    sigpending.o sigprocmask.o sigsuspend.o \
	    strcat.o strcpy.o strcmp.o strlen.o write_rnd.o \
	    fp_const.o rindex.o index.o bcmp.o \
	    bzero.o getlogin.o


AT386SOBJS = Ovfork.o __udivsi3.o __divsi3.o _exit.o _getlogin.o _setjmp.o \
	     _sigpending.o _sigprocmask.o _sigsuspend.o abs.o \
	     accept.o access.o acct.o adjtime.o alloca.o \
	     asyncdaemon.o bind.o cerror.o chdir.o \
	     chflags.o chmod.o chown.o chroot.o close.o \
	     connect.o dup.o dup2.o execl.o execle.o \
	     exect.o execv.o execwl.o execve.o fabs.o fchdir.o \
	     fchflags.o fchmod.o fchown.o fcntl.o fixdfsi.o \
	     ffs.o flock.o fork.o fpinit.o fstat.o fstatfs.o \
	     fsync.o ftruncate.o getaddrconf.o getdirentri.o \
	     getdomainnm.o \
	     getdtablesz.o \
	     getegid.o geteuid.o getfh.o getfsstat.o getgid.o \
	     getgroups.o gethostid.o gethostname.o getitimer.o \
	     getpagesize.o getpeername.o getpgrp.o getpid.o \
	     getppid.o getpriority.o getrlimit.o getrusage.o \
	     getsockname.o getsockopt.o gettimeofdy.o \
	     getuid.o htonl.o htons.o ioctl.o insque.o kill.o  kloadcall.o\
	     link.o listen.o lseek.o lstat.o madvise.o mkdir.o \
	     mknod.o mmap.o mount.o mprotect.o \
	     msgctl.o msgget.o msgrcv.o msgsnd.o msleep.o msync.o \
	     munmap.o mvalid.o mwakeup.o msem_tas.o \
	     ngetpeername.o nrecvfrom.o nsendmsg.o \
	     naccept.o ngetsockname.o nrecvmsg.o \
	     nfssvc.o ntohl.o ntohs.o open.o pipe.o plock.o poll.o profil.o \
	     ptrace.o quotactl.o read.o readlink.o readv.o \
	     reboot.o recv.o recvfrom.o recvmsg.o rename.o remque.o revoke.o\
	     rmdir.o  select.o semctl.o semget.o \
	     semop.o send.o sendmsg.o sendto.o setgid.o \
	     setdomainnm.o \
	     setgroups.o sethostid.o sethostname.o setitimer.o \
	     setjmp.o setlogin.o setpattr.o setpgid.o  \
	     setpriority.o setregid.o setreuid.o \
	     setrlimit.o setsid.o setsockopt.o \
	     settimeofdy.o setuid.o shutdown.o shmat.o shmctl.o \
	     shmdt.o shmget.o sigaction.o \
	     sigreturn.o sigstack.o \
	     socket.o socketpair.o stat.o statfs.o \
	     swapon.o symlink.o sync.o syscall.o table.o truncate.o \
	     umask.o uname.o unlink.o  \
	     utimes.o wait4.o write.o writev.o \
	     setpgrp.o umount.o 

#not shared -  modf.o brk.o sbrk.o

umount.o_CFLAGS		= -DSYS_umount=SYS_unmount
crt0.o_CFLAGS		= -DCRT0 -DMACH
fpinit.o_CFLAGS		= -DASSEMBLER

# N.B. Remove the following two make rules when frexp.s and ldexp.s
# are no longer in the source tree.  Unfortunately, make uses the
# default .s.o rule and picks up the .s files when the .c files are
# true sources.

frexp.o: frexp.c
	${_CC_} ${_CCFLAGS_} -c $*.c

ldexp.o: ldexp.c
	${_CC_} ${_CCFLAGS_} -c $*.c

${AT386SOBJS:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${AT386SOBJS}: $${@:.o=.S}
	${_CC_} ${_CCFLAGS_} -traditional -O -c $*.S
#	${LD} -x -r $*.o
#	${MV} -f a.out $*.o

#${AT386SOBJS}: $${@:.o=.s}
#	${CP} $*.s $*.S
#	${_CC_} -traditional ${_CCFLAGS_} ${ALLSFLAGS} -E $*.S > $*.i
#	${RM} -f $*.S
#	grep -v "^#" $*.i > $*.as
#	${RM} -f $*.i
#	${AS} ${ASFLAGS} $*.as -o $*.o
#	${RM} -f $*.as
#	${LD} -x -r $*.o
#	${MV} -f a.out $*.o


