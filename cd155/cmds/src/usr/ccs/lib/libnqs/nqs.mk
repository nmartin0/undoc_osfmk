# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#++ Makefile -
#
# $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nqs.mk,v $
#
# DESCRIPTION:
#
#	Network Queueing System (NQS) Makefile
#
#
#	To "make" NQS for NPSN UNIX, one should first change directory to
#	the ../proto directory (as a SUPERUSER) and type:
#
#		make directories	(Unnecessary if the NQS directories
#					 already exist)
#	and then (as a NON-SUPERUSER):
#
#		make
#
#	then as SUPERUSER (still in the ../proto directory),
#
#		make install
#
#
#	NOTE:
#		The targets of:  "all" and "install" need
#		to be tailored as appropriate.
#
#
#
#	NQS Run-time Directory Hierarchy:
#
#
#	/usr/spool/nqs/			    NQS directory tree;	       mode 711
#		      new/		    Parent of new request      mode 700
#		      |	 |		    database hierarchy
#		      |	 |		    (mode 700 must be maintained
#		      |  |		    to prevent unauthorized
#		      |	 |		    linking to and deleting of
#		      |  V		    new request files);
#		      |  requests/	    New request files and      mode 777
#		      |			    FIFO request pipe to
#		      |			    NQS local daemon;
#		      private/		    NQS private directories;   mode 700
#		      |	     root/	    NQS root directory;	       mode 777
#		      |		 V	    (Mode MUST be 777)
#		      |		 control/   Parent of the request      mode 777
#		      |		 |	|   control file directories;
#		      |		 |      V
#		      |		 |     <subdirectories>		       mode 777
#		      |		 V
#		      |		 data/	    Parent of the request      mode 777
#		      |		 |   |	    data files (e.g. print
#		      |		 |   |      files, commands for batch
#		      |          |   |      requests;
#		      |		 |   V
#		      |		 |  <subdirectories>		       mode 777
#		      |		 V
#		      |		 database/  NQS database files (see    mode 777
#		      |		 |	    database_qa, database_qo,
#		      |		 V	    and transact also);
#		      |		 database_qa/
#		      |		 V	    Queue access files;	       mode 777
#		      |		 database_qo/
#		      |		 V	    Queue ordering files;      mode 777
#		      |		 failed/    Saved copies of requests   mode 777
#		      |		 V	    that caused problems;
#		      |		 interproc/ Inter-process	       mode 777
#		      |		 |	    communication files for
#		      |		 |	    completion code results FROM
#		      |		 |	    the NQS daemon to local client
#		      |		 V	    processes;
#		      |		 output/    Spooled output files,      mode 777
#		      |		 |     |    and mail log files for
#		      |		 |     |    NQS batch requests.
#		      |		 |     V
#		      |		 |    <subdirectories>		       mode 777
#		      |		 V
#		      |		 transact/  Transaction files;	       mode 777
#		      |			 V
#		      |			<subdirectories>	       mode 777
#		      |			
#		      V
#		      scripts/		    Contains links to active   mode 711
#					    shell scripts when the
#					    NQS shell-strategy is
#					    set to free mode.
#				
#
#
#	Author:
#	-------
#	Brent A. Kingsbury, Sterling Software Incorporated.
#	November 12, 1985.
#
#
# STANDARDS VIOLATIONS:
#
# REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:27:25 $ $State: Exp $)
# $Log: nqs.mk,v $
# Revision 1.5  1994/11/19  02:27:25  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/11/02  01:24:29  mwan
# 1.2 mods
#
# Revision 1.3  1992/12/17  18:10:33  mwan
# T6 update 2 (added NXACCT to makefile).
#
# Revision 1.2  1992/10/17  00:23:47  rkl
# Changed CFLAGS to NQSFLAGS so individual makefiles can define their own.
#
# Revision 1.1  1992/09/24  16:46:03  rkl
# Initial revision
#
#Revision 3.2  91/02/23  12:50:08  root
#Version 2.0 Source
#
# Revision 2.2  87/04/22  15:00:42  hender
# Sterling version 4/22/87
# 
#
#--
#
#
#  System type: [BSD42, BSD43, SGI, SYS52, ULTRIX, UNICOS, UTS]
#
SYSTEM_TYPE = OSF

# Mike: put this in to take care of the MAP_DIRECTORY
#       Also, note that the NQSHOME part must not
#	have closing \" so that the file ../h/nqsdirs.h
#	can change directories appropriately.  This is
#	YuK!!!!!!!!!
#
DEFS =	-D$(SYSTEM_TYPE) \
	-DIOBLKSIZ=\"$(IOBLKSIZ)\" -DNETDB=\"$(NETDB)\" \
	-DMAP_DIRECTORY=\"$(MAP_DIRECTORY)\" \
 	-DNMAP=\"$(NMAP)\" -DNQS_HOME=\"$(NQSHOME) \
	-DNQS_LIBEXE=\"$(NQS_LIBEXE)\"

#
#	Define BYTES_PER_WORD for your hardware.
BYTES_PER_WORD = 2

#	Define PRECISION_ULONG (precision of an unsigned long)
#	in bits for your hardware and compiler.
PRECISION_ULONG = 32

#	Define CPU_LIMIT_GRAN (cpu time limit granularity)
#	in hertz IF your hardware and OS support cpu time limits.
CPU_LIMIT_GRAN = 1

#	Define DISK_LIMIT_GRAN (disk limit granularity) in bytes
#	IF your hardware and OS support file size limits.
DISK_LIMIT_GRAN = 1

#	Define MEM_LIMIT_GRAN (memory limit granularity) in bytes
#	IF your hardware and OS support memory use limits.
MEM_LIMIT_GRAN = 1

#	Define CPU_MEAS_GRAN (cpu measurement granularity)
#	in hertz IF your cpu measurement system call
#	(times(2), getrusage(2)) returns a structure the units
#	of which may vary from one hardware type to another.
CPU_MEAS_GRAN = 60

#	HARDWARE_FLAGS:
#
#	Always include -DBYTES_PER_WORD=$(BYTES_PER_WORD)
#	and -DPRECISION_ULONG=$(PRECISION_ULONG).
#	Include -DCPU_LIMIT_GRAN=$(CPU_LIMIT_GRAN) if your system
#	supports cpu time limits.  Cray-2, Cray-XMP UNICOS, Vax BSD4.2/4.3,
#	and Vax ULTRIX do.
#	Include -DDISK_LIMIT_GRAN=$(DISK_LIMIT_GRAN) if your system
#	supports file size limits.  Cray-2, Cray-XMP UNICOS, Vax BSD4.2/4.3,
#	Vax ULTRIX, Vax SYS5.2, 68K SGI, and Amdahl 58xx UTS do.
#	Include -DMEM_LIMIT_GRAN=$(MEM_LIMIT_GRAN) if your system supports
#	memory use limits.  Cray-2, Cray-XMP UNICOS, Vax BSD4.2/4.3, and
#	Vax ULTRIX do.
#	Include -DCPU_MEAS_GRAN=$(CPU_MEAS_GRAN) if you are NOT
#	running BSD4.2/4.3 or Vax ULTRIX.  Include NET_ORDER if (byte
#	swapping) host order and net order are the same on your hardware.
#       They are the same on the Cray-2, Cray-X/MP, 68K, and Amdahl 58xx.
#
HARDWARE_FLAGS = -DBYTES_PER_WORD=$(BYTES_PER_WORD) \
	-DPRECISION_ULONG=$(PRECISION_ULONG) \
	-DDISK_LIMIT_GRAN=$(DISK_LIMIT_GRAN) \
	-DCPU_LIMIT_GRAN=$(CPU_LIMIT_GRAN) \
	-DMEM_LIMIT_GRAN=$(MEM_LIMIT_GRAN) \
	-DCPU_MEAS_GRAN=$(CPU_MEAS_GRAN) \
	-DNET_ORDER

#     Site dependent compile flags:
#     If your optimizer has bugs, ask for optimization ONLY AT YOUR OWN RISK.
#     Add -DNETWORKED -DSTREAM_SOCKETS for networking over Berkeley sockets.
#
NQSFLAGS = $(IFLAGS) -DNETWORKED -DSTREAM_SOCKETS -DSDSC -DRES -DNXACCT -DSMD_IP_IFACE $(HARDWARE_FLAGS) $(DEFS)

#
# (Start of Makefile.dist)  Do not move this line!
#
#	Makefile configuration variables
#
#  DIR is the name of the Berkeley $(INCLUDE)/sys/dir.h file,
#	or System V libndir emulation:  ../libndir/dir.4.2.h
#  FILE is the name of the System V $(INCLUDE)/fcntl.h file,
#	or the Berkeley version in $(INCLUDE)/sys/file.h
#  INCLUDE is the supporting system include directory.
#  IOBLKSIZ is the name of the <ioblksiz.h> file.  For NPSN UNIX
#	systems, this file resides in $(INCLUDE)/ioblksiz.h.
#	For all other systems, this file resides in:
#	../npsn_compat/ioblksiz.h.  Change this make macro
#	accordingly.
#  NETDB is the name of the <netdb.h> file.  For Berkeley and NPSN
#	UNIX systems, this file resides in $(NETINCLUDE)/netdb.h.
#	For all other systems, this file resides in:
#	../npsn_compat/netdb.h. Change this make macro accordingly.
#  NETINCLUDE is the directory where networking headers are found.
#  NMAP is the name of the <nmap.h> file.  For NPSN systems,
#	this resides in /usr/include/nmap.h.  For all other
#	systems, this file resides in: ../npsn_compat/nmap.h.
#	Change this make macro accordingly.
#  NQS_GROUP is a group that will be given special access
#	to the queueing system (system maintainers).
#  NQS_LIBEXE is where ancillary NQS programs and files live.
#  NQS_OWNER is the name of the user who owns the NQS directory
#	identified by NQS_SPOOL, and all directories descended
#	from the NQS_SPOOL directory.
#  NQS_SPOOL is the top of the NQS spooling tree (.../spool/nqs).
#  NQS_USREXE is where user interface NQS programs live.
#  QMGR_HELPFILE defines the full path of the installed NQS Qmgr
#	help file.
#  QUOLIM is the name of the full path for the "quolim.h" file.
#  RESOURCE is the name of the Berkeley $(INCLUDE)/sys/resource.h
# 	file, the name of the UNICOS $(INCLUDE)/sys/category.h and
#	$(INCLUDE)/sys/resource.h files, or the null string for
#	other systems.
#  ROOT is the name of the superuser account on your system.
#  TIME is the name of the System V $(INCLUDE)/time.h file,
#	or the Berkeley version in $(INCLUDE)/sys/time.h
#
#
#	Define the UNIX system include directory:
#
IOBLKSIZ = nqs/ioblksiz.h
NMAP = nqs/nmap.h
NQS_OWNER = root
NQS_GROUP = bin
NETDB = netdb.h
MAP_DIRECTORY = /etc/nmap
RESINCLUDE=$(IPATH)/res

NQSHOME =       /
NQS_USR =	$(NQSHOME)/usr
NQS_USRLIB =	$(NQS_USR)/lib
NQS_USRSPOOL =	$(NQS_USR)/spool
NQS_SPOOL =	$(NQS_USRSPOOL)/nqs
NQS_LIBEXE =	$(NQS_USRLIB)/nqs
NQS_USREXE =	$(NQS_USR)/bin
QMGR_HELPFILE =	$(NQS_LIBEXE)/qmgr.hlp

NQSDEP =	$(IPATH)/nqs/nqs.h \
		$(IPATH)/nqs/nqsstruct.h \
		$(IPATH)/nqs/nmap.h \
		$(IPATH)/nqs/ioblksiz.h \
		$(IPATH)/nqs/quolim.h
