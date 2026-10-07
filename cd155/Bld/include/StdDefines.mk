######################################################################
##
##  Bld/Include/StdDefines.mk
##
##    This 'included' makefile contains make variables used by
##    ALL of the build projects makefiles.
##


##  Names of the different build systems used.  It is assumed that the
##  build is being driven from the 'SunBuildHost'.  This name is also
##  used as part of the generated path to the build sandbox.
SunBuildHost           := $(shell hostname)

##  These are not used at this time, but are defined for future use.
BuildPerson            := $(shell whoami)

##  Determine where the build sandbox is located. From this we derrive
##  many other paths.  This assumes you are NEVER calling this from
##  deeper than 1 level from the build area root.
InitialPWD             := $(shell pwd)
ifeq "$(MasterBuild)" "True"
  BuildRoot              := $(InitialPWD)
else
  BasePWD                := $(shell basename $(InitialPWD))
  BuildRoot              := $(shell ( cd ..; pwd ) )
endif
BuildArea              := $(shell echo $(BuildRoot) | sed -e 's|.*/tmp_mnt/|/|')


##  Various directories used by the build process.  There is nothing
##  forcing all these to be within the same sandbox, these are defined
##  so that they could be moved if that were desireable.
ParagonBuildArea       := $(BuildArea)
ImportsArea            := $(BuildArea)/Imports
ExportsArea            := $(BuildArea)/Exports
ReleaseArea            := $(BuildArea)/Release
LogArea                := $(BuildArea)/Bld/logs
StatusArea             := $(BuildArea)/Bld/logs

##  Hardcoded path to the executables needed by the build process.
MkLog                  := $(BuildArea)/Bld/bin/MkLog
Chown                  := /usr/etc/chown

.EXPORT: MkLog Chown

BUILD_XDEV       := $(BuildArea)/Bld/Tools
PARAGON_XDEV     := $(BuildArea)/Bld/Compilers

.EXPORT: BUILD_XDEV PARAGON_XDEV
