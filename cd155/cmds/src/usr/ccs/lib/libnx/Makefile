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
 
#
# (c) Copyright 1992, INTEL , CORP.
# ALL RIGHTS RESERVED
#
#
# INTEL  1.0
# $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/Makefile,v 1.81 1994/11/29 19:52:28 tnt Exp $
#
#
# History
# $Log: Makefile,v $
# Revision 1.81  1994/11/29  19:52:28  tnt
# PTS #: 11639
# Mandatory?:  No (H1)
# Description: During a fork() the memory allocated for the stacks
#              of the NX threads was not freed.  This memory space is
#              now reclaimed during the start-up of the forked child.
#              Two versions of forknx.o now exist, one for libnx.a and
#              one for libc.a.  The libc.a version is as it was in R1.2.
# Reviewer(s): Shane Story
# Risk:        Medium
# Testing:     Developer tests, Message Passing, Controlc, Rmcall,
#              and Misc. EATs.  Also, built libc.a and tested running
#              non-NX applications on service node; booted from ramdisk
#              and ran non-NX applications on boot node.
# Module(s):   usr/ccs/lib/common_nx_c/forknx.c
#              usr/ccs/lib/libnx/nxlib.c
#              usr/ccs/lib/libnx/Makefile
#
# Changes made so that a version of forknx.o is built specifically for
# libnx.a.
#
# Revision 1.80  1994/11/19  02:28:33  mtm
# Copyright additions/changes
#
# Revision 1.79  1994/08/31  20:22:31  bradf
#    This commit is part of the R1_3 branch -> mainline collapse. This
#    action was approved by the R1.X meeting participants.
#
#    Reviewer:        None
#    Risk:            Something didn't get merged properly, or something
#                     left on the mainline that wasn't approved for RTI
#                     (this is VERY unlikely)
#    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
#                     developers will not have to make sure their
#                     changes get onto two separate branches.
#    Testing:         R1_3 branch will be compared (diff'd) with the new
#                     main. (Various tags have been set incase we have to
#                     back up)
#    Modules:         Too numerous to list.
#
# Revision 1.78.2.1  1994/08/22  21:19:48  nina
#  Reviewer:yzz, hobbes
#  Risk:Medium
#  Benefit or PTS #:7618, 10383. See also 7155 and 10114.
#  Testing:NFS EATS
#  Module(s):
# 	server/sys/socketvar.h
# 	server/bsd/uipc_socket.c
# 	server/vsocket/vs_netops.c
# 	server/vsocket/vs_subr.c
#
# 	cmds_libs/usr/src/sbin/portmap/Makefile
# 	cmds_libs/usr/src/sbin/portmap/portmap.c
#
# 	cmds_libs/usr/src/sbin/mountd/Makefile
# 	cmds_libs/usr/src/sbin/mountd/mountd.c
#
# 	cmds_libs/usr/src/sbin/nfsd/Makefile
# 	cmds_libs/usr/src/sbin/nfsd/nfsd.c
#
# 	cmds_libs/usr/src/sbin/nfsiod/Makefile
# 	cmds_libs/usr/src/sbin/nfsiod/nfsiod.c
#
# 	cmds_libs/usr/src/ccs/lib/libnx/Makefile
# 	cmds_libs/usr/src/ccs/lib/libnx/tnc_subs.c (NEW)
#
# 	cmds_libs/sbin/init.d/nfs
#
#
# Modifications have been made to the server, libnx, the NFS
# daemons and nfs start/stop script so that each configured
# network server node runs its own copy of the NFS daemons.
#
# Added tnc_subs.c to the libnx makefile.
#
# Revision 1.78  1994/07/14  21:47:56  shala
#  Reviewer: NONE
#  Risk: Low
#  Benefit or PTS #: Extra file
#  Testing: Build nxlib.o object.
#  Module(s): cmds_libs/src/usr/ccs/lib/libnx/nxlib.c
#             cmds_libs/src/usr/ccs/lib/libnx/Makefile
#
# Removed nxconcur.o since R5.0 compiler does not need it anymore.
# _mp_init() is handled differently.
#
# Revision 1.77  1994/06/28  16:55:50  shala
#  Reviewer: NONE
#  Risk: Low
#  Benefit or PTS #: Bug #9326
#  Testing: Made a new libnx.a and tested NX applications using perror.
#  Module(s): cmds_libs/src/usr/ccs/lib/libnx/Makefile
#             cmds_libs/src/usr/ccs/lib/libnx/perror.c
#
# Added NX version of perror() to libnx.a in order to do buffering before
# printing the message to stderr.
#
# Revision 1.76  1994/06/16  18:00:00  dleslie
#  Reviewer: Shala Arshi
#  Risk: low
#  Benefit or PTS #: autoinit.o and __denormal.o are installed in the
# 	correct place by the appropriate Makefiles.  postinstall.sh
# 	no longer has to move them. This will help the patch process
# 	know where these files are supposed to go.
#  Testing: verified with 'cmake -n install_all' that the files will be
# 	installed correctly
#  Module(s):
# 	setup/postinstall.sh
# 	usr/ccs/lib/libc/Makefile
# 	usr/ccs/lib/libnx/Makefile
#
# Revision 1.75  1994/06/14  02:33:47  brad
# Added fcntl_.c for getting and setting per-file PFS stripe attributes
# from a FORTRAN program.
#
#  Reviewer: None
#  Risk: Low
#  Benefit or PTS #: 7593
#  Testing: Developer testing, PFS EATs on 64 nodes
#  Module(s): cmds_libs/src/usr/include/fnx.h
#             cmds_libs/src/usr/ccs/lib/libnx/{Makefile,fcntl_.c}
#
#
#  Reviewer:
#  Risk:
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.74  1994/06/03  16:04:32  dleslie
#  Reviewer: Shala Arshi, Sean Griffen, Kathy Carver
#  Risk: L
#  Benefit or PTS #: produce nxconcur.o, which can be linked in by the
# 	compiler driver when -Mconcur is specified, so _mp_init() is
# 	called, so the application
#    	gives the runtime library a chance to create enough threads
#    	to make all of the processors run.
#  Testing: builds
#  Module(s): Makefile
#
# Revision 1.73  1994/06/01  20:12:44  mag
# Mesh utilities changes adding Node Attributes
#  Reviewer: cfj, sdh, shala
#  Risk: High
#  Benefit or PTS #: Needed for MP support
#  Testing: EATS: rmcall, rmcmd, sched
#  Module(s): Makefile, bitmap.c, bitmap2.c, nx_initve.c, nx_initve_.c,
# 	    nx_part_ops.c, partprint.c, partutils.c, utils.c,
# 	    _copyargs.c (new), attrprint.c (news), attributes.c (new)
#  Related: server, emulator, allocator, bootmesh, mkpart, showpart, lspart
#
# Revision 1.72  1994/05/24  16:46:44  dleslie
#  Reviewer: Shala Arshi
#  Risk: low
#  Benefit or PTS #: PTS 9436, strip OS commands
#  Testing: built all of commands
#  Module(s): Makeconf
# 		usr/ccs/lib/libc/Makefile
# 		usr/ccs/lib/libnx/Makefile
# 		usr/ccs/lib/libpm/Makefile
#
# Revision 1.71  1994/05/16  23:31:24  shala
#  Reviewer: NONE
#  Risk: Low
#  Benefit or PTS #: Get data.c from common_nx_c directory instead of libc.
#  Testing: Build libnx.a.
#  Module(s): cmds_libs/src/usr/ccs/lib/libnx/Makefile
#
# Since data.c is shared between libnx and libc, it is better to have it
# common_nx_c directory.
#
# Revision 1.70  1994/04/11  19:48:57  shala
# Needed to use a different version of data.c for NX application. The
# fix to bug #8577 caused NX application to get merged output to stderr
# from more than one node. Version of data() in libnx.a uses buffering.
# Ifdef NX is used in data.c to use buffering for libnx version and use non
# buffering for libc.
#
#  Risk: Low
#  Benefit or PTS #: Bug # 8901
#  Testing: Built libnx.a and libc.a and tested application.
#  Module(s): cmds_libs/src/usr/ccs/lib/libnx/Makefile
#             cmds_libs/src/usr/ccs/lib/libc/data.c
#
# Revision 1.69  1994/02/04  20:48:46  brad
# Modified extended math support so that the routines that actually
# implement the math are pulled in from the server/emulator source
# tree, rather than duplicating the same code in the libesize source.
# Also cleaned up so that common_nx_c and libesize are automatically
# built if necessary.
#  Reviewer: Shala Arshi, Dave Leslie
#  Risk: Low.
#  Benefit or PTS #:
#  Testing: Ran PFS EATs, ran emath tests.
#  Module(s): libnx/Makefile
#
# Revision 1.68  1994/01/04  23:16:57  dleslie
#  Reviewer: Shala Arshi
#  Risk: low
#  Benefit or PTS #: add a step to create a symbolic link in the obj
#                 tree at (for example) obj/ipsc860/usr/ccs/lib/libc.a pointing
#                 at libc/libc.a.  This is so that these libraries and .o files
#                 (crt0.o for example) can be found later on in the cmds/libs
#                 build process.
#  Testing: builds
#  Module(s): Makefile
#
# Revision 1.67  1993/12/20  19:42:04  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.66  1993/12/01  19:19:09  carbajal
# added hash.o to rmobjs
#  Reviewer: None
#  Risk: Low
#  Benefit or PTS #:
#  Testing:
#  Module(s):
#
# Revision 1.65  1993/11/18  19:24:31  dleslie
#  Reviewer: shala
#  Risk: low
#  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
# 	tree.  This allows users to build without having an obj tree fully
# 	populated with headers
#  Testing: built libnx
#  Module(s):
#     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
#     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
#     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
#     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
#     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
#     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
#     writepart.c
#
#
# VS:    writepart.c
#
# Revision 1.64  1993/11/17  18:45:42  carbajal
#  Reviewer: None
#  Risk: Low
#  Benefit or PTS #: added mypart_.o to rmobjs
#  Testing:
#  Module(s):
#
# Revision 1.63  1993/11/17  02:40:22  carbajal
#  Reviewer: None
#  Risk: Medium
#  Benefit or PTS #: R1.2 User Model Support
#  Testing:
#  Module(s):
#
# Revision 1.62  1993/11/10  02:06:55  carbajal
# Added support for R1.2 User Model allocator system calls
#
#  Reviewer: NONE
#  Risk: LOW
#  Benefit or PTS #: R1.2 User Model
#  Testing: developer
#  Module(s): allocsys.c, allocsys_.c, Makefile
#
# Revision 1.61  1993/10/26  00:39:42  carbajal
# Added bitmap2.o module
#
# Revision 1.60  1993/10/12  19:42:20  shala
# Added _mynode.o.
#
# Revision 1.59  1993/09/29  17:31:09  shala
# Moved execve.c to common_nx_c directory to be shared with libc.
# Find fork.o, forknx.o, execve.o, execvenx.o, and _numnodes.o in
# common_nx_c directory. That is where they get built.
#
# Revision 1.58  1993/09/28  23:32:49  shala
# Put myptype.c back under libnx directory instead of common_nx_c.
#
# Revision 1.57  1993/09/28  01:06:54  brad
# Moved extended math source files to new libesize directory.
#
# Revision 1.56  1993/09/21  20:24:03  shala
# Added _myptype.o, myptype.o, _dclock.o and dclock.o to CMN_NX_C_OBJS and
# NX_C_OBJS.
#
# Revision 1.55  1993/09/20  17:50:57  shala
# Removed dclock.o, _dclock.o and myptype.o. These modules are moved to
# common_nx_c directory. This is done to avoid linking libnx.a by default.
# myptype() is called from libpm library and dclock() is called from libXt.
# Fixed bug #5365.
#
# Revision 1.54  1993/09/09  22:02:16  andrews
# Added support for set_provide_threshold() call.
#
# Revision 1.53  1993/07/29  18:32:59  carbajal
# Added allocport.o
#
# Revision 1.52  1993/07/26  22:18:59  brad
# Added esub1().
#
# Revision 1.51  1993/07/13  21:44:55  carbajal
# Added create.o
#
# Revision 1.50  1993/06/29  15:20:40  shala
# Removed mach3 dependencies.
#
# Revision 1.49  1993/06/10  18:35:37  brad
# Added lestat().
#
# Revision 1.48  1993/05/26  17:37:10  carbajal
# Moved allocUser.c into libnx for use by partition mgmgt commands
#
# Revision 1.47  1993/05/24  18:26:47  terry
# added new file nx_lock.c to build proceedure.
#
# Revision 1.46  1993/05/11  21:09:05  carbajal
# Added allocsys.o and allocsys2.o
#
# Revision 1.45  1993/05/06  18:23:22  carbajal
# Added writepart.c to Makefile
#
# Revision 1.44  1993/04/02  22:41:23  brad
# Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
# Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
#
# Revision 1.38.2.6  1993/03/20  23:35:12  brad
# Added _cread, _cwrite, creadv, _creadv, creadv_, cwritev, _cwritev, cwritev_,
# ireadv, ireadv_, iwritev, and iwritev_.
#
# Revision 1.38.2.5  1993/02/13  01:04:19  dbm
# Added gopen_.c and fgopen.c for supporting Fortran gopen.
#
# Revision 1.38.2.4  1993/02/12  17:38:27  dbm
# Added gopen functionality.
#
# Revision 1.43  1993/02/12  00:11:13  dleslie
# Added mkidinfo version string stuff
#
# Revision 1.42  1993/01/28  00:06:27  dleslie
# Removed version.o
#
# Revision 1.41  1993/01/27  19:24:02  carbajal
# Added new module partprint.c to be used by lspart and
# showpart.
#
# Revision 1.40  1993/01/21  17:13:35  brad
# Added div.o.
#
# Revision 1.38.2.3  1993/01/21  17:14:03  brad
# Added div.o.
#
# Revision 1.38.2.2  1993/01/12  00:54:35  dbm
# Added getpfsinfo.c to libnx.
# 
# Revision 1.39  1993/01/11  16:37:02  shala
# Added nx_part_ops_.o .
#
# Revision 1.38.2.1  1993/01/08  23:34:41  dbm
# Added changes to support pfs files, added _pfs_setio.o to makefile.
#
# Revision 1.38  1992/12/27  00:25:19  shala
# Added version.o .
#
# Revision 1.37  1992/12/18  02:39:36  carbajal
# added nodeparser.o.
# Added TNC to CFLAGS so that SIGMIGRATE will be defined.
#
# Revision 1.36  1992/12/14  23:55:05  dleslie
# Added execve.o.  At Joel Clark's suggestion
#
# Revision 1.35  1992/12/11  22:54:22  shala
# Removed the DEBUG switch.
#
# Revision 1.34  1992/11/19  19:24:27  shala
# Add nx_part_ops_.o which defines user interface for more of the
# resource management routines.
#
# Revision 1.33  1992/11/03  00:17:35  carbajal
# Added new module partlock.o
#
# Revision 1.32  1992/10/28  18:46:34  regnier
# Remove incremental link of nx_port.c with libmach3.a. New compiler
# drivers will automatically search libmach3 for symbols (T6).
#
# Revision 1.31  1992/10/16  18:20:02  andrews
# Added NX switch for use with table.h include file (IPD support).
#
# Revision 1.30  1992/10/13  22:41:06  carbajal
# Added history line
# Added partutils.o, procinfo.o, parsesched.o and
# parsepart.o
#
#

NX_C_SRC	= ${MAKETOP}usr/ccs/lib/common_nx_c
VPATH		= ${TARGET_MACHINE}:${NX_C_SRC}

FC		= if77
FFLAGS		= -Mcoff -Y0,$(OSFTOP)/tools/ipsc860/hostbin \
		  -Ya,$(OSFTOP)/tools/ipsc860/hostbin -c  -O

OTHERS		= libnx.a
OBJECTS		= autoinit.o
autoinit.o_NOSTRIP =

EXPLIB_TARGETS	= export_libnx.a
EXPCRT0_TARGETS	= export_autoinit.o

ILIST		= libnx.a autoinit.o
IDIR		= /usr/ccs/lib/
IMODE		= 644

autoinit.o_IDIR =  /usr/ccs/lib/options/

INCFLAGS	= -I${TARGET_MACHINE}

CFLAGS		= -DCOMPAT_43 -DLANGUAGE_C -DLIBC_SCCS -DCUBE -DTNC -DNX \
		  -DPFS ${${TARGET_MACHINE}CFLAGS}

OFILES		= ${RM_OBJS} \
		  ${COMMON_OBJS} \
		  ${MP_C_OBJS} \
		  ${MP_F_OBJS} \
		  ${NX_C_OBJ} \
		  ${PFS_C_OBJS} \
		  ${PFS_F_OBJS} \
		  ${EX_OBJS} \
		  ${${TARGET_MACHINE}SOBJS} \
		  ${${TARGET_MACHINE}COBJS} \
		  ${${TARGET_MACHINE}TNCOBJS} 

GARBAGE		= *.S *.as tags

CONCUR_FLAG	= -DCONCUR

RM_OBJS		= nx_initve.o nx_loadve.o nx_load.o nx_nfork.o \
		  bitmap.o bitmap2.o nx_getopt.o utils.o \
		  nx_loadve_.o nx_load_.o nx_nfork_.o nx_initve_.o \
		  nx_pri_.o nx_pri.o nx_waitall.o nx_waitall_.o \
		  parsesched.o parsepart.o create.o partutils.o writepart.o \
		  partlock.o allocport.o allocsys.o allocsys2.o partprint.o \
		  procinfo.o allocUser.o allocsys_.o nodeparser.o \
		  nx_part_ops.o nx_part_ops_.o hash.o mypart.o mypart_.o \
		  nx_acctid.o nx_getaent.o attributes.o attrprint.o \
		  _copyargs.o tnc_subs.o

COMMON_DIR	= ../common_nx_c

NX_C_OBJ 	= data.o forknx.o

COMMON_OBJS	= nx_setup.o execl.o execle.o execlp.o execv.o execvp.o \
		  _myptype.o _mynode.o _dclock.o dclock.o __eseek.o _eseek.o \
		  _estat.o estat.o _festat.o festat.o _lestat.o lestat.o \
		  statpfs.o fstatpfs.o execve.o fork.o _numnodes.o \
		  execvenx.o

#
# Perform pattern replacement on the COMMON_OBJS macro to get a macro that
# references all the "source" object files in the common_nx_c directory. 
#
# NOTE: When we switch to pmake, vs. CMU make, the commented out construct
# (below) should be used, as it is the preferred method but is not supported
# by CMU make.  (And pmake does not support the CMU make construct.)
# 
#ORIG_COMMON_OBJS= ${COMMON_OBJS:%=${COMMON_DIR}/%}
ORIG_COMMON_OBJS= ${COMMON_OBJS/*/${COMMON_DIR}\\/&}

MP_C_OBJS	= _flick.o _gcol.o _gcolx.o _ginv.o _gops.o _gray.o _gsendx.o \
		  _gsync.o _mclock.o _nodedim.o _copyname.o \
		  _csendrecv.o _isendrecv.o \
		  nxlib.o rklib.o rkassert.o rkmem.o \
		  load.o _load.o \
		  killproc.o _killproc.o killcube.o _killcube.o \
		  cprobe.o crecv.o csend.o nx_perror.o dbglib.o \
		  csendrecv.o flick.o flushmsg.o gcol.o gcolx.o gdhigh.o \
		  gdlow.o gdprod.o gdsum.o giand.o gihigh.o gilow.o ginv.o \
		  gior.o giprod.o gisum.o gland.o glor.o gopf.o gray.o \
		  gsendx.o gshigh.o gslow.o gsprod.o gssum.o gsync.o \
		  hrecv.o hsend.o hsendrecv.o hwclock.o \
		  infocount.o infonode.o infopid.o infotype.o infoptype.o \
		  iprobe.o irecv.o isend.o isendrecv.o led.o masktrap.o \
		  mclock.o msgcancel.o msgdone.o msgwait.o \
		  myhost.o mynode.o mypid.o myptype.o nodedim.o numnodes.o \
		  nx_lock.o setptype.o msgignore.o msgmerge.o \
		  crecvx.o irecvx.o hrecvx.o hsendx.o cprobex.o iprobex.o \
		  _hsendrecv.o nx_port.o setpth.o perror.o


MP_F_OBJS	= dclock_.o gcol_.o gcolx_.o ginv_.o gray_.o \
		  gsendx_.o gsync_.o mclock_.o nodedim_.o cprobe_.o \
		  crecv_.o csend_.o csendrecv_.o flick_.o flushmsg_.o \
		  gdhigh_.o gdlow_.o gdprod_.o gdsum_.o giand_.o gihigh_.o \
		  gilow_.o gior_.o giprod_.o gisum_.o gland_.o glor_.o \
		  gopf_.o gshigh_.o gslow_.o gsprod_.o gssum_.o \
		  hrecv_.o hsend_.o hsendrecv_.o hwclock_.o \
		  infocount_.o infonode_.o infopid_.o infotype_.o \
		  nx_perror_.o infoptype_.o iprobe_.o irecv_.o \
		  isend_.o isendrecv_.o led_.o masktrap_.o mclock_.o \
		  msgcancel_.o msgdone_.o msgwait_.o \
		  myhost_.o mynode_.o mypid_.o myptype_.o nodedim_.o \
		  numnodes_.o setptype_.o msgignore_.o msgmerge_.o \
		  crecvx_.o irecvx_.o hrecvx_.o hsendx_.o \
		  cprobex_.o iprobex_.o \
		  load_.o killcube_.o killproc_.o setpth_.o

PFS_C_OBJS	= cread.o _cread.o creadv.o _creadv.o cwrite.o _cwrite.o \
		  cwritev.o _cwritev.o eseek.o esize.o _esize.o getpfsinfo.o \
		  gopen.o _gopen.o iodone.o iomode.o iowait.o \
		  iread.o ireadv.o iseof.o iwrite.o iwritev.o lsize.o \
		  restrictvol.o setiomode.o _setiomode.o _pfs_setio.o

PFS_F_OBJS	= cread_.o creadv_.o cwrite_.o cwritev_.o eseek_.o esize_.o \
		  gopen_.o fcntl_.o fgopen.o iodone_.o iomode_.o iowait_.o \
		  iread_.o ireadv_.o iseof_.o iwrite_.o iwritev_.o lseek_.o \
		  lsize_.o restrictvol_.o setiomode_.o

EX_DIR		= ../libesize

EX_C_OBJS	= _eadd.o eadd.o _eadd1.o eadd1.o _ecmp.o ecmp.o \
		  _ediv.o ediv.o _emod.o emod.o _emul.o emul.o \
		  _esub.o esub.o _esub1.o esub1.o _etos.o etos.o \
		  _stoe.o stoe.o pfs_emath.o

EX_F_OBJS	= eadd_.o eadd1_.o ecmp_.o ediv_.o emod_.o emul_.o \
		  esub_.o esub1_.o etos_.o stoe_.o

EX_S_OBJS	= mul.o

EX_OBJS		= ${EX_C_OBJS} \
		  ${EX_F_OBJS} \
		  ${EX_S_OBJS}

#
# Perform pattern replacement on the EX_OBJS macro to get a macro that
# references all the "source" object files in the libesize directory. 
#
# NOTE: When we switch to pmake, vs. CMU make, the commented out construct
# (below) should be used, as it is the preferred method but is not supported
# by CMU make.  (And pmake does not support the CMU make construct.)
# 
#ORIG_EX_OBJS	= ${EX_OBJS:%=${EX_DIR}/%}
ORIG_EX_OBJS	= ${EX_OBJS/*/${EX_DIR}\\/&}

-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c

# i860 COFF doesn't require lorder ...
#COFF_LORDER=`lorder *.o | tsort`
COFF_LORDER=`ls *.o`
A_OUT_LORDER=${OFILES}
MACHO_LORDER=${OFILES}


libnx.a: libnx.a(${OFILES})
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${AR} ${DEF_ARFLAGS} $@ _cvs_id.o $?
	${RM} -rf tmp 
	mkdir tmp
	cd tmp && { \
		${AR} x ../libnx.a; ${RM} -f __.SYMDEF __________ELELX; \
		${AR} cr libnx.a ${${OBJECT_FORMAT}_LORDER}; }
	${MV} -f tmp/libnx.a .
	${RM} -rf tmp
	${RANLIB} $@
	${RM} -f $? _cvs_id.c _cvs_id.o ../libnx.a
	ln -s libnx/libnx.a ../libnx.a

${COMMON_OBJS}: ${COMMON_DIR}/$$@
	@cd ${COMMON_DIR}; ${MAKE} ${MFLAGS} all
	for i in ${COMMON_OBJS}; do cp ${COMMON_DIR}/$$i .; done

${ORIG_COMMON_OBJS}:
	@cd ${COMMON_DIR}; ${MAKE} ${MFLAGS} all

${EX_OBJS}: ${EX_DIR}/$$@
	@cd ${EX_DIR}; ${MAKE} ${MFLAGS} all
	for i in ${EX_OBJS}; do cp ${EX_DIR}/$$i .; done

${ORIG_EX_OBJS}:
	@cd ${EX_DIR}; ${MAKE} ${MFLAGS} all

include ${MAKEFILEPATH}/depend.mk
-include Makedep
