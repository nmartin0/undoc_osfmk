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
 
#if SEC_BASE && SEC_ACL_SWARE

/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

/*	Copyright (c) 1988 SecureWare, Inc.  All rights reserved.

	This Module contains Proprietary Information of SecureWare, Inc.
	and should be treated as Confidential.

	This is the include file for Access Control List Defines and
	Structure Definitions.

	@(#)acl.h	5.1 15:35:34 8/15/90
	Based on:
	  "@(#)acl.h	2.1 12:33:23 1/25/89"
*/


#ifndef __ACL__
#define __ACL__

/* Definition of Special ACL Owner/Group Id Values */

#define ACL_OWNER	   (-3)		/* match object owner */
#define ACL_WILDCARD	   (-4)		/* match any ID */

/* argument that causes access to be governed by mode bits */

#define ACL_DELETE	((acle_t *) -1)	/* remove ACL from object */

/* Mode Bit Defines for ACL Mode Word */

#define ACL_READ	0x4		/* ACL Read bit in mode */
#define ACL_WRITE	0x2		/* ACL Write bit in mode */
#define ACL_EXEC	0x1		/* ACL Exec bit in mode */

/* Return status definitions for library routines */

#define	ACL_ERR		0
#define	ACL_OK		1

/* structure definitions for acl entries and arguments to ipc acl calls */

typedef struct {
	uid_t 	acl_uid;		/* user ID */
	gid_t	acl_gid;		/* group ID */
	ushort	acl_perm;		/* permissions */
} acle_t;

typedef struct {
	int	acl_size;		/* in:  elems in acl_entries */
					/* out: entries in acl */
	acle_t	*acl_entries;		/* pointer to ACL buffer */
} ipcacl_t;

/* Defines for ACL synonym database files */

#define	ACL_DBENV	"ACLSYNDB"
#define	ACL_DBFILE	".acl"
#define	ACL_MAGIC	0xC1C3CCD3

/* Layout of database file header */

struct dbhead {
	int		magic;		/* magic number */
	int		syns;		/* number of synonym structures */
	int		strings;	/* number of bytes of synonym names */
	int		entries;	/* number of ACL entries */
};

/* Structure of a synonym definition */

typedef struct aclsyn_t {
	char		*syn_name;	/* name of synonym */
	int		syn_count;	/* number of ACL entries */
	acle_t		*syn_ents;	/* array of ACL entries */
	struct aclsyn_t	*syn_next;	/* link to next synonym */
} aclsyn_t;

/* Definition of the ACL configuration parameter file */

struct acl_config {
	char		dbase[50];
	unsigned long	cache_size;
	unsigned long	buffers;
	unsigned short	subj_tags;
	unsigned short	obj_tags;
	unsigned short	first_subj_tag;
	unsigned short	first_obj_tag;
	dev_t		minor_device;
	unsigned short	policy;
};

/* Defines for required ACL policy files */

#define ACL_PARAM_FILE	"/etc/policy/acl/config"

/* Variables defined by the library */

extern struct acl_config acl_config;

/* Routines provided by the ACL library */

int		acl_init();		/* init parms but not databases */
int		acl_delete_syn();	/* remove a synonym from the list */
int		acl_read_syn_db();	/* read a synonym database file */
int		acl_write_syn_db();	/* write a synonym database file */
int		acl_expand_syn();	/* increase size of synonym's ACL */
int		acl_1ir_to_er();	/* convert 1 entry to external rep */
int		acl_load_syns();	/* load synonym database from a file */
int		acl_store_syns();	/* store synonym database to a file */
void		acl_free_syn();		/* deallocate an ACL synonym struct */
void		acl_insert_syn();	/* insert a synonym into the list */
char		*acl_ir_to_er();	/* convert internal to external rep */
acle_t		*acl_er_to_ir();	/* convert external to internal rep */
aclsyn_t	*acl_lookup_syn();	/* lookup an ACL synonym */
aclsyn_t	*acl_alloc_syn();	/* allocate an ACL synonym struct */

#endif  /* __ACL__ */
#endif	/* SEC_BASE && SEC_ACL_SWARE */
