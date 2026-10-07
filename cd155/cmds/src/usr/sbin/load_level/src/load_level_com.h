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
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: load_level_com.h,v $
 * Revision 1.4  1995/02/01  23:45:36  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.3  1994/11/19  03:07:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/11/18  20:53:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.1.1.3  1993/07/01  21:16:55  cfj
 * Adding new code from vendor
 *
 * Revision 3.0  93/04/23  16:04:04  bolsen
 * [SPE 0007] added common structures, variables and NEW macros for shared
 * 	memory (ie. common to load_leveld and onnode (fast & fastnode)).
 * 
 */

/* maximum number of load vector elements */
#define MAX_NUM_LVEC_ELEMENTS	200

/* maximum size of key_file array */
#define MAX_KEY_FILE_SIZE	1024

/* key directory name */
#define KEY_DIR		"/etc/load_level/shm_keys"

/* get key macro */
#define GET_KEY(key, key_file)						\
    key = ftok(key_file, (char)((node_self()) % 128) + 1);

/* get key file name */
#define GET_KEY_FILE(key_file, key_dir)					\
    sprintf(key_file, "%s/key_%d", key_dir, ((node_self())/128));


struct load_vec_info
{
    int node; /* node number */
    double lm; /* load measure */
};

struct load_info
{
    int num_elements; /* number of elements in the load vector */
    struct load_vec_info load_vector[MAX_NUM_LVEC_ELEMENTS];
};


/* key */
key_t key;

/* pointer to the string containing the pathname to a directory in which
   the key files for shared memory are created */
char *key_dir;

/* key file name */
char key_file[MAX_KEY_FILE_SIZE];

