# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
default: all

#
# Include file and library search paths.
#
CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH):../../../../lib/tape
.EXPORT: CPATH LPATH

#
# i860 Support
#
CC = $(DOI860?icc:cc)
LD = $(DOI860?ld860:ld)
AS = $(DOI860?as860:as)
AR = $(DOI860?ar860:ar)

#
# Note that $(MAKETOP) is the top of the object tree, and
# $(MAKECONF:h) is the top of the source tree.  (If there were
# no separate object tree then these would be identical.)
# $(MKINC), the microkernel include files, is defined in Makeconf.
#
SOURCECWD = $(MAKECONF:h)$(MAKEDIR)

INCS = \
	-I.\
	-I$(MAKETOP)/server \
	-I$(MAKETOP)/server/include \
	-I$(MAKETOP)/usr/include \
	-I$(MAKETOP)/usr/include/$(TARGET_MODEL) \
	-I$(MAKECONF:h)/server \
	-I$(MAKECONF:h)/server/$(cputype) \
	-I$(MKINC) \
	-I$(SOURCECWD)/../../../../lib/tape/include

FILE_TARGETS = \
	node.rx

CLEANLIST = $(FILE_TARGETS) $(DIR_TARGETS)

.INOBJECTDIR: packtest.sh 
	rm -rf ./$<
	cp -p $< ./$<

all: $(FILE_TARGETS)

$(FILE_TARGETS): node.c
	$(CC) -o node.rx $(INCS) node.c -l 3480  -nx 

clean:
	rm -rf $(CLEANLIST) _cvs_id.o _cvs_id.c

print_env:
	@echo env MAKECPP is $$MAKECPP
	@echo MAKECPP is $(MAKECPP)
	@echo SOURCECWD is $(SOURCECWD)
	@echo MAKEDIR is $(MAKEDIR)
	@echo MAKESUB is $(MAKESUB)
	@echo MAKETOP is $(MAKETOP)
	@echo VPATH is $(VPATH)
	@echo CPATH is $(CPATH)
	@echo LPATH is $(LPATH)
	@echo LOCAL_CPATH is $(LOCAL_CPATH)
	@echo LOCAL_LPATH is $(LOCAL_LPATH)
	@echo TARGET_CPATH is $(TARGET_CPATH)
	@echo TARGET_LPATH is $(TARGET_LPATH)
