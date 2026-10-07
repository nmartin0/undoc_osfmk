##
## Bld/Makefile
##
##     Build system's Sys makefile.  This makefile contains the targets
##     for importing, exporting, and releasing.
##

include ../Config.mk
include include/StdDefines.mk
include include/OS.mk
include include/Path.mk
include include/Raid.mk
include include/Crossdev.mk
include include/Tarfiles.mk

.EXPORT: BuildPerson  LogArea ImportsArea ParagonExportsArea ExportsArea ReleaseArea KernelConfigurations KernelObjects KernelExportsArea ServerConfigurations ServerObjects ServerExportsArea EmulatorConfigurations EmulatorObjects EmulatorExportsArea BuildArea KernelSources KernelArea ServerArea CommandsArea PrebuiltArea KernelSources ServerSources CommandsSources  RaidBuildArea RaidSourceArea 

######################################################################
##  
##  Start
##    
##    There are some things that need to be done at the very beginning
##    of a build, some that should only be done at the beginning of the
##    first build of a new sandbox.
##
.PHONY: Start
Start: $(LogArea)/.first.build.setup.complete
 
$(LogArea)/.first.build.setup.complete: $(BuildEnv)
	@rm -f $(SourceLogFile)
	@touch $(LogArea)/.first.build.setup.complete

BuildEnv = $(ExportsArea) $(ParagonExportsArea) 
$(BuildEnv):
	@for dir in $(BuildEnv); do \
            [ -d $$dir ] || mkdir $$dir ;\
         done

