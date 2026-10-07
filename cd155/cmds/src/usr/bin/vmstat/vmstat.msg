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
$ (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
$ ALL RIGHTS RESERVED 
$
$
$ OSF/1 1.1
$

$quote "

$set MS_VMSTAT

USAGE 		"Usage: vmstat [-f | -s] [interval [count]]\n"
EPERM		"missing privilege to run"
MSG_NOPRIV		"%s: insufficient privileges\n"
ENOMEM		"insufficient memory for process table"
NO_VMSTATS	"cannot get vm statistics info"
NO_SYSINFO	"cannot get system info"
NO_INTR		"cannot get interrupt info"
RPT_VMSTATS	"Virtual Memory Statistics: (pagesize = %d)\n"
RPT_FORKS	"forks"
RPT_VFORKS	"vforks"
RPT_ACTIVE	"active pages"
RPT_INACTIVE	"inactive pages"
RPT_FREEPAGES	"free pages"
RPT_WIRED	"wired pages"
RPT_FAULTS	"virtual memory page faults"
RPT_COW		"copy-on-write page faults"
RPT_ZEROFILL	"zero fill page faults"
RPT_REACTIVE	"reattaches from reclaim list"
RPT_PAGEINS	"pages paged in"
RPT_PAGEOUTS	"pages paged out"
RPT_LOOKUPS	"number of lookups"
RPT_HITS	"number of hits"
RPT_CACHE	"hit ratio percentage"
RPT_CONTEXT	"task and thread context switches"
RPT_DEVINTR	"device interrupts"
RPT_SYSCALL	"system calls"
HDR_MSG_R	"r"
HDR_MSG_W	"w"
HDR_MSG_U	"u"
HDR_MSG_ACT	"act"
HDR_MSG_FREE	"free"
HDR_MSG_WIRE	"wire"
HDR_MSG_FAULT	"fault"
HDR_MSG_COW	"cow"
HDR_MSG_ZERO	"zero"
HDR_MSG_REACT	"react"
HDR_MSG_PIN	"pin"
HDR_MSG_POUT	"pout"
HDR_MSG_IN	"in"
HDR_MSG_SY	"sy"
HDR_MSG_CS	"cs"
HDR_MSG_US	"us"
HDR_MSG_SY2	"sy"
HDR_MSG_ID	"id"
HDR_MSG_PROCS	"procs"
HDR_MSG_MEMORY	"memory"
HDR_MSG_PAGES	"pages"
HDR_MSG_INTR	"intr"
HDR_MSG_CPU	"cpu"
