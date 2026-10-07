######################################################################
##
##  servers/Makefile
##
##      This is the file that contains all the Makefile targets
##      used to produce the server deliverables.
##
##    Targets:
##
##      all        This is the main, 'public' target that is used
##                 to build all the servers.
##
##      setup
##                 Makes sure that the SetupServer script gets run
##                 to properly setup the object area.  This way
##                 a builder can remove the object area without
##                 remorse.
##
##      clean
##                 An easy way to remove unsightly object areas.
##


##  Deliverable MUST be defined in the makefile BEFORE the includes 
##  because it is used in the included makefiles.
Deliverable            := server

include ../Config.mk
include ../Bld/include/StdDefines.mk
include ../Bld/include/OS.mk
include ../Bld/include/Path.mk

##  The REAL definition of ServerArea is in ../Bld/OSVariables.mk 
##  because it is needed by the commands/libs build as well.
#ServerArea             := $(ParagonBuildArea)/servers
ServerSources          := $(ServerArea)/svr/src/latest
ServerObjects          := $(ServerArea)/svr/obj/i860_mach/latest/server
ServerExportsArea      := $(ParagonExportsArea)/mach_servers
BuildSources           := $(ServerSources)

ServerName             := svr

EmulatorObjects        := $(ServerArea)/svr/obj/i860_mach/latest/emulator
EmulatorExportsArea    := $(ParagonExportsArea)/mach_servers

######################################################################
##  setup
##
##    There is a shell script at the top of the server source
##    that sets up the object tree prior to acually building
##    the sources.  This rule makes sure that shell script gets
##    run.
##
##    This is also a good place to make sure the 'exports' directory
##    exists.  In this case we make sure both the servers and the
##    emulators exports areas are ready.
##
##    Dependencies:
##
##       None
##
.PHONY: setup
setup:
	@(cd $(ServerSources);./SetupServer $(KernelArea)/release)
	@[ -d $(ServerExportsArea)   ] || mkdir -p $(ServersExportsArea)
	@[ -d $(EmulatorExportsArea) ] || mkdir -p $(EmulatorsExportsArea)


######################################################################
##  clean
##
##    Remove any/all derrived objects from the build area.
##
##    Dependencies:
##
##        none
##
.PHONY: clean
clean:
	rm -rf $(ServerArea)/svr/obj $(ServerArea)/mkrel


######################################################################
##
##  Specific build rules for each server and emulator to be built
##
##    The following server build 'definitions' are made up of:
##            
##        1.  A comment stating the type of server/emulator being built
##            in that section.
##        2.  Variable definitions for that specific server/emulator.
##        3.  A rule for building that configuration of a
##            server/emulator.
##        4.  A rule for 'exporting' that single server/emulator. If a
##            server/emulator was known by more than one name, the additional
##            copies/symlinks would be added to this rule.
##
##    The variables used are:
##        
##        ExportName
##        Config
##        S?
##        S?ExportName
##        S?BldCmd
##
 
############################################################
## 'Normal' server
##
ExportName   := startup
Config       := STD+WS

S1           := $(ServerObjects)/STD+WS/vmunix
S1ExportName := $(ExportName)
S1BldCmd     := $(BuildTemplate)

$(S1) : setup
	@$(S1BldCmd)

$(ServerExportsArea)/$(S1ExportName) : $(S1)
	@cp $^ $@


############################################################
## Compute (lite) server
##
ExportName   := startup.compute
Config       := LITE+WS

S2           := $(ServerObjects)/LITE+WS/vmunix
S2ExportName := $(ExportName)
S2BldCmd     := $(BuildTemplate)

$(S2) : setup
	@$(S2BldCmd)

$(ServerExportsArea)/$(S2ExportName) : $(S2)
	@cp $^ $@


############################################################
## 'Normal' emulator
##
ExportName   := emulator
Config       := STD+WS

E1           := $(EmulatorObjects)/STD+WS/emulator
E1ExportName := $(ExportName)
E1BldCmd     := $(BuildTemplate)

$(E1) : setup
	@$(E1BldCmd)

$(EmulatorExportsArea)/$(E1ExportName) : $(E1)
	@cp $^ $@


######################################################################
##  servers
##
##    Dependencies:
##
##        $(ServerExports)
##                A list of all the server deliverables in the 
##                exports area. Make will backtrack the dependencies
##                to ensure everything gets built.
##

ServerExports = $(ServerExportsArea)/$(S1ExportName)  \
                $(ServerExportsArea)/$(S2ExportName)

.PHONY: servers
servers: $(ServerExports)

######################################################################
##  emulators
##
##    Dependencies:
##
##        $(EmulatorExports)
##                A list of all the emulator deliverables in the 
##                exports area. Make will backtrack the dependencies
##                to ensure everything gets built.
##

EmulatorExports = $(EmulatorExportsArea)/$(E1ExportName)

.PHONY: emulators
emulators: $(EmulatorExports)

######################################################################
##  all
##
##     Dependencies:
##
##       servers
##       emulators
##
.PHONY: all
all: servers emulators
startup: $(S1)
startup.compute: $(S2)
emulator: $(E1)
build_all:  startup startup.compute emulator
install_all: $(ServerExports) $(EmulatorExports)
