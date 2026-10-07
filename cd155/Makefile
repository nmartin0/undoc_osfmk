######################################################################
##
##  Makefile
##
##    Topmost Makefile for driving other makefiles in a specific
##    order.
##

##  Must be defined in this file before any includes are done, this 
##  helps with figuring out the directory level the build was kicked 
##  off from which in turn is needed to determine the CWD.
MasterBuild = True

include Config.mk
include Bld/include/StdDefines.mk


######################################################################
##
##  all
##
##    This target builds and installs all of the necessary pieces in a 
##    specific order. 
##
##    Ordering:
##         1. kernels needed by servers
##         2. server headers and libraries needed by cmds
##         3. cmds headers and libraries needed by everything
##            except kernels and servers + sysfiles.tar needed by raid
##         4. raid
##         5. tarfiles to create root.tar, usr.tar, and mach_svr.tar
##
Primary     = kernels servers cmds
Secondary   = raid 
SubSystems  = $(Primary) $(Secondary)

all: Start $(Primary) $(Secondary) tarfiles Finish

build_all: Start build_kernels build_servers build_cmds Finish

install_all: Start install_kernels install_servers install_cmds Finish

tarfiles: 
	@$(MAKE) -C Bld tarfiles 

######################################################################
##
##  A generic target for building any specific subsystem.
##
.PHONY: $(SubSystems)
$(SubSystems): 
	@$(MAKE) -C $@ all

build_kernels:
	@$(MAKE) -C kernels build_all
build_servers:
	@$(MAKE) -C servers build_all
build_cmds:
	@$(MAKE) -C cmds build_all
install_kernels:
	@$(MAKE) -C kernels install_all
install_servers:
	@$(MAKE) -C servers install_all
install_cmds:
	@$(MAKE) -C cmds install_all



BldGlue     = install.sysfiles 

.PHONY: $(BldGlue)
$(BldGlue)::
	@$(MAKE) -C Bld $@

######################################################################
##
##  Start and Finish
##
##    The only purpose of these two routines is to provide a little 
##    bit of information to the builder, notably the date/time of 
##    the build start and finish and the version string being used
##    for this build.
##
.PHONY: Start Finish
Start :
	@echo "[ $@ time  `date` ]"
	@echo "[ Building version $(BUILD_VERSION_STRING) ]"

Finish:
	@echo "[ $@ time `date` ]"

