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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/* @(#)secpolicy.h	6.2 15:10:06 2/17/91 SecureWare */
#ifndef __SECPOLICY__
#define __SECPOLICY__

/*
 * Copyright (c) 1988-1990 SecureWare, Inc.  All rights reserved.
 *
 * Based on:
 *	@(#)secpolicy.h	2.6.2.3 14:35:33 12/27/89 SecureWare
 *
 * This is unpublished proprietary source code of SecureWare, Inc. and
 * should be treated as confidential.
 */

#if SEC_ARCH

#include <sys/secdefines.h>
#ifdef _KERNEL
#include <kern/lock.h>
#endif

/*
 * The last slot in each object tag pool is reserved to hold an object
 * number for use in indexing into the process bitmaps used to maintain
 * information labels.  We therefore define a new constant for those
 * places that are interested in the number of policy tags, and leave
 * the old one for those places that care about the size of the tag pool.
 */
#if SEC_ILB
#define	SEC_NUM_TAGS	(SEC_TAG_COUNT - 1)
#else
#define	SEC_NUM_TAGS	SEC_TAG_COUNT
#endif

/* Policy independent wildcard tag value */

#define SEC_WILDCARD_TAG_VALUE	0
#define	sec_wildcard(t)		(*(t) == SEC_WILDCARD_TAG_VALUE)

/* Maximum IR size that the kernel can handle */

#define SEC_MAX_IR_SIZE		8192

/*
 * Prototype structure for IRs imbedded in policy daemon messages.
 * The actual data associated with the IR immediately follows the
 * length field.
 */

typedef struct {
	ulong	ir_length;	/* length of internal representation */
} ir_t;

/* Prototype structure for decisions imbedded in policy daemon messages.
 * The actual data associated with the decision immediately follows the
 * length field.
 */

typedef struct {
	ulong	dec_length;	/* decision length */
} dec_t;

/*
 * Indicator for whether the internal representation comes from kernel
 * or user space for SP_MAPTAG().
 */

#define SEC_IR_KERNEL	0
#define SEC_IR_USER	1

/*
 * Security policy module/daemon message types
 */

#define SPD_MAP_TAG		1	/* Map Tag to IR */
#define SPD_SET_TAG		2	/* Set Tag on Object */
#define SPD_MAKE_DECISION	3	/* Make an Access Decision */
#define	SPD_DECISION		4	/* Decision from Daemon */
#define SPD_GET_ATTRIBUTE	5	/* Read IR for Tag */
#define SPD_INTERNAL_REP	6	/* Response with IR for Tag */
#define SPD_CHANGE_DISCR	7	/* Change Discretionary */
#define SPD_NEW_DISCR		8	/* Change Discretionary Response */
#define SPD_SHUTDOWN		9	/* Terminate the Daemon */
#define SPD_SHUTDOWN_RESP	10	/* ACK the Shutdown Request */
#define SPD_PREPARE_SHUTDOWN	11	/* Shutdown Notification */
#define SPD_PREPARE_RESP	12	/* Shutdown Notification Response */
#define SPD_ERROR		13	/* Error Message type */


/*
 * Security policy module/daemon message formats
 */

/*********** Common Security Policy Message Header Format ************/

typedef struct spd_message_header {
	long	sync_id;	/* process sync id for driver */
	short	msg_type;	/* request-response type */
	short	mh_flags;	/* state field */
	short	error_code;	/* error return for audit */
} mhdr_t;

#define SPD_MHDR_SIZE	(sizeof(mhdr_t))

/*********** Message Pair for Mapping IRs to Tags ************/

struct spd_map_tag {
	mhdr_t		mhdr;		/* message header */
	udac_t		unixdac;	/* owner/creator uid/gid, mode */
	short		which_tag;	/* policy-specific tag number */
	attrtype_t	ir_type;	/* is tag for subject or object */
	ir_t		ir;		/* IR to be mapped to a tag */
};

#define SPD_MAP_TAG_SIZE	(sizeof(struct spd_map_tag))

struct spd_set_tag {
	mhdr_t		mhdr;		/* message header */
	tag_t		tag;		/* tag returned */
	dac_t		obj_discr;	/* object discretionary */
};

#define SPD_SET_TAG_SIZE	(sizeof(struct spd_set_tag))

/*********** Message Pair for Requesting Policy Decisions ************/

struct spd_make_decision {
	mhdr_t	mhdr;			/* message header */
	tag_t	subject;		/* subject tag */
	tag_t	object;			/* object tag */
};

#define SPD_MAKE_DEC_SIZE	(sizeof(struct spd_make_decision))

struct spd_decision {
	mhdr_t	mhdr;			/* message header */
	tag_t	subject;		/* subject tag */
	tag_t	object;			/* object tag */
	dec_t	decision;		/* policy decision */
};

#define SPD_DECISION_SIZE	(sizeof(struct spd_decision))

/*********** Message Pair for Mapping Tags to IRs ************/

struct spd_get_attribute {
	mhdr_t		mhdr;		/* message header */
	tag_t		tag;		/* tag to map to ir */
	attrtype_t	tag_type;	/* subject or object */
};

#define SPD_GET_ATTR_SIZE	(sizeof(struct spd_get_attribute))

struct spd_internal_rep {
	mhdr_t	mhdr;			/* message header */
	tag_t	tag;			/* tag that was mapped */
	ir_t	ir;			/* internal representation */
};

#define SPD_INTERNAL_REP_SIZE	(sizeof(struct spd_internal_rep))

/*********** Message Pair for Discretionary Changes ************/

struct spd_change_discr {
	mhdr_t		mhdr;		/* message header */
	tag_t		object_tag;	/* object tag value */
	dac_t		object_dac;	/* new owner, group, and mode */
	attrtype_t	object_type;	/* subject or object */
	ushort		object_flags;	/* indicates which dac changed */
};

#define SPD_CHANGE_DISCR_SIZE	(sizeof(struct spd_change_discr))

struct spd_new_discr {
	mhdr_t	mhdr;			/* message header */
	tag_t	object_tag;		/* new object tag */
	dac_t	object_discr;		/* new owner, group, and mode */
	ushort	object_flags;		/* indicates which dac to change */
};

#define SPD_NEW_DISCR_SIZE	(sizeof(struct spd_new_discr))

/*
 * Error Codes for Message Passing Functions
 */

#define SPD_OK		0		/* No error */
#define SPD_EFULLDB	1		/* Database is full */
#define SPD_ENOSUBJTAG	2		/* No subject tag found */
#define SPD_ENOOBJTAG	3		/* No object tag found */
#define SPD_EBADIREP	4		/* Bad internal representation */
#define SPD_EDEADLOCK	5		/* Re-send message after wait */
#define SPD_EBADMSG	6		/* Bad message type */

/*
 * Security Policy Magic Numbers for Configuraton Recognition
 * Changes to these definitions must be reflected in the policy_name[]
 * table in seccmd/reduce.h.
 */

#define SEC_ACL_MAGIC		0xf000	/* Access Control Lists */
#define SEC_MAC_MAGIC		0xf001	/* Mandatory Access Control */
#define SEC_NCAV_MAGIC		0xf003	/* Nationality Caveats */
#define SEC_MACILB_MAGIC	0xf004	/* MAC and Information Labels */

#define	SEC_MAGIC_MAX		SEC_MACILB_MAGIC

#define	SEC_MAGIC_COUNT		(((unsigned short) SEC_MAGIC_MAX - \
				  (unsigned short) SEC_ACL_MAGIC) + 1)
/*
 * Map a tag within a policy to a tag pool offset
 */

/*
 * Ioctl Structure Definitions 
 */

/* Structure for SPIOC_INIT and SPIOC_GETCONF commands */

struct sp_init {
	ulong	cache_size;		/* number of cache entries */
	ulong	cache_width;		/* decision size */
	ushort	magic;			/* policy magic number */
	ushort	spminor;		/* policy minor device */
	ushort	policy;			/* policy number */
	ushort	first_subj_tag;		/* first subject tag */
	ushort	subj_tag_count;		/* subject tag count */
	ushort	first_obj_tag;		/* first object tag */
	ushort	obj_tag_count;		/* object tag count */
};

#define SP_INITSIZE	(sizeof(struct sp_init))

/* Structure for SPIOC_INV_TAG command */

struct sp_inv_tag {
	tag_t	tag;			/* tag to invalidate */
};

/* Structure for SPIOC_SET_CACHE_SIZE command */

struct sp_set_cache_size {
	ulong	cache_size;		/* new cache size */
	ulong	cache_width;		/* new cache width */
};

/* Structure for SPIOC_GET_STATS command */

struct sp_mod_stats {
	ulong	decisions;		/* decisions made */
	ulong	queries;		/* daemon queries made */
	ulong	responses;		/* responses from daemon */
	ulong	collisions;		/* cache entry collisions */
	ulong	delay;			/* aggregate delay */
	ulong	max_delay;		/* maximum decision delay */
	ulong	cache_size;		/* number of cache entries */
	ulong	cache_width;		/* decision size */
};

#define SP_MOD_STAT_SIZE	(sizeof(struct sp_mod_stats))

#ifdef KERNEL
extern struct sp_mod_stats sp_mod_stats[];
#endif

/**************** Security Policy Configuration Definitions ****************/

/* Security Policy Configuration and Message Driver Files */

#define SP_CONFIG_FILE		"/tcb/files/sp_config"
#define SP_DAEMON_DEVICE	"/dev/spd"

/* Security Policy Configuration Devices */

#define	SPD_CONTROL_DEVICE	"/dev/spdcontrol"
#define SPD_CONTROL_MINOR	0xff		/* minor device number */

/* Security Policy Configuration File Format */

struct sec_policy_conf {
	char	daemon[64];	/* policy daemon executable */
	char	confdir[64];	/* policy daemon config directory */
};

#ifdef KERNEL

/*
 * Each policy daemon may have a decision cache.
 * Cache management routines in sec/spd_cache.c manipulate these cache
 * entries.
 */

#define DCACHE_BUCKETS	4		/* number of cache buckets per slot */

/* Definition of each element of the decision cache */

struct dcache_entry {
	tag_t	tag1;			/* first tag to hash */
	tag_t	tag2;			/* second tag to hash */
	time_t	time;			/* LRU time value */
	char	*decision;		/* decision pointer */
	struct {
		uint	valid:1;	/* valid entry flag */
	} dc_flags;
};

/*
 * This is the definition of a decision cache hash slot.
 * 
 *			 MP locking notes
 * On multiprocessor systems, a spin lock exists for each hash
 * slot in a cache.  The lock protects all buckets in that slot.  No
 * thread will ever hold more than one cache slot lock at a time, nor
 * will it pend with a cache slot lock held.
 */
 
struct decision_cache {
  decl_simple_lock_data(, lock)
  struct dcache_entry bucket[DCACHE_BUCKETS];
};

typedef struct decision_cache dec_cache_t;

extern dec_cache_t **dcachep;

#define DEC_CACHE_ENTRY_SIZE	(sizeof(struct decision_cache))
#endif

#if SEC_ACL

/*
 * Definitions for Access Control Lists
 */

typedef struct {
	udac_t		acl_head;	/* Creator/owner uid/gid, mode */
/*	acle_t		acl_entry[1];	   COMMENT - ACL entries */
} acl_t;

/* tag allocation */

#define ACL_SUBJ_TAG	0	/* subject identity */
#define ACL_OBJ_TAG	0	/* object access control list */

#define	ACL_HEADERSIZE	(sizeof(udac_t))

/* Access Control List Policy Special Tag Values */

#define SEC_ACL_NULL_TAG	1L	/* empty ACL-no access */

/* ACL Internal Representation Structure */

struct acl_ir {
	ulong	ir_length;		/* length of ir */
	acl_t	ir;			/* ACL header and entries */
};

/* Access Control List Cache Decision Format */

struct acl_decision {
	ulong	dec_length;		/* decision length */
	struct {
		uint	read:1,		/* subject may read object */
			write:1,	/* subject may write object */
			exec:1,		/* subject may execute object */
			delete:1,	/* subject may delete object */
			rfu:28;
	} dec;
};

typedef struct acl_decision acl_dec_t;

#define ACL_DECISION_SIZE	(sizeof(struct acl_decision) - sizeof(long))

struct acl_cache_decision {
	struct {
		uint	read:1,		/* subject may read object */
			write:1,	/* subject may write object */
			exec:1,		/* subject may execute object */
			delete:1,	/* subject may delete object */
			rfu:28;
	} decision;
};

typedef struct acl_cache_decision acache_dec_t;

#define	ACL_CACHE_WIDTH	(sizeof(struct acl_cache_decision))

#endif /* SEC_ACL */

#if SEC_MAC

/*
 * Definitions for Mandatory Access Control
 */

/* MAC Special Tag Values */

#define SEC_MAC_NULL_TAG	0L	/* no access */
#define SEC_MAC_SYSLO_TAG	2L	/* system lo tag */
#define SEC_MAC_SYSHI_TAG	3L	/* system hi tag */

/* MAC tag allocation */

#define MAND_SUBJ_SL_TAG	0	/* subject sensitivity label */
#define MAND_OBJ_SL_TAG		0	/* object sensitivity label */
#define MAND_SUBJ_CL_TAG	1	/* subject clearance */
#if SEC_ILB
#define MAND_SUBJ_IL_TAG	2	/* subject information label */
#define MAND_OBJ_IL_TAG		1	/* object information label */
#endif

/* MAC Internal Representation Structure */

struct mac_ir {
	ulong		ir_length;	/* length of ir */
	ulong		class;		/* classifications */
					/* compartments and markings follow */
};

#define MAC_IR_SIZE	(sizeof(struct mac_ir))

/* MAC Decision Format */

struct mac_decision {
	ulong	dec_length;		/* decision length */
	struct {
		uint	subj_dom:1,	/* subject dominates object */
			obj_dom:1,	/* object dominates subject */
			same:1,		/* levels are equal */
			incomp:1,	/* levels are incomparable */
#if SEC_ILB
			nocomb:1,	/* labels can't be combined */
			ilb_subj_dom:1,	/* subject IL dominates object IL */
			ilb_obj_dom:1,	/* object IL dominates subject IL */
			ilb_same:1,	/* subject and object are same IL */
			rfu:24;
#else
			rfu:28;
#endif
	} dec;
#if SEC_ILB
	tag_t	combination;
#endif
};

typedef struct mac_decision mac_dec_t;

#define MAC_DECISION_SIZE	(sizeof(struct mac_decision) - sizeof(long))

/* MAC Cache Decision Format */

struct mac_cache_decision {
	struct {
		uint	subj_dom:1,	/* subject dominates object */
			obj_dom:1,	/* object dominates subject */
			same:1,		/* levels are equal */
			incomp:1,	/* levels are incomparable */
#if SEC_ILB
			nocomb:1,	/* labels can't be combined */
			ilb_subj_dom:1,	/* subject IL dominates object IL */
			ilb_obj_dom:1,	/* object IL dominates subject IL */
			ilb_same:1,	/* subject and object are same IL */
			rfu:24;
#else
			rfu:28;
#endif
	} decision;
#if SEC_ILB
	tag_t	combination;
#endif
};

typedef struct mac_cache_decision mcache_dec_t;

#define	MAC_CACHE_WIDTH	(sizeof(struct mac_cache_decision))

/* Return values from mac_getdecision */

#define	MAC_SDOM	1
#define	MAC_ODOM	2
#define	MAC_INCOMP	4
#define	MAC_SAME	8
#if SEC_ILB && !defined(ILB_SDOM)
#define	ILB_SDOM	0x10
#define	ILB_ODOM	0x20
#define	ILB_SAME	0x40
#endif /* SEC_ILB && !defined(ILB_SDOM) */

#endif /* SEC_MAC */

#if SEC_NCAV

/* Nationality Caveat Policy Decision Format */

struct ncav_decision {
	ulong	dec_length;		/* decision length */
	struct {
		uint	subj_dom:1,	/* subject dominates object */
			obj_dom:1,	/* object dominates subject */
			same:1,		/* levels are equal */
			incomp:1,	/* levels are incomparable */
			rfu:28;
	} dec;
};

typedef struct ncav_decision ncav_dec_t;

#define NCAV_DECISION_SIZE	(sizeof(struct ncav_decision) - sizeof(long))

/* Nationality Caveat tag allocation */

#define NCAV_SUBJ_TAG	0	/* nationality caveat subject tag */
#define NCAV_OBJ_TAG	0	/* nationality caveat object tag */

/* Nationality Caveat Cache Decision Format */

struct ncav_cache_decision {
	struct {
		uint	subj_dom:1,	/* subject dominates object */
			obj_dom:1,	/* object dominates subject */
			same:1,		/* levels are equal */
			incomp:1,	/* levels are incomparable */
			rfu:28;
	} decision;
};

typedef struct ncav_cache_decision ncache_dec_t;

#define	NCAV_CACHE_WIDTH	(sizeof(struct ncav_cache_decision))

#endif /* SEC_NCAV */

#ifdef KERNEL
/*
 * This is the definition of the Security Policy Switch. This
 * switch serves as the connection between the various security
 * policy checks such as access or object create and the Security
 * Modules that are responsible for making the policy decisions.
 */

struct sec_module_desc {
	int	(*sw_init)	    (/* policy */);
	int	(*sw_access)	    (/* subjtags,objtags,mode,dcheck,udac */);
	int	(*sw_obj_create)    (/* subjtags,objtags,partags,attrtype */);
	int	(*sw_obj_delete)    (/* subjtags,objtags,attrtype */);
	int	(*sw_getattr)	    (/* attrtype,tag,attr */);
	int	(*sw_setattr)	    (/* attrtype,tags,tagnum,new */);
	int	(*sw_setattr_check) (/* objtype,objp,parent,tags,tagnum,new */);
	int	(*sw_maptag) 	    (/* attrtype,tagnum,attr,udac,new */);
	ushort	sw_magic;
	ushort	sw_priv_required;
	char	sw_subj_count;
	char	sw_obj_count;
	char	sw_first_subj_tag;
	char	sw_first_obj_tag;
};

extern struct sec_module_desc sp_switch[];
extern int spolicy_cnt;

/*
 * Macros to access elements of the policy switch
 */
#define	SP_INIT(policy)		(*sp_switch[policy].sw_init)(policy)
#define	SP_GETATTR(policy,attrtype,tag,attr)\
	(*sp_switch[policy].sw_getattr)(attrtype,tag,attr)
#define	SP_SETATTR(policy,attrtype,tags,tagnum,new)\
	(*sp_switch[policy].sw_setattr)(attrtype,tags,tagnum,new)
#define	SP_SETATTR_CHECK(policy,objtype,objp,par,tags,tagnum,new)\
	(*sp_switch[policy].sw_setattr_check)(objtype,objp,par,tags,tagnum,new)
#define	SP_MAPTAG(policy,attrtype,tagnum,attr,udac,new)\
	(*sp_switch[policy].sw_maptag)(attrtype,tagnum,attr,udac,new)

#define	OBJECT_TAG(policy,which)   (sp_switch[policy].sw_first_obj_tag  + which)
#define	SUBJECT_TAG(policy,which)  (sp_switch[policy].sw_first_subj_tag + which)
#define	SUBJECT_TAG_COUNT(policy)	(sp_switch[policy].sw_subj_count)
#define	OBJECT_TAG_COUNT(policy)	(sp_switch[policy].sw_obj_count)

/*
 * Security Policy Miscellaneous Function Table
 */

struct sec_functions {
	int	(*sw_owner)		(/* owner, creator */);
	int	(*sw_change_subj)	();
	int	(*sw_change_obj)	(/* tags, newdac, flags */);
	int	sw_discr_index;
#if SEC_ILB
	int	(*sw_check_float)	(/* rw, tags, new */);
	void	(*sw_do_float)		(/* rw, tags, new */);
	void	(*sw_empty_object)	(/* tags */);
#endif
};

extern struct sec_functions sp_functions;

/*
 * Macros to call members of the sec_functions table
 */

#if SEC_ARCH
#define	sec_owner(ouid,cuid)	(*sp_functions.sw_owner)(ouid, cuid)
#else
#define	sec_owner(ouid,cuid)\
	(u.u_uid==(ouid) || u.u_uid==(cuid) || privileged(SEC_OWNER, EPERM))
#endif

#define	SP_CHANGE_SUBJECT()	(*sp_functions.sw_change_subj)()
#define	SP_CHANGE_OBJECT(tags,newdac,flags)\
	(*sp_functions.sw_change_obj)(tags,newdac,flags)

#if SEC_ILB
#define	SP_CHECK_FLOAT(rw,tags,new)\
	(*sp_functions.sw_check_float)(rw,tags,new)
#define	SP_DO_FLOAT(rw,tags,new)\
	(*sp_functions.sw_do_float)(rw,tags,new)
#define	SP_EMPTY_OBJECT(tags)	(*sp_functions.sw_empty_object)(tags)
#endif /* SEC_ILB */


/*
 * Macros and declarations for tag pools that are allocated separately
 * from their associated object data structures (IPC objects and ptys).
 */

extern tag_t semtag[], shmtag[], msgtag[];

#define SEMTAG(sp,t)	(&semtag[((((sp) - sema) * SEC_TAG_COUNT) + (t))])
#define SHMTAG(sp,t)	(&shmtag[((((sp) - shmem) * SEC_TAG_COUNT) + (t))])
#define MSGTAG(mp,t)	(&msgtag[((((mp) - msgque) * SEC_TAG_COUNT) + (t))])

#if SEC_PTY

extern tag_t ptctag[], ptstag[];

#define	PTCTAG(d,t)	(&ptctag[(minor(d) * SEC_TAG_COUNT) + (t)])
#define	PTSTAG(d,t)	(&ptstag[(minor(d) * SEC_TAG_COUNT) + (t)])

#endif /* SEC_PTY */

/*
 * External declarations for policy specific information (sec_conf.c)
 */

#if SEC_ACL
extern int aclpolicy, aclobjtag, aclsubjtag;
#endif

#if SEC_MAC
extern int macpolicy, macobjtag, macsubjtag, macclrnce;
#endif

#if SEC_ILB
extern int ilbobjtag, ilbsubjtag;
#endif

#if SEC_NCAV
extern int ncavpolicy, ncavobjtag, ncavsubjtag;
#endif

#if SEC_ILB

/*
 * Macros and declarations for Information Label Policy
 */

#define	SEC_OBJNUM(o)	((int) (o)[SEC_NUM_TAGS])
#define	SEC_SETOBJNUM(o,n)	((o)[SEC_NUM_TAGS] = (tag_t)(n))
#define	SEC_OBJ_BITS(o)	(&sec_obj_bits[SEC_OBJNUM(o) * sec_obj_mapwords])
#define	sp_free_bits(t)

mask_t	*sec_obj_bits;		/* pointer array for object bitmaps */
mask_t	*sec_objnummap;		/* object number allocation bitmap */
int	sec_curobjmap;		/* current index into sec_objnummap */
int	sec_lastobjmap;		/* index of last word in sec_objnummap */
int	nsocket;		/* number of dynamic objects (sockets) */
int	sec_nobject;		/* number of objects in the system */
int	sec_firstdynobj;	/* first dynamic object number */
int	sec_obj_mapwords;	/* number of mask_t's in an object bitmap */
int	sec_subj_mapwords;	/* number of mask_t's in a subject bitmap */

extern mask_t	*sp_bitmap_alloc();

#endif /* SEC_ILB */
#endif /* KERNEL */

#endif /* SEC_ARCH */
#endif /* __SECPOLICY__ */
