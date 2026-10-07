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
 * data structures used to hold parsed rc file for sandboxes
 */

struct arg_list {
    struct arg_list *next;
    char **tokens;
    int ntokens;
    int maxtokens;
};

struct field {
    struct field *next;		/* next field */
    char *name;			/* name of this field */
    struct arg_list *args;	/* args for this field */
};

#define RC_HASHBITS 6		/* bits used in hash function */
#define RC_HASHSIZE (1<<RC_HASHBITS) /* range of hash function */
#define RC_HASHMASK (RC_HASHSIZE-1)	/* mask for hash function */
#define RC_HASH(i) ((i)&RC_HASHMASK)

struct hashent {
    struct hashent *next;
    struct field *field;
};

struct rcfile {
    struct hashent *hashtable[RC_HASHSIZE];
    struct field *list;
    struct field *last;
};
