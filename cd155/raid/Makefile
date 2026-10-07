##
##  raid/Makefile
##


include ../Config.mk
include ../Bld/include/StdDefines.mk
include ../Bld/include/Path.mk
include ../Bld/include/Raid.mk
include ../Bld/include/Crossdev.mk

Deliverable := raid

RaidBuildArea          := $(ParagonBuildArea)/raid
RaidSourceArea         := $(RaidBuildArea)/src/raid


.PHONY: all
all: setup install.sysfiles raid exports

SourceArea     := $(RaidSourceArea)
SourceVersion  := $(RaidSourceVersion)
SourceProject  := raid

######################################################################
## setup
##
.PHONY: setup
setup:
	@[ -d $(RaidSourceArea)/oemsrc/lib/Uv4 ] || mkdir -p $(RaidSourceArea)/oemsrc/lib/Uv4

######################################################################
## raid
##
.PHONY: raid 
raid: $(StatusArea)/Raid.Build.Finished.Successfully

$(StatusArea)/Raid.Build.Finished.Successfully:
	@echo "[ Building Raid ]"
	@$(MkLog) $(LogArea)/raid.build
	@date >  $(LogArea)/raid.build
	@(cd $(RaidSourceArea); pmake -Nv all >> $(LogArea)/raid.build 2>&1)
	@date >>  $(LogArea)/raid.build
	@touch $@

######################################################################
## exports
##
RaidSbinExports := $(RaidSourceArea)/oemsrc/src/Ace/ace       \
                   $(RaidSourceArea)/oemsrc/src/Acf/acf       \
                   $(RaidSourceArea)/oemsrc/src/Arc/arc       \
                   $(RaidSourceArea)/oemsrc/src/Apc/apc       \
                   $(RaidSourceArea)/oemsrc/src/Apr/apr       \
                   $(RaidSourceArea)/libio/scsi/inquire       \
                   $(RaidSourceArea)/oemsrc/src/Special/Can/can

RaidSbinInitExports := $(RaidSourceArea)/oemsrc/src/init.d/amd

RaidUsrSbinExports := $(RaidSourceArea)/oemsrc/src/Amd/amd   

RaidEtcArrayExports := $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/ADRV.HLP       \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/CLUN.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/DDRV.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/DISDRST.HLP        \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/DLUN.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/FDRV.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/FLUN.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/HDRV.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/HLUN.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/MLUN.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/MODDRST.HLP        \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/MRECON.HLP         \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/NAVDRST.HLP        \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/NAVLUN.HLP         \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/ODRVST.HLP         \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/OLUN.HLP           \
                   $(RaidSourceArea)/oemsrc/src/Ace/acehelp/ascii/RDRV.HLP

RaidEtcArrayCCExports := $(RaidSourceArea)/oemsrc/src/Acf/Ccfiles/ccf_b

RaidEtcDefaultExports := $(RaidSourceArea)/oemsrc/src/Acf/Ccfiles/acf

RaidExports     := $(RaidSbinExports)                         \
                   $(RaidSourceArea)/oemsrc/src/init.d/amd 


exports: $(RaidExports)
	@[ -d $(RaidExportsArea)/sbin/init.d       ] ||  mkdir -p $(RaidExportsArea)/sbin/init.d
	@[ -d $(RaidExportsArea)/usr/sbin          ] ||  mkdir -p $(RaidExportsArea)/usr/sbin
	@[ -d $(RaidExportsArea)/etc/array/cc      ] ||  mkdir -p $(RaidExportsArea)/etc/array/cc 
	@[ -d $(RaidExportsArea)/etc/default       ] ||  mkdir -p $(RaidExportsArea)/etc/default
	@[ -d $(RaidExportsArea)/etc/array/acehelp ] ||  mkdir -p $(RaidExportsArea)/etc/array/acehelp
	@[ -d $(RaidExportsArea)/var/array/logs    ] ||  mkdir -p $(RaidExportsArea)/var/array/logs 
	@for fid in $(RaidSbinExports); do                              \
            strip860 $$fid                                             ;\
             cp $$fid $(RaidExportsArea)/sbin                           ;\
          done
	@for fid in $(RaidUsrSbinExports); do                           \
            strip860 $$fid                                             ;\
             cp $$fid $(RaidExportsArea)/usr/sbin             ;\
          done
	@for fid in $(RaidSbinInitExports); do                          \
             cp $$fid $(RaidExportsArea)/sbin/init.d          ;\
          done
	@for fid in $(RaidEtcDefaultExports); do                        \
             cp $$fid $(RaidExportsArea)/etc/default          ;\
          done
	@for fid in $(RaidEtcArrayExports); do                          \
             cp $$fid $(RaidExportsArea)/etc/array/acehelp    ;\
          done
	@for fid in $(RaidEtcArrayCCExports); do                        \
             cp $$fid $(RaidExportsArea)/etc/array/cc         ;\
          done
	@for fid in $(RaidEtcDefaultExports); do                        \
             cp $$fid $(RaidExportsArea)/etc/default          ;\
          done
	@cd $(RaidExportsArea); /bin/sh $(RaidExportsArea)/raidinstall

build_all: setup install.sysfiles raid
install_all: exports
