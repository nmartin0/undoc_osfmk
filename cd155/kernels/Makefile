######################################################################
##
##  kernels/Makefile
##
##      This is the file that contains all the Makefile targets
##      used to produce the kernel deliverables.
##
##    Targets defined herein:
##
##      all        This is the 'public' target that will almost 
##                 always be used to produce the kernels. This
##                 target is the starting chain that ties the
##                 proper build targets/dependencies together.
##
##      setup
##                 Makes sure that the SetupKernel script get run.
##                 This is so you can blow away the entire object
##                 tree and be able to rebuild without manual 
##                 intervention.
##
##      clean
##                 An easy way to remove unsightly object areas.
##


##  Deliverable MUST be defined in the makefile BEFORE the includes
##  because it is used in the included makefiles.
Deliverable           := kernel

include ../Config.mk
include ../Bld/include/StdDefines.mk
include ../Bld/include/OS.mk
include ../Bld/include/Path.mk

##  KernelArea is actually defined in ../Bld/OSVariables.mk because
##  it is required for the server build as well as the kernel build.
##
#KernelArea             := $(ParagonBuildArea)/kernels
KernelSources          := $(KernelArea)/src/mk
KernelObjects          := $(KernelArea)/obj/i860_mach/mk/kernel
KernelExportsArea      := $(ParagonExportsArea)/mach_servers
BuildSources           := $(KernelSources)

KernelName             := mk

######################################################################
##  setup
##
##    There is a shell script at the top of the kernel source
##    that sets up the object tree prior to acually building
##    the sources.  This rule makes sure that shell script gets
##    run.
##
##    This is also a good place to make sure the 'exports' directory
##    exists.
##
##    Dependencies:
##
##        None
##
.PHONY: setup
setup:
	@(cd $(KernelSources);./SetupKernel)
	@[ -d $(KernelExportsArea) ] || mkdir -p $(KernelExportsArea) 

######################################################################
##  clean
##
##    Get rid of everything except the src tree, this Makefile and
##    the CVS directory.
##
##    Dependencies: 
##
##        none
## 
.PHONY: clean
clean:
	@rm -rf obj release


###################################################################### 
##
##  Specific build rules for each kernel to be built
##
##    The following kernel build 'definitions' are made up of:
##    
##        1.  A comment stating the type of kernel being built
##            in that section.
##        2.  Variable definitions for that specific kernel.
##        3.  A rule for building that configuration of a
##            kernel.
##        4.  A rule for 'exporting' that single kernel. If a
##            kernel was known by more than one name, the additional
##            copies/symlinks would be added to this rule.
##
##    The variables used are:
##
##        ExportName
##        Config
##        K?
##        K?ExportName
##        K?BldCmd
##

############################################################
## NIC A kernel without big packets support
##
ExportName    := mach_kernela
Config        := ASMP-bigpkts

K1            := $(KernelObjects)/ASMP-bigpkts/mach_kernel
K1ExportName  := $(ExportName)
K1BldCmd      := $(BuildTemplate)

$(K1) : setup
	@$(K1BldCmd)
 
$(KernelExportsArea)/$(ExportName): $(K1)
	@cp $^ $@
	@$(Chown) root $@
	@chgrp 1 $@

$(KernelExportsArea)/mach_kernela.compute: $(KernelExportsArea)/$(K1ExportName)
	@rm -f $(KernelExportsArea)/mach_kernela.compute 
	@ln -s $(K1ExportName) $(KernelExportsArea)/mach_kernela.compute

############################################################
## NIC A debug kernel without big packets support
##
ExportName    := mach_kernela.db
Config        := ASMP-bigpkts+assert

K2            := $(KernelObjects)/ASMP-bigpkts+assert/mach_kernel
K2ExportName  := $(ExportName)
K2BldCmd      := $(BuildTemplate)


$(K2) : setup
	@$(K2BldCmd)
 
$(KernelExportsArea)/$(ExportName): $(K2)
	@cp $^ $@
	@$(Chown) root $@
	@chgrp 1 $@

$(KernelExportsArea)/mach_kernela.compute.db: $(KernelExportsArea)/$(K2ExportName)
	@rm -f $(KernelExportsArea)/mach_kernela.compute.db
	@ln -s $(K2ExportName) $(KernelExportsArea)/mach_kernela.compute.db

############################################################
## NIC B kernel 
##
ExportName    := mach_kernelb
Config        := ASMP+nicb

K3            := $(KernelObjects)/ASMP+nicb/mach_kernel
K3ExportName  := $(ExportName)
K3BldCmd      := $(BuildTemplate)

$(K3) : setup
	@$(K3BldCmd)
 
$(KernelExportsArea)/$(ExportName): $(K3)
	@cp $^ $@
	@$(Chown) root $@
	@chgrp 1 $@

$(KernelExportsArea)/mach_kernelb.compute: $(KernelExportsArea)/$(K3ExportName)
	@rm -f $(KernelExportsArea)/mach_kernelb.compute
	@ln -s $(K3ExportName) $(KernelExportsArea)/mach_kernelb.compute

############################################################
## NIC B debug kernel 
##
ExportName    := mach_kernelb.db
Config        := ASMP+nicb+assert

K4            := $(KernelObjects)/ASMP+nicb+assert/mach_kernel
K4ExportName  := $(ExportName)
K4BldCmd      := $(BuildTemplate)

$(K4) : setup
	@$(K4BldCmd)
 
$(KernelExportsArea)/$(ExportName): $(K4)
	@cp $^ $@
	@$(Chown) root $@
	@chgrp 1 $@

$(KernelExportsArea)/mach_kernelb.compute.db: $(KernelExportsArea)/$(K4ExportName)
	@rm -f $(KernelExportsArea)/mach_kernelb.compute.db ]
	@ln -s $(K4ExportName) $(KernelExportsArea)/mach_kernelb.compute.db

######################################################################
##  all
##
##     Dependencies:
##
##       $(KernelExports)
##                 The goal is to produce the kernels in a 'publicly' 
##                 consumable manner. We make the main target depend
##                 on the end results and let the dependcies backtrack
##                 to enable/use make to regulate what gets built.
##
KernelExports =  setperm $(KernelExportsArea)/$(K1ExportName)  \
                 $(KernelExportsArea)/mach_kernela.compute     \
                 $(KernelExportsArea)/$(K2ExportName)          \
                 $(KernelExportsArea)/mach_kernela.compute.db  \
                 $(KernelExportsArea)/$(K3ExportName)          \
                 $(KernelExportsArea)/mach_kernelb.compute     \
                 $(KernelExportsArea)/$(K4ExportName)          \
                 $(KernelExportsArea)/mach_kernelb.compute.db 

#mach_kernela      := $(KernelObjects)/ASMP-bigpkts/mach_kernel
#mach_kernela.db   := $(KernelObjects)/ASMP-bigpkts+assert/mach_kernel
#mach_kernelb      := $(KernelObjects)/ASMP+nicb/mach_kernel
#mach_kernelb.db   := $(KernelObjects)/ASMP+nicb+assert/mach_kernel
mach_kernela: $(K1)
mach_kernela.db: $(K2)
mach_kernelb: $(K3)
mach_kernelb.db: $(K4) 
.PHONY: kernels 
all: $(KernelExports)
build_all: mach_kernela mach_kernela.db mach_kernelb mach_kernelb.db
install_all:  $(KernelExports)
setperm:
	@[ -d $(KernelExportsArea) ] || mkdir -p $(KernelExportsArea) 
	@$(Chown) root $(KernelExportsArea)
	@chgrp -R wheel $(ExportsArea)
	@chmod -R g+ws $(ExportsArea)
