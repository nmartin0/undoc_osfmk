######################################################################
##
##  Bld/include/Tarfiles.mk
##

.PHONY: tarfiles
tarfiles: setup $(ReleaseArea)/mach_svr.tar $(ReleaseArea)/usr.tar $(ReleaseArea)/root.tar 


######################################################################
##
##  raid tar files
##
.PHONY: $(ReleaseArea)/raid.tar
$(ReleaseArea)/raid.tar:
	@echo "[ Creating $@ ]"
	@(cd $(RaidExportsArea);tar cf $@ *)

setup:
	@[ -d $(ReleaseArea) ] || mkdir $(ReleaseArea)
	@chgrp wheel $(ReleaseArea)
	@chmod g+ws $(ReleaseArea)

######################################################################
##  mach_svr.tar
##
##
##     Dependencies:
##

MachServers_tarfiles = mach_servers/startup \
                       mach_servers/startup.compute \
                       mach_servers/emulator \
                       mach_servers/mach_kernela.db \
                       mach_servers/mach_kernela \
                       mach_servers/mach_kernelb.db \
                       mach_servers/mach_kernelb \
                       mach_servers/envmgr \
                       mach_servers/mach_init \
                       mach_servers/machid \
                       mach_servers/snames \
                       mach_servers/mach_kernela.compute \
                       mach_servers/mach_kernela.compute.db \
                       mach_servers/mach_kernelb.compute \
                       mach_servers/mach_kernelb.compute.db

.PHONY: $(ReleaseArea)/mach_svr.tar
$(ReleaseArea)/mach_svr.tar: $(addprefix $(ParagonExportsArea)/, $(MachServers_tarfiles))
	@echo "[ Creating $@ ]"
	@( cd $(ParagonExportsArea); tar cfp $@ $(MachServers_tarfiles) )
	@$(Chown) $(BuildPerson) $@
	@chmod 755 $@


######################################################################
##  root.tar
##
##     Dependencies:
##
##       $(RootTarDeps)
##                This is a list of the directory trees within the
##                $(ParagonExportsArea) that go into making up the root.tar
##                file.


RootTarDeps = $(ParagonExportsArea)/bin       \
              $(ParagonExportsArea)/dev       \
              $(ParagonExportsArea)/etc       \
              $(ParagonExportsArea)/home      \
              $(ParagonExportsArea)/lib       \
              $(ParagonExportsArea)/mnt       \
              $(ParagonExportsArea)/postboot  \
              $(ParagonExportsArea)/postboot1 \
              $(ParagonExportsArea)/sbin      \
              $(ParagonExportsArea)/tmp       \
              $(ParagonExportsArea)/var

Root_tarfiles = $(shell echo $(RootTarDeps) | perl -ne '{(@l)=split;foreach (@l){s|^$(ParagonExportsArea)/||o;print(" $$_");}}' )

$(ReleaseArea)/root.tar: $(RootTarDeps)
	@echo "[ Creating $@ ]"
	@[ -d $(ReleaseArea) ] || mkdir $(ReleaseArea)
	@( cd $(ParagonExportsArea); tar cfp $@ $(Root_tarfiles) )
	@$(Chown) $(BuildPerson) $@
	@chmod 755 $@


######################################################################
##  usr.tar
##
##     Dependencies:
##
##       $(UsrTarDeps)
##                This is a list of the directories that go into
##                makeing up the usr.tar file.

UsrTarDeps = $(ParagonExportsArea)/usr

$(ReleaseArea)/usr.tar: $(UsrTarDeps)
	@echo "[ Creating $@ ]"
	@[ -d $(ReleaseArea) ] || mkdir $(ReleaseArea)
	@( cd $(ParagonExportsArea); tar cfp $@ usr )
	@chmod 755 $@


######################################################################
##  raidinstall
##
##    The raid software s built elsewhere but 'installed' in the
##    release area so that it can be put into the same tar file
##    as most of the commands and libraries files.
##
##    Dependencies:
##
##
.PHONY: install-raidware
install-raidware: 
	@echo "[ Running $@ ]"
	@(                                                                                       \
           cd $(ParagonExportsArea)                                                             ;\
           if [ $$? -ne 0 ]; then                                                                \
               exit 1                                                                           ;\
           fi                                                                                   ;\
           zcat $(ImportsArea)/Raid/raid.tar.Z |  tar xf -                              \
         )



