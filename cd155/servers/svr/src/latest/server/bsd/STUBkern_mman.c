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
 
madvise() {printf("madvise: ---\n"); return 0;}
mincore() {printf("mincore: ---\n"); return 0;}
mprotect() {printf("mprotect: ---\n"); return 0;}
mremap() {printf("mremap: ---\n"); return 0;}
munmap() {printf("munmap: ---\n"); return 0;}
obreak() {printf("obreak: ---\n"); return 0;}
ovadvise() {printf("ovadvise: ---\n"); return 0;}
sbrk() {printf("sbrk: ---\n"); return 0;}
smmap() {printf("smmap: ---\n"); return 0;}
sstk() {printf("sstk: ---\n"); return 0;}

#include <uxkern/import_mach.h>

/* ARGSUSED */
getpagesize(p, args, retval)
        struct proc *p;
        void *args;
        int *retval;
{

        *retval = vm_page_size;
        return (0);
}
