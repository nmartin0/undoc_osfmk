/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#include <i860paragon/vcf/vcf_pmachine.h>

#ifndef _MSGIN_H_

#define _MSGIN_H_

#define  VALID_WRITE_ADDRESS(addr)  1
/* Should be:
 *
 * PageDirEntry = *(int *)(DTB + (((int)addr >> 20) & 0xffc))
 * if ((PageDirEntry & 7) != 7)  /* User, writable, & present. * /
 *   return(0);
 * PageAddress = *(int *)((PageDirEntry & ~0xfff) +
 *                        (((int)addr >> 10) & 0xffc));
 * if ((PageAddress & 7) != 7)  /* User, writable, & present. * /
 *   return(0);
 * return(1);
 */

extern void  (*in_ltu_active)(void);
extern void  vcf_handle_message(int msgtype, vcf_chand ch);
extern unsigned int INVERT_PATH(unsigned int path);

#endif
