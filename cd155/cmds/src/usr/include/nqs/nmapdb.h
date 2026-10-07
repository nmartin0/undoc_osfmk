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
 
/*++ nmapdb.h - nmap database internal structure
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nmapdb.h,v $
 *
 * DESCRIPTION:
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:44:01 $ $State: Exp $)
 * $Log: nmapdb.h,v $
 * Revision 1.4  1994/11/19  02:44:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:06:07  mwan
 * R1.2 mods.
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:56  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:59:45  hender
 * Sterling version 4/22/87
 * 
 *
 */
/*
 *	Configurable #defs that can be tailored for an individual system.
 */
#define	MAX_PATHLENGTH	1024	/* The maximum length of a file pathname */

#ifndef MAP_DIRECTORY                /* Mike: allows varied places for directory */
#define MAP_DIRECTORY   "/etc/nmap"
#endif
				/* Map database file directory.  ALL nmap */
				/* files are placed here.  This directory */
				/* should be created by the cmd/nmap */
				/* makefile.  The nmap_create() */
				/* function does NOT automatically create */
				/* this directory. */

#define MACHINES	"machines"
				/* Name of machine to mid mapping file in */
				/* the directory:  MAP_DIRECTORY. */

#define	MAX_NAMELENGTH	255	/* Maximum length of a machine name */
				/* The maximum length of a machine name */
				/* plus the associated structure overhead */
				/* (see name_map struct defn below) must */
				/* NOT exceed ATOMICBLKSIZ bytes in length! */

#define	ALIGNTYPE	double	/* Alignment type */

#define	HASHNAMENTS	256	/* Number of machine-name to mid hash chains */

#define	HASHENTSPERBLK	(ATOMICBLKSIZ / sizeof (long))
				/* Number of entries per block for a hash */
				/* table. */

#define	HASHNAMBLKS	((HASHNAMENTS - 1) / HASHENTSPERBLK + 1)
				/* Number of blocks that must be allocated */
				/* for the machine-name to mid hash table */
/*
 *	WARNING:
 *
 *	These relations may have to be changed for some compiler/machine
 *	implementations based upon any padding algorithms the compiler
 *	follows when declaring structures.
 */
#define	GIDHDRSIZ	(sizeof (struct gidmap8) - 8 * sizeof (gid_t))

#define	UIDHDRSIZ	(sizeof (struct uidmap8) - 8 * sizeof (uid_t))

#define	GIDSPERBLK	((ATOMICBLKSIZ - GIDHDRSIZ) / (2 * sizeof (gid_t)))
				/* The number of group-id mappings per block */

#define	UIDSPERBLK	((ATOMICBLKSIZ - UIDHDRSIZ) / (2 * sizeof (uid_t)))
				/* The number of user-id mappings per block */

struct xidparm {
	union {
		struct {
			uid_t remote_uid;
			uid_t local_uid;
		} u;
		struct {
			gid_t remote_gid;
			gid_t local_gid;
		} g;
	} u;
};


/* Data structures for database */

struct m_header {
	char name [MAX_NAMELENGTH+1];
				/* Null terminated machine name.	   */
	char defuid_enabled;	/* Default user-id mapping enabled.	   */
	char defgid_enabled;	/* Default group-id mapping enabled.	   */
	uid_t defuid;		/* Default user-id mapping value.	   */
	gid_t defgid;		/* Default group-id mapping value.	   */
	time_t enatime;		/* When to reenable mapping, 0=never, 1=on */
};

struct gidmap8 {		/* Only used for GIDSPERBLK computation	*/
	long next;		/* Addr of next block in hash chain	*/
	unsigned n_pairs;	/* There are this many gid to gid map	*/
				/* pairs in this block.			*/
	gid_t pairs [8];	/* Gid to gid pairs......		*/
};

struct gidmap {
	long next;		/* Addr of next block in hash chain	*/
	unsigned n_pairs;	/* There are this many gid to gid map	*/
				/* pairs in this block.			*/
	gid_t pairs [GIDSPERBLK];
};				/* Gid to gid pairs......		*/

struct uidmap8 {		/* Only used for UIDSPERBLK computation	*/
	long next;		/* Addr of next block in hash chain	*/
	unsigned n_pairs;	/* There are this many uid to uid map	*/
				/* pairs in this block.			*/
	uid_t pairs [8];	/* Uid to uid pairs......		*/
};

struct uidmap {
	long next;		/* Addr of next block in hash chain	*/
	unsigned n_pairs;	/* There are this many uid to uid map	*/
				/* pairs in this block.			*/
	uid_t pairs [UIDSPERBLK];
};				/* Uid to uid pairs......		*/

struct hashblk {
	long chain [HASHENTSPERBLK];	/* Hash chain pointers		*/
};

struct nammap {
	mid_t	 mid;		/* Machine-id				*/
	unsigned short size;	/* Number of bytes contained in this	*/
				/* entry (even if NOT allocated!)	*/
	unsigned short namelen;	/* Length of machine-name.		*/
				/* Zero implies this entire entry is	*/
				/* not used at all.			*/
	char	 name [MAX_NAMELENGTH];
};				/* Name of machine.			*/

struct n_header {
	long nexthash;		/* Address of next block in hash chain	*/
	struct nammap map;	/* First mapping entry in the block	*/
};

struct rawblock {		/* Raw map file block properly aligned	*/
	union {
		ALIGNTYPE alignment;		/* Align the block	*/
		struct m_header mhdr;		/* Machine file hdr blk	*/
		struct uidmap umap;		/* User-id mapping blk	*/
		struct gidmap gmap;		/* Group-id mapping blk	*/
		struct hashblk hash;		/* Hash ptr block	*/
		struct n_header nhdr;		/* First nammap entry	*/
		char bytes [ATOMICBLKSIZ];	/* Raw block contents	*/
	} u;
};
