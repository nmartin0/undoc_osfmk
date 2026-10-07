/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */


#ifndef _SYSENT_H_
#define _SYSENT_H_

#include <standards.h>

_SSD_FUNC_PROTO_BEGIN_

#include <sys/types.h>
#include <sys/wait.h>

int acct __protomagic((const char*));
int chdir __protomagic((const char*));
int chroot __protomagic((const char*));
void sync __protomagic((void));
int system __protomagic((const char*));

int mknod __protomagic((const char*, int, int));
int sethostid __protomagic((long));
int getdomainname __protomagic((char*, int));
int gethostname __protomagic((char*, int));
void profil __protomagic((short *, unsigned int, unsigned int, unsigned int));
int setdomainname __protomagic((const char*, int));
int sethostname __protomagic((const char*, int));
int shutdown __protomagic((int, int));
int swapon __protomagic((const char*));
pid_t vfork __protomagic(());
int getdtablesize __protomagic(());
int flock __protomagic((int, int));
long gethostid __protomagic(());
int getpagesize __protomagic(());
int killpg __protomagic((int, int));
int syscall __protomagic((int ...));
int vadvise __protomagic((int));
int mount __protomagic((const char*, const char*, int));
int umount __protomagic((const char*));
int sbrk __protomagic((int));
int setpgrp __protomagic((pid_t, pid_t));
int setreuid __protomagic((int, int));
int setregid __protomagic((int, int));
void vhangup __protomagic(());
int setegid __protomagic((int));
int seteuid __protomagic((int));
int setrgid __protomagic((int));
int setruid __protomagic((int));

_SSD_FUNC_PROTO_END_

#endif /* _SYSENT_H_ */
