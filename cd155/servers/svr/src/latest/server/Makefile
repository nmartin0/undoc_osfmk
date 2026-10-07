# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.8  1995/02/27  22:29:37  yazz
#  Reviewer: Brad Figg, Suri Brahmaroutu
#  Risk: Lo
#  Benefit or PTS #: #12496
#  Testing: All configs built & rebuilt; needless rebuild steps now skipped.
#  Module(s):
# 	svr/server/Makefile
# 	svr/server/conf/makesyscalls.sh
#
# Revision 1.7  1994/11/18  20:26:09  mtm
# Copyright additions/changes
#
# Revision 1.6  1994/04/28  19:00:04  chrisp
# Invocations of the tncgen steps performed prior to descent into
# the constituent server subdirectories.  Change "lint:" target to
# go through make script for correct PATH environment.
#
#  Reviewer: dleslie, cfj
#  Risk: M
#  Benefit or PTS #: 9188
#  Testing: Builds, builds, builds.
#  Module(s): Makefile
#
# Revision 1.5  1994/01/03  20:46:23  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: allow 'lint' to be run on individual files or collections
# 	of files
#  Testing: built, ran 'lint' successfully
#  Module(s): Makefile
#
# Revision 1.4  1993/07/14  17:34:51  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  18:27:03  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:01:07  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:19:31  cfj
# Initial 1.0.3 code drop
#
# Revision 1.2  1992/11/30  22:10:37  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:14:14  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.10  1993/04/08  11:18:51  loverso
# 	Pass -f to csh scripts so that .cshrc is not read again.
#
# Revision 2.9  92/06/08  18:18:03  pjg
# 	Fixed so it builds server_build.h (rabii)
# 
# Revision 2.8  92/05/24  14:10:55  pjg
# 	removed -p from cp Makefile $@,  per Grenoble 3.6 release
# 	[92/05/20            srl]
# 
# Revision 2.7  91/12/18  12:45:13  roy
# 	91/11/15  15:31:25  condict
# 	Copy the machine-dependent header files from the server/${target_cpu}
# 	src dir to the server/machine object dir.
# 
# 	91/11/07  15:15:03  condict
# 	Making of server include files is now done with simple recursive call 
# 	to make, instead of bizarre shenanigans.
# 
# 	91/10/02  10:50:18  condict
# 	Change unconditional reference to src/latest to use BCSBBASE instead.
# 
# 	91/09/06  13:48:22  condict
# 	Trivial changes to allow building under OSF/1.
# 
# Revision 2.6  91/11/15  17:31:05  rabii
# 	Added i860 support
# 
# Revision 2.5  91/10/16  11:38:01  roy
# Deleted "+second" from default CONFIG (now part of STD def.)
# 
# Revision 2.4  91/10/14  16:47:25  roy
# For syscalls target, make 'include' dir automatically.
# 
# Revision 2.3  91/10/08  11:33:42  rabii
# 	Added paul roy's changes to make shadowing work
# 
# Revision 2.2  91/08/30  16:41:10  rabii
# 	Initial V2 Checkin
# 
# Revision 3.6  91/08/30  10:28:27  condict
# Change make built-in var TARGET_MACHINE to TARGET_CPU, allowing cross-compiles.
# 
# Revision 3.5  91/08/28  10:53:32  barbou
# Store $CONFIG in vers.config (for upgrade to OSF/1.0.2).
# 
# Revision 3.4  91/08/27  15:26:00  barbou
# Upgrade to UX26.
# 
# Revision 3.3  91/07/30  15:11:04  jose
# added CONFIG as an argument for lower level Makefile
# 
# Revision 3.2  91/04/12  13:44:52  condict
# Add target and actions to automatically generate syscalls.c, init_sysent.c,
# and syscall.h from server/conf/syscalls.master, as in OSF/1
# 
# Revision 3.1  91/03/13  09:16:04  condict
# Modified to work with the OSF/1 header files
# 
# Revision 3.0  91/01/17  12:07:40  condict
# Unchanged copy from Mach 3.0 BSD UNIX server
# 
# Revision 2.6  91/01/08  17:44:07  rpd
# 	Allow "server" as a target to make the "kernel"
# 	[91/01/04  11:45:47  rvb]
# 
# Revision 2.5  90/11/05  15:32:47  rpd
# 	Change MACHINE to TARGET_MACHINE.
# 	[90/10/30            rwd]
# 
# Revision 2.4  90/10/25  15:06:50  rwd
# 	Add references for i386 the same as at386.
# 	[90/10/24            rwd]
# 
# Revision 2.3  90/05/21  13:49:02  dbg
# 	Added AT386.
# 	[90/03/14            dbg]
# 
# Revision 2.2  89/11/29  15:27:07  af
# 	Added pmax.
# 	[89/11/28  14:07:01  af]
# 
# Revision 2.1  89/08/04  15:22:44  rwd
# Created.
# 
# Revision 2.27  89/04/22  15:12:18  gm0w
# 	Changed commands to create the "make" script for the build area
# 	into a sed of "conf/make.template".  Moved commands to create
# 	configuration for build into "conf/mkconfig.csh".
# 	[89/04/14            gm0w]
# 
# Revision 2.26  89/04/18  16:41:50  mwyoung
# 	Allow additional flags (e.g., "-p") to be passed to config.
# 	[89/01/05            mwyoung]
# 
# Revision 2.18.3.1  89/03/27  22:55:09  mwyoung
# 	Allow additional flags (e.g., "-p") to be passed to config.
# 	[89/01/05            mwyoung]
# 
# Revision 2.25  89/03/07  00:48:53  rpd
# 	Removed ../sys from the CPATH used in the build directory.
# 
# Revision 2.24  89/02/25  14:12:36  gm0w
# 	Changes for cleanup.
# 
# Revision 2.23  89/02/25  14:07:41  gm0w
# 	Changes for cleanup.
# 
# Revision 2.22  89/01/31  01:19:58  rpd
# 	Support multimax.
# 	[89/01/24            dlb]
# 
# Revision 2.21  89/01/23  22:01:13  af
# 	Changes for MIPS and I386: <foo>_cpu, <FOO>_OUTPUT, and FEATURES_<FOO>
# 	[89/01/09            rvb]
# 
# Revision 2.20  88/12/22  11:52:53  mja
# 	Remove Makefile.top-level before copying to it in case it may
# 	already exist non-writeable.
# 
# Revision 2.19  88/12/20  13:38:27  rpd
# 	Fixed the log (??).
# 	[88/11/26  21:13:27  rpd]
# 
# Revision 2.18  88/10/18  03:13:20  mwyoung
# 	Make it possible to invoke this Makefile using the SOURCE_DIR
# 	technology -- getting Makefile from ".." only worked because
# 	VPATH was being used.
# 	[88/10/10            mwyoung]
# 
# Revision 2.17  88/09/27  17:29:50  rvb
# 	Fixed Log
# 
# Revision 2.16  88/09/27  16:05:59  rvb
# 	Add new pmake target and PMAKE variable and link pmake to make
# 	in the build directory.
# 	[88/09/25            rvb]
# 
# Revision 2.15  88/09/07  15:44:57  rpd
# 	Moved CONFIG macros to Makeconf, so that the user
# 	can easily customize them by modifying Makeconf.
# 	[88/09/07  01:53:58  rpd]
# 	
# 	Removed redundant definition of CONFIG_SUFFIX as empty string.
# 	This allows a definition to go into Makeconf if desired.
# 	[88/09/05  23:46:20  rpd]
# 	
# 	Eliminated a dependency on ${CONFIG}.
# 	Use dependencies to create ${CONFIG}/Makeconf.
# 	[88/09/05  21:41:22  rpd]
# 	
# 	Fixed typo.
# 	[88/09/05  20:51:19  rpd]
# 	
# 	Removed dependencies on directories; they cause to many recompilations.
# 	Put VPATH and OBJECTDIR values into the make script when it is generated.
# 	Moved from NO_SOURCE_DIR to USE_SOURCE_DIR.
# 	[88/09/05  20:30:48  rpd]
# 	
# 	Fixed up some more.  Now VPATH's value isn't wired-in to the script when
# 	it is created.
# 	[88/09/02  18:15:05  rpd]
# 	
# 	Several changes with the following goals:
# 	 1) The object directory can be a subdirectory of the source directory.
# 	 Solution: create an empty Makeconf file in the object directory, so
# 	 that makes in the object directory don't get confused by a Makeconf
# 	 file that might be in the source directory.
# 	
# 	 2) The generated make script in the object directory now properly handles
# 	 passed-in VPATHs with zero, one, or more components.  When possible, it
# 	 tries to use SOURCE_DIR instead of VPATH.
# 	[88/09/02  14:04:08  rpd]
# 
# Revision 2.14  88/09/01  15:52:59  mwyoung
# 	Don't use the '-N' switch, as it turns off OBJECTDIR processing
# 	as well as Makeconf searching.  Sigh.
# 	[88/08/30            mwyoung]
# 
# Revision 2.13  88/08/30  17:22:42  mwyoung
# 	Remove occurances of string "make" from the "./make" script.
# 	[88/08/30            mwyoung]
# 
# Revision 2.12  88/08/30  00:12:25  mwyoung
# 	When building the "make" script in the build directory, don't
# 	precompute the location of the appropriate "make" program.
# 	Computing it at runtime allows for other "make" programs (e.g.,
# 	"pmake") and allows compilation to be done on more than one
# 	different machine.
# 	
# 	Move the machine-specific top-level Makefile into the
# 	machine-specific directory.
# 	[88/08/28            mwyoung]
# 	
# 	Eliminate make dependencies on ${CONFIG}... create the directory
# 	if required in the rules for a particular object.  [Otherwise, the
# 	directory modification time changes, causing things to rebuild.]
# 	[88/08/27            mwyoung]
# 
# Revision 2.11  88/08/25  19:17:44  mwyoung
# 	If the incoming VPATH is simple, pass a value in for SOURCE_DIR
# 	to reduce the number of wasted name lookups.
# 	[88/08/21            mwyoung]
# 	
# 	If the incoming VPATH is simple, pass a value in for SOURCE_DIR
# 	to reduce the number of wasted name lookups.
# 	[88/08/25  19:28:56  mwyoung]
# 
# Revision 2.10  88/08/25  18:39:56  mwyoung
# 	Added some dependencies, so that "make CONFIG=foo config" works
# 	when configuration foo doesn't have a build directory.
# 	[88/08/15  23:14:58  rpd]
# 
# Revision 2.9  88/08/06  17:57:50  rpd
# Changed the generated make script: MAKECONF should be MAKEDIR;
# do more with CPATH now.
# 
# Revision 2.8  88/07/29  03:17:03  rpd
# Removed dependencies upon Makeconf, so that the build directories
# can be subdirectories of the source directory.
# 
# Revision 2.7  88/07/21  16:25:33  rpd
# Include machine-dependent Makefile.  Slight change to config rule,
# to allow the machine-dependent Makefile to specify further actions.
# 
# Revision 2.6  88/07/19  20:40:08  mwyoung
# Always rebuild the config program... it would only get rebuilt if
# the binary didn't exist.
# Attempt to build <sys/features.h> after configuring.  If
# the set of files used to build the features file changes, then
# it will have to be rerun manually because I have no interest
# in figuring out how to get make to do it for me.
# 
# Revision 2.5  88/07/18  16:50:00  mwyoung
# Fix configuration_file-building script.
# 
# Allow "doconf" or "config" targets to be made before
# a full "make" has taken place.
# 
# Revision 2.4  88/07/16  01:04:08  mwyoung
# Resurrect MASTER.local files.
# 
# Revision 2.3  88/07/15  18:19:26  mja
# Created.
# 

#
#	Targets of interest:
#		doconf		Rebuild the configuration file
#		config		Reconfigure
#		kernel		Rebuild just the kernel
#		server		Rebuild just the kernel
#		pmake		Rebuild just the kernel using pmake
#		buildconf	Build all of the "standard" configurations
#

VAX_cpu=vax
IBMRT_cpu=ca
SUN3_cpu=sun3
MMAX_cpu=mmax
PMAX_cpu=mips
AT386_cpu=i386
I386_cpu=i386
I860_cpu=i860
IPSC860_cpu=i860
cpu=$(${TARGET_CPU}_cpu)

VAX_OUTPUT=Makefile
SUN3_OUTPUT=Makefile
IBMRT_OUTPUT=Makefile
MMAX_OUTPUT=Makefile
PMAX_OUTPUT=Makefile
AT386_OUTPUT=Makefile
I386_OUTPUT=Makefile
I860_OUTPUT=Makefile
IPSC860_OUTPUT=Makefile

# Specify which configuration of OSF/1 to build, by default:
VAX_CONFIG=STD+WS
SUN3_CONFIG=STD+WS
PMAX_CONFIG=STD+WS
I386_CONFIG=STD+WS
AT386_CONFIG=STD+WS
I860_CONFIG=STD+WS
IPSC860_CONFIG=STD+WS
CONFIG = ${CONFIG?${CONFIG}:$(${TARGET_CPU}_CONFIG)}

CONFIG_INPUT=${CONFIG}/${CONFIG}
CONFIG_OUTPUT=${CONFIG}/$(${TARGET_CPU}_OUTPUT)

MASTER_DIR=conf
MASTER=${MASTER_DIR}/MASTER
MASTER_CPU=${MASTER}.${cpu}
MASTER_LOCAL=${MASTER_DIR}/MASTER.local
MASTER_CPU_LOCAL=${MASTER}.${cpu}.local

SYSCALLS=${CONFIG}/init_sysent.c ${CONFIG}/syscalls.c include/sys/syscall.h

MAKE_ARGS=
PMAKE_ARGS= -k -P6 "P=&" all

SOURCE_DIR=..

${CONFIG}.all:	${CONFIG}/make \
		${CONFIG}/Makeconf \
		${CONFIG}.doconf \
		usr_include_files \
		machine_include_files \
		${CONFIG}.config \
		TNC_generated_files \
		${CONFIG}.make

${CONFIG}.make kernel server:	${CONFIG}/make ${CONFIG}/Makeconf
	@echo "[ Making ${CONFIG} ]"
	@cd ${CONFIG} && USE_SOURCE_DIR=1 csh -f ./make ${MAKE_ARGS} CONFIG=${CONFIG}

TNC_generated_files:	${CONFIG}/make ${CONFIG}/Makeconf ${CONFIG}/Makefile \
			vproc/vprocgen.mk tnc/tncgen.mk
	@echo "[ Generating ${CONFIG}/vproc ]"
	@grep "^IDENT=" ${CONFIG}/Makefile | \
		cat - vproc/vprocgen.mk >${CONFIG}/vprocgen.mk
	@cd ${CONFIG} && SOURCE_DIR=$(SOURCE_DIR) \
		${MAKE} -f vprocgen.mk CONFIG=${CONFIG}
	@-if `grep -s "^IDENT=.*-DTNC" ${CONFIG}/Makefile`; then \
		echo "[ Generating ${CONFIG}/tnc ]"; \
		grep "^IDENT=" ${CONFIG}/Makefile | \
			cat - tnc/tncgen.mk >${CONFIG}/tncgen.mk; \
		cd ${CONFIG} && SOURCE_DIR=$(SOURCE_DIR) \
			${MAKE} -f tncgen.mk CONFIG=${CONFIG}; \
	else \
		true; \
	fi

usr_include_files: ./include ./include/sys
	@echo "[ Making server/include ]"
	@cd include; ${MAKE}

machine_include_files: ./machine
	@echo "[ Making server/machine ]"
	@cd machine; ${MAKE}

pmake:	${CONFIG}/make ${CONFIG}/Makeconf
	@echo "[ Pmaking ${CONFIG} ]"
	@cd ${CONFIG} && USE_SOURCE_DIR=1 csh -f ./pmake ${PMAKE_ARGS}

clean:	${CONFIG}/make ${CONFIG}/Makeconf
	@echo "[ Cleaning ${CONFIG} ]"
	@cd ${CONFIG} && USE_SOURCE_DIR=1 csh -f ./make ${MAKE_ARGS} clean

${CONFIG}.doconf doconf: ${CONFIG_INPUT}

${CONFIG}.config config::	${CONFIG}/make \
				${CONFIG}/Makeconf \
				${CONFIG}/Makefile.top-level \
				./bin/config \
				${CONFIG_INPUT} \
				./include/sys/server_build.h \
				${SYSCALLS}
	@cd ${CONFIG} && \
         csh -f ./make CONFIG=${CONFIG} \
		CONFIG_SUFFIX=${CONFIG_SUFFIX} \
		CONFIG_FLAGS=${CONFIG_FLAGS} \
		-f Makefile.top-level \
		make.config

./bin ../bin ./src ./src/config ./include ./include/sys ./machine: 
	mkdir $@

./bin/config: ./bin ./src ./src/config ALWAYS
	@echo "[ Generating $@ ]"
	@S=src/config; cd $$S && SRCSUFFIX=$$S ${MAKE} ../../bin/config 

./bin/buildconf: ./bin ./src
	@echo "[ Generating $@ ]"
	@S=src; cd $$S && SRCSUFFIX=$$S ${MAKE} ../bin/buildconf

${CONFIG}/Makeconf:
	@echo "[ Generating $@ ]"
	@-> $@

${CONFIG}/Makefile.top-level:	Makefile
	@echo "[ Generating $@ ]"
	@rm -f $@; cp  Makefile $@

${CONFIG}/make:	Makefile conf/make.template
	@echo "[ Generating $@ ]"
	@-if [ -d ${CONFIG} ]; then true; else mkdir ${CONFIG}; fi
	@sed -e 's;@OBJECTDIR@;${OBJECTDIR};g' -e 's;@VPATH@;${VPATH};g' \
	 < conf/make.template > $@.tmp; \
	 chmod +x $@.tmp; \
         mv $@.tmp $@; \
	 rm -f ${CONFIG}/pmake; \
	 ln $@ ${CONFIG}/pmake

${CONFIG_INPUT}:	${MASTER} ${MASTER_CPU} \
			${MASTER_LOCAL} ${MASTER_CPU_LOCAL} \
			Makefile conf/mkconfig.csh
	@echo "[ Generating $@ from ${MASTER_DIR}/MASTER{,.${cpu}}{,.local} ]"
	@-if [ -d ${CONFIG} ]; then true; else mkdir ${CONFIG}; fi
	@csh -f conf/mkconfig.csh ${CONFIG} ${MASTER} ${MASTER_CPU} ${MASTER_LOCAL} ${MASTER_CPU_LOCAL} >${CONFIG_INPUT}.tmp
	@-if [ -s ${CONFIG_INPUT}.tmp ]; then \
	    if [ -f ${CONFIG_INPUT} ]; then \
		diff ${CONFIG_INPUT} ${CONFIG_INPUT}.tmp; \
		mv ${CONFIG_INPUT} ${CONFIG_INPUT}.old; \
	    fi; \
	    mv ${CONFIG_INPUT}.tmp ${CONFIG_INPUT}; \
	else \
	    rm -f ${CONFIG_INPUT}.tmp; \
	fi

./include/sys/server_build.h: conf/version.build
	@echo "[ Generating $@ from ${MASTER_DIR}/version.build ]"
	@echo "#define SERVER_BUILD \"`cat conf/version.build`\"" > ./include/sys/server_build.h

# Generate the files that depend on the list of system calls and their
# attributes.  Avoid updating files unnecessarily, to prevent useless
# rebuilding when building differing configurations.  Do not have the
# generated syscalls file depend on directories.
#
${SYSCALLS}:	conf/syscalls.master conf/makesyscalls.sh
	@echo "[ Generating ${SYSCALLS} from conf/syscalls.master ]"
	@-if [ -d include ] ; then true; else mkdir include; fi
	@-if [ -d include/sys ] ; then true; else mkdir include/sys; fi
	@sh conf/makesyscalls.sh -hk conf/syscalls.master ${CONFIG}

make.config:	../${CONFIG_OUTPUT}${CONFIG_SUFFIX}

../${CONFIG_OUTPUT}${CONFIG_SUFFIX}:	\
		../${CONFIG_INPUT} \
		$(SOURCE_DIR)/conf/files \
		$(SOURCE_DIR)/conf/files.${cpu} \
		$(SOURCE_DIR)/conf/Makefile.template \
		$(SOURCE_DIR)/conf/Makefile.${cpu} \
		./Makefile.top-level
	@echo "[ Configuring in ${CONFIG} ]"
	../bin/config -c $(SOURCE_DIR)/conf ${CONFIG_FLAGS} ${CONFIG}
	@echo ${CONFIG} >../${CONFIG}/vers.config
	@-if [ -d sys ] ; then true; else mkdir sys; fi

buildconf: ./bin/buildconf conf/buildconf.${cpu}
	@-if [ "_${BUILDCONF}_" = "__" ]; then \
	      argv=`sed <conf/buildconf.${cpu} -e "/^#/d"`; \
	  else \
	      argv=${BUILDCONF}; \
	  fi; \
	exec ./bin/buildconf ${OPTIONS} $$argv

lint:
	cd ${CONFIG} && USE_SOURCE_DIR=1 csh -f ./make lint


ALWAYS:

-include ${cpu}/Makefile.top-level
# To lint individual files, or groups of files:
#	make LINTFILES="tnc/tnc_mig.c pfs/pfs_vfsops.c" lint
#	make LINTFILES=nx/nx_mig.c lint
.EXPORT: LINTFILES
