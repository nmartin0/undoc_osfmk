######################################################################
##
##  Bld/include/Crossdev.mk
##
##    This included makefile contains all the rules for creating
##    the new cross development environment based on the current
##    build.
##

CompilerAreaRoot        := $(BuildArea)/Bld/Compilers
CoreCompilerArea        := $(CompilerAreaRoot)


######################################################################
##
##  install.sysfiles
##
##    After the cmds/libraries have been build, and before anything
##    else can be built, the header files and libraries built by 
##    cmds/libs must be installed into the crossdev envirnment being
##    used.
##
.PHONY: install.sysfiles
install.sysfiles: $(CoreCompilerArea)/paragon/.Sysfiles.TimeStamp 

$(CoreCompilerArea)/paragon/.Sysfiles.TimeStamp: $(ExportsArea)/sysfiles.tar
	@echo "[ Installing sysfiles for Core Compilers ]"
	@(                                                                                    \
          cd $(CoreCompilerArea)/paragon                                                     ;\
          if [ $$? -eq 0 ]; then                                                              \
              tar xf $^                                                                      ;\
          fi                                                                                  \
        )
	@touch $@
