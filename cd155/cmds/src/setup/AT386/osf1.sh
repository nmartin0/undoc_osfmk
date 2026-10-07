# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.1

#
# Setup variables describing the current environment
#
MACHINE="i386"
TARGET_MACHINE="AT386"
target_machine="at386"
CPUTYPE="I386"
cputype="i386"

export MACHINE TARGET_MACHINE target_machine CPUTYPE cputype

#
# Environment variables for the host compilers
#

#
# Use native object or macho version of gcc
#
HOST_OBJECT_FORMAT=A_OUT
CMD_OBJECT_FORMAT=MACHO
KERNEL_OBJECT_FORMAT=A_OUT

ARCHIVE_FORMAT=ASCARCH

export ARCHIVE_FORMAT

#
# Define these if you do not have them
#
#NO_VFPRINTF=NO_VFPRINTF
#export NO_VFPRINTF
#NO_SETENV=NO_SETENV
#export NO_SETENV
#NO_PW_STAYOPEN=NO_PW_STAYOPEN
#export NO_PW_STAYOPEN
#NO_DIRENT=NO_DIRENT
#export NO_DIRENT
#NO_UUDECODE=NO_UUDECODE
#export NO_UUDECODE
# Remove to get shared libs for at386
#NO_SHARED_LIBRARIES=NO_SHARED_LIBRARIES
#export NO_SHARED_LIBRARIES
