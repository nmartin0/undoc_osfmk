######################################################################
##
##  Bld/include/Raid.mk
##
##      This file contains all the targets for building the 'Raid'
##      projects deliverables.
##

RaidExportsArea        := $(ExportsArea)/paragon

PATH      := $(PARAGON_XDEV)/paragon/bin.sun4:$(PATH)
CPATH     := $(PARAGON_XDEV)/paragon/include
LPATH     := $(PARAGON_XDEV)/paragon/lib-coff

.EXPORT: CPATH LPATH
