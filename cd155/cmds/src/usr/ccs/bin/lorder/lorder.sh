#!/bin/sh
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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

# @(#)$RCSfile: lorder.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:53:58 $

#
#  COMPONENT_NAME: CMDAOUT (lorder command)
#
#  FUNCTIONS: lorder
#
#  ORIGINS: 27, 9, 3
#
#  (C) COPYRIGHT International Business Machines Corp. 1989
#  All Rights Reserved
#  Licensed Materials - Property of IBM
#
#  US Government Users Restricted Rights - Use, duplication or
#  disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# lorder.sh	1.4  com/cmd/aout/lorder,3.1,9021 1/18/90 10:00:17
#

#	COMMON LORDER
trap "rm -f /tmp/$$sym?ef; exit" 0 1 2 13 15
case $# in
0)	echo usage: lorder file ...
	exit ;;
1)	case $1 in
	*.o)	set $1 $1
	esac
esac


#	The following sed script is commented here.
#	The first two parts of the sed script put the pattern
#	(in this case the file name) into the hold space
#	and creates the "filename filename" lines and
#	writes them out. The first part is for .o files,
#	the second is for .o's in archives.
#	The next 3 sections of code are exactly alike but
#	they handle different external symbols, namely the
#	symbols that are defined in the text section, data section
#	or symbols that are referenced but not defined in this file.
#	A line containing the symbol (from the pattern space) and 
#	the file it is referenced in (from the hold space) is
#	put into the pattern space.
#	If its text or data it is written out to the symbol definition
#	(symdef) file, otherwise it was referenced but not declared
#	in this file so it is written out to the symbol referenced
#	(symref) file.
#
nm -eu $* |
	sed '
	/\.o:$/{
		s/://
		s/^.* //
		h
		s/.*/& &/
		p
		d
	}
	/\.o]:$/{
		s/]://
		s/^.*\[//
		h
		s/.*/& &/
		p
		d
	}
	/|\.text/{
		s/ *|.*//
		G
		s/\n/ /
		w '/tmp/$$symdef'
		d
	}
	/|\.data/{
		s/ *|.*//
		G
		s/\n/ /
		w '/tmp/$$symdef'
		d
	}
	/|\.rdata/{
		s/ *|.*//
		G
		s/\n/ /
		w '/tmp/$$symdef'
		d
	}
	s/ *|.*//
	G
	s/\n/ /
	w '/tmp/$$symref'
	d
'
sort /tmp/$$symdef -o /tmp/$$symdef
sort /tmp/$$symref -o /tmp/$$symref
join /tmp/$$symref /tmp/$$symdef | sed 's/[^ ]* *//'
