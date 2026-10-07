######################################################################
##
##  Bld/include/OS.mk
##
##    This 'included' makefile contains variables used by more 
##    than one of the Operating System build projects. This file
##    is included by ALL OS build projects.
##
##    There are 2 groupings of variables in this file, the first
##    are needed for the BIG build process and the second are 
##    needed by the actuall source makefiles/Makeconf.
##

##
##  BIG build process's make variables
##

KernelArea             := $(ParagonBuildArea)/kernels
ServerArea             := $(ParagonBuildArea)/servers
CommandsArea           := $(ParagonBuildArea)/cmds
PrebuiltArea           := $(ParagonBuildArea)/prebuilt

ParagonExportsArea     := $(ExportsArea)/paragon


######################################################################
##
##  NOTE!!!
##
##      When using this with SSD's pmake:
##
##        1. Invoke with -N.  If not, pmake will look for a Makeconf 
##           file and if it doesn't find one set some 'magic' variables
##           that shouldn't be set.
##
##        2. Because it's invoked with -N the MFLAGS and MAKEFLAGS 
##           have to be unset for the 'real' make or it won't look
##           for a Makeconf file (which it needs to do once it's down
##           into the source).
##
MakeFlags        := MFLAGS='' MAKEFLAGS=''
Builder          := $(MakeFlags) make 
PMAKEN           := pmake -N

######################################################################
##
##  Specific build rules for all kernels
##
##    The following kernel build 'definitions' are made up of the
##    following:
##        1.  A comment stating the type of kernel being built
##            in that section.
##        2.  Variable definitions for that specific kernel.
##        3.  A rule for building a that configuration of a
##            kernel.
##        4.  A rule for 'exporting' that single kernel. If a
##            kernel was known by more than one name, the additional
##            copies/symlinks would be added to this rule.
##
##    The variables used are:
##
##        Kernel?Rename
##                  This is the name the kernel will have when it has
##                  been copied to it's export location.  This is the
##                  name the kernel will be installed as.  If a kernel
##                  is to have more than one name (ie symlinks to it)
##                  these would be added to the exports rule for this
##                  kernel.
##                  
##        Kernel?Config
##                  This is the configuration string assigned to the
##                  I860_CONFIG variable when make is invoked to build
##                  this kernel.
##                  
##        Kernel?
##                  This is assigned the name of the kernel, as it's 
##                  built in the kernel object area.
##                  
##    The 'build' rule (line by line):
##
##        Line No.  Purpose
##        --------  ---------------------------------------------------
##        1         Target line with 'kernels-setup' as the dependency
##                  so that if the object area is completely removed
##                  it will be reinitialized for the build.
##        2         An echo statement letting the builder know something
##                  is happening.
##        3         Setup a new log for this build. If a previous log 
##                  exists move it asside.
##        4         Put the starting date/time in the log file.
##        5 - 8     A subshell process that does the building by first;
##                  cd-ing to the top of the kernel source area; second,
##                  invoking make with the proper configuration for this
##                  kernel while redirecting ALL output to the build log.
##        9 - 12    Conditional to that the make done in the subshell
##                  exited successfully, this is necessary because this
##                  was a subshell inside of a subshell and the exit 
##                  status needs to be passed back up to the original 
##                  make.
##        13        Put the ending date/time in the log file.
##
##    The 'export' rule (line by line):
##
##        Line No.  Purpose
##        --------  ---------------------------------------------------
##        1         Target line with the 'exported' name as the target
##                  and the 'build' target as the dependency.
##        2         Copy the 'build' file to the 'exported' location.
##

ifeq "$(Deliverable)" "kernel"
    CFG_VAR = I860_CONFIG
else
    CFG_VAR = CONFIG
endif

BuildTemplate = \
        BldLog=$(LogArea)/$(ExportName).build                         ;\
	echo "[ $(ExportName) ($(Config)) ]"                          ;\
	$(MkLog) $$BldLog                                             ;\
	date > $$BldLog                                               ;\
	(                                                              \
           cd $(BuildSources)                                         ;\
           $(Builder) $(CFG_VAR)=$(Config) >> $$BldLog 2>&1           ;\
        )                                                             ;\
        BuildStatus=$$?                                               ;\
        if [ $$BuildStatus -ne 0 ]; then                               \
          exit $$BuildStatus                                          ;\
        fi                                                            ;\
	date >> $$BldLog
 
##
##  Source code environment variables
##

BLD              := $(ParagonBuildArea)
.EXPORT: BLD

BLD_HOME         := $(ParagonBuildArea)
.EXPORT: BLD_HOME

BUILD_HOST       := sparc
.EXPORT: BUILD_HOST

LPATH            := $(BUILD_XDEV)/lib
.EXPORT: LPATH

XMIT_BLD         := 1
.EXPORT: XMIT_BLD

TOSTAGE          := $(ParagonExportsArea)
.EXPORT: TOSTAGE

SERVER           := server
.EXPORT: SERVER

MK               := mk
.EXPORT: MK

MKREL            := $(KernelArea)/release/i860_mach
.EXPORT: MKREL

SVROBJ           := $(ServerArea)/svr/obj/i860_mach/latest/server
.EXPORT: SVROBJ

