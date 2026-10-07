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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0
#
# template.mk
#	This is a template for an OSF/1 Makefile.
#	It is divided into three parts:
#	  Makefiles with sub-directories
#	  Makefiles with targets
#	  A copy of the template without comments to actually use in
#	    creating a new Makefile

#===========================================================================
# This section for Makefiles with sub-directories
# 
#	'SUBDIRS' lists the sub-directories of the current directory that
#	build/make cd's into and recursively invokes `make`. Exactly when 
#	build/make 'builds' these sub-directories is not defined.

SUBDIRS			= include ccs lib bin sbin dict local

#	If any of the sub-directories have header files, "file.h", which
#	need to be exported, these sub-directories need to be listed in
#	an explicit sub-directory variable:
# EXPINC_SUBDIRS	= include

#	If any of the sub-directories have libraries, "lib??.a", which
#	need to be exported, these sub-directories need to be listed in
#	an explicit sub-directory variable:
# EXPLIB_SUBDIRS	= ccs

#	There are two common makefiles which must be included in all
#	Makefiles which have subdirs.  They are:

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk


#===========================================================================
# This section is for Makefiles with targets.

#	If there is an additional place to search for the files which
#	are going to be used in the rest of the Makefile, for example
#	a machine dependent directory or kernel header files, set VPATH.
# VPATH			= ${MAKETOP}/kernel/sys
# VPATH			= ${TARGET_MACHINE}


# -------- listing targets -------
#	Because common makefiles are used, most "targets" turn out
#	to be lists assigned to appropriate variables.
#	The variables usually begin with the list of items to build.
#	In the case of .c files, these are listed under PROGRAMS
# PROGRAMS 		= ar cb mkstr nm size strings tsort what xstr

#	libraries under:
# LIBRARIES 		= libsb libcurses

#	objects under (when they differ from PROGRAMS):
# OBJECTS 		=  LR0.o allocate.o closure.o conflicts.o derives.o

#	scripts are listed under:
# SCRIPTS		= cflow lorder mig ranlib

#	data files (often these are headers):
# DATAFILES		= audit.h auditsysc.h auxv.h bkmac.h buf.h

#	other files:
# OTHERS		= mach_debug_server.c mach_debug_user.c mach_debug.h

#	man pages under:
# MANPAGES		= bci blog mklinks bco bmerge mksb

#	document files under:
# DOCUMENTS		= lpr

#	message headers under:
# MSGHDRS		= cb_msg.h cflow_msg.h xstr_msg.h ar_msg.h nm_msg.h

#	catalog files under:
# CATFILES		= cb.cat cflow.cat xstr.cat ar.cat nm.cat size.cat


# -------- header file and library search paths -------
#	If necessary, expand the search paths for header files and libraries:
# INCFLAGS		= -I.. -I../..
# LIBFLAGS		= -L/usr/lib -L/lib


# -------- exporting and installing -------
#	If there are files to be exported, they should be listed.  Header
#	files are usually listed under INCLUDES and libraries under
#	the already defined heading LIBRARIES:
# INCLUDES		= ${DATAFILES} ${OTHERS}

#	If there is no existing list of INCLUDES and LIBRARIES, the following
#	two variables can be substituted for them:
# EXPINC_TARGETS	= export_ar_msg.h
# EXPLIB_TARGETS	= export_libc.a

#	The directory which the files to be are to be placed in is designated
#	by the EXPDIR variable (NOTE: this variable is not needed if the
#	IDIR variable is used and set to the same value.)
# EXPDIR		= /usr/ccs/lib

#	If there are files to be installed, the installation list, the
#	directory to install to, the mode, owner, and group are set:
#	The defaults are owner = bin; group = bin; mode = 755.
# ILIST			=  authorize defaultdb devassign files lps
# IDIR			= /etc/auth/system
# IOWNER		= root
# IGROUP		= system
# IMODE			= 644

#	A common form needed for setting the install directory, modes,
#	owner, etc for a single file is file_TARGET.  For example:
# file_IDIR		=
# file_IOWNER		=
# file_IGROUP		=
# file_IMODE		=

#	If links are to be created at install time, use ILINKS:
# ILINKS		= ${IDIR}uncompress ${IDIR}zcat
# file_ILINKS		= 

#	To make have file installed without being stripped, use
# NOSTRIP		=	# does not take an argument


# -------- optimization -------
#	To set the level of optimization for the compiler and loader to
#	something other than the default, -O:
# OPT_LEVEL		= -g

#	The optimization level can be to just the compiler with:
# CC_OPT_LEVEL		=


# -------- flags -------
#	The following 'flag' variables are provided. Use these variables
#	rather than hard coding the options into the target's action
#	line:  CFLAGS YFLAGS LDFLAGS LINTFLAGS PFLAGS SED_OPTIONS 
# CFLAGS 		= -DSEC_ACL_SWARE -DSEC_MAC_OB -DSEC_PRIV
# YFLAGS		= -d
# LDFLAGS		= -u _sbrk
# LINTFLAGS		= -hbacvx

#	or for a specific file use the <file>_FLAG notation. For example:
# file_CFLAGS 		= -DSEC_ILB ${CFLAGS}

#	To set the compiler type to something other than "ansi", the
#	default, use CCTYPE.  Possible values are "writable_strings",
#	"host", and "traditional".
# CCTYPE		=
# file_CCTYPE		=

# -------- include headers -------
#	List the header files the object files depend on.
# HFILES		= debug.h dump.h logvol.h lvmcmds.h lvmdefmsg.h


# -------- link with libraries -------
# 	List the libraries the programs need to link with.
# LIBS			= -lplot -lm
# file_LIBS		=


# -------- build with objects -------
# 	List the object files. Defaults to ${PROGRAMS} with .o appended to
#	each file.
# OFILES		= init.o getcmd.o signals.o output.o

#	For machine dependent objects, a line like is needed:
# OFILES		= init.o getcmd.o ${${TARGET_MACHINE}_OFILES}

#	For object files for a particular file only:
# file_OFILES		=


# -------- misc -------
#	Other generic list are:
# GARBAGE	=	# List the files to remove when a `make` finishes.
# LINTFILES	=	# List the files to lint. Defaults to all .o files.
# CLEANFILES	=	# List the files to remove when a `make clean` is
#			# done. Defaults

#	The following 'program' variables are provided. They normally are
#	left as defaults.  If they are modified, please use these variables
#	rather than hard coding the program to run:   AR AS CHMOD CP ECHO LD
#	LINT MV NROFF PC RANLIB RM SED SORT TR YACC

#	There are a number of variable used for MIG including: MIGFLAGS,
#	MIG_HDRS, and MIG_DEFS.  See the documentation for details.

# -------- common makefiles -------
#  	If the component this Makefile builds has machine dependencies,
#	uncomment the following line: (Be sure the file exists)
# include ${TARGET_MACHINE}/machdep.mk

#
#	Uncomment the necessary common makefiles.
#	(Uncomment objects.mk when using PROGRAMS or LIBRARIES.)
#
#	Makefile			# Feature Provided
#	--------			------------------

include ${MAKEFILEPATH}/standard.mk	# Always include!

# include ${MAKEFILEPATH}/programs.mk	# Program creation
# include ${MAKEFILEPATH}/libs.mk	# Library creation
# include ${MAKEFILEPATH}/objects.mk	# OBJECTS
# include ${MAKEFILEPATH}/scripts.mk	# SCRIPTS
# include ${MAKEFILEPATH}/manpages.mk	# MANSECTION
# include ${MAKEFILEPATH}/datafiles.mk	# .txt, .
# include ${MAKEFILEPATH}/others.mk	# Anything else


# -------- user area -------
#
#	Begin user targets here, for example:
# dirs: dirs.h

#	Note: use ${ALWAYS} to force the target to be always be built.
#	For example:
# dirs: ${ALWAYS}


# To create a real Makefile, cut here
==========================  cut  ============================================

# This section for Makefiles with sub-directories

SUBDIRS			=
# EXPINC_SUBDIRS	=
# EXPLIB_SUBDIRS	=

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk

# This section is for Makefiles with targets.
# VPATH			=

# PROGRAMS 		=
# LIBRARIES 		=
# OBJECTS 		=
# SCRIPTS		=
# DATAFILES		=
# OTHERS		=
# MANPAGES		=
# DOCUMENTS		=
# MSGHDRS		=
# CATFILES		=

# INCFLAGS		=
# LIBFLAGS		=

# INCLUDES		=
# EXPINC_TARGETS	=
# EXPLIB_TARGETS	=
# EXPDIR		=
# ILIST			=
# IDIR			=
# IOWNER		=
# IGROUP		=
# IMODE			=
# file_IDIR		=
# file_IOWNER		=
# file_IGROUP		=
# file_IMODE		=
# ILINKS		=
# file_ILINKS		=
# NOSTRIP		=

# OPT_LEVEL		=
# CC_OPT_LEVEL		=

# CFLAGS 		=
# file_CFLAGS 		=
# YFLAGS		=
# LDFLAGS		=
# LINTFLAGS		=
# CCTYPE		=
# file_CCTYPE		=
# HFILES		=

# LIBS			=
# file_LIBS		=
# OFILES		=
# file_OFILES		=

# GARBAGE		=
# LINTFILES		=
# CLEANFILES		=

include ${MAKEFILEPATH}/standard.mk
# include ${TARGET_MACHINE}/machdep.mk
# include ${MAKEFILEPATH}/programs.mk
# include ${MAKEFILEPATH}/libs.mk
# include ${MAKEFILEPATH}/objects.mk
# include ${MAKEFILEPATH}/scripts.mk
# include ${MAKEFILEPATH}/manpages.mk
# include ${MAKEFILEPATH}/datafiles.mk
# include ${MAKEFILEPATH}/others.mk
