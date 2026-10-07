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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: errlst.c,v $ $Revision: 2.34 $ (OSF) $Date: 1995/01/20 18:26:51 $";
#endif
/* History
 * $Log: errlst.c,v $
 * Revision 2.34  1995/01/20  18:26:51  joel
 *  Reviewer:      sean
 *  Risk:          low
 *  Benefit or PTS #: 11968 sender exits before recv_continue (new error msg)
 *  Testing:       testcase, message EATs
 *  Module(s): msgp_inq.c msgp_nxdat.c msgp_atdt.c mcmsg_nx.h nxlib.c
 *             libc/errlst.c  server/sys/errno.h
 *
 * Revision 2.33  1994/11/19  02:04:31  mtm
 * Copyright additions/changes
 *
 * Revision 2.32  1994/06/01  20:31:26  mag
 * Added code 166
 *  Reviewer: cfj, sdh, shala
 *  Risk: low
 *  Benefit or PTS #: MP allocator changes
 *  Testing:
 *  Module(s):
 *
 * Revision 2.31  1993/12/01  01:52:27  carbajal
 * Defined error code 143-146
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 2.30  1993/10/04  18:41:54  cfj
 * Added EINCOMPAT error code.
 *
 * Revision 2.29  1993/09/23  18:56:40  ellend
 * Fix 3870 - Change Export to export
 *
 * Revision 2.28  1993/09/21  21:12:38  carbajal
 * Added EAOVLP to clear PTS #6493
 *
 * Revision 2.27  1993/08/28  00:53:23  carbajal
 * Added EANXACCT for PTS # 6141
 *
 * Revision 2.26  1993/08/04  17:00:34  wunder
 * Added errno 215 error message for async I/O.
 *
 * Revision 2.25  1993/04/22  02:13:20  brad
 * Merged in PFS error messages.
 *
 * Revision 2.22.2.2  1993/02/23  05:03:22  brad
 * Added ENOSDIR and EPFSBUSY.
 *
 * Revision 2.22.2.1  1993/01/20  21:11:28  brad
 * Added error strings for ESTRIPED and ENOTPFS.
 *
 * Revision 2.24  1993/01/20  01:05:11  andrews
 * Fixed bug #3983 (errno set IPD message support calls in libnx/dbglib.c).
 * Created new error (EINTERNAL #165).
 *
 * Revision 2.23  1993/01/18  20:08:30  carbajal
 * added EANOEXIST message
 *
 * Revision 2.22  1992/12/18  02:41:29  carbajal
 * added string for EXBRS (163)
 *
 * Revision 2.21  1992/10/13  19:54:14  carbajal
 * Added error codes 159,160 and 161 for chpart command.
 *
*/
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: sys_errlist, sys_nerr
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * errlst.c	1.10  com/lib/c/gen,3.1,8943 10/30/89 00:34:04
 */

/* errno lives here */
int errno;

char	*sys_errlist[] = {
	"Error 0",                             /* Corresponding errno */
	"Not owner",                           /*  EPERM          1 */
	"No such file or directory",           /*  ENOENT         2 */
	"No such process",                     /*  ESRCH          3 */
	"Interrupted system call",             /*  EINTR          4 */
	"I/O error",                           /*  EIO            5 */
	"No such device or address",           /*  ENXIO          6 */
	"Arg list too long",                   /*  E2BIG          7 */
	"Exec format error",                   /*  ENOEXEC        8 */
	"Bad file number",                     /*  EBADF          9 */
	"No child processes",                  /*  ECHILD         10 */
	"Resource deadlock avoided",           /*  EDEADLK        11 */
	"Not enough space",                    /*  ENOMEM         12 */
	"Permission denied",                   /*  EACCES         13 */
	"Bad address",                         /*  EFAULT         14 */
	"Block device required",               /*  ENOTBLK        15 */
	"Device busy",                         /*  EBUSY          16 */
	"File exists",                         /*  EEXIST         17 */
	"Cross-device link",                   /*  EXDEV          18 */
	"No such device",                      /*  ENODEV         19 */
	"Not a directory",                     /*  ENOTDIR        20 */
	"Is a directory",                      /*  EISDIR         21 */
	"Invalid argument",                    /*  EINVAL         22 */
	"File table overflow",                 /*  ENFILE         23 */
	"Too many open files",                 /*  EMFILE         24 */
	"Not a typewriter",                    /*  ENOTTY         25 */
	"Text file busy",                      /*  ETXTBSY        26 */
	"File too large",                      /*  EFBIG          27 */
	"No space left on device",             /*  ENOSPC         28 */
	"Illegal seek",                        /*  ESPIPE         29 */
	"Read-only file system",               /*  EROFS          30 */
	"Too many links",                      /*  EMLINK         31 */
	"Broken pipe",                         /*  EPIPE          32 */
	"Argument out of domain",              /*  EDOM           33 */
	"Result too large",                    /*  ERANGE         34 */

/* non-blocking and interrupt i/o */
	"Operation would block",		/* EWOULDBLOCK    35 */
     /* "Resource temporarily unavailable",     * EAGAIN         35 */
	"Operation now in progress",		/* EINPROGRESS    36 */
	"Operation already in progress",	/* EALREADY       37 */

/* ipc/network software */

	/* argument errors */
	"Socket operation on non-socket",	/* ENOTSOCK        38 */
	"Destination address required",		/* EDESTADDRREQ    39 */
	"Message too long",			/* EMSGSIZE        40 */
	"Protocol wrong type for socket",	/* EPROTOTYPE      41 */
	"Option not supported by protocol",	/* ENOPROTOOPT     42 */
	"Protocol not supported",		/* EPROTONOSUPPORT 43 */
	"Socket type not supported",		/* ESOCKTNOSUPPORT 44 */
	"Operation not supported on socket",	/* EOPNOTSUPP      45 */
	"Protocol family not supported",	/* EPFNOSUPPORT    46 */
	"Address family not supported by protocol family",
						/* EAFNOSUPPORT    47 */
	"Address already in use",		/* EADDRINUSE      48 */
	"Can't assign requested address",	/* EADDRNOTAVAIL   49 */

	/* operational errors */
	"Network is down",			/* ENETDOWN       50 */
	"Network is unreachable",		/* ENETUNREACH    51 */
	"Network dropped connection on reset",	/* ENETRESET      52 */
	"Software caused connection abort",	/* ECONNABORTED   53 */
	"Connection reset by peer",		/* ECONNRESET     54 */
	"No buffer space available",		/* ENOBUFS        55 */
	"Socket is already connected",		/* EISCONN        56 */
	"Socket is not connected",		/* ENOTCONN       57 */
	"Can't send after socket shutdown",	/* ESHUTDOWN      58 */
	"Too many references: can't splice",	/* ETOOMANYREFS   59 */
	"Connection timed out",			/* ETIMEDOUT      60 */
	"Connection refused",			/* EREFUSED       61 */
	"Too many levels of symbolic links",	/* ELOOP          62 */
	"File name too long",			/* ENAMETOOLONG   63 */
	"Host is down",				/* EHOSTDOWN      64 */
	"Host is unreachable",			/* EHOSTUNREACH   65 */
	"Directory not empty",			/* ENOTEMPTY      66 */
	"Too many processes",			/* EPROCLIM       67 */
	"Too many users",			/* EUSERS         68 */
	"Disc quota exceeded",			/* EDQUOT         69 */
	"Missing file or filesystem",           /* ESTALE         70 */
	"Item is not local to host",            /* EREMOTE        71 */
	"RPC structure is bad",			/* EBADRPC        72 */
	"RPC version is wrong",			/* ERPCMISMATCH   73 */
	"RPC program not available",		/* EPROGUNAVAIL   74 */
	"Program version wrong",		/* EPROGMISMATCH  75 */
	"Bad procedure for program",		/* EPROCUNAVAIL   76 */
	"No locks available",                   /* ENOLCK         77 */
	"Function not implemented",             /* ENOSYS         78 */
	"Error number 79 occurred",		/* ??????         79 */
	"No message of desired type",           /* ENOMSG         80 */
	"Identifier removed",                   /* EIDRM          81 */
	"Out of STREAMS resources",		/* ENOSR	  82 */
	"System call timed out",		/* ETIME	  83 */
	"Next message has wrong type",		/* EBADMSG	  84 */
	"Error in protocol",			/* EPROTO	  85 */
	"No message on stream head read q",	/* ENODATA	  86 */
	"fd not associated with a stream",	/* ENOSTR	  87 */
	"Tells open to clone the device",	/* ECLONEME	  88 */
	"Mounting a dirty fs w/o force",	/* EDIRTY	  89 */
	"Duplicate package name",		/* EDUPPKG	  90 */
	"Version mismatch",			/* EVERSION 	  91 */
	"Unresolved package name",		/* ENOPKG	  92 */
	"Unresolved symbol name",		/* ENOSYM	  93 */
        "Error 94",
        "Error 95",
        "Error 96",
        "Error 97",
        "Error 98",
        "Error 99",
        "Error 100",
        "Error 101",
        "Error 102",
        "Error 103",
        "Error 104",
        "Error 105",
        "Error 106",
        "Error 107",
        "Error 108",
        "Error 109",
        "Error 110",
        "Error 111",
        "Error 112",
        "Error 113",
        "Error 114",
        "Error 115",
        "Error 116",
        "Error 117",
        "Error 118",
        "Error 119",
        "Error 120",
        "Error 121",
        "Error 122",
        "Error 123",
        "Error 124",
        "Error 125",
        "Error 126",
        "Error 127",
        "Error 128",
        "Error 129",
        "Allocator internal error",		/* EPALLOCERR	  130 */	
        "Exceeds partition resources",		/* EPXRS	  131 */	
        "Bad node specification",		/* EPBADNODE	  132 */	
        "Partition not found",			/* EPINVLPART	  133 */	
        "Invalid priority",			/* EPINVALPRI	  134 */	
        "Partition not empty",	 	       	/* EPNOTEMPTY	  135 */	
        "Invalid mode",	 	 	      	/* EPINVALMOD	  136 */	
        "Partition exists", 	 	      	/* EPPARTEXIST	  137 */
	"Invalid Scheduling",                   /* EPINVALSCHED   138 */
        "Partition permission denied",          /* EPACCES        139 */
        "Do a rfork instead of a fork",         /* ERFORK         140 */
        "NX accounting permission denied",	/* EANXACCT	  141 */
	"Request overlaps with nodes in use",	/* EAOVLP	  142 */
	"Change to SPACE SHARED partition not allowed", /* EINVALSPS 143 */
	"Use of -plk not allowed in gang scheduled partition", /* EAREJPLK 144 */
	"Scheduling parameters conflict with allocator configuration", /* ESCHEDCONF      145 */
	"Exceeded allocator configuration parameters",	/* EEXCEEDCONF     146 */
        "Error 147",
        "Error 148",
        "Error 149",
        "Packet size invalid or out of range",          /* EAINVALPKT   150 */
        "Memory buffer invalid or out of range",        /* EAINVALMBF   151 */
        "Memory export invalid or out of range",        /* EAINVALMEX   152 */
        "Memory each invalid or out of range",          /* EAINVALMEA   153 */
        "Send threshold invalid or out of range",       /* EAINVALSTH   154 */
        "Send count invalid or out of range",           /* EAINVALSCT   155 */
        "Give threshold invalid or out of range",       /* EAINVALGTH   156 */
        "Calling process not process group leader",     /* EANOTPGL     157 */
        "Application exists for process group",         /* EAEXIST      158 */
	"Invalid user id",				/* EPINUSER	159 */
	"Invalid group id",				/* EPINGRP	160 */
	"Invalid partition rename",			/* EPINRN	161 */
	"Partition lock denied",			/* EPLOCK       162 */
	"Partition request contains unavailable nodes",	/* EPBXRS	163 */
        "Application does not exist for process group", /* EANOEXIST    164 */
        "Miscellaneous internal error",			/* EINTERNAL	165 */
        "Some nodes in the map or rectangle do not qualify", /* EPNOMATCH 166 */
        "Error 167",
        "Error 168",
        "Error 169",
	"Invalid buffer pointer",                       /* 170 EQPBUF */
        "Buffer length exceeds allocation",             /* 171 EQBLEN */
        "Invalid length",                               /* 172 EQLEN */
        "Time limit exceeded",                          /* 173 EQTIME */
        "Received message too long for buffer",         /* 174 EQMSGLONG */
        "Invalid ptype",                                /* 175 EQPID */
        "Invalid node",                                 /* 176 EQNODE */
        "Invalid type",                                 /* 177 EQTYPE */
        "Invalid message id",                           /* 178 EQMID */
        "Invalid handler type",                         /* 179 EQHND */
        "Out of process slots",                         /* 180 EQNOPROC */
        "Ptype already in use",                         /* 181 EQUSEPID */
        "No active process",                            /* 182 EQNOACT */
        "Invalid object file",                          /* 183 EQBADFIL */
        "Invalid parameter",                            /* 184 EQPARAM */
        "Invalid file name pointer",                    /* 185 EQPFIL */
        "Invalid cnode pointer",                        /* 186 EQPCNODE */
        "Invalid cpid pointer",                         /* 187 EQPCPID */
        "Invalid ccode pointer",                        /* 188 EQPCCODE */
        "Privileged operation",                         /* 189 EQPRIV */
        "Not enough memory",                            /* 190 EQMEM */
        "Too many requests",                            /* 191 EQNOMID */
        "Ptype already set",                            /* 192 EQSET */
        "No ptype defined",                             /* 193 EQNOSET */
        "Invalid diagnostic channel usm id",            /* 194 EQUSM */
        "Invalid dimension",                            /* 195 EQDIM */
        "Invalid diagnostic channel mode",              /* 196 EQMODE */
        "Invalid diagnostic channel status",            /* 197 EQSTATUS */
        "Received message too short for buffer",        /* 198 EQMSGSHORT */
        "Seek to different file pointers",              /* 199 ECFPS */
        "Different file pointers",                      /* 200 ENFPS */
        "Mixed file operations",                        /* 201 EMIXIO */
        "Bad io mode number",                           /* 202 EIMODE */
        "File is not synchronized",                     /* 203 ESETIO */
        "No CFS available",                             /* 204 ENOCFS */
        "Arithmetic overflow",                          /* 205 EQESIZE */
        "Attempt to read past end of file",             /* 206 ERDEOF */
        "Path name too long",                           /* 207 EQPATH */
        "Max number of applications under debug was reached",  /* 208 EQTAM */
        "Supplied processes group does not exist or is under control of another TAM",                  				       /* 209 EQPGRP */
        "Operation not supported by this file system",	/* 210 EFSNOTSUPP */
        "Error 211",					/* 211 ESTRIPED */
        "Not a PFS file",				/* 212 ENOTPFS */
        "PFS stripe directory or stripe file not available",  /* 213 ENOSDIR */
        "PFS stripe file in use",			/* 214 EPFSBUSY */
        "Asynchronous request ID invalid",		/* 215 EBADID */
        "Parallel application incompatible with OS release", /* 216 EINCOMPAT */
	"Sender aborted during message receive"		/* 217 EQSENDABORT */
};

int	sys_nerr = { sizeof(sys_errlist)/sizeof(sys_errlist[0]) };
