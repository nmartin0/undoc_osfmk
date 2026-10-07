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

emulator: ${CONFIG}.all

${CONFIG}.all:	${CONFIG}/make \
		${CONFIG}/Makeconf \
		${CONFIG}.doconf \
		${CONFIG}.config \
		${CONFIG}.make

${CONFIG}.make :	${CONFIG}/make ${CONFIG}/Makeconf
	@echo "[ Making ${CONFIG} ]"
	@cd ${CONFIG} && USE_SOURCE_DIR=1 csh -f ./make ${MAKE_ARGS} CONFIG=${CONFIG} emulator

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
				${CONFIG_INPUT} 
	@cd ${CONFIG} && \
         csh -f ./make CONFIG=${CONFIG} \
		CONFIG_SUFFIX=${CONFIG_SUFFIX} \
		CONFIG_FLAGS=${CONFIG_FLAGS} \
		-f Makefile.top-level \
		make.config

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

# Generate the files that depend on the list of system calls and their
# attributes.  After doing so, in case ./include/sys got updated by the cp
# command, we touch the files again, so they will be newer than the dir:
#

make.config:	../${CONFIG_OUTPUT}${CONFIG_SUFFIX}

../${CONFIG_OUTPUT}${CONFIG_SUFFIX}:	\
		../${CONFIG_INPUT} \
		$(SOURCE_DIR)/conf/files \
		$(SOURCE_DIR)/conf/files.${cpu} \
		$(SOURCE_DIR)/conf/Makefile.template \
		$(SOURCE_DIR)/conf/Makefile.${cpu} \
		./Makefile.top-level
	@echo "[ Configuring in ${CONFIG} ]"
	../../server/bin/config -c $(SOURCE_DIR)/conf ${CONFIG_FLAGS} ${CONFIG}
	@echo ${CONFIG} >../${CONFIG}/vers.config

# To lint individual files, or groups of files:
#	make LINTFILES="tnc/tnc_mig.c pfs/pfs_vfsops.c" lint
#	make LINTFILES=nx/nx_mig.c lint
lint:
	cd ${CONFIG} && USE_SOURCE_DIR=1 csh -f ./make lint


ALWAYS:

-include ${cpu}/Makefile.top-level
.EXPORT: LINTFILES
