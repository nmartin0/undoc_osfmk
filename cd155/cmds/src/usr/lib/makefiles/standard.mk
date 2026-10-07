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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2

#
#  Default rule - All other rules appear after variable definitions
#
_default_: build_all;@

#
#  Debugging entry for checking environment
#
print_env:
	@printenv

#
#  Use this as a dependency for any rule which should always be triggered
#  (e.g. recursive makes).
#
ALWAYS=_ALWAYS_

#
#  Shortened for macro definitions, not to be used within a Makefile.
#
_T_M_=${TARGET_MACHINE}

#
#  Definitions for object file format - A_OUT, COFF or MACHO
#
_T_M_OBJECT_FORMAT_=${${_T_M_}_OBJECT_FORMAT?${${_T_M_}_OBJECT_FORMAT}:MACHO}
OBJECT_FORMAT=${OBJECT_FORMAT?${OBJECT_FORMAT}:${_T_M_OBJECT_FORMAT_}}

#
#  Definitions for archive file format - ASCARCH or COFF
#
_T_M_ARCHIVE_FORMAT_=${${_T_M_}_ARCHIVE_FORMAT?${${_T_M_}_ARCHIVE_FORMAT}:COFF}
ARCHIVE_FORMAT=${ARCHIVE_FORMAT?${ARCHIVE_FORMAT}:${_T_M_ARCHIVE_FORMAT_}}

#
#  Set defaults for input variables which are not already defined
#
DEF_RMFLAGS=${DEF_RMFLAGS?${DEF_RMFLAGS}:-f}
DEF_ARFLAGS=${DEF_ARFLAGS?${DEF_ARFLAGS}:cr}
DEF_MDFLAGS=${DEF_MDFLAGS?${DEF_MDFLAGS}:-d}

ROOT_OWNER=${ROOT_OWNER?${ROOT_OWNER}:root}
KMEM_GROUP=${KMEM_GROUP?${KMEM_GROUP}:kmem}
TTY_GROUP=${TTY_GROUP?${TTY_GROUP}:tty}
SETUID_MODE=${SETUID_MODE?${SETUID_MODE}:4711}
SETGID_MODE=${SETGID_MODE?${SETGID_MODE}:2711}

IOWNER=${IOWNER?${IOWNER}:${OWNER?${OWNER}:root}}
IGROUP=${IGROUP?${IGROUP}:${GROUP?${GROUP}:system}}
IMODE=${IMODE?${IMODE}:755}

#
#  Program macros
#
AR=${_AR_?${_AR_}:ar}
AS=as
CC=cc
CHMOD=chmod
CHPRIV=chpriv
CP=cp
CTAB=ctab
ECHO=echo
GENCAT=gencat
GENPATH=genpath
LD=ld
LIBLOC=libloc
LINT=lint
LN=ln
MAKEPATH=makepath
MD=md
MIG=mig
MKCATDEFS=xmkcatdefs
MV=mv
PC=pc
RANLIB=ranlib
RELEASE=release
RM=rm
SED=sed
SORT=sort
TAGS=ctags
TOUCH=touch
TR=tr
XSTR=xstr
YACC=yacc

#
#  Define ${_T_M_}_VA_ARGV to be either VA_ARGV_IS_RECAST
#  to recast to char **, otherwise define VA_ARGV_IS_ROUTINE
#  If not defined here, we become VA_ARGV_UNKNOWN which should invoke
#  a #error directive where needed.
#
_HP_M68K_VA_ARGV=VA_ARGV_IS_RECAST
_HP300_VA_ARGV=VA_ARGV_IS_RECAST
_IBMRT_VA_ARGV=VA_ARGV_IS_RECAST
_MACII_VA_ARGV=VA_ARGV_IS_RECAST
_MMAX_VA_ARGV=VA_ARGV_IS_RECAST
_PMAX_VA_ARGV=VA_ARGV_IS_RECAST
_SUN3_VA_ARGV=VA_ARGV_IS_RECAST
_SUN4_VA_ARGV=VA_ARGV_IS_RECAST
_SUN_VA_ARGV=VA_ARGV_IS_RECAST
_VAX_VA_ARGV=VA_ARGV_IS_RECAST
_AT386_VA_ARGV=VA_ARGV_IS_RECAST
_IPSC860_VA_ARGV=VA_ARGV_IS_ROUTINE
${_T_M_}_VA_ARGV=${_${_T_M_}_VA_ARGV?${_${_T_M_}_VA_ARGV}:VA_ARGV_UNKNOWN}

#
#  Defined whether characters are sign or zero extended
#
_HP_M68K_CHAR_EXTEND=CHARS_EXTEND_ZERO
_HP300_CHAR_EXTEND=CHARS_EXTEND_SIGN
_IBMRT_CHAR_EXTEND=CHARS_EXTEND_ZERO
_MACII_CHAR_EXTEND=CHARS_EXTEND_SIGN
_MMAX_CHAR_EXTEND=CHARS_EXTEND_ZERO
_PMAX_CHAR_EXTEND=CHARS_EXTEND_ZERO
_SUN3_CHAR_EXTEND=CHARS_EXTEND_SIGN
_SUN4_CHAR_EXTEND=CHARS_EXTEND_SIGN
_SUN_CHAR_EXTEND=CHARS_EXTEND_SIGN
_VAX_CHAR_EXTEND=CHARS_EXTEND_SIGN
_AT386_CHAR_EXTEND=CHARS_EXTEND_SIGN
_IPSC860_CHAR_EXTEND=CHARS_EXTEND_SIGN
${_T_M_}_CHAR_EXTEND=${_${_T_M_}_CHAR_EXTEND?${_${_T_M_}_CHAR_EXTEND}:CHARS_EXTEND_UNKNOWN}

#
#  Define MAILSYSTEM for SITE somehow...
#
MAILSYSTEM=${MAILSYSTEM?${MAILSYSTEM}:SENDMAIL}

#
#  Security flags
#
_SEC_C2_DEFS_=-DSEC_BASE -DSEC_ACL_SWARE
_SEC_B1_DEFS_=-DSEC_BASE -DSEC_ACL_SWARE -DSEC_PRIV -DSEC_MAC_OB

_SET_SEC_DEFS_=${SEC_DEFS?SEC_DEFS:${SEC_LEVEL?SEC_DEFS:}}
${_SET_SEC_DEFS_}=${SEC_DEFS?${SEC_DEFS}:${_SEC_${SEC_LEVEL}_DEFS_}}

SEC_LIBS=${SEC_DEFS?-lsecurity:}
PROGRAMS_C2=${PROGRAMS_BASE} ${PROGRAMS_ACL}
PROGRAMS_B1=${PROGRAMS_C2} ${PROGRAMS_PRIV} ${PROGRAMS_MAC}
#
#  Default locale/language information
#
LOCALE=${LOCALE?${LOCALE}:C}
LOCALEPATH=${MAKETOP}usr/lib/nls/loc
MSGLANG=${MSGLANG?${MSGLANG}:en_US.88591}
MSGLANGPATH=${MAKETOP}usr/lib/nls/msg/${MSGLANG}

#
#  Internationalization flag
#
I18N_FLAG=${I18N_FLAG?${I18N_FLAG}:NLS}

#
#  Internationalization CC flags
#
_NLS_I18N_DEFS_=-DNLS
_KJI_I18N_DEFS_=-DKJI
_I18N_DEFS_=${_${I18N_FLAG}_I18N_DEFS_}

#
#  Flags to get the loader to strip symbols
#
#LDSTRIP=-%ld," -x" -%ld," -S"
# Intel i860 SSD hack
LDSTRIP=${NO_LD_STRIP?${NO_LD_STRIP}:-%ld," -x" -%ld," -S"}

#
#  Shared libraries definitions
#
_SET_USE_SHLIBS_=${NO_SHARED_LIBRARIES?:${SHARED_LIBRARIES?USE_SHARED_LIBRARIES:}}
${_SET_USE_SHLIBS_}=

#
#  C compiler variations
#
CCTYPE=${CCTYPE?${CCTYPE}:ansi}
_CCTYPE_=${${@}_CCTYPE?${${@}_CCTYPE}:${CCTYPE}}

_GCC_EXEC_PREFIX_=${${OBJECT_FORMAT}_GCC_EXEC_PREFIX}
_GLINE_=${GLINE?${GLINE}:-gline}

_host_CC_=${HOST_CC?${HOST_CC}:cc}
_ansi_CC_=${ANSI_CC?${ANSI_CC}:${_GCC_EXEC_PREFIX_}gcc -B${_GCC_EXEC_PREFIX_} ${_GLINE_}}
_traditional_CC_=${TRADITIONAL_CC?${TRADITIONAL_CC}:${_ansi_CC_} -traditional}
_writable_strings_CC_=${WRITABLE_STRINGS_CC?${WRITABLE_STRINGS_CC}:${_ansi_CC_} -fwritable-strings}
_CC_=${_${_CCTYPE_}_CC_}

_host_LD_=${HOST_LD?${HOST_LD}:ld}
_ansi_LD_=${ANSI_LD?${ANSI_LD}:${_GCC_EXEC_PREFIX_}ld}
_traditional_LD_=${TRADITIONAL_LD?${TRADITIONAL_LD}:${_ansi_LD_}}
_writable_strings_LD_=${WRITABLE_STRINGS_LD?${WRITABLE_STRINGS_LD}:${_ansi_LD_}}
_LD_=${_${_CCTYPE_}_LD_}

_host_NOSTDINC_=
_ansi_NOSTDINC_=-nostdinc -MD
_traditional_NOSTDINC_=-nostdinc -MD
_writable_strings_NOSTDINC_=-nostdinc -MD
_CC_NOSTDINC_=${_${_CCTYPE_}_NOSTDINC_}

_host_GENINC_=
_ansi_GENINC_=`${GENPATH} -I.` -I-
_traditional_GENINC_=`${GENPATH} -I.` -I-
_writable_strings_GENINC_=`${GENPATH} -I.` -I-
_CC_GENINC_=${_${_CCTYPE_}_GENINC_}

_CCDEFS_=-DBSD44 -DMSG ${_I18N_DEFS_} -DMACH -DCMU -D${SITE} -D${MACHINE} -D__${MACHINE}__
_SHCCDEFS_=${NO_SHARED_LIBRARIES?:${USE_SHARED_LIBRARIES?-D_SHARED_LIBRARIES:${USE_STATIC_LIBRARIES?:-D_SHARED_LIBRARIES}}}

_PMAX_MACHO_CFLAGS_=-G 0
_MMAX_MACHO_CFLAGS_=-mnosb
_O_F_CFLAGS_=${_${_T_M_}_${OBJECT_FORMAT}_CFLAGS_}

_host_CFLAGS_=
_ansi_CFLAGS_=${_O_F_CFLAGS_} ${_CCDEFS_} ${_SHCCDEFS_}
_traditional_CFLAGS_=${_ansi_CFLAGS_}
_writable_strings_CFLAGS_=${_ansi_CFLAGS_}
_CC_CFLAGS_=${_${_CCTYPE_}_CFLAGS_}

_host_INCDIRS_=
_ansi_INCDIRS_=${INCDIRS}
_traditional_INCDIRS_=${INCDIRS}
_writable_strings_INCDIRS_=${INCDIRS}
_CC_INCDIRS_=${_${_CCTYPE_}_INCDIRS_}

_host_GENLIB_=
_ansi_GENLIB_=
_traditional_GENLIB_=
_writable_strings_GENLIB_=
_CC_GENLIB_=${_${_CCTYPE_}_GENLIB_}

_host_LDFLAGS_=
_ansi_LDFLAGS_=
_traditional_LDFLAGS_=
_writable_strings_LDFLAGS_=
_CC_LDFLAGS_=${_${_CCTYPE_}_LDFLAGS_}

_GLUE_=${NO_SHARED_LIBRARIES?:${USE_SHARED_LIBRARIES?-%ld," -warn_nopic -glue":${USE_STATIC_LIBRARIES?:-%ld," -warn_nopic -glue"}}}
_host_GLUE_=
_ansi_GLUE_=${_GLUE_}
_traditional_GLUE_=${_GLUE_}
_writable_strings_GLUE_=${_GLUE_}
_CC_GLUE_=${_${_CCTYPE_}_GLUE_}

_host_LIBDIRS_=
_ansi_LIBDIRS_=${NO_SHARED_LIBRARIES?:${USE_SHARED_LIBRARIES?${SHLIBDIRS}:${USE_STATIC_LIBRARIES?:${SHLIBDIRS}}}} ${LIBDIRS}
_traditional_LIBDIRS_=${_ansi_LIBDIRS_}
_writable_strings_LIBDIRS_=${_ansi_LIBDIRS_}
_CC_LIBDIRS_=${_${_CCTYPE_}_LIBDIRS_}

_PICLIB_=${NO_SHARED_LIBRARIES?:${USE_SHARED_LIBRARIES?-pic-lib ${_GENLIB_} ${_CC_LIBDIRS_} ${_LIBS_}:${USE_STATIC_LIBRARIES?:-pic-lib ${_GENLIB_} ${_CC_LIBDIRS_} ${_LIBS_}}}}
_host_PICLIB_=
_ansi_PICLIB_=${_PICLIB_}
_traditional_PICLIB_=${_PICLIB_}
_writable_strings_PICLIB_=${_PICLIB_}
_CC_PICLIB_=${_${_CCTYPE_}_PICLIB_}

#
#  Compilation optimization level.  This should be set to whatever
#  combination of -O and -g flags you desire.  Defaults to -O.
#
#  Allow these flags to be overridden per target
#
CC_OPT_LEVEL=${CC_OPT_LEVEL?${CC_OPT_LEVEL}:-O}
_CC_OPT_LEVEL_=${${@}_CC_OPT_LEVEL?${${@}_CC_OPT_LEVEL}:${CC_OPT_LEVEL}}
_CC_OL_=${${@}_OPT_LEVEL?${${@}_OPT_LEVEL}:${OPT_LEVEL?${OPT_LEVEL}:${_CC_OPT_LEVEL_}}}
LD_OPT_LEVEL=${LD_OPT_LEVEL?${LD_OPT_LEVEL}:}
_LD_OPT_LEVEL_=${${@}_LD_OPT_LEVEL?${${@}_LD_OPT_LEVEL}:${LD_OPT_LEVEL}}
_LD_OL_=${${@}_OPT_LEVEL?${${@}_OPT_LEVEL}:${OPT_LEVEL?${OPT_LEVEL}:${_LD_OPT_LEVEL_}}}

#
#  Program flags for makefile, environment and command line args
#
_GENINC_=${_CC_GENINC_} `${GENPATH} ${${@}_INCARGS?${${@}_INCARGS}:${INCARGS}} ${${@}_INCFLAGS?${${@}_INCFLAGS}:${INCFLAGS}} ${${@}_INCENV?${${@}_INCENV}:${INCENV}}` ${_CC_INCDIRS_}
_GENLIB_=${_CC_GENLIB_} `${GENPATH} ${${@}_LIBARGS?${${@}_LIBARGS}:${LIBARGS}} ${${@}_LIBFLAGS?${${@}_LIBFLAGS}:${LIBFLAGS}} ${${@}_LIBENV?${${@}_LIBENV}:${LIBENV}}`
_LIBS_=${${@}_LIBSENV?${${@}_LIBSENV}:${LIBSENV}} ${${@}_LIBS?${${@}_LIBS}:${LIBS}} ${${@}_LIBSARGS?${${@}_LIBSARGS}:${LIBSARGS}} ${GLINE_LIBC?-lc_g:}
_CCFLAGS_=${_CC_CFLAGS_} \
	  ${_CC_OL_} \
	  ${${@}_CENV?${${@}_CENV}:${CENV}} \
	  ${${@}_CFLAGS?${${@}_CFLAGS}:${CFLAGS}} \
	  ${${@}_CARGS?${${@}_CARGS}:${CARGS}} \
	  ${_CC_NOSTDINC_} ${_GENINC_} ${_CC_PICLIB_}
_COMMON_LDFLAGS_=${_CC_LDFLAGS_} ${_LD_OL_} ${${@}_LD_OPT_LEVEL?${${@}_LD_OPT_LEVEL}:${LD_OPT_LEVEL}} ${${@}_LDENV?${${@}_LDENV}:${LDENV}} ${${@}_LDFLAGS?${${@}_LDFLAGS}:${LDFLAGS}} ${${@}_LDARGS?${${@}_LDARGS}:${LDARGS}} ${_GENLIB_} ${_CC_LIBDIRS_}
_LDFLAGS_=${_CC_GLUE_} ${_COMMON_LDFLAGS_}
_SHLDFLAGS_=-R ${EXPORTS} ${_COMMON_LDFLAGS_}

_PCFLAGS_=${${@}_PENV?${${@}_PENV}:${PENV}} ${${@}_PFLAGS?${${@}_PFLAGS}:${PFLAGS}} ${${@}_PARGS?${${@}_PARGS}:${PARGS}}
_YFLAGS_=${${@}_YENV?${${@}_YENV}:${YENV}} ${${@}_YFLAGS?${${@}_YFLAGS}:${YFLAGS}} ${${@}_YARGS?${${@}_YARGS}:${YARGS}}
_LINTFLAGS_=${${@}_LINTENV?${${@}_LINTENV}:${LINTENV}} ${${@}_LINTFLAGS?${${@}_LINTFLAGS}:${LINTFLAGS}} ${${@}_LINTARGS?${${@}_LINTARGS}:${LINTARGS}} ${_GENINC_}
_TAGSFLAGS_=${${@}_TAGSENV?${${@}_TAGSENV}:${TAGSENV}} ${${@}_TAGSFLAGS?${${@}_TAGSFLAGS}:${TAGSFLAGS}} ${${@}_TAGSARGS?${${@}_TAGSARGS}:${TAGSARGS}}
_RMFLAGS_=${${@}_DEF_RMFLAGS?${${@}_DEF_RMFLAGS}:${DEF_RMFLAGS}}
_MIGFLAGS_=-cpp ${_GCC_EXEC_PREFIX_}cpp ${${@}_MIGENV?${${@}_MIGENV}:${MIGENV}} ${${@}_MIGFLAGS?${${@}_MIGFLAGS}:${MIGFLAGS}} ${${@}_MIGARGS?${${@}_MIGARGS}:${MIGARGS}} ${_CC_NOSTDINC_} ${_GENINC_}

_MDFLAGS_=${${@}_DEF_MDFLAGS?${${@}_DEF_MDFLAGS}:${DEF_MDFLAGS}}

#
#  Define these with default options added
#
_RELEASE_=${RELEASE_PREFIX}${RELEASE} ${RELEASE_OPTIONS}

#
#  Define binary targets
#
_SET_BINARIES_=${PROGRAMS?BINARIES:${LIBRARIES?BINARIES:${OBJECTS?BINARIES:}}}
${_SET_BINARIES_}=${PROGRAMS} ${LIBRARIES} ${OBJECTS}

#
# definitions for build
#
_MIG_HDRS_=${MIG_DEFS:.defs=.h}
_MIG_USRS_=${MIG_DEFS:.defs=User.c}
_MIG_SRVS_=${MIG_DEFS:.defs=Server.c}

#
# definitions for tdinst
#
_TDINSTFILES_=${TDINSTFILES?${TDINSTFILES}:${$%_TDINSTFILES?${$%_TDINSTFILES}}}

#
# definitions for clean
#
_CLEANFILES_=${CLEANFILES?${CLEANFILES}:${$%_CLEANFILES?${$%_CLEANFILES}:$%.X ${_CLEAN_OFILES_} ${$%_GARBAGE?${$%_GARBAGE}:${GARBAGE}}}}

#
# definitions for lint
#
_LINTFILES_=${LINTFILES?${LINTFILES}:${$%_LINTFILES?${$%_LINTFILES}:${_LINT_OFILES_:.o=.c}}}

#
# definitions for tags
#
_TAGSFILES_=${TAGSFILES?${TAGSFILES}:${$%_TAGSFILES?${$%_TAGSFILES}:${_TAGS_OFILES_:.o=.c}}}

#
# definitions for export
#
_EXPDIR_=${$%_EXPDIR?${$%_EXPDIR}:${EXPDIR?${EXPDIR}:${$%_IDIR?${$%_IDIR}:${IDIR}}}}
_EXPORTBASE_=${EXPORTBASE?${EXPORTBASE}:_EXPORTBASE_}
_EXPLINKS_=${$%_EXPLINKS?${$%_EXPLINKS}:${EXPLINKS}}
_DO_EXPLINKS_=\
	for i in ${_EXPLINKS_}; do ${RM} ${_RMFLAGS_} $$i; ${LN} $% $$i; done
_MAKE_EXPLINKS_=${$%_EXPLINKS?${_DO_EXPLINKS_}:${EXPLINKS?${_DO_EXPLINKS_}:@}}

#
# definitions for install
#
_ITARGET_=${TOSTAGE?_TOSTAGE_:_NO_TOSTAGE_}

_INSTALL_TARGET_=${_ITARGET_}$%

_STAGEFILES_=${TOSTAGE?${FROMSTAGE?:_TOSTAGE_%}:}
_STAGEBASES_=${TOSTAGE?${FROMSTAGE?_TOSTAGE_%:}:}

#
#  Default single suffix compilation rules
#
.SUFFIXES:
.SUFFIXES: .o .s .pp .c .y .l .p .sh .csh .txt .uu

#
#  Default double suffix compilation rules
#
${MSGHDRS}: $${@:_msg.h=.msg}
	${MKCATDEFS} ${@:_msg.h=} ${@:_msg.h=.msg} > ${@:_msg.h=.cat.in}
	-${RM} ${_RMFLAGS_} ${MSGLANGPATH}/${@:_msg.h=.cat}
	${MAKEPATH} ${MSGLANGPATH}/${@:_msg.h=.cat}
	-${GENCAT} ${MSGLANGPATH}/${@:_msg.h=.cat} ${@:_msg.h=.cat.in}
	${RM} ${_RMFLAGS_} ${@:_msg.h=.cat.in}

${CATFILES}: $${@:.cat=.msg}
	${MKCATDEFS} ${@:.cat=} ${@:.cat=.msg} > $@.in
	-${RM} ${_RMFLAGS_} ${MSGLANGPATH}/$@
	${MAKEPATH} ${MSGLANGPATH}/$@
	-${GENCAT} ${MSGLANGPATH}/$@ $@.in
	${RM} ${_RMFLAGS_} $@.in

#
#  Special rules
#

#
#  Prefix used for targets which are neither installed nor built by
#  default at the local site.  They can still be named specifically but
#  will not be included by "clean_all", "build_all", "lint_all" or
#  "install_all" rules.
#
NOTSITE=_NOTSITE_

#
#  Use this as a dependency for any rule which should always be triggered
#  (e.g. recursive makes).
#
${ALWAYS}:;@

TAR=tar

#
#  Include pass definitions for standard targets
#
include passes.mk

#
#  Compilation rules
#
all: build_all;@

build_all: $${_BUILD_TARGETS_};@

_build_prefix_%: %;@

_build_prefix_${NOTSITE}%:
	@echo % is not used at ${SITE}

dopass_all: $${_DOPASS_TARGETS_};@

comp_all: $${_COMP_TARGETS_};@

_comp_prefix_%: %;@

_comp_prefix_${NOTSITE}%:
	@echo % is not used at ${SITE}

#
#  Tdinstall rules
#
tdinst_all: $${_TDINST_TARGETS_};@

tdinst_%: _tdinst_prefix_%;@

_tdinst_prefix_%:
	-${RM} -rf ${DESTDIR}/${RELDIR}
	mkdir ${DESTDIR}/${RELDIR}
	${TDBIN}/tdinit ${DESTDIR}/${RELDIR}
	csh -f ${MKRULES} -r ${RULE} ${DESTDIR}/${RELDIR}
	sed \
		-e "s/CASENAME/${TESTCASE}/" \
		-e "s/RULE/${RULE}/" \
		-e "s/PURPOSE/Security functionality/" \
		../../TDmod.in | ${TDBIN}/tdmod ${DESTDIR}/${RELDIR}

_tdinst_prefix_${NOTSITE}%:;@

#
#
#  Secinstall rules
#
secinst_all: $${_SECINST_TARGETS_};@

secinst_%: _secinst_prefix_%;@

_secinst_prefix_%: % ${TESTCASE}.0.0.o ${TESTCASE}.0.0.e
	-${CP} % ${DESTDIR}/${RELDIR}/Files
	${CHPRIV} -p -a suspendaudit,configaudit,writeaudit,execsuid,chmodsugid,chown,acct,limit,lock,linkdir,mknod,mount,sysattr,setprocident,chroot,debug,shutdown,filesys,remote,kill,owner,sucompat,supropagate,downgrade,writeupclearance,writeupsyshi,chpriv,multileveldir,allowmacaccess,allowdacaccess ${DESTDIR}/${RELDIR}/Files/%
	${CP} ${TESTCASE}.0.0.o ${DESTDIR}/${RELDIR}/Stdout
	${CP} ${TESTCASE}.0.0.e ${DESTDIR}/${RELDIR}/Stderr

_secinst_prefix_${NOTSITE}%:;@

#
#  Clean up rules
#
clean: clean_all

clean_all: $${_CLEAN_TARGETS_}
	-${RM} ${_RMFLAGS_} core Makedep

clean_%: _clean_prefix_%;@

_clean_prefix_%:
	-${RM} ${_RMFLAGS_} ${_CLEANFILES_}

_clean_prefix_${NOTSITE}%:;@

rmtarget_all: $${_RMTARGET_TARGETS_}
	-${RM} ${_RMFLAGS_} core

rmtarget_%: _rmtarget_prefix_%;@

_rmtarget_prefix_%:
	-${RM} ${_RMFLAGS_} %

_rmtarget_prefix_${NOTSITE}%:;@

clobber_all: $${_CLOBBER_TARGETS_}
	-${RM} ${_RMFLAGS_} core Makedep

clobber_%: _clobber_prefix_%;@

_clobber_prefix_%: _clean_prefix_% _rmtarget_prefix_%;@

_clobber_prefix_${NOTSITE}%:;@

#
#  Lint rules
#
lint_all: $${_LINT_TARGETS_};@

lint_%: _lint_prefix_%;@

_lint_prefix_%: $${_LINTFILES_}
	${LINT} ${_LINTFLAGS_} $>

_lint_prefix_${NOTSITE}%:;@

#
#  Tags rules
#
tags_all: $${_TAGS_TARGETS_};@

tags_%: _tags_prefix_%;@

_tags_prefix_%: $${_TAGSFILES_}
	${TAGS} ${_TAGSFLAGS_} $>

_tags_prefix_${NOTSITE}%:;@

#
#  Export rules
#
export_all: $${_EXPORT_TARGETS_};@

export_%: _export_prefix_%;@

_export_prefix_%: ${_EXPORTBASE_}${_EXPDIR_}%;@

${_EXPORTBASE_}%2/%: %
#	-${RM} ${_RMFLAGS_} $@
#	${MAKEPATH} $@
#	${_MAKE_EXPLINKS_}
#	${TAR} cf - % ${_EXPLINKS_} | (cd ${_EXPORTBASE_}%2; tar xf -)

_export_prefix_${NOTSITE}%:;@

#
#  Installation/release rules
#
#  This breaks installation of the sat 'install' program . . . 
#install: install_all

install_all: $${_INSTALL_TARGETS_};@

install_%: _install_prefix_%;@

_install_prefix_%: $${_INSTALL_TARGET_};@

_install_prefix_${NOTSITE}%:;@

${_STAGEFILES_}: ${ALWAYS}
	${_RELEASE_} ${%_NOSTRIP?-nostrip:${NOSTRIP?-nostrip:}}\
		-o ${%_IOWNER?${%_IOWNER}:${IOWNER}}\
		-g ${%_IGROUP?${%_IGROUP}:${IGROUP}}\
		-m ${%_IMODE?${%_IMODE}:${IMODE}}\
		-tostage ${TOSTAGE}\
		-fromfile %\
		${%_IDIR?${%_IDIR}:${IDIR}}%\
		${%_ILINKS?${%_ILINKS}:${ILINKS}}

${_STAGEBASES_}: ${ALWAYS}
	${_RELEASE_} ${%_NOSTRIP?-nostrip:${NOSTRIP?-nostrip:}}\
		-o ${%_IOWNER?${%_IOWNER}:${IOWNER}}\
		-g ${%_IGROUP?${%_IGROUP}:${IGROUP}}\
		-m ${%_IMODE?${%_IMODE}:${IMODE}}\
		-tostage ${TOSTAGE}\
		-fromstage ${FROMSTAGE}\
		${%_IDIR?${%_IDIR}:${IDIR}}%\
		${%_ILINKS?${%_ILINKS}:${ILINKS}}

