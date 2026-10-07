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
 
/*++ nmapcommon.c -
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nmapcommon.c,v $
 *
 * DESCRIPTION:
 *	Nmapcommon.c:  Common routines needed for the network mapping functions
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:22 $ $State: Exp $)
 * $Log: nmapcommon.c,v $
 * Revision 1.4  1994/11/19  02:27:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:24:15  mwan
 * 1.2 mods
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:53  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:59:30  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 * 
 *
 *
 *
 *	WARNINGS:
 *	  The #define symbol: MAP_DIRECTORY must be changed accordingly
 *	  if the location of the mapping database is to exist elsewhere
 *	  in the host file system.
 *
 *	  The #define type:  ALIGNTYPE  should be configured for the
 *	  appropriate supporting hardware.
 *
 *	  The function:  namechain() assumes that the hardware
 *	  representation of an integer is done using twos complement.
 *
 *	  The #define equations for HASHENTSPERBLK, GIDHDRSIZ, UIDHDRSIZ,
 *	  GIDSPERBLK, UIDSPERBLK MAY have to be changed if the compiler
 *	  implements a really strange padding algorithm for array
 *	  elements.
 *
 */


#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nmapcommon.c	1.2 (nmapcommon.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nmapcommon.h"

#ifdef OSF
#include	"core_dump.h"
#endif

int name_mapfile = -1;		/* File descriptor for machines		*/
				/* database: -1 if closed, > -1 if open.*/
int machine_file = -1;		/* File descriptor for machine file	*/
				/* -1 if closed, > -1 if open.		*/
int nfds = 0;			/* This variable determines the number	*/
				/* file descriptors that the network	*/
				/* mapping software will be allowed to	*/
				/* keep open between calls (0, 1, 2).	*/

int n_latom = ATOMICBLKSIZ;	/* for identifying versions */


/*
 *
 *
 *	void nmap_ctl (opcode, parameter)
 *
 *	Nmap module control function.  Currently defined functions
 *	include:
 *
 *	      Opcode:	      Parameter (int):
 *	      -------	      ----------------
 *
 *		NMAP_OPNOFD:	Set maximum number of file descriptors
 *				that the network mapping routines will
 *				be allowed to keep open between calls
 *				(0, 1, or 2).
 *
 *		NMAP_OPUIDC:	Bytes of UID cache <not-implemented>.
 *
 *		NMAP_OPGIDC:	Bytes of GID cache <not-implemented>.
 *
 *		NMAP_OPNAMC:	Bytes of name cache <not-implemented>.
 *
 */
void nmap_ctl (opcode, parameter)
int opcode;
int parameter;
{
	if (parameter < 0) parameter = 0;
	if (parameter > 2) parameter = 2;
	if (parameter == 0) {
		/*
		 *  We are on an austerity file descriptor budget.
		 */
		if (name_mapfile != -1) {
		    CLOSE (name_mapfile);
		    name_mapfile = -1;
		}
		if (machine_file != -1) {
		    CLOSE (machine_file);
		    machine_file = -1;
		}
	}
	else if (parameter == 1) {
		/*
		 *  Semi-austerity.
		 */
		if (name_mapfile != -1) {
			/*
			 *  Sigh....
			 */
			CLOSE (name_mapfile);
			name_mapfile = -1;
		}
	}
	nfds = parameter;	/* Remember our file descriptor quota limit */
}


/*
 *
 *
 *	int xidop (mid, parm, mode)
 *
 *	Mode determines the specific operation to be performed: [GET_OP,
 *	ADD_OP, DEL_OP].  Mode also determines the direction of the
 *	mapping: [BY_UID, BY_GID].
 *
 *	For mode as GET_OP, this function returns:
 *	   0: (NMAP_SUCCESS) If the mapping exists (in which case
 *	      parm->u.u.local_uid or parm->u.g.local_gid is returned as
 *	      the mapped local-uid or local-gid depending on the value
 *	      of: mode).
 *	   1: (NMAP_DEFMAP) No mapping existed for the specified mid/uid(gid)
 *	      pair, but the default uid(gid) mapping was enabled, and so
 *	      the default uid(gid) mapping for the specified machine has
 *	      been returned.
 *	  -1: (NMAP_EUNEXPECT) A fatal error occurred in the mapping software.
 *	  -2: (NMAP_ENOPRIV) Insufficient privilege to access database.
 *	  -3: (NMAP_ENOMAP) No mapping exists for the specified machine-id
 *	      and remote-uid or remote-gid pair.
 *	For mode as NEW_OP, this function returns:
 *	   0: (NMAP_SUCCESS) If the new mapping was created successfully
 *	      (parm->u.u.local_uid or parm->u.g.local_gid specifies the
 *	       value of the mapped result).
 *	  -1: (NMAP_EUNEXPECT) A fatal error occurred in the mapping software.
 *	  -2: (NMAP_ENOPRIV) Insufficient privilege to update database.
 *	  -3: (NMAP_ENOMAP) A mapping for the source uid/gid already
 *	      existed which is different from the specified new mapping
 *	      (the mapping remains unchanged).
 *	  -4: (NMAP_ENOMID) No such machine-id.
 *	For mode as DEL_OP, this function returns:
 *	   0: (NMAP_SUCCESS) If the mapping was deleted for the specified
 *	      uid/gid.
 *	  -1: (NMAP_EUNEXPECT) A fatal error occurred in the mapping software.
 *	  -2: (NMAP_ENOPRIV) Insufficient privilege to update database.
 *	  -3: (NMAP_ENOMAP) No mapping for the source uid/gid existed to
 *	      delete.
 */
int xidop (mid, parm, mode)
mid_t mid;
struct xidparm *parm;
int mode;
{

	struct rawblock block;
	struct rawblock newblock;
	struct rawblock topblock;
	register long address;
	register int i;			/* Index variable */
	register int n;			/* index variable */
	register long free;		/* Address of block containing */
					/* free space for new entries */
	register long curr;		/* Address of current block */
	int accessflags;		/* Uid/gid file access flags */
	register int status;		/* Status return from open_machine() */

	if (mode & GET_OP) accessflags = O_RDONLY;
	else accessflags = O_RDWR;
	if ((status = open_machine (mid, accessflags)) != NMAP_SUCCESS) {
		if (mode & ADD_OP) return (status);
					/* Return NMAP_EUNEXPECT */
					/*	  NMAP_ENOPRIV */
					/*	  NMAP_ENOMID */
		if (status == NMAP_ENOMID) return (NMAP_ENOMAP);
					/* No mapping exists */
		return (status);	/* Otherwise return: */
					/*	  NMAP_EUNEXPECT */
					/*	  NMAP_ENOPRIV */
	}
	if (mode & GET_OP) {
		/*
		 * We must read in the mhdr structure to see if mapping
		 * is disabled or not.
		 */
		if (LSEEK (machine_file, 0L, 0) == -1) {
			return (NMAP_EUNEXPECT);
		}
		if (READ (machine_file, topblock.u.bytes,
			  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
			return (NMAP_EUNEXPECT);
		}
		if ((topblock.u.mhdr.enatime == 0) || (topblock.u.mhdr.enatime > time(0L))) {
			return (NMAP_ENOMAP);
		}
	}
	if (mode & BY_UID) address = ATOMICBLKSIZ;	/* Block 1 of file */
	else address = ATOMICBLKSIZ * 2;		/* Block 2 of file */
	/*
	 *  Get address of first block in chain.
	 */
	free = -1;			/* No free blocks seen yet */
	do {
		if (LSEEK (machine_file, address, 0) == -1) {
			return (NMAP_EUNEXPECT);
		}
		if (READ (machine_file, block.u.bytes,
			  ATOMICBLKSIZ)!=ATOMICBLKSIZ) {
			return (NMAP_EUNEXPECT);
		}
		curr = address;		/* Remember current block */
		if (mode & BY_UID) {
		    n = block.u.umap.n_pairs;
		    if (free == -1 && n < UIDSPERBLK) free = address;
		    for (i=0; i < n; i++) {
			if (block.u.umap.pairs [i+i] == parm->u.u.remote_uid) {
			    if (mode & GET_OP) {
				parm->u.u.local_uid = block.u.umap.pairs[i+i+1];
				return (NMAP_SUCCESS);	/* Mapping successful */
			    }
			    if (mode & DEL_OP) {
				if (--block.u.umap.n_pairs) {
				    /*
				     *  Fill hole created by the removal of
				     *  the uid mapping.
				     */
				    block.u.umap.pairs [i+i]
					= block.u.umap.pairs [n+n-2];
				    block.u.umap.pairs [i+i+1]
					= block.u.umap.pairs [n+n-1];
				}
				if (LSEEK (machine_file, address, 0) == -1) {
					return (NMAP_EUNEXPECT);
				}
				if (WRITE (machine_file, block.u.bytes,
					   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
				    return (NMAP_EUNEXPECT);
				}
				return (NMAP_SUCCESS); /* Deletion successful */
			    }
			    /*  Mode must be ADD_OP */
			    if (parm->u.u.local_uid==block.u.umap.pairs[i+i+1]){
				return (NMAP_SUCCESS);
						/* Same mapping already there */
			    }
			    return (NMAP_ECONFLICT); /*Conflicting map exists*/
			}
		    }
		    /*
		     *  The mapping was not found in this block.
		     *  Examine the next mapping block in the chain.
		     */
		    address = block.u.umap.next;
		}
		else {	/* Mapping is by GID */
		    n = block.u.gmap.n_pairs;
		    if (free == -1 && n < GIDSPERBLK) free = address;
		    for (i=0; i < n; i++) {
			if (block.u.gmap.pairs [i+i] == parm->u.g.remote_gid) {
			    if (mode & GET_OP) {
				parm->u.g.local_gid = block.u.gmap.pairs[i+i+1];
				return (NMAP_SUCCESS);	/* Success! */
			    }
			    if (mode & DEL_OP) {
				if (--block.u.gmap.n_pairs) {
				    /*
				     *  Fill hole created by the removal of
				     *  the gid mapping.
				     */
				    block.u.gmap.pairs [i+i]
					= block.u.gmap.pairs [n+n-2];
				    block.u.gmap.pairs [i+i+1]
					= block.u.gmap.pairs [n+n-1];
				}
				if (LSEEK (machine_file, address, 0) == -1) {
					return (NMAP_EUNEXPECT);
				}
				if (WRITE (machine_file, block.u.bytes,
					   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
				    return (NMAP_EUNEXPECT);
				}
				return (NMAP_SUCCESS); /* Deletion successful */
			    }
			    /*  Mode must be ADD_OP */
			    if (parm->u.g.local_gid==block.u.gmap.pairs[i+i+1]){
				return (NMAP_SUCCESS);
						/* Same mapping already there */
			    }
			    return (NMAP_ECONFLICT);
					/*Conflicting mapping exists*/
			}
		    }
		    /*
		     *  The mapping was not found in this block.
		     *  Examine the next mapping block in the chain.
		     */
		    address = block.u.gmap.next;
		}
	} while (address != 0L);
	if (mode & ADD_OP) {
		/*
		 *  No mapping was found for the specified uid/gid.
		 *  Enter the new mapping into the database for the
		 *  specified machine.
		 */
		if (free == -1) {
			/*
			 *  No free space existed in the chain.
			 */
			for (i=0; i < ATOMICBLKSIZ; i++) {
				newblock.u.bytes [i] = '\0';
			}
			if (mode & BY_UID) {
				newblock.u.umap.next= 0;	/* NIL ptr */
				newblock.u.umap.n_pairs = 1;
				newblock.u.umap.pairs[0] = parm->u.u.remote_uid;
				newblock.u.umap.pairs[1] = parm->u.u.local_uid;
			}
			else {	/* Mapping is by GID */
				newblock.u.gmap.next= 0;	/* NIL ptr */
				newblock.u.gmap.n_pairs = 1;
				newblock.u.gmap.pairs[0] = parm->u.g.remote_gid;
				newblock.u.gmap.pairs[1] = parm->u.g.local_gid;
			}
			/*
			 *  Seek to end of file to add block.
			 */
			if ((free = LSEEK (machine_file, 0L, 2)) == -1) {
				return (NMAP_EUNEXPECT);
			}
			/*
			 *  Write-out new file block.
			 */
			if (WRITE (machine_file, newblock.u.bytes,
				   ATOMICBLKSIZ)!=ATOMICBLKSIZ) {
				return (NMAP_EUNEXPECT);
			}
			/*
			 *  Add new block onto the end of the
			 *  chain.  The last block in the chain
			 *  is in block.
			 */
			if (LSEEK (machine_file, curr, 0) == -1) {
				return (NMAP_EUNEXPECT);
			}
			if (mode & BY_UID) block.u.umap.next = free;
			else block.u.gmap.next = free;
			if (WRITE (machine_file, block.u.bytes,
				   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
				return (NMAP_EUNEXPECT);
			}
		}
		else {
			/*
			 *  Free space did exist in the chain.
			 */
			if (curr != free) {
				/*
				 *  We need to go get the block with the
				 *  free space in it.
				 */
				if (LSEEK (machine_file, free, 0) == -1) {
					return (NMAP_EUNEXPECT);
				}
				if (READ (machine_file, block.u.bytes,
					  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
					return (NMAP_EUNEXPECT);
				}
			}
			/*
			 *  Now seek to target file location.
			 */
			if (LSEEK (machine_file, free, 0) == -1) {
				return (NMAP_EUNEXPECT);
			}
			if (mode & BY_UID) {
				n = block.u.umap.n_pairs++;
				block.u.umap.pairs[n+n] = parm->u.u.remote_uid;
				block.u.umap.pairs[n+n+1] = parm->u.u.local_uid;
			}
			else {	/* Mapping is by GID */
				n = block.u.gmap.n_pairs++;
				block.u.gmap.pairs[n+n] = parm->u.g.remote_gid;
				block.u.gmap.pairs[n+n+1] = parm->u.g.local_gid;
			}
			if (WRITE (machine_file, block.u.bytes,
				   ATOMICBLKSIZ)!=ATOMICBLKSIZ) {
				return (NMAP_EUNEXPECT);
			}
		}
		return (NMAP_SUCCESS);	/* The new mapping was added ok. */
	}
	if (mode & GET_OP) {
		/*
		 *  No mapping was found for the mid/uid(gid) pair.  See
		 *  if the default uid(gid) mapping is enabled.
		 */
		if (mode & BY_UID) {
			if (topblock.u.mhdr.defuid_enabled) {
				parm->u.u.local_uid = topblock.u.mhdr.defuid;
				return (NMAP_DEFMAP);
			}
		}
		else {
			if (topblock.u.mhdr.defgid_enabled) {
				parm->u.g.local_gid = topblock.u.mhdr.defgid;
				return (NMAP_DEFMAP);
			}
		}
	}
	/*
	 *  Else, this request was a GET_OP (and the default mapping was
 	 *  disabled) or a DEL_OP and no mapping could be found.
	 */
	return (NMAP_ENOMAP);		/* No mapping */
}


/*
 *
 *
 *	int namop (name, mid, mode)
 *
 *	Do one of the operations of: GET_MID, ADD_NAM, or DEL_NAM.
 *
 *	For mode as GET_MID, this function returns.
 *	   0: (NMAP_SUCCESS) If the mapping exists (in which case mid is
 *	      returned as the mapped mid for the specified name).
 *	  -1: (NMAP_EUNEXPECT) A fatal error occurred in the mapping software.
 *	  -2: (NMAP_ENOPRIV) Insufficient privilege to access database.
 *	  -4: (NMAP_ENOMAP) No mapping exists for the specified name.
 *	For mode as ADD_NAM, this function returns:
 *	   0: (NMAP_SUCCESS) If the new mapping was created successfully.
 *	  -1: (NMAP_EUNEXPECT) A fatal error occurred in the mapping software.
 *	  -2: (NMAP_ENOPRIV) Insufficient privilege to update the mapping
 *	      database.
 *	  -3: (NMAP_ECONFLICT) A mapping already existed for the source
 *	      target which defined a mapping result target different from
 *	      the new target.
 *	  -4: (NMAP_ENOMID) No such machine-id.  An attempt was made
 *	      to create a name to non-existent machine-id (mid) mapping.
 *	  -6: (NMAP_EBADNAME) Null name or name too long for mapping was
 *	      specified.
 *	For mode as DEL_NAM, this function returns:
 *	   0: (NMAP_SUCCESS) If the mapping was deleted successfully.
 *	  -1: (NMAP_EUNEXPECT) A fatal error occurred in the mapping software.
 *	  -2: (NMAP_ENOPRIV) Insufficient privilege to update the mapping
 *	      database.
 *	  -4: (NMAP_ENOMAP) No mapping existed to delete.
 */
int namop (name, mid, mode)
char *name;
mid_t *mid;
int mode;
{

	char path [MAX_PATHLENGTH+1];
	struct rawblock block;
	struct rawblock newblock;
	register long address;
	unsigned prevoffset;
	unsigned offset;
	int hashval;
	register struct nammap *mmap;
	register int i;			/* Index var and temporary */
	register long free;		/* Address of block containing */
					/* free space for new mapping */
	unsigned freeoffset;		/* Offset of adequate space in */
					/* block */
	register long curr;		/* Address of current block */
	int need;			/* Number of bytes needed */
	int size;			/* Size of entry */
	int lenofname;			/* Length of name if MID_REQ */
	int accessflags;		/* Mid mapping file access flags */
	register int status;		/* Status return from open_name_map() */

	if (mode == GET_MID) accessflags = O_RDONLY;
	else accessflags = O_RDWR;
	if ((status = open_name_map (accessflags)) != NMAP_SUCCESS) {
		return (status);	/* Return NMAP_EUNEXPECT */
					/*	  NMAP_ENOPRIV */
	}
	lenofname = strlen (name);
	if (lenofname == 0 ||
	    lenofname > MAX_NAMELENGTH) {
		if (mode == ADD_NAM) return (NMAP_EBADNAME);	/* Bad name */
		return (NMAP_ENOMAP);
	}
	if (mode == ADD_NAM) {
		/*
		 *  Before establishing a name to machine-id mapping,
		 *  check to see that the destination machine-id
		 *  exists!
		 */
		machine_name (*mid, path);	/* Get uid/gid mapnam */
		if (access (path, 0) == -1) {
			/*
			 *  Bad problems or the target machine-id
			 *  does not exist.
			 */
			if (errno == ENOENT) {
				return (NMAP_ENOMID); /* No such machine */
			}
			return (NMAP_EUNEXPECT);
		}
		/*
		 *  We are adding a new mapping to the database.  Determine
		 *  the size of the new entry in bytes as a multiple of
		 *  sizeof (ALIGNTYPE).
		 */
		need = sizeof (struct nammap) - MAX_NAMELENGTH + lenofname;
		i = need % sizeof (ALIGNTYPE);
		if (i) need += sizeof (ALIGNTYPE) - i;
	}
	hashval = namechain (name);
	address = (hashval / HASHENTSPERBLK) * ATOMICBLKSIZ;
	if (LSEEK (name_mapfile, address, 0) == -1) return (NMAP_EUNEXPECT);
	if (READ (name_mapfile, block.u.bytes, ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		return (NMAP_EUNEXPECT);
	}
	/*
	 * Get address of first block in the hash chain.
	 */
	address = block.u.hash.chain [hashval % HASHENTSPERBLK];
	free = -1;		/* No sufficient free space seen yet */
	curr = -1;		/* No current block */
	while (address != 0L) {
		if (LSEEK (name_mapfile, address, 0) == -1) {
			return (NMAP_EUNEXPECT);
		}
		if (READ (name_mapfile, block.u.bytes,
			  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
			return (NMAP_EUNEXPECT);
		}
		curr = address;	/* Remember current block */
		/*
		 *  Skip over nextptr for hash chain link and examine
		 *  this block.
		 */
		offset = ((char *)(&block.u.nhdr.map)) - block.u.bytes;
		prevoffset = offset;
		do {
			mmap = (struct nammap *) (block.u.bytes + offset);
			if (mmap->namelen == lenofname &&
			    strncmp (mmap->name, name, lenofname) == 0) {
				/*
				 *  The mapping exists.
				 */
				if (mode == GET_MID) {	/* Mapping operation */
					*mid = mmap->mid;
					return (NMAP_SUCCESS);	/* Success */
				}
				if (mode == DEL_NAM) {	/* Delete operation */
					mmap->namelen = 0;	/* Free entry */
					if (prevoffset != offset) {
						/*
						 *  The entry to be freed is
						 *  not the first entry in the
						 *  block.
						 */
						size=mmap->size;/* Entry size */
						mmap=(struct nammap *)
						     (block.u.bytes+prevoffset);
						mmap->size+=size;/* Merge with*/
								/* prev block */
					}
					if (LSEEK(name_mapfile,address,0)==-1){
						return (NMAP_EUNEXPECT);
					}
					if (WRITE (name_mapfile, block.u.bytes,
						   ATOMICBLKSIZ)!=ATOMICBLKSIZ){
						return (NMAP_EUNEXPECT);
					}
					return (NMAP_SUCCESS);	/* Success */
				}
				/*
				 *  Mode must be ADD_NAM.
				 */
				if (*mid == mmap->mid) {
					return (NMAP_SUCCESS);
						/* Same mapping */
						/* already there */
				}
				return (NMAP_ECONFLICT);	/* Conflict */
			}
			/*
			 *  If the operation to be performed is an ADD_NAM
			 *  and we have not yet found free space for the
			 *  mapping, then check to see if available
			 *  free space exists.
			 */
			if (mode == ADD_NAM && free == -1) {
				if (mmap->namelen == 0) size = mmap->size;
				else {
					size = sizeof (struct nammap)
					     - MAX_NAMELENGTH + mmap->namelen;
					/*
					 *  Size has number of bytes used in
					 *  the entry.
					 */
					i = size % sizeof (ALIGNTYPE);
					if (i) size += sizeof (ALIGNTYPE) - i;
					/*
					 *  At this exact moment, size equals
					 *  the size of the allocated portion
					 *  of the entry rounded up to:
					 *  sizeof (ALIGNTYPE) bytes.
					 */
					size = mmap->size - size;
					/* Size could be < 0 now.... */
				}
				/*
				 *  Size new equals the number
				 *  of free bytes in the entry that can be
				 *  allocated.  Note that size could be
				 *  negative at this point if the physical
				 *  block size (ATOMICBLKSIZ) et. al. conspire
				 *  to cause the size of an entry to NOT be
				 *  a multiple of sizeof (ALIGNTYPE) bytes.
				 */
				if (size >= need) {
					/*
					 *  This entry satisfies the need
					 *  for the new mapping.
					 */
					free = address;
					freeoffset = offset;
				}
			}
			/*
			 *  Prepare to access the next entry.
			 */
			prevoffset = offset;
			offset += mmap->size;
		} while (offset < ATOMICBLKSIZ);
		/*
		 *  The name-to-mid mapping was not found in this
		 *  block.  Try the next block.
		 */
		address = block.u.nhdr.nexthash;	/* Next block address */
	}
	if (mode == ADD_NAM) {
		/*
		 *  No mapping was found.  Enter the new mapping into
		 *  the database for the specified name-to-mid relation.
		 */
		if (free == -1) {
			/*
			 *  No free space existed in the hash chain.
			 */
			for (i=0; i < ATOMICBLKSIZ; i++) {
				newblock.u.bytes [i] = '\0';
			}
			newblock.u.nhdr.nexthash = 0;	/* NIL next ptr */
			offset = ((char *)(&newblock.u.nhdr.map))
			       - newblock.u.bytes;
			mmap = (struct nammap *) (newblock.u.bytes + offset);
			mmap->size = ATOMICBLKSIZ-offset;
			mmap->mid = *mid;
			mmap->namelen = lenofname;
			for (i=0; i < lenofname; i++) {	/* Save name */
				mmap->name [i] = name [i];
			}		/* Do NOT null terminate the name */
			/*
			 *  Seek to end of file to add new block.
			 */
			if ((free = LSEEK (name_mapfile, 0L, 2)) == -1) {
				return (NMAP_EUNEXPECT);
			}
			/*
			 *  Write out the new file block.
			 */
			if (WRITE (name_mapfile, newblock.u.bytes,
				   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
				return (NMAP_EUNEXPECT);
			}
			/*
			 *  Add new block into the hash chain.
			 */
			if (curr == -1) {
				/*
				 *  Create the hash chain.
				 *  The hash header block is in block.
				 */
				if (LSEEK (name_mapfile,
					   (long) ((hashval / HASHENTSPERBLK)
						    * ATOMICBLKSIZ), 0) == -1) {
					return (NMAP_EUNEXPECT);
				}
				block.u.hash.chain [hashval % HASHENTSPERBLK]
					= free;
				if (WRITE (name_mapfile, block.u.bytes,
					   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
					return (NMAP_EUNEXPECT);
				}
			}
			else {
				/*
				 *  Add new block onto the end of the hash
				 *  chain.  The last block in the hash
				 *  chain is in block.
				 */
				if (LSEEK (name_mapfile, curr, 0) == -1) {
					return (NMAP_EUNEXPECT);
				}
				block.u.nhdr.nexthash = free;
				if (WRITE (name_mapfile, block.u.bytes,
					   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
					return (NMAP_EUNEXPECT);
				}
			}
		}
		else {
			/*
			 *  Free space did exist in the hash chain.
			 */
			if (curr != free) {
				/*
				 *  We need to go get the block with the
				 *  free space in it.
				 */
				if (LSEEK (name_mapfile, free, 0) == -1) {
					return (NMAP_EUNEXPECT);
				}
				if (READ (name_mapfile, block.u.bytes,
					  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
					return (NMAP_EUNEXPECT);
				}
			}
			/*
			 *  Now seek to target file location.
			 */
			if (LSEEK (name_mapfile, free, 0) == -1) {
				return (NMAP_EUNEXPECT);
			}
			mmap = (struct nammap *) (block.u.bytes + freeoffset);
			if (mmap->namelen != 0) {
				/*
				 *  The entry is allocated but has sufficient
				 *  free space.
				 */
				size = sizeof (struct nammap) - MAX_NAMELENGTH
				     + mmap->namelen;
				i = size % sizeof (ALIGNTYPE);
				if (i) size += sizeof (ALIGNTYPE) - i;
				need = mmap->size - size;
					/* Need=amount of remaining free space*/
				mmap->size = size;
					/* Adjust size of entry to minimum */
				freeoffset += size;
					/* Freeoffset points to adequate */
					/* free space to store new mapping */
				mmap = (struct nammap *)
				       (block.u.bytes + freeoffset);
				mmap->size = need;	/* Set entry size */
			}
			mmap->mid = *mid;	/* Record machine-id */
			mmap->namelen = lenofname;
			for (i=0; i < lenofname; i++) {
				mmap->name [i] = name [i];
			}
			/*
			 *  Now, write-out updated block.
			 */
			if (WRITE (name_mapfile, block.u.bytes,
				   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
				return (NMAP_EUNEXPECT);
			}
		}
		return (NMAP_SUCCESS);
				/* The new mapping was successfully added */
	}
	/*
	 *  Else this request was a GET_MID or DEL_NAM and no mapping could
	 *  be found.
	 */
	else return (NMAP_ENOMAP);	/* No mapping */
}


/*
 *
 *
 *	int namechain (name)
 *
 *	This function returns the integer hash name chain in which the
 *	specified machine-name would appear, if a mapping existed in
 *	the mapping database for the specified name.
 */
int namechain (name)
char *name;
{
	/*register int hashval;*/
	register unsigned int hashval;
	register unsigned int maxuint;

	hashval = 0;
	maxuint = ~0;
	while (*name) {
		/*hashval *= 36;*/
		hashval += *name++;
	}
	/*
	 * Zero left-most bit; return remainder modulo HASHNAMENTS.
	 * The assignment into maxuint gets around a compiler bug:
	 * VAX System V "(unsigned) ~0 >> 1" sign-extends as if
	 * "(unsigned) ~0" were negative.
	 */
	return ((hashval & (maxuint >> 1)) % HASHNAMENTS);
}


/*
 *
 *
 *	int open_name_map (access)
 *
 *	This function does NOT guarantee that the file pointer references
 *	the first byte of the file.  It is up to the caller to ensure that
 *	the proper lseek() calls are made as necessary.
 *
 *	Returns [NMAP_SUCCESS, NMAP_EUNEXPECT, NMAP_ENOPRIV, NMAP_ECONFLICT].
 */
int open_name_map (access)
int access;
{
	static char *map_dir = MAP_DIRECTORY;
	static char *machines_file = MACHINES;
	static int curr_access = 0;
	char path [MAX_PATHLENGTH+1];
	char *copyfrom;
	char *copyto;

	copyfrom = map_dir;
	copyto = path;
	while (*copyfrom) *copyto++ = *copyfrom++;
	*copyto++ = '/';
	copyfrom = machines_file;
	while (*copyfrom) *copyto++ = *copyfrom++;
	*copyto = '\0';
	if (name_mapfile != -1 && curr_access != access &&
	    curr_access != O_RDWR) {
		/*
		 *  We need a different file access mode that can only be
		 *  gotten by reopening the file (sigh).
		 */
		CLOSE (name_mapfile);
		name_mapfile = -1;
	}
	if (name_mapfile == -1) {
		/*
		 *  We've got to open the map file.
		 */
		curr_access = access & (O_RDONLY | O_WRONLY | O_RDWR);
		if ((name_mapfile = openf (path, access)) == -1) {
			/*
			 *  Something went wrong trying to open the file.
			 *  Check for a lack of process file descriptors
			 *  and see if we can use the descriptor for the
			 *  uigidfile.
			 */
			if (errno == EMFILE && machine_file != -1) {
				/*
				 *  We must close the uid/gid
				 *  map file (sigh...)
				 */
				CLOSE (machine_file);
				machine_file = -1;
				if ((name_mapfile = openf (path, access)) > -1){
					return (NMAP_SUCCESS);
				}
			}
			if (errno==EACCES || errno==EROFS) return(NMAP_ENOPRIV);
			if (errno==EEXIST) return (NMAP_ECONFLICT);
			return (NMAP_EUNEXPECT);
		}
	}
	return (NMAP_SUCCESS);
}


/*
 *
 *
 *	open_machine (mid, access)
 *
 *	This function does NOT guarantee that the file pointer references
 *	the first byte of the file.  It is up to the caller to ensure that
 *	the proper lseek() calls are made as necessary.
 *
 *	Returns [NMAP_SUCCESS, NMAP_EUNEXPECT, NMAP_ENOPRIV, NMAP_ENOMID,
 *		 NMAP_ECONFLICT].
 */
int open_machine (mid, access)
mid_t mid;
int access;
{
	static int curr_access;	/* File access mode.			*/
	static mid_t curr_mid;	/* If machine_file is open, then	*/
				/* curr_mid defines the mid of the	*/
				/* machine for which the machine_file	*/
				/* is open.				*/

	char path [MAX_PATHLENGTH+1];

	if (machine_file != -1 && curr_access != access &&
	    curr_access != O_RDWR) {
		/*
		 *  We need a different file access mode that can only be
		 *  gotten by reopening the file (sigh), or we set O_CREAT
		 *  in the access mask.
		 */
		CLOSE (machine_file);
		machine_file = -1;
	}
	if (machine_file == -1 || curr_mid != mid) {
		/*
		 *  We must open another uid/gid mapping file for mid.
		 */
		curr_access = access & (O_RDONLY | O_WRONLY | O_RDWR);
		curr_mid = mid;		/* Remember mid */
		if (machine_file != -1) CLOSE (machine_file);
		machine_name (mid, path);		/* Get pathname */
		/*
		 *  Path now has the complete pathname of the uid/gid
		 *  mapping file for the specified mid.
		 */
		if ((machine_file = openf (path, access)) == -1) {
			/*
			 *  Something went wrong trying to open the file.
			 *  Check for a lack of process file descriptors
			 *  and see if we can use the descriptor for the
			 *  uigidfile.
			 */
			if (errno == EMFILE && name_mapfile > -1) {
				/*
				 *  We must close the mid
				 *  map file (sigh...)
				 */
				CLOSE (name_mapfile);
				name_mapfile = -1;
				if ((machine_file=openf (path, access)) >-1) {
					return (NMAP_SUCCESS);
				}
			}
			if (errno==EACCES || errno==EROFS) {
				return (NMAP_ENOPRIV);
			}
			if (errno==ENOENT) return (NMAP_ENOMID);
			if (errno==EEXIST) return (NMAP_ECONFLICT);
			return (NMAP_EUNEXPECT);	/* Problems... */
		}
	}
	return (NMAP_SUCCESS);
}


/*
 *
 *
 *	machine_name (mid, path)
 *
 *	Format the null terminated machine name of the uid/gid mapping
 *	file for the specified machine-id (mid) in the character string
 *	array of: path.
 *
 *	WARNING * * * WARNING * * * WARNING
 *
 *		This procedure will not work if mid is < 0.
 */
machine_name (mid, path)
mid_t mid;
char *path;
{
	static char *map_dir = MAP_DIRECTORY;

	register char *copyto;
	register char *copyfrom;
	register int i;

	copyfrom = map_dir;
	copyto = path;
	while (*copyfrom) *copyto++ = *copyfrom++;
	*copyto++ = '/';
	*copyto++ = 'm';
	for (i=1; i <= 13; i++) {
		*copyto++ = '0';
	}
	*copyto-- = '\0';		/* Tack on terminating null */
					/* and go backwards */
	while (mid != 0) {
		*copyto-- = mid % 10 + '0';/* I  hope mids never exceed */
                mid /= 10;              /* 13 decimal digits in size. */
	}
}


/*
 *
 *
 *	openf (pathname, access)
 *
 *	This dumb little procedure tacks on the file create mode of
 *	0644 if O_CREAT is set in the access mode.
 */
static int openf (pathname, access)
char *pathname;
int access;
{
	if (access & O_CREAT) {
		return (OPEN (pathname, access, 0644));
	}
	else return (OPEN (pathname, access));
}
