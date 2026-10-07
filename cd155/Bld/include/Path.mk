######################################################################
##
##  Bld/include/Path.mk
##
##    This 'included' makefile contains variables used by more 
##    than one of the Operating System build projects. This file
##    is included by ALL OS build projects.
##
##

ifeq "$(Deliverable)" "kernel"
    CFG_VAR = I860_CONFIG
else
    CFG_VAR = CONFIG
endif

######################################################################
##
##  The following section establishes the build's PATH variable.
##

PATH             := $(BUILD_XDEV)/bin
PATH             := $(PATH):$(PARAGON_XDEV)/paragon/bin.sun4
PATH             := $(PATH):/bin

                 # 'head' used in kernel builds
PATH             := $(PATH):/usr/ucb

                 # 'chown' used in postinstall
PATH             := $(PATH):/usr/etc

.EXPORT: PATH
