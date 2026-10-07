$ 
$ $Copyright
$ Copyright 1993, 1994, 1995  Intel Corporation
$ INTEL CONFIDENTIAL
$ The technical data and computer software contained herein are subject
$ to the copyright notices; trademarks; and use and disclosure
$ restrictions identified in the file located in /etc/copyright on
$ this system.
$ Copyright$
$ 
 
$
$ @OSF_COPYRIGHT@
$
$
$ HISTORY
$ $Log: ufs_fsck.msg,v $
$ Revision 1.3  1994/11/19  03:18:22  mtm
$ Copyright additions/changes
$
$ Revision 1.2  1994/03/11  22:01:26  brad
$ Merged revision 1.1.14.1 from the R1.2 branch.
$
$ Revision 1.1.14.1  1994/03/11  21:52:53  brad
$ Fixed printing of warning messages for reserved disk blocks that
$ need to be cleared: 1) now only one message displayed per indirect
$ block rather than one message per data block (which can result in
$ many many messages), and 2) messages displayed under BLK_RESERVE_SUPPORT
$ ifdef's now properly ask the user for guidance if -y not specified.
$
$  Reviewer: Dave Minturn
$  Risk: Low
$  Benefit or PTS #: 8445
$  Testing: Developer testing, crashed system and ran fsck, etc.
$  Module(s): inode.c pass1.c ufs_fsck.msg
$
$ Revision 1.1  1992/10/12  22:09:23  shala
$ New version to support maj, min and node numbers.
$
$ Revision 1.3.2.10  1992/03/22  20:29:33  kittlitz
$ 	bug 5799. ismounted must diagnose failure to open block device
$ 	[1992/03/22  20:24:38  kittlitz]
$
$ Revision 1.3.2.9  1992/03/10  14:04:26  hosking
$ 	bug 4373: do MAC checks before reconnecting to lost+found
$ 	[1992/03/10  14:01:06  hosking]
$ 
$ Revision 1.3.2.8  1992/03/03  19:06:29  garyf
$ 	add INVALIDB
$ 	[1992/03/03  18:49:26  garyf]
$ 
$ Revision 1.3.2.7  1992/02/28  17:16:39  garyf
$ 	remove positionals (not needed for english)
$ 	add FASTLINK
$ 	move INVALID to end of messages (for
$ 	compatibility)
$ 	[1992/02/28  17:16:00  garyf]
$ 
$ Revision 1.3.2.6  1992/02/11  23:09:28  garyf
$ 	add INVALID
$ 	[1992/02/11  23:03:49  garyf]
$ 
$ Revision 1.3.2.5  1992/01/30  15:56:12  garyf
$ 	add ISMOUNTED,SKIPFS
$ 	[1992/01/30  15:54:52  garyf]
$ 
$ Revision 1.3.2.4  1991/12/30  23:43:37  garyf
$ 	update block check messages
$ 	[1991/12/30  23:42:30  garyf]
$ 
$ Revision 1.3.2.3  1991/12/12  15:49:19  garyf
$ 	add FS_MTD
$ 	[1991/12/12  15:48:42  garyf]
$ 
$ Revision 1.3.2.2  1991/12/12  15:30:56  garyf
$ 	add BADSPCL
$ 	[1991/12/12  15:30:12  garyf]
$ 
$ Revision 1.3  1991/06/20  12:33:51  devrcs
$ 	Source merge for osc1.1b8
$ 	[91/06/11  14:30:21  devrcs]
$ 
$ 	move preen.c messages to separate set
$ 	[91/06/07  08:46:22  garyf]
$ 
$ 	undo bsubmit damage
$ 	[91/06/03  15:33:13  garyf]
$ 
$ 	created for NLS support
$ 	[91/05/13  11:50:33  garyf]
$ 
$ $EndLog$

$quote "
$set MS_UFS_FSCK

$ dir.c
COUNT	" COUNT %d SHOULD BE %d"
LCOUNT	"LINK COUNT %s"
LINKCOUNT	"LINK COUNT INCREASING"
ADJUSTED	" (ADJUSTED)\n"
ADJUST		"ADJUST"
RECONNECTED	" (RECONNECTED)\n"
RECONNECT	"RECONNECT"
CREATED	" (CREATED)\n"
PARENTI	"PARENT WAS I=%lu\n"
EXPANDED	" (EXPANDED)\n"
EXPAND	"EXPAND"
NAME	"NAME=%s\n"
FILE	"FILE"
DIR	"DIR"
MLDC	"SORRY. CANNOT CREATE lost+found DIRECTORY IN A MULTILEVEL DIRECTORY"
LFOUND	"SORRY. CANNOT CREATE lost+found DIRECTORY"
LFOUND1	"lost+found IS NOT A DIRECTORY"
LFOUND2	"SORRY. CANNOT CREATE lost+found DIRECTORY\n\n"
LFOUND3	"SORRY. NO lost+found DIRECTORY\n\n"
LFOUND4	"SORRY. NO SPACE IN lost+found DIRECTORY"
LFOUND5	"NO lost+found DIRECTORY"
UNREF	"UNREF"
UNREF1	"UNREF %s "
CONNECTED	"DIR I=%lu CONNECTED. "
NOSPACE	"NO SPACE LEFT IN %s"
CREATE	"CREATE"
REALLOCATE	"REALLOCATE"
WRONGT	"wrong type to dirscan %d\n"
DIRBAD	"DIRECTORY CORRUPTED"
WILDLAF	"SORRY.  lost+found HAS A WILDCARD SENSITIVITY\nLABEL"
DOMLAF	"SORRY.  INODE DOES NOT DOMINATE lost+found DIRECTORY\nINODE"

$ inode.c
PARTI		"PARTIALLY TRUNCATED INODE I=%lu"
BLKCG		"blk %ld < cgdmin %ld;"
BLKCG1		" blk + cnt %ld > cgsbase %ld\n"
BLKCG2		"blk %ld >= cgdmin %ld;"
BLKCG3		" blk + cnt %ld > sblock.fs_fpg %ld\n"
CLEARED		" (CLEARED)\n"
IEQ		" I=%lu "
OWNER		" OWNER="
MODE1		"MODE=%o\n"
SIZE		"SIZE=%lu "
MTIME		"MTIME=%12.12s %4.4s "
BLKERR		"%ld %s I=%lu"
BADI		"bad inode number %d to ginode\n"
BADI1		"bad inode number %d to nextinode\n"
ALLOC		"Cannot allocate space for inode buffer\n"
DIRLIST		"cannot increase directory list"
FIND		"cannot find inode %d\n"
BADSTATE	"BAD STATE %d TO BLKERR"
CLEAR		"CLEAR"
RESBLOCKS	"BLOCKS (starting at %ld) RESERVED BEYOND EOF I=%lu"
RESBLOCKS2	"BLOCKS (starting at %ld) IN I=%lu ARE RESERVED"
INRESBLOCKS	"WARNING: RESERVED INDIRECT BLOCKS I=%lu (level=%d)"
FREEING		" (FREEING)\n"

$ main.c
SYSADMIN	"%s: need sysadmin authorization\n"
PRIV	"%s: insufficient privileges\n"
ALTSUP	"Alternate super block location: %d\n"
LFOUND7	"** lost+found creation mode %o\n"
LMOUNT	"** Last Mounted on %s\n"
ROOT	"** Root file system\n"
PHASE1	"** Phase 1 - Check Blocks and Sizes\n"
PHASE1B	"** Phase 1b - Rescan For More DUPS\n"
PHASE2	"** Phase 2 - Check Pathnames\n"
PHASE3	"** Phase 3 - Check Connectivity\n"
PHASE4	"** Phase 4 - Check Reference Counts\n"
PHASE5	"** Phase 5 - Check Cyl groups\n"
FRAGS	"(%ld frags, %ld blocks, %.1f%% fragmentation)\n"
MISSING	"%ld files missing\n"
MISSING2	"%ld blocks missing\n"
DUPBLOCK	"The following duplicate blocks remain:"
ZEROLINK	"The following zero link count inodes remain:"
MODIFIED	"\n***** FILE SYSTEM WAS MODIFIED *****\n"
REBOOT	"\n***** REBOOT UNIX *****\n"
HOTROOT	"\n**** COULD NOT UPDATE HOT ROOT ****\n"
REBOOT1	"\n**** REBOOT IMMEDIATELY ****\n"
NOCHECK	"CAN'T CHECK FILE SYSTEM."
INTERR	"internal error: bad return from setup\n"
INTERR1	"INTERNAL ERROR: dups with -p"
FLAG	"-%c flag requires a %s\n"
NUMBER	"number"
MODE	"mode"
BADMODE	"bad mode to -m: %o\n"
OPTION	"%c option?\n"
STARTING	"starting\n"
FILES	"%ld files, %ld used, %ld free "
FS_MTD	"cannot fsck a mounted filesystem %s.\n"
ISMOUNTED "filesystem %s is mounted as %s\n"
ISMOUNTED_ERROR "Cannot determine if %s is mounted"
SKIPFS "skipping mounted filesystem (%s)\n"
INVALIDB "Invalid -b (alternate super block) of %d.\n"

$ pass1.c
BADSIZE		"bad size %lu:"
BADSIZE2	"bad size %lu ndb %d:"
BADDIRECT	"bad direct addr: %ld, in: %d\n"
BADINDDIR	"bad indirect addr: %ld, in: %d\n"
CORRECTED	" (CORRECTED)\n"
CORRECT		"CORRECT"
PARTIALA	"PARTIALLY ALLOCATED INODE I=%lu"
LINKCTT		"LINK COUNT TABLE OVERFLOW"
CONTINUE	"CONTINUE"
UNKNOWN1	"UNKNOWN FILE TYPE I=%lu"
DUPTAB		"DUP TABLE OVERFLOW."
HOLD		"HOLD BAD BLOCK"
BADBLOCK	"INCORRECT BLOCK COUNT I=%lu (%ld should be %ld)"
BAD		"BAD"
EXCESS		"EXCESSIVE BAD BLKS I=%lu"
SKIPPING	" (SKIPPING)\n"
DUP		"DUP"
BADSPCL		"INCORRECT BLOCKS IN SPECIAL FILE INODE I=%lu"
RESBLOCK	"DIRECT BLOCK (%ld) RESERVED BEYOND EOF I=%lu"R
RESBLOCK2	"DIRECT BLOCK (%ld) IN I=%lu IS RESERVED"
INRESBLOCK	"INDIRECT BLOCK (%ld) RESERVED BEYOND EOF I=%lu"
RESNOTREG	"WARNING: RESERVED BLOCK NOT FROM FILE (mode=0x%x)\n"
SIZETOOBIG	"WARNING: FILE SIZE TOO LARGE I=%lu (size=%lu)"
FREED		" (FREED)\n"
FREE		"FREE"

$ pass2.c
IGNORED	" (IGNORED)\n"
REMOVE	"REMOVE"
ROOTI	"ROOT INODE UNALLOCATED"
ALLOCATE	"ALLOCATE"
NOALLOC	"CANNOT ALLOCATE ROOT INODE\n"
DUPS	"DUPS/BAD IN ROOT INODE"
ROOTI1	"ROOT INODE NOT DIRECTORY"
FIX0	"FIX"
BADSTATE1	"BAD STATE %d FOR ROOT INODE"
DIRSHORT	"DIRECTORY TOO SHORT"
DIRLEN	"DIRECTORY %s: LENGTH %d NOT MULTIPLE OF %d"
MISSING4	"MISSING '..'"
BADINUM	"BAD INODE NUMBER FOR '..'"
BADINUM1	"BAD INODE NUMBER FOR '.'"
MISSING3	"MISSING '.'"
NOFIX0	"CANNOT FIX, FIRST ENTRY IN DIRECTORY CONTAINS %s\n"
NOFIX1	"CANNOT FIX, INSUFFICIENT SPACE TO ADD '.'\n"
NOFIX2	"CANNOT FIX, SECOND ENTRY IN DIRECTORY CONTAINS %s\n"
NOFIX3	"CANNOT FIX, INSUFFICIENT SPACE TO ADD '..'\n"
EXTRA	"EXTRA '.' ENTRY"
EXTRA1	"EXTRA '..' ENTRY"
RANGE	"I OUT OF RANGE"
UNALLOCATED	"UNALLOCATED"
ZEROLEN1	"ZERO LENGTH DIRECTORY"
DUPBAD	"DUP/BAD"
EXTRAN	"IS AN EXTRANEOUS HARD LINK TO DIRECTORY"
BADSTATE2	"BAD STATE %d FOR INODE I=%d"

$ pass4.c
ZEROLEN	"ZERO LENGTH"
BADDUP	"BAD/DUP"

$ pass5.c
ROTTAB	"UNKNOWN ROTATIONAL TABLE FORMAT %d\n"
BADMAGIC	"CG %d: BAD MAGIC NUMBER\n"
FREECTS	"FREE BLK COUNT(S) WRONG IN SUPERBLK"
BLKMISS	"BLK(S) MISSING IN BIT MAPS"
BADSUM	"SUMMARY INFORMATION BAD"

$ setup.c
NOSTAT2	"Can't stat %s: %s\n"
NOTCHAR1	"%s is not a character device"
NOOPEN	"Can't open %s: %s\n"
STAR	"** %s"
NOWRITE1	"NO WRITE ACCESS"
NOWRITE	" (NO WRITE)"
NOALLOC1	"cannot allocate space for superblock\n"
LOOKALT	"LOOK FOR ALTERNATE SUPERBLOCKS"
ALTSUP1	"USING ALTERNATE SUPERBLOCK AT %d\n"
SFAIL	"SEARCH FOR ALTERNATE SUPER-BLOCK FAILED. YOU MUST USE THE"
SFAIL1	"-b OPTION TO FSCK TO SPECIFY THE LOCATION OF AN ALTERNATE"
SFAIL2	"SUPER-BLOCK TO SUPPLY NEEDED INFORMATION; SEE fsck(8)."
BADOP	"UNDEFINED OPTIMIZATION IN SUPERBLOCK"
SETTO	"SET TO DEFAULT"
BADMIN	"IMPOSSIBLE MINFREE=%d IN SUPERBLOCK"
BADINTER	"IMPOSSIBLE INTERLEAVE=%d IN SUPERBLOCK"
FIXED1	" (FIXED)\n"
BADNPS	"IMPOSSIBLE NPSECT=%d IN SUPERBLOCK"
CONVERT1	"CONVERTING TO NEW FILE SYSTEM FORMAT\n"
CONVERT2	"CONVERT TO NEW FILE SYSTEM FORMAT"
PARMS	"PARAMETERS OF CURRENT FILE SYSTEM DO NOT\n\t"
PARMS1	"ALLOW CONVERSION TO OLD FILE SYSTEM FORMAT\n"
CONVERT3	"CONVERTING TO OLD FILE SYSTEM FORMAT\n"
CONVERT4	"CONVERT TO OLD FILE SYSTEM FORMAT"
UNKNOWN2	"UNKNOWN FILE SYSTEM FORMAT\n"
BADSUM1	"BAD SUMMARY INFORMATION"
NOALLOC2	"cannot alloc %u bytes for blockmap\n"
NOALLOC3	"cannot alloc %u bytes for statemap\n"
NOALLOC4	"cannot alloc %u bytes for lncntp\n"
NOALLOC5	"cannot alloc %u bytes for inphead\n"
BADMAGIC1	"MAGIC NUMBER WRONG"
BADNCG	"NCG OUT OF RANGE"
BADCPG	"CPG OUT OF RANGE"
BADNCYL	"NCYL LESS THAN NCG*CPG"
PREPSIZE	"SIZE PREPOSTEROUSLY LARGE"
EXTFS	"** Extended Format Filesystem\n"
NXFS	"** Non-extended Format Filesystem\n"
VALUES	"VALUES IN SUPER BLOCK DISAGREE WITH THOSE IN FIRST ALTERNATE"
BADSUP	"BAD SUPER BLOCK: %s\n"
BADPART	"%s: CANNOT FIGURE OUT FILE SYSTEM PARTITION\n"
BADLABEL	"%s: NOT LABELED AS A BSD FILE SYSTEM (%s)\n"
UNKNOWN	"unknown"
BADIOCTL	"ioctl (GCINFO): %s\n"
CANTREAD	"%s: can't read disk label\n"
CLEANLY	"File system unmounted cleanly - no fsck needed\n"

$ ufs_subr.c
ISBLOCK	"isblock"
CLRBLOCK	"clrblock"
SETBLOCK	"setblock"

$ utilities.c
BADFILE	"bad file type 0%o\n"
INTERNAL1	"INTERNAL ERROR: GOT TO reply()"
NOALLOC6	"cannot allocate buffer pool\n"
DEADLOCK	"deadlocked buffer pool\n"
WRITING	"WRITING %sZERO'ED BLOCK %d TO DISK\n"
PARTIALLY	"PARTIALLY "
CANNOT	"CANNOT %s: BLK %ld"
TERMINATED	"Program terminated\n"
UPDSTD	 "UPDATE STANDARD SUPERBLOCK"
PANICBUF	"Panic: lost %d buffers\n"
CACHEMISS	"cache missed %ld of %ld (%d%%)\n"
SEEK	"SEEK"
NOTREAD	"THE FOLLOWING DISK SECTORS COULD NOT BE READ:"
WRITE	"WRITE"
NOTWRITE	"THE FOLLOWING SECTORS COULD NOT BE WRITTEN:"
SINGLE	"returning to single-user after filesystem check\n"
SALVAGED	" (SALVAGED)\n"
SALVAGE	"SALVAGE"
UNKNOWN3	"UNKNOWN INODESC FIX MODE %d\n"
UNEXP	"%s: UNEXPECTED INCONSISTENCY; RUN fsck MANUALLY.\n"
INTERNAL2	"INTERNAL INCONSISTENCY:"
READ	"READ"
YESNO	"%s? [yn] "
NO	"%s? no\n\n"
YES	"%s? yes\n\n"

$ new messages get added here
INVALID "Invalid -l (parallel checks) of %d.\n"
FASTLINK "FASTLINK on for directory, in: %d "

$set MS_UFS_FSCK_PREEN
$ preen.c
NOCHKLST	"Can't open checklist file: %s\n"
BADDISK	"BAD DISK NAME %s\n"
BADPID	"Unknown pid %d\n"
EXITSIG	"%s (%s): EXITED WITH SIGNAL %d\n"
FOLLOWING	"THE FOLLOWING FILE SYSTEMS HAD AN UNEXPECTED INCONSISTENCY\n\t"
NOMEM0	"out of memory"
FSTAB1	"%s in fstab more than once!\n"
NOSTAT	"Can't stat root\n"
NOSTAT1	"Can't stat %s\n"
NOTCHAR	"%s is not a character device\n"
NOSENSE	"Can't make sense out of name %s\n"
FORK	"fork"
