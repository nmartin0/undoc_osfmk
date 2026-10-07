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
 *   user_interf.h
 *
 *   Contents:
 *	Functions to interact with the user (mainly, I/O functions).
 *	See the LVM commands library module "user_interf.c".
 */

int read_uint(unsigned int *ptr);
int read_line(char *buf, int sizeofbuf);
int lvm_perror(int ioctl_id);
