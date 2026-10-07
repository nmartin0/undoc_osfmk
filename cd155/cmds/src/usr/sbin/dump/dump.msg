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
$ $Log: dump.msg,v $
$ Revision 1.3  1994/11/19  03:05:33  mtm
$ Copyright additions/changes
$
$ Revision 1.2  1992/10/12  21:47:20  shala
$ New version to understand maj, min and node numbers.
$
$ Revision 1.6  90/10/07  22:07:41  devrcs
$ 	Added EndLog Marker.
$ 	[90/09/29  15:08:45  gm]
$ 
$ Revision 1.5  90/08/25  12:28:28  devrcs
$ 	insert rcsid string;
$ 	[90/08/09  03:52:19  nm]
$ 
$ Revision 1.4  90/07/27  11:18:29  devrcs
$ 	delete some comment;
$ 	[90/07/18  06:32:45  nm]
$ 
$ Revision 1.3  90/06/22  22:11:38  devrcs
$ 	integration to osc.12
$ 	[90/05/31  04:37:07  nm]
$ 
$ 	enter copyright
$ 	[90/05/07  07:44:32  nm]
$ 
$ Revision 1.2  90/04/14  00:12:35  devrcs
$ 	creation
$ 	[90/04/10  08:08:42  nm]
$ 
$ $EndLog$

$ @(#) $RCSfile: dump.msg,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:05:33 $

$quote "
$set MS_DUMP
ABORT1		"Do you want to abort"
ABORTDUM	"Do you want to abort dump"
ABORTING	"SIGSEGV received -- ABORTING!\n"
AULFS		"An unlisted file system"
BADCODE		"Bad exit status from child %d: %d\n"
BADFS		"Bad file system: invalid superblock magic\n"
BADKEY		"Bad option: '%c'\n"
BDMGK		"Bad magic number in tape header\n"
BLKREM		"Blocks remaining on volume: %d"
BREADERR	"More than %d block read errors from %s\n"
BWTDT1		"Cannot get the previous write date of tape\n"
BWTDT2		"Tape may be either 1) not a dump tape, 2) a new,\n"
BWTDT3		"unused tape, or 3) not at the load-point\n"
CANTODISK	"Cannot open file-system file %s\n"
CANTODT		"Cannot open fstab file %s\n"
CHANGC		"Change Cartridges: Insert volume %d, tape # %04d\n"
CHANGC2		"CHANGE CARTRIDGES!\7\7\n"
CHANGD		"Change Diskettes: Insert volume %d, diskette # %04d\n"
CHANGD2		"CHANGE DISKETTES!\7\7\n"
CHANGT		"Change Tapes: Mount volume %d, tape # %04d\n"
CHANGT2		"CHANGE TAPES!\7\7\n"
CHILDR		"Child %d returns low-byte status %o\n"
CKPTBC		"Check position of tape before continuing\n"
CKSMERR		"Checksum error in tape header\n"
CNBSP		"Cannot block slave process %d\n"
CNGLHN		"Cannot get local host name\n"
CNOTFD		"Cannot open tape for date check\n"
CNOTST		"Cannot get status of tape or file\n"
CNTBH		"We cannot be here in query() in edump!\n"
CNTFK		"Cannot fork to broadcast message\n"
CNUSP		"Cannot unblock slave process %d\n"
CNWSP		"Slave process %d cannot wait successfully\n"
CODHFR		"Cannot open dump history file %s for reading\n"
CODHFRR		"Cannot open dump history file %s for reading and rewriting\n"
CONTINU		"Do you want to attempt to continue"
CORDIR		"Corrupted directory, i-node: %d\n"
COUTMP		"Cannot open /etc/utmp for reading\n"
CREPLACE	"Then replace the faulty cartridge with a new one\n"
CREWIND		"Cartridge rewinding\n"
CRODF		"Cannot re-open file system file %s\n"
CSDHF		"Cannot seek back to beginning of dump history file %s\n"
CTDHF		"Cannot truncate dump history file %s\n"
CWREWIND	"This cartridge will rewind\n"
CWTS		"Cannot write to slave pipe\n"
DISKBEG		"Volume %d, diskette # %04d, begins with blocks from i-node %d\n"
DISKEE		"DISKETTE ERROR!\7\7\n"
DISKWRE		"Diskette write error %d blocks into volume %d, diskette # %04d\n"
DMPOK		"Dump completed ok"
DONE		"Dump completed at %s\n"
DONE2		"DUMP IS DONE!\7\7\n"
DUMPAIL		"DUMP IS AILING!\7\7\n"
DUMPBLKS	"Actual: %ld tape blocks on %d volume(s)\n"
DUMPD		"Date of this level %c dump: %s\n"
DUMPDL		"Date of last level %c dump: %s\n"
DUMPFSYS	"Dump these file systems:\n"
DUMPH		"Dumping from host %s\n"
DUMPING		"Dumping %s "
DUMPL		"Level %c dump on %s\n"
DUMPLL		"%c %8s\t(%6s) Last dump: Level %c, Date %s\n"
DUMPPWAIT	"Parent %d waiting for child %d has another child %d return\n"
DYRWTO		"Do you really wish to overwrite this tape"
DYWTABT		"Do you wish to abort the dump"
DYWTCA		"Do you want to continue anyway"
DYWTOT		"Do you want to overwrite this tape"
DYWTR		"Do you want to retry"
ENTIRE		"The ENTIRE dump is aborted\n"
EPOCH		"the start of the epoch "
ESTBLK		"Estimate: %ld tape blocks on %3.2f volume(s)\n"
FINISH		"%3.2f%% done -- finished in %02d:%02d\n"
FOPENTTY	"Cannot fopen /dev/tty for reading\n"
FORKF		"Checkpoint fork fails in parent %d\n"
FTOKF		"Cannot obtain key\n"
FTREM		"Feet remaining on tape: %d"
IFMORE1		"If this volume contained more than one\n"
IFMORE2		"dump, only the last one will be rewritten\n"
IFMORE3		"on the new volume; previous dumps are still\n"
IFMORE4		"intact on the faulty volume\n"
IOCFAIL		"Cannot get device info for %s\n"
ILLMEDIUM	"%s(%d):Illegal flag %d.\n"
INTR		"Interrupt received.\n"
ITCTMR		"Is the correct tape mounted and ready to go"
LASTDU		"Last dump(s) done (Dump '>' file systems):\n"
LOCKF		"Cannot create key-creation lock file %s\n"
LOSTCON		"Lost connection to remote peer\n"
LSEEKF		"Bad lseek in input disk file: %s, block number: %d\n"
MAP1		"Mapping (Pass I) [regular files]\n"
MAP2		"Mapping (Pass II) [directories]\n"
MAP2C		"Mapping (Pass II) [directories] (continued)\n"
MAP3		"Dumping (Pass III) [directories]\n"
MAP4		"Dumping (Pass IV) [regular files]\n"
MASSLV1		"Master/slave protocol botched\n"
MASSLV2		"Cannot read from master pipe\n"
MEDIAOPEN	"Cannot open device file %s\n"
MFD1		"Message from the dump program to all operators "
MFD2		"at %d:%02d ... \r\n\n"
MNTCTLE		"Cannot get status of mounted file systems\n"
NEEDATT		"NEEDS ATTENTION: %s?: (\"yes\" or \"no\") "
NEEDATT1	"NEEDS ATTENTION: %s?: (\"%s\" or \"%s\") "
NEEDATTB	"DUMP NEEDS ATTENTION!\7\7\n"
NEEDKEY		"Need option 'f' followed by remote device \"host:tape\"\n"
NEWCART 	"Is the new cartridge inserted and ready to go"
NEWDISK		"Is the new diskette ready to go"
NEWTAPE		"Is the new tape mounted and ready to go"
NOCON		"Cannot establish connection to remote rmt command\n"
NOGRP		"No entry in /etc/group for %s\n"
NOLBL		"No label"
NOTHAP		"Bad read from input disk file: %s, block number: %d, bytes wanted: %d, bytes got: %d\n"
OPENRTF		"Cannot open remote device file %s\n"
OPENTF		"Cannot open device file %s\n"
PIPEB		"Broken pipe\n"
PROTO		"rmt_reply(): Protocol to remote tape server botched (code %s)\n"
PROTO1		"rmt_gets(): Protocol to remote tape server botched\n"
REPLACE		"Then replace the faulty tape with a new one\n"
REPLACED	"Replace the faulty diskette with a new one\n"
RERR		"Communication error in connection to remote peer\n"
RESTART		"Do you want to restart this volume"
REWIND		"This tape will rewind\n"
REWRITE		"This dump volume will be rewritten\n"
REWRITE1	"Unexpected signal -- attempting to rewrite from last checkpoint\n"
RRBN		"Cannot read buffer from socket\n"
RRSCN		"Cannot read single character from socket\n"
RTRYOP		"Do you want to retry the open"
RUIN1		"This tape was written last on %s\n"
RUIN2		"That was less than a week ago!\n"
RUIN3		"You may be RUINING a good tape!  Be careful!\n"
RWBN		"Cannot write buffer to socket\n"
RWWLN		"Cannot write command line to socket\n"
SCNTRD		"Slave cannot read from pipe\n"
SDCHK		"Skipping date check\n"
SEMCTL2		"Cannot remove semaphores\n"
SEMCTLE		"Cannot set semaphore values\n"
SEMGETE		"Cannot get semaphores\n"
SLAVES1		"Cannot open slave pipe\n"
SLAVES2		"Cannot fork slave %d\n"
SOCSZ		"Cannot set socket send-buffer size\n"
STDOUT		"standard output"
TAPEBEG		"Volume %d, tape # %04d, begins with blocks from i-node %d\n"
TAPEEE		"TAPE ERROR!\7\7\n"
TAPWERW		"Tape write error -- wanted to write: %d, only wrote: %d\n"
TAPEWRE		"Write error on %s -- cannot recover\n"
TAPEWRE1	"Write error %d blocks into volume %d, tape # %04d\n"
TAPEWRE2	"Tape write error %d feet into volume %d, tape # %04d\n"
THNTTH		"Tape header record not of type tape-header\n"
TOEHST		"to %s on %s\n"
TOHOST		"to %s on host %s\n"
TOLOCLA		"to %s\n"
TPBSIZE		"TP_BSIZE must be a multiple of DEV_BSIZE\n"
TPBSNIN		"TP_BSIZE * TP_NINDIR must be a multiple of the dumped file system's block size\n"
TPFF		"Tape flush failed\n"
TREWF 		"Tape rewind failed\n"
TREWIND		"Tape rewinding and unloading\n"
TREWTH		"Tape read error while trying to read tape header\n"
TRRF		"Tape repositioning (rewind) failed\n"
TRYREW		"SIGHUP received -- Try rewriting\n"
TRYREW1		"SIGTRAP received -- Try rewriting\n"
TRYREW2		"SIGFPE received -- Try rewriting\n"
TRYREW3		"SIGBUS received -- Try rewriting\n"
TRYREW4		"SIGALRM received -- Try rewriting\n"
TRYREW5		"SIGTERM received -- Try rewriting\n"
UNKIF		"Bad dump history format in %s: line %d\n"
UNKSERV		"rdump: shell/tcp: unknown service\n"
UNKSIG		"Unexpected signal -- cannot recover\n"
UNLDCR1		"Unloading current tape\n"
UNLDCR2		"Expecting new tape to be mounted\n"
UNMNTC1 	"Remove last cartridge\n"
UNMNTC2 	"REMOVE LAST CARTRIDGE!\7\7\n"
UNMNTD1		"Remove last diskette\n"
UNMNTD2		"REMOVE LAST DISKETTE!\7\7\n"
UNMNTT1		"Unmount last tape\n"
UNMNTT2		"UNMOUNT LAST TAPE!\7\7\n"
UNRECERR	"This is an unrecoverable error.\n"
UTCDS		"Unable to check drive status\n"
UTDT		"Unknown tape drive type: %d (0x%x)\n"
VLDDT		"Mounted tape has a valid date\n"
VOLUSD		"Volumes used: %d"
WARNING1	"File system still mounted -- inconsistent dump possible\n"
YESNO		"\"Yes\" or \"No\"?\n\n"
YESNO1		"\"%s\" or \"%s\"?\n\n"
