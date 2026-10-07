######################################################################
##
##  cmds/Makefile
##
##      This is the file that contains all the Makefile targets
##      used to produce the command and libraries deliverables.
##
##    Targets defined herein:
##
##      all        This target is for building and exporting all
##                 of the commands and libraries deliverables.
##
##      commands   This is for just building the commands and 
##                 libraries.
##
##      exports    This is for 'exporting' all the commands and
##                 libraries derrived obects. The effect of this
##                 is to move the correct objects from the build
##                 area to a release area where they can/will be
##                 packaged for release.
##
##
##      setup
##                 Runs the setup script that is in the top
##                 directory of the sources. This setup script
##                 initializes the build area.
##

include ../Config.mk
include ../Bld/include/StdDefines.mk
include ../Bld/include/OS.mk
include ../Bld/include/Path.mk


######################################################################
##
##  Cmds/Libs
##

CommandsSources        := $(CommandsArea)/src


######################################################################
##  all
##
##    Build and export all commands and libraries deliverables.
##
##    Dependencies:
##
##
.PHONY: all
all: exports $(ExportsArea)/sysfiles.tar
.PHONY: build_all
build_all:  commands
.PHONY: install_all
install_all: ins_exports $(ExportsArea)/sysfiles.tar


######################################################################
##  setup
##
##    There is a shell script at the top of the commands and libraries
##    source that sets up the object tree prior to acually building   
##    the sources.  This rule makes sure that shell script gets
##    run.
##
##    Dependencies:
##
##        None
##
setup:
	@echo "[ Setup cmds for building ]"
	@(cd $(CommandsSources);./SetupCmds)


######################################################################
##  commands
##
##    Build all commands and libraries deliverables.
##
##    How:
##        Line No.  Purpose
##        --------  ---------------------------------------------------
##  
##        1,2       The usuall log stuff
##        3         Change current directory to the top of the commands
##                  source area.
##        4         Subinvoke cmake with the target for building all the
##                  objects while piping the output to ...
##        5         ... the log file for future reference and ...
##        6         ... to STDOUT via a perl script to pretty it up.
##        8,9       The usuall log stuff.
##
##    Dependencies:
##
##        setup
##                  Makes sure the object tree is initialized.
##
.PHONY: commands
commands: setup
	@$(MkLog) $(LogArea)/commands.build
	@date > $(LogArea)/commands.build
	@( cd $(CommandsSources)                                                                ;\
           MAKE='' MFLAGS='' MAKEFLAGS='' cmake -ckEF build_all 2>&1                            |\
           tee -a $(LogArea)/commands.build                                                     |\
           perl -lnae '{if(/^\[/){s/^\[/[ Building cmds:/;print("$$_");}}'         \
         )
	@date >> $(LogArea)/commands.build


######################################################################
##  exports
##
##    Export all commands and libraries deliverables.
##
##
##    How:
##        Line No.  Purpose
##        --------  ---------------------------------------------------
##  
##        1,2       The usuall log stuff
##        3         Change current directory to the top of the commands
##                  source area.
##        4         Subinvoke cmake with the target for installing all the
##                  objects while piping the output to ...
##        5         ... the log file for future reference and ...
##        6         ... to STDOUT via a perl script to pretty it up.
##        8,9       The usuall log stuff.
##
##    Dependencies:
##
##        commands
##                  Can't export what we havn't built.
##
.PHONY: exports
exports: commands
	@$(MkLog) $(LogArea)/commands.install
	@date > $(LogArea)/commands.install
	@[ -d $$TOSTAGE ] || mkdir $$TOSTAGE
	@( cd $(CommandsSources)                                                                ;\
           MAKE='' MFLAGS='' MAKEFLAGS='' cmake -ckEF install_all 2>&1                          |\
           tee -a $(LogArea)/commands.install                                                   |\
           perl -lnae '{if(/^\[/){s/^\[/[ Installing cmds:/;print("$$_");}}'       \
         )
	@( cd $(PrebuiltArea); pmake install 2>&1 | tee -a $(LogArea)/commands.install )
	@date >> $(LogArea)/commands.install

ins_exports: 
	@$(MkLog) $(LogArea)/commands.install
	@date > $(LogArea)/commands.install
	@[ -d $$TOSTAGE ] || mkdir $$TOSTAGE
	@( cd $(CommandsSources)                                                                ;\
           MAKE='' MFLAGS='' MAKEFLAGS='' cmake -ckEF install_all 2>&1                          |\
           tee -a $(LogArea)/commands.install                                                   |\
           perl -lnae '{if(/^\[/){s/^\[/[ Installing cmds:/;print("$$_");}}'       \
         )
	@( cd $(PrebuiltArea); pmake install 2>&1 | tee -a $(LogArea)/commands.install )
	@date >> $(LogArea)/commands.install
######################################################################
##  sysfiles.tar
##
##     Dependencies:
##
##       $(SysfilesTarDeps)
##                This is a list of all the .o and .a files within
##                $(ParagonExportsArea)/usr/ccs that make up the sysfiles.tar
##                file.

SysfilesTarDeps = $(ParagonExportsArea)/usr/ccs/lib/crt0.o                       \
                  $(ParagonExportsArea)/usr/ccs/lib/guard.o                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libAF.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libPW.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libbsd.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/libc.a                       \
                  $(ParagonExportsArea)/usr/ccs/lib/libc_r.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/libcs.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libcthreads.a                \
                  $(ParagonExportsArea)/usr/ccs/lib/libcurses.a                  \
                  $(ParagonExportsArea)/usr/ccs/lib/libdbm.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/libenv.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/libg.a                       \
                  $(ParagonExportsArea)/usr/ccs/lib/libhippi.a                   \
                  $(ParagonExportsArea)/usr/ccs/lib/libl.a                       \
                  $(ParagonExportsArea)/usr/ccs/lib/libld.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libln.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libmach.a                    \
                  $(ParagonExportsArea)/usr/ccs/lib/libmach3.a                   \
                  $(ParagonExportsArea)/usr/ccs/lib/libmach_sa.a                 \
                  $(ParagonExportsArea)/usr/ccs/lib/libmachid.a                  \
                  $(ParagonExportsArea)/usr/ccs/lib/libmp.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libnetname.a                 \
                  $(ParagonExportsArea)/usr/ccs/lib/libnx.a                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libpm.o                      \
                  $(ParagonExportsArea)/usr/ccs/lib/libpthreads.a                \
                  $(ParagonExportsArea)/usr/ccs/lib/libresolv.a                  \
                  $(ParagonExportsArea)/usr/ccs/lib/librpc.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/libservice.a                 \
                  $(ParagonExportsArea)/usr/ccs/lib/libsys5.a                    \
                  $(ParagonExportsArea)/usr/ccs/lib/libtermcap.a                 \
                  $(ParagonExportsArea)/usr/ccs/lib/libtermlib.a                 \
                  $(ParagonExportsArea)/usr/ccs/lib/libtli.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/libutil.a                    \
                  $(ParagonExportsArea)/usr/ccs/lib/libxti.a                     \
                  $(ParagonExportsArea)/usr/ccs/lib/liby.a                       \
                  $(ParagonExportsArea)/usr/ccs/lib/mcrt0.o                      \
                  $(ParagonExportsArea)/usr/ccs/lib/noieee/__denormal.o          \
                  $(ParagonExportsArea)/usr/ccs/lib/options/autoinit.o           

Sysfiles_tarfiles = $(shell echo $(SysfilesTarDeps) | perl -ne '{(@l)=split;foreach (@l){s|$(ParagonExportsArea)/usr/ccs/lib|lib-coff|;print(" $$_");}}' )

$(ExportsArea)/sysfiles.tar: postinstall $(SysfilesTarDeps)
	@echo "[ Creating $@ ]"
	@[ -d $(ExportsArea) ] || mkdir $(ExportsArea)
	@rm -f $(ParagonExportsArea)/usr/ccs/lib-coff; ln -s $(ParagonExportsArea)/usr/ccs/lib $(ParagonExportsArea)/usr/ccs/lib-coff
	@( cd $(ParagonExportsArea)/usr/ccs; tar cfph $@ $(Sysfiles_tarfiles) )
	@( cd $(ParagonExportsArea)/usr; tar rfp $@ include )
	@rm -f $(ParagonExportsArea)/usr/ccs/lib-coff
	@$(Chown) $(BuildPerson) $@
	@chmod 755 $@


######################################################################
##  Post command&libraries installation step.
##
##    The postinstall script 'fixes' many things in the export
##    tree prior to packaging for actual release.  In order to
##    be able to rerun postinstall more than once, it's necessary
##    to first remove any links to directories that postinstall is
##    going to try and recreate.  If we didn't remove them, we
##    would end up with links within the directories that pointed
##    back to the directory (recursive symlinks).
##
##    Dependencies:
##
##        None
##
Links2Directories = usr/adm                 \
                    usr/mail                \
                    usr/news                \
                    usr/preserve            \
                    usr/spool               \
                    usr/lib/options         \
                    lib                     \
                    bin                     \
                    usr/ucb                 \
                    usr/man                 \
                    usr/lib/me              \
                    usr/lib/ms              \
                    usr/lib/term            \
                    usr/lib/tabset          \
                    usr/lib/terminfo        \
                    usr/lib/tmac

.PHONY: postinstall
postinstall::
	@echo "[ Running $@ ]"
	@(                                                                                       \
           cd $(ParagonExportsArea)                                                             ;\
           if [ $$? -ne 0 ]; then                                                                \
               exit 1                                                                           ;\
           fi                                                                                   ;\
           rm -f $(Links2Directories)                                                 ;\
           $(MkLog) $(LogArea)/postinstall                                                      ;\
           ./postinstall $(ParagonExportsArea) > $(LogArea)/postinstall 2>&1           \
         )


BadUsernames =  mach_servers                        \
                var/spool                           \
                var/spool/mqueue                    \
                var/spool/cron                      \
                var/adm/sendmail                    \
                home                                \
                var/adm/sendmail/sendmail.cw        \
                var/adm/cron                        \
                mnt                                 \
                var/spool/uucp                      \
                var/spool/cron/crontabs             \
                var/spool/lpd                       \
                var/news                            \
                var/adm/sendmail/sendmail.st        \
                sbin/sed                            \
                var/spool/cron/atjobs               \
                var/adm/syslog                      \
                var/mail                            \
                var/spool/uucp/.Workspace           \
                var/spool/crontab                   \
                sbin/awk                            \
                sbin/init.d                         \
                var/adm/wtmp                        \
                var/preserve                        \
                var/tmp                             \
                var                                 \
                var/adm                             \
                var/adm/crash                       \
                sbin                                \
                var/run                             \
                tmp                                 \
                sbin/rc0.d                          \
                sbin/rc2.d                          \
                sbin/rc3.d                          \
                var/rwho                            \
                var/named   

postinstall:: 
	@for dir in $(BadUsernames); do                                                          \
            chgrp daemon $(ParagonExportsArea)/$$dir                                  ;\
          done

##  Fix permissions on 'special' directories
Macs.Nqs.Hack := usr/lib/macs usr/lib/macs/setup usr/lib/nqs usr/lib/nqs/setup
postinstall::
	@for dir in $(Macs.Nqs.Hack); do                                                         \
            chmod 775 $(ParagonExportsArea)/$$dir                                     ;\
            chmod g-s $(ParagonExportsArea)/$$dir                                     ;\
        done

