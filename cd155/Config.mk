######################################################################
##
##  Config.mk
##
##    This makefile is included by the topmost Makefile as well as all
##    project Makefiles.  It's purpose is to provide easy access to all
##    makefile variables that are commonly changed by the builder.
##


######################################################################
##
##  Version String
##

BUILD_VERSION_STRING := R1_3
.EXPORT: BUILD_VERSION_STRING
