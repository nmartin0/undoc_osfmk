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
$ (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$
$
$ OSF/1 Release 1.0

$
$ IBM CONFIDENTIAL
$ Copyright International Business Machines Corp. 1989
$ Unpublished Work
$ All Rights Reserved
$ Licensed Material - Property of IBM
$
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ @(#)libc.msg	1.24  com/lib/c/msghdr,3.1,9013 1/24/90 12:13:18

$set MS_LIBC

$ The following are from libc/gen/errlst.c, for perror
1 Not owner
2 No such file or directory
3 No such process
4 Interrupted system call
5 I/O error
6 No such device or address
7 Argument list too long
8 Exec format error
9 Bad file number
10 No child processes
11 Resource temporarily unavailable
12 Not enough space
13 Permission denied
14 Bad address
15 Block device required
16 Device busy
17 File exists
18 Cross-device link
19 No such device
20 Not a directory
21 Is a directory
22 Invalid argument
23 File table overflow
24 Too many open files
25 Not a typewriter
26 Text file busy
27 File too large
28 No space left on device
29 Illegal seek
30 Read-only file system
31 Too many links
32 Broken pipe
33 Argument out of domain
34 Result too large
35 Operation would block
36 Operation now in progress
37 Operation already in progress
38 Socket operation on non-socket
39 Destination address required
40 Message too long
41 Protocol wrong type for socket
42 Option not supported by protocol
43 Protocol not supported
44 Socket type not supported
45 Operation not supported on socket
46 Protocol family not supported
47 Address family not supported by protocol family
48 Address already in use
49 Can't assign requested address
50 Network is down
51 Network dropped connection on reset
52 Network is unreachable
53 Software caused connection abort
54 Connection reset by peer
55 No buffer space available
56 Socket is already connected
57 Socket is not connected
58 Can't send after socket shutdown
59 Too many references: can't splice
60 Connection timed out
61 Connection refused
62 Too many levels of symbolic links
63 File name too long
64 Host is down
65 Host is unreachable
66 Directory not empty
67 Too many processes
68 Too many users
69 Disc quota exceeded
70 Missing file or filesystem
71 Item is not local to host
72 RPC structure is bad
73 RPC version is wrong
74 RPCprogram not available
75 Program version wrong
76 Bad procedure for program
77 No locks available
78 Function not implemented
79 Error number 79 occured
80 No message of desired type
81 Identifier removed
82 Error number 82 occurred
83 Error number 83 occurred
84 Error number 84 occurred
85 Error number 85 occurred
86 Error number 86 occurred
87 Error number 87 occurred
88 Error number 88 occurred
89 Error number 89 occurred
90 Duplicate package name
91 Version mismatch
92 Unresolved package name
93 Unresolved symbol name

$ The following message is for perror
M_PERROR Error %d occurred.

$ The following message is for NLinit
M_NLINIT Cannot obtain standard C locale\n

$ The following messages are for assert
M_ASSERT1 Assertion failed:\040
M_ASSERT2 , file\040
$ Do not add message number for previous string.
M_ASSERT3 , line NNNNN\n
$ Do not add message number for previous string.
$ The previous string must contain NNNNN.
$ _assert will replace NNNNN with the line number (__LINE__).

$ The following message is for initgroups
M_INITG	initgroups: %s is in too many groups\n

$ The following messages are for getgrent
M_SETGR	setgrent: getdomainname system call missing\n
M_GETGR	getgrent: getdomainname system call missing\n

$ The following messages are for getpwent
M_SETPW setpwent: getdomainname system call missing\n
M_GETPW getpwent: getdomainname system call missing\n

$ The following messages are for getopt
M_OPTILL : illegal option -- 
M_OPTARG : option requires an argument -- 

$ The following messages are for getwd
M_RSTAT getwd: can't fullstat /
M_HSTAT getwd: can't fullstat .
M_OPENPAR getwd: can't open ..
M_READPAR getwd: read error in ..

$ The following messages are for psignal()
$ THIS MUST BE KEPT IN_SYNC WITH siglist.c
M_PSIGNAL       Signal 0
M_SIGHUP        Hangup
M_SIGINT        Interrupt
M_SIGQUIT       Quit
M_SIGILL        Illegal instruction
M_SIGTRAP       Trace/BPT trap
M_SIGABRT       IOT/Abort trap
M_SIGEMT        EMT trap
M_SIGFPE        Floating point exception
M_SIGKILL       Killed
M_SIGBUS        Bus error
M_SIGSEGV       Segmentation fault
M_SIGSYS        Bad system call
M_SIGPIPE       Broken pipe
M_SIGALRM       Alarm clock
M_SIGTERM       Terminated
M_SIGURG        Urgent I/O condition
M_SIGSTOP       Stopped (signal)
M_SIGSTP        Stopped
M_SIGCONT       Continued
M_SIGCHLD       Child exited
M_SIGTTIN       Stopped (tty input)
M_SIGTTOU       Stopped (tty output)
M_SIGIO         I/O possible/complete
M_SIGXCPU       Cputime limit exceeded
M_SIGXFSZ       Filesize limit exceeded
M_SIGVTALRM     Virtual timer expired
M_SIGPROF       Profiling timer expired
M_SIGWINCH      Window size changes
M_SIGINFO       Information request
M_SIGUSR1       User defined signal 1
M_SIGUSR2       User defined signal 2
M_NO_SIGNAL     Unknown signal

$ The following messages are for initstate
M_INITST initstate: not enough state (%d bytes) with which to do operation; ignored.\n
M_SETST	setstate: state info has been damaged; not changed.\n

$ The following messages are for mcount
M_MONCNT %s: Number of %s (%u) exceeds profiling limit (%u)\n

$ The following messages are for monitor
M_MON1 %s: No space for profiling buffer(s) (%u bytes)\n
M_MON2 %s: Error processing output data file: %s\n
M_MON3 %s: Null address range specified Lo: %8.8x  Hi: %8.8x\n
M_MON4 %s: _mondata.prof_type(%d) must be _PROF_TYPE_IS_P or _PROF_TYPE_IS_PG\n
M_MON5 %s: No data area found. (Stopped without starting?)\n
M_MON6 %s: Buffer range not contiguous. p[%d]: %8.8x size[%d]: %8.8x p[%d]: %8.8x\n
M_MON7 %s: Ranges not in ascending order. hi[%d]: %8.8x lo[%d]: %8.8x\n

$ The following messages are for crt0/mcrt0/gcrt0
M_CRT0 %s: Profiling setup call to %s failed. rc: %d\n

$ The following message is for re_comp
M_RETOOLONG Regular expression too long
M_NOPREV No previous regular expression
M_UNMATCH unmatched \\(
M_MISSING missing ]
M_TOOMANY too many \\(\\) pairs
M_UNMATCHLEFT unmatched \\)

$ The following message is for longjmperr
M_SETJMPER longjmp or siglongjmp function used outside of saved context\n
$	Cause:	This is a programming error with the setjmp and longjmp or
$		sigsetjmp and siglongjmp functions.  Either the program does
$		not use the setjmp or sigsetjmp function to save the program
$		context, or the function containing the setjmp or sigsetjmp	$		invocation has terminated in the interim.
$
$ The messages 1201-196 and 1201-197 are used above for new errnos.
$ If you are adding please start at 1201-198
$
